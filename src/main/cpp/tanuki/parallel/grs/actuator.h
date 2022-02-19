#ifndef TANUKI_PARALLEL_GRS_ACTUATOR_H
#define TANUKI_PARALLEL_GRS_ACTUATOR_H

#include <cstddef>
#include <functional>
#include <map>
#include <set>
#include <utility>
#include <vector>

#include <armadillo>
#include <mpi.h>

#include "tanuki/math/linear/indexed_vector_pair.h"
#include "tanuki/math/linear/rotation_matrix_spec.h"
#include "tanuki/number/types.h"
#include "tanuki/parallel/concurrent_actuator.h"
#include "tanuki/parallel/grs/jacobi_sidedness.h"
#include "tanuki/parallel/mpi/mpi_host_based_comms.h"
#include "tanuki/parallel/mpi/mpi_shared_memory.h"

namespace tanuki {
namespace parallel {
namespace grs {

using arma::Col;
using arma::Mat;

using math::linear::IndexedVectorPair;
using math::linear::RotationMatrixSpec;
using parallel::mpi::MpiHostBasedComms;
using parallel::mpi::MpiSharedMemory;

using tanuki::number::real_t;

/**
 *  @brief Result of a GRS actuation.
 *
 *  @tparam T
 *    Type of elements in an Armadillo matrix.
 */
template <typename T>
struct Result final {
 public:
  /**
   *  @brief Matrix transform.
   */
  Mat<T> transform;

  /**
   *  @brief Number of iterations.
   */
  size_t num_iters;

  /**
   *  @brief Whether GRS has converged.
   */
  bool has_converged;
};

/**
 *  @brief Actuator of the Givens Relaxation Scheme (GRS).
 *
 *  GRS is a generalization of the Jacobi Relaxation Scheme (JRS) developed by
 *  Rajasekaran 2008.
 *
 *  @tparam T
 *    Type of elements in an Armadillo matrix.
 */
template <typename T>
class Actuator final : public ConcurrentActuator<
    const IndexedVectorPair<T> &, RotationMatrixSpec, Mat<T>, Result<T>> {
 public:
  /**
   *  @brief Type of the inquiry function.
   *
   *  Type of function that returns the specification of a rotation matrix for
   *  a given vector pair. The rotation being specified is unrelaxed.
   */
  using InquiryFn =
      std::function<RotationMatrixSpec(const IndexedVectorPair<T> &)>;

  /**
   *  @brief Type of the convergence checker.
   */
  using ConvergenceFn = std::function<bool(const Mat<T> &, const Mat<T> &)>;

  /**
   *  @param mpi_comm
   *    MPI communicator. The level of thread support must be at least
   *    <tt>MPI_THREAD_FUNNELED</tt>. Number of MPI processes at each host must
   *    be the same.
   *
   *  @param max_threads
   *    Maximum number of threads to use. It cannot exceed
   *    <tt>omp_get_max_threads()</tt>.
   *
   *  @param sidedness
   *    Sidedness of GRS to perform. If a two-sided GRS is to be performed,
   *    <tt>matrix</tt> must be a square matrix (otherwise
   *    <tt>std::invalid_argument</tt> is thrown), and only columns will be
   *    queried for rotations; the rotations are then applied to both rows and
   *    columns.
   *
   *  @param init_relax
   *    Initial relaxation parameter. If it is not in the range \f$ [0, 1) \f$,
   *    <tt>std::domain_error</tt> is thrown.
   *
   *  @param num_groups
   *    Positive number of groups of rotation sets to form. If zero,
   *    <tt>std::domain_error</tt> is thrown.
   *
   *  @param max_iterations
   *    Maximum number of iterations. If not positive,
   *    <tt>std::domain_error</tt> is thrown.
   *
   *  @param relax_fn
   *    Function that returns the relaxation parameter to use for the next
   *    group of rotation sets. Arguments are iteration index, previous
   *    relaxation parameter, and current group index, respectively. If the
   *    next relaxation parameter is not in the range \f$ [0, 1) \f$,
   *    <tt>std::domain_error</tt> is thrown.
   *
   *  @param convergence_checker
   *    Function that returns whether convergence is attained given the
   *    previous and current matrices, respectively, from the transformation
   *    process.
   */
  Actuator(
      MPI_Comm mpi_comm,
      size_t max_threads,
      JacobiSidedness sidedness,
      real_t init_relax,
      size_t num_groups,
      size_t max_iterations,
      std::function<real_t(size_t, real_t, size_t)> relax_fn,
      ConvergenceFn convergence_checker);

  /**
   *  @brief Actuates GRS.
   *
   *  The rotation returned from the inquiry function is not relaxed.
   *  Relaxation is performed by the actuator. Matrix index pair is always
   *  @link MatrixIndexPair::PairType::COLUMNS @endlink.
   *
   *  All MPI processes must invoke this outside any OpenMP parallel region.
   */
  Result<T> Actuate(const Mat<T> &input, InquiryFn inquiry_fn) override;

  MPI_Comm mpi_comm() const override;

  size_t max_threads() const override;

 private:
  /**
   *  @brief Applies the rotations in a rotation set by distributing them
   *  across MPI processes and threads.
   *
   *  It must be invoked by all MPI processes outside of any OpenMP parallel
   *  region.
   *
   *  @param use_mpi
   *    Whether to parallelize the rotation applications across MPI processes.
   *
   *  @param rotation_set
   *    Two-row matrix of a rotation set, as column indices, that contains
   *    non-conflicting rotation pairs. Each column is a rotation pair, where
   *    the column with an element that is <tt>-1</tt> (if there is one) is
   *    ignored. Whether the rotation pairs are non-conflicting is not
   *    validated. It is undefined if the rotation pairs do not encompass all
   *    indices in a non-conflicting manner or any of the indices are
   *    out-of-range with respect to <tt>matrix</tt>.
   *
   *  @param rotation_specs
   *    Rotation specifications. Number of elements must be the same as the
   *    number of columns in <tt>rotation_set</tt>. The element that
   *    corresponds to the dummy pair (if there is one) is ignored.
   *
   *  @param is_by_col
   *    Whether the rotations are applied to the columns (<tt>true</tt>) or
   *    rows (<tt>false</tt>).
   *
   *  @param matrix
   *    Output matrix whose columns or rows are concurrently rotated.
   */
  void DistApplyRotationSet(
      const arma::Mat<long long> &rotation_set,
      const std::vector<RotationMatrixSpec> &rotation_specs,
      bool is_by_col,
      arma::Mat<T> &matrix) const;

  /**
   *  @brief Name of the shared memory for the previous matrix.
   */
  const std::string shared_mem_prev_mat_name_ =
      std::string("parallel_grs_actuator_prev_matrix");

  /**
   *  @brief Name of the shared memory for the current matrix.
   */
  const std::string shared_mem_curr_mat_name_ =
      std::string("parallel_grs_actuator_curr_matrix");

  /**
   *  @brief MPI communicator of the processes across hosts.
   */
  MPI_Comm mpi_comm_;

  /**
   *  @brief Rank of this MPI process in @link mpi_comm_ @endlink.
   */
  int mpi_rank_;

  /**
   *  @brief Number of MPI processes in @link mpi_comm_ @endlink.
   */
  int mpi_comm_size_;

  /**
   *  @brief Host-based MPI communicators.
   */
  MpiHostBasedComms host_based_comms_;

  /**
   *  @brief Maximum number of threads to use.
   */
  const size_t max_threads_;

  /**
   *  @brief Sidedness of GRS.
   */
  const JacobiSidedness sidedness_;

  /**
   *  @brief Relaxation parameter.
   */
  const real_t init_relax_;

  /**
   *  @brief Number of groups of rotation sets.
   */
  const size_t num_groups_;

  /**
   *  @brief Maximum number of iterations.
   */
  const size_t max_iterations_;

  /**
   *  @brief Function that returns the relaxation parameter to use for the next
   *  group of rotation sets.
   *
   *  See the constructor for more description.
   */
  const std::function<real_t(size_t, real_t, size_t)> relax_fn_;

  /**
   *  @brief Convergence checker.
   */
  const ConvergenceFn convergence_checker_;
};

/**
 *  @brief Recommended initial relaxation parameter for one-sided Jacobi.
 *
 *  Recommendation is based on Rajasekaran 2008.
 *
 *  @param n
 *    Positive number of rows.
 */
real_t GrsOneSidedRelaxParam(size_t n);

/**
 *  @brief Recommended initial relaxation parameter for two-sided Jacobi.
 *
 *  Recommendation is based on Rajasekaran 2008.
 *
 *  @param n
 *    Positive number of rows.
 */
real_t GrsTwoSidedRelaxParam(size_t n);

} // namespace grs
} // namespace parallel
} // namespace tanuki

#include "tanuki/parallel/grs/actuator.hxx"

#endif
