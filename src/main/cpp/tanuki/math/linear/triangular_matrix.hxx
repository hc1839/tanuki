#ifndef TANUKI_MATH_LINEAR_TRIANGULAR_MATRIX_HXX
#define TANUKI_MATH_LINEAR_TRIANGULAR_MATRIX_HXX

#include <cassert>
#include <memory>

#include <boost/interprocess/creation_tags.hpp>
#include <omp.h>

#include "tanuki/common/divider/group_delimiter.h"
#include "tanuki/parallel/mpi/mpi_basic_datatype.h"
#include "tanuki/parallel/mpi/mpi_shared_memory.h"

namespace tanuki {
namespace math {
namespace linear {

using boost::interprocess::open_or_create;

using tanuki::common::divider::GroupIndices;
using tanuki::parallel::mpi::MpiBasicDatatype;
using tanuki::parallel::mpi::MpiSharedMemory;

/**
 *  @brief Solves a system of linear equations, \f$ \mathbf{L} \mathbf{x} =
 *  \mathbf{b} \f$, using forward substitution on a contiguous subset of
 *  columns of the constants.
 *
 *  @tparam
 *    Type of matrix elements.
 *
 *  @param start_col
 *    Index of the starting column in the contiguous subset of columns in
 *    <tt>constants</tt>.
 *
 *  @param end_col_exclusive
 *    Index of past-the-end column in the contiguous subset of columns in
 *    <tt>constants</tt>.
 *
 *  @param [out] solutions
 *    Solution, \f$ \mathbf{x} \f$, that is pre-allocated to the same size as
 *    <tt>constants</tt>. Only columns in the contiguous subset specified by
 *    <tt>start_col</tt> and <tt>end_col_exclusive</tt> are written to contain
 *    the computed solutions.
 *
 *  @private
 */
template <typename T>
void ForwardSubstitute(
    const Mat<T> &lower_coeffs,
    const Mat<T> &constants,
    size_t start_col,
    size_t end_col_exclusive,
    Mat<T> &solutions) {
  assert(start_col >= 0);
  assert(start_col <= end_col_exclusive);
  assert(end_col_exclusive <= constants.n_cols);
  assert(arma::size(solutions) == arma::size(constants));

  if (end_col_exclusive == start_col) {
    return;
  }

  // Initialize the values that will become the solutions.
  solutions.submat(0, start_col, solutions.n_rows - 1, end_col_exclusive - 1) =
      constants.submat(
          0, start_col, constants.n_rows - 1, end_col_exclusive - 1);

  // Loop over the columns of the constants on the right-hand side.
  for (size_t rhs_col = start_col; rhs_col != end_col_exclusive; ++rhs_col) {
    // Calculate each solution of the column.
    for (size_t curr_sol_idx = 0;
         curr_sol_idx != solutions.n_rows;
         ++curr_sol_idx) {
      const auto solution = solutions(curr_sol_idx, rhs_col) /
          lower_coeffs(curr_sol_idx, curr_sol_idx);
      solutions.col(rhs_col) = solutions.col(rhs_col) -
          lower_coeffs.col(curr_sol_idx) * solution;
      solutions(curr_sol_idx, rhs_col) = solution;
    }
  }
}

/**
 *  @brief Solves a system of linear equations, \f$ \mathbf{U} \mathbf{x} =
 *  \mathbf{b} \f$, using back substitution on a contiguous subset of columns
 *  of the constants.
 *
 *  @param start_col
 *    Index of the starting column in the contiguous subset of columns in
 *    <tt>constants</tt>.
 *
 *  @param end_col_exclusive
 *    Index of past-the-end column in the contiguous subset of columns in
 *    <tt>constants</tt>.
 *
 *  @param [out] solutions
 *    Solution, \f$ \mathbf{x} \f$, that is pre-allocated to the same size as
 *    <tt>constants</tt>. Only columns in the contiguous subset specified by
 *    <tt>start_col</tt> and <tt>end_col_exclusive</tt> are written to contain
 *    the computed solutions.
 *
 *  @private
 */
template <typename T>
void BackSubstitute(
    const Mat<T> &upper_coeffs,
    const Mat<T> &constants,
    size_t start_col,
    size_t end_col_exclusive,
    Mat<T> &solutions) {
  assert(start_col >= 0);
  assert(start_col <= end_col_exclusive);
  assert(end_col_exclusive <= constants.n_cols);
  assert(arma::size(solutions) == arma::size(constants));

  if (end_col_exclusive == start_col) {
    return;
  }

  // Initialize the values that will become the solutions.
  solutions.submat(0, start_col, solutions.n_rows - 1, end_col_exclusive - 1) =
      constants.submat(
          0, start_col, constants.n_rows - 1, end_col_exclusive - 1);

  // Loop over the columns of the constants on the right-hand side.
  for (size_t rhs_col = start_col; rhs_col != end_col_exclusive; ++rhs_col) {
    // Calculate each solution of the column.
    for (size_t curr_sol_idx = solutions.n_rows - 1;
         curr_sol_idx != -1;
         --curr_sol_idx) {
      const auto solution = solutions(curr_sol_idx, rhs_col) /
          upper_coeffs(curr_sol_idx, curr_sol_idx);
      solutions.col(rhs_col) = solutions.col(rhs_col) -
          upper_coeffs.col(curr_sol_idx) * solution;
      solutions(curr_sol_idx, rhs_col) = solution;
    }
  }
}

template <typename T>
Mat<T> ForwardSubstitute(
    MPI_Comm mpi_comm, const Mat<T> &lower_coeffs, const Mat<T> &constants) {
  int intrahost_rank;
  int intrahost_size;

  // Allocate shared memory.
  unique_ptr<MpiSharedMemory> shm(
      new MpiSharedMemory(
          mpi_comm,
          "triangular_matrix_substitution_cache",
          sizeof(T) * constants.n_elem,
          open_or_create));

  MPI_Comm_rank(shm->comms().intrahost(), &intrahost_rank);
  MPI_Comm_size(shm->comms().intrahost(), &intrahost_size);

  const int intrahost_color = shm->comms().intrahost_color();
  const size_t num_hosts = shm->comms().hosts().num_hosts();

  unique_ptr<Mat<T>> solutions(
      new Mat<T>(
          static_cast<T *>(shm->mem_address()),
          constants.n_rows, constants.n_cols,
          false, true));

  // Indices of the batches grouped by host.
  const auto host_batches = GroupIndices(0, constants.n_cols, num_hosts);

  // Indices of the local batches grouped by MPI process at this host.
  const auto local_batches = GroupIndices(
      host_batches[intrahost_color],
      host_batches[intrahost_color + 1],
      intrahost_size);

  // Indices of the chunks grouped by OpenMP thread of this MPI process.
  const auto chunks = GroupIndices(
      local_batches[intrahost_rank],
      local_batches[intrahost_rank + 1],
      omp_get_max_threads());

  #pragma omp parallel default(shared)
  {
    ForwardSubstitute(
        lower_coeffs,
        constants,
        chunks[omp_get_thread_num()],
        chunks[omp_get_thread_num() + 1],
        *solutions);
  }

  MPI_Barrier(mpi_comm);

  if (intrahost_rank == 0) {
    for (int host_color = 0; host_color != num_hosts; ++host_color) {
      const size_t num_cols =
          host_batches[host_color + 1] - host_batches[host_color];

      const size_t num_elem = num_cols * solutions->n_rows;

      MPI_Bcast(
          solutions->colptr(host_batches[host_color]),
          num_elem,
          MpiBasicDatatype<T>(),
          host_color,
          shm->comms().interhost());
    }
  }

  MPI_Barrier(mpi_comm);

  return Mat<T>(*solutions);
}

template <typename T>
Mat<T> BackSubstitute(
    MPI_Comm mpi_comm, const Mat<T> &upper_coeffs, const Mat<T> &constants) {
  int intrahost_rank;
  int intrahost_size;

  // Allocate shared memory.
  unique_ptr<MpiSharedMemory> shm(
      new MpiSharedMemory(
          mpi_comm,
          "triangular_matrix_substitution_cache",
          sizeof(T) * constants.n_elem,
          open_or_create));

  MPI_Comm_rank(shm->comms().intrahost(), &intrahost_rank);
  MPI_Comm_size(shm->comms().intrahost(), &intrahost_size);

  const int intrahost_color = shm->comms().intrahost_color();
  const size_t num_hosts = shm->comms().hosts().num_hosts();

  unique_ptr<Mat<T>> solutions(
      new Mat<T>(
          static_cast<T *>(shm->mem_address()),
          constants.n_rows, constants.n_cols,
          false, true));

  // Indices of the batches grouped by host.
  const auto host_batches = GroupIndices(0, constants.n_cols, num_hosts);

  // Indices of the local batches grouped by MPI process at this host.
  const auto local_batches = GroupIndices(
      host_batches[intrahost_color],
      host_batches[intrahost_color + 1],
      intrahost_size);

  // Indices of the chunks grouped by OpenMP thread of this MPI process.
  const auto chunks = GroupIndices(
      local_batches[intrahost_rank],
      local_batches[intrahost_rank + 1],
      omp_get_max_threads());

  #pragma omp parallel default(shared)
  {
    BackSubstitute(
        upper_coeffs,
        constants,
        chunks[omp_get_thread_num()],
        chunks[omp_get_thread_num() + 1],
        *solutions);
  }

  MPI_Barrier(mpi_comm);

  if (intrahost_rank == 0) {
    for (int host_color = 0; host_color != num_hosts; ++host_color) {
      const size_t num_cols =
          host_batches[host_color + 1] - host_batches[host_color];

      const size_t num_elem = num_cols * solutions->n_rows;

      MPI_Bcast(
          solutions->colptr(host_batches[host_color]),
          num_elem,
          MpiBasicDatatype<T>(),
          host_color,
          shm->comms().interhost());
    }
  }

  MPI_Barrier(mpi_comm);

  return Mat<T>(*solutions);
}

} // namespace linear
} // namespace math
} // namespace tanuki

#endif
