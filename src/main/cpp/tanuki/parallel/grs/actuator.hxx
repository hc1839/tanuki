#ifndef TANUKI_PARALLEL_GRS_ACTUATOR_HXX
#define TANUKI_PARALLEL_GRS_ACTUATOR_HXX

#include <algorithm>
#include <cassert>
#include <cmath>
#include <complex>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <stdexcept>
#include <string>

#include <boost/interprocess/shared_memory_object.hpp>
#include <omp.h>

#include "tanuki/common/divider/group_delimiter.h"
#include "tanuki/math/combinatorics/round_robin_tourney.h"
#include "tanuki/math/linear/matrix_index_pair.h"
#include "tanuki/parallel/memory/copy.h"

namespace tanuki {
namespace parallel {
namespace grs {

using std::calloc;
using std::complex;
using std::free;
using std::map;
using std::memcpy;
using std::pair;
using std::set;
using std::strcpy;
using std::string;
using std::vector;

using arma::uvec;
using boost::interprocess::mapped_region;
using boost::interprocess::open_only;
using boost::interprocess::open_or_create;
using boost::interprocess::read_write;
using boost::interprocess::shared_memory_object;

using common::divider::GroupConsecutive;
using common::divider::GroupIndices;
using common::divider::GroupSizes;
using math::combinatorics::RoundRobinTourney;
using math::linear::CreateGivensRotation;
using math::linear::MatrixIndexPair;
using math::linear::RotationMatrixSpec;

template <typename T>
Actuator<T>::Actuator(
    MPI_Comm mpi_comm,
    size_t max_threads,
    JacobiSidedness sidedness,
    real_t init_relax,
    size_t num_groups,
    size_t max_iterations,
    std::function<real_t(size_t, real_t, size_t)> relax_fn,
    ConvergenceFn convergence_checker)
        : mpi_comm_(mpi_comm),
          max_threads_(max_threads),
          sidedness_(sidedness),
          init_relax_(init_relax),
          num_groups_(num_groups),
          max_iterations_(max_iterations),
          relax_fn_(relax_fn),
          convergence_checker_(convergence_checker),
          host_based_comms_(MpiHostBasedComms(mpi_comm)) {
  int thread_support;
  MPI_Query_thread(&thread_support);
  assert(thread_support >= MPI_THREAD_FUNNELED);

  assert(max_threads_ <= omp_get_max_threads());

  if (init_relax_ < 0.0 || init_relax_ >= 1.0) {
    throw std::domain_error(
        "Relaxation parameter is not in the interval [0, 1).");
  }

  if (num_groups_ == 0) {
    throw std::domain_error("Number of groups of rotation sets is zero.");
  }

  if (max_iterations_ == 0) {
    throw std::domain_error("Maximum number of iterations is not positive.");
  }

  MPI_Comm_rank(mpi_comm_, &mpi_rank_);
  MPI_Comm_size(mpi_comm_, &mpi_comm_size_);

  const auto &hosts = host_based_comms_.hosts();

  {
    set<size_t> num_procs_at_hosts;

    for (const auto &host_name : hosts.HostNames()) {
      num_procs_at_hosts.insert(hosts.RanksAt(host_name).size());
    }

    assert(num_procs_at_hosts.size() == 1);
  }
}

template <typename T>
Result<T> Actuator<T>::Actuate(const Mat<T> &input, InquiryFn inquiry_fn) {
  assert(!omp_in_parallel());

  if (input.n_rows == 0 || input.n_cols < 2) {
    throw std::length_error("Invalid size of the input matrix to transform.");
  }

  if (sidedness_ == JacobiSidedness::TWO_SIDED && !input.is_square()) {
    throw std::invalid_argument(
        "GRS is two-sided but the input matrix is not square.");
  }

  // Column indices as competitors in a round-robin tournament, where each
  // round is a non-conflicting rotation set.
  RoundRobinTourney<long long> tourney(input.n_cols);

  // Number of rotation sets for each group.
  const auto group_sizes = GroupSizes(0, tourney.num_rounds(), num_groups_);

  // Shared memory for the previous matrix.
  MpiSharedMemory prev_mat_shm(
      mpi_comm_, shared_mem_prev_mat_name_,
      sizeof(T) * input.n_elem, open_or_create);

  // Pointer to the shared memory for the previous matrix in the iteration.
  const auto prev_mat_shm_ptr = static_cast<T *>(prev_mat_shm.mem_address());

  parallel::memory::Copy(
      host_based_comms_.intrahost(),
      input.memptr(),
      prev_mat_shm_ptr,
      sizeof(T) * input.n_elem);

  // Matrix from the previous iteration to check for convergence.
  Mat<T> prev_matrix(
      prev_mat_shm_ptr,
      input.n_rows, input.n_cols,
      false, true);

  // Shared memory for the current matrix.
  MpiSharedMemory curr_mat_shm(
      mpi_comm_, shared_mem_curr_mat_name_,
      sizeof(T) * input.n_elem, open_or_create);

  // Pointer to the shared memory for the current matrix in the iteration.
  const auto curr_mat_shm_ptr = static_cast<T *>(curr_mat_shm.mem_address());

  parallel::memory::Copy(
      host_based_comms_.intrahost(),
      input.memptr(),
      curr_mat_shm_ptr,
      sizeof(T) * input.n_elem);

  // Matrix in the current iteration.
  Mat<T> curr_matrix(
      curr_mat_shm_ptr,
      input.n_rows, input.n_cols,
      false, true);

  real_t relaxation = init_relax_;

  Result<T> retval;
  retval.has_converged = false;

  for (size_t iter = 0; iter != max_iterations_; ++iter) {
    // Iterator to a rotation set.
    auto rs_it = tourney.begin();

    // Iterate over groups of rotation sets.
    for (size_t gr = 0; gr != num_groups_; ++gr) {
      // Rotation pairs, corresponding by column, in the group.
      Mat<long long> rotation_pairs;

      // Concatenate rotation sets in the group.
      for (size_t rs = 0; rs != group_sizes[gr]; ++rs) {
        rotation_pairs = arma::join_horiz(rotation_pairs, rs_it->t());
        ++rs_it;
      }

      // Matrix of relaxed rotation cosines and sines at the top and bottom
      // rows, respectively, for rotation pairs.
      Mat<real_t> cosine_sine(2, rotation_pairs.n_cols);

      // Delimitation of rotation pairs by MPI processes.
      const auto batch_indices = GroupIndices(
          0, rotation_pairs.n_cols, mpi_comm_size_);

      // Delimitation of rotation pairs by threads.
      const auto chunk_indices = GroupIndices(
          batch_indices[mpi_rank_],
          batch_indices[mpi_rank_ + 1],
          max_threads());

      #pragma omp parallel default(shared)
      {
        const auto thread_num = omp_get_thread_num();

        if (thread_num < max_threads()) {
          // Calculate and store the relaxed rotation for each rotation pair.
          for (size_t rp = chunk_indices[thread_num];
               rp != chunk_indices[thread_num + 1];
               ++rp) {
            MatrixIndexPair indices = {
              .type = MatrixIndexPair::PairType::COLUMNS,
              .first = static_cast<size_t>(rotation_pairs(0, rp)),
              .second = static_cast<size_t>(rotation_pairs(1, rp))
            };

            // Ensure the first index is not greater than the second.
            if (indices.first > indices.second) {
              std::swap(indices.first, indices.second);
            }

            if (indices.first == -1 || indices.second == -1) {
              cosine_sine(0, rp) = 1.0;
              cosine_sine(1, rp) = 0.0;
            } else {
              const IndexedVectorPair<T> vectors = {
                .indices = indices,
                .first = Col<T>(
                    curr_matrix.colptr(indices.first),
                    curr_matrix.n_rows,
                    false,
                    true),
                .second = Col<T>(
                    curr_matrix.colptr(indices.second),
                    curr_matrix.n_rows,
                    false,
                    true)
              };

              // Unrelaxed rotation.
              const auto rotation_spec = inquiry_fn(vectors);

              const auto relaxed_angle = (1.0 - relaxation) *
                  std::atan2(rotation_spec.sine, rotation_spec.cosine);

              cosine_sine(0, rp) = std::cos(relaxed_angle);
              cosine_sine(1, rp) = std::sin(relaxed_angle);
            }
          }
        }
      }

      // Broadcast the relaxed rotations.
      for (int rank = 0; rank != mpi_comm_size_; ++rank) {
        // Number of elements to broadcast.
        const size_t num_elements =
            (batch_indices[rank + 1] - batch_indices[rank]) * 2;

        MPI_Bcast(
            cosine_sine.colptr(batch_indices[rank]),
            num_elements,
            MPI_DOUBLE,
            rank,
            mpi_comm_);
      }

      const auto num_pairs_per_round = tourney.num_pairs_per_round(true);

      // Apply the rotations in each rotation set in a distributed manner.
      for (size_t start_rp = 0;
           start_rp != rotation_pairs.n_cols;
           start_rp += num_pairs_per_round) {

        const Mat<long long> rotation_set(
            rotation_pairs.colptr(start_rp),
            2,
            num_pairs_per_round,
            false,
            true);

        vector<RotationMatrixSpec> rotation_specs;

        // Populate the rotation specs.
        for (size_t rp = start_rp;
             rp != start_rp + num_pairs_per_round;
             ++rp) {
          RotationMatrixSpec rotation_spec = {
            .cosine = cosine_sine(0, rp),
            .sine = cosine_sine(1, rp)
          };

          rotation_specs.push_back(rotation_spec);
        }

        DistApplyRotationSet(rotation_set, rotation_specs, true, curr_matrix);

        if (sidedness_ == JacobiSidedness::TWO_SIDED) {
          DistApplyRotationSet(
              rotation_set, rotation_specs, false, curr_matrix);
        }
      }

      relaxation = relax_fn_(iter, relaxation, gr);

      if (relaxation < 0.0 || relaxation >= 1.0) {
        throw std::domain_error(
            "Relaxation parameter is not in the interval [0, 1).");
      }
    }

    auto has_converged = convergence_checker_(prev_matrix, curr_matrix);

    MPI_Allreduce(
        MPI_IN_PLACE, &has_converged, 1, MPI_CXX_BOOL, MPI_LAND, mpi_comm_);

    retval.num_iters = iter + 1;

    if (has_converged) {
      retval.has_converged = true;
      break;
    } else {
      parallel::memory::Copy(
          host_based_comms_.intrahost(),
          curr_matrix.memptr(),
          prev_matrix.memptr(),
          curr_matrix.n_elem * sizeof(T));
    }
  }

  MPI_Barrier(mpi_comm_);

  retval.transform = Mat<T>(
      curr_matrix.memptr(),
      curr_matrix.n_rows,
      curr_matrix.n_cols);

  return retval;
}

template <typename T>
MPI_Comm Actuator<T>::mpi_comm() const {
  return mpi_comm_;
}

template <typename T>
size_t Actuator<T>::max_threads() const {
  return max_threads_;
}

template <typename T>
void Actuator<T>::DistApplyRotationSet(
    const arma::Mat<long long> &rotation_set,
    const std::vector<RotationMatrixSpec> &rotation_specs,
    bool is_by_col,
    Mat<T> &matrix) const {
  assert(!omp_in_parallel());
  assert(!rotation_set.is_empty());
  assert(rotation_set.n_rows == 2);
  assert(rotation_specs.size() == rotation_set.n_cols);

  if (!is_by_col) {
    Mat<T> matrix_st(matrix.st());
    DistApplyRotationSet(rotation_set, rotation_specs, true, matrix_st);
    matrix = matrix_st.st();
    return;
  }

  int intrahost_rank;
  MPI_Comm_rank(host_based_comms_.intrahost(), &intrahost_rank);

  int intrahost_size;
  MPI_Comm_size(host_based_comms_.intrahost(), &intrahost_size);

  // Subdivide the rotation set by MPI process.
  const auto rp_batches = GroupIndices(
      0, rotation_specs.size(), intrahost_size);

  // Subdivide the batch of rotation pairs by thread.
  const auto rp_chunks = GroupIndices(
      rp_batches[intrahost_rank],
      rp_batches[intrahost_rank + 1],
      max_threads());

  // Concurrently rotate the columns assigned to this MPI process.
  #pragma omp parallel default(shared)
  {
    const auto thread_num = omp_get_thread_num();

    // Number of rotations assigned to this thread.
    const size_t num_rotations = thread_num < max_threads() ?
        rp_chunks[thread_num + 1] - rp_chunks[thread_num] : 0;

    if (num_rotations != 0) {
      // Iterator to the beginning of the chunk of rotation specs.
      auto spec_first = rotation_specs.begin();
      std::advance(spec_first, rp_chunks[thread_num]);

      // Iterator to the end of the chunk of rotation specs for loop
      // termination.
      auto spec_last = spec_first;

      std::advance(spec_last, num_rotations);

      // Iterator to the current rotation pair.
      auto pair_it = rotation_set.begin_col(rp_chunks[thread_num]);

      // Apply the rotations assigned to this thread.
      for (auto spec_it = spec_first;
           spec_it != spec_last;
           ++spec_it, std::advance(pair_it, 2)) {
        pair<long long, long long> rotation_pair(*pair_it, *(pair_it + 1));

        if (rotation_pair.first > rotation_pair.second) {
          std::swap(rotation_pair.first, rotation_pair.second);
        }

        if (rotation_pair.first == -1) {
          continue;
        }

        const auto &rotation_spec = *spec_it;

        const auto row_rotation =
            CreateGivensRotation(rotation_spec, 2, 0, 1);

        const uvec idxs = {
          static_cast<size_t>(rotation_pair.first),
          static_cast<size_t>(rotation_pair.second)
        };

        matrix.cols(idxs) = matrix.cols(idxs) * row_rotation.t();
      }
    }
  }

  // Wait for the MPI processes at this host to finish applying the rotations.
  MPI_Barrier(host_based_comms_.intrahost());
}

} // namespace grs
} // namespace parallel
} // namespace tanuki

#endif
