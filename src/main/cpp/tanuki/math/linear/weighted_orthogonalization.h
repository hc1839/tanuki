#ifndef TANUKI_MATH_LINEAR_WEIGHTED_ORTHOGONALIZATION_H
#define TANUKI_MATH_LINEAR_WEIGHTED_ORTHOGONALIZATION_H

#include <cstddef>
#include <vector>

#include <armadillo>
#include <mpi.h>

#include "tanuki/math/linear/indexed_vector_pair.h"
#include "tanuki/math/linear/rotation_matrix_spec.h"
#include "tanuki/number/types.h"
#include "tanuki/parallel/grs/actuator.h"

namespace tanuki {
namespace math {
namespace linear {

using std::vector;

using arma::Mat;

using math::linear::IndexedVectorPair;
using math::linear::RotationMatrixSpec;
using parallel::grs::Actuator;

using tanuki::number::real_t;

/**
 *  @brief Orthogonalization with weights.
 *
 *  Implementation is the weighted orthogonalization (WO) by West 2014.
 *
 *  @tparam T
 *    Type of elements in an Armadillo matrix.
 *
 *  @param nonortho_matrix
 *    Nonorthogonal matrix where the columns are normalized.
 *
 *  @param prelim_ortho_matrix
 *    Orthogonal matrix whose columns are to be rotated to maximize the overlap
 *    with <tt>nonortho_matrix</tt>. It must have the same size as
 *    <tt>nonortho_matrix</tt>.
 *
 *  @param weights
 *    Non-negative orthogonalization weights in corresponding order to @link
 *    nonortho_matrix @endlink. Number of elements must be the number of
 *    columns in @link nonortho_matrix @endlink.
 *
 *  @param zero_abs_thresh
 *    Absolute threshold, as a positive number, for deciding whether a weight
 *    or the \f$ A \f$ coefficient is close enough to zero. It is a threshold
 *    that avoids division by a number that is too close to zero.
 *
 *  @param actuator
 *    Actuator of the parallelization strategy.
 *
 *  @return
 *    Result from GRS parallelization strategy.
 */
template <typename T>
parallel::grs::Result<T> WeightOrthogonalized(
    const Mat<T> &nonortho_matrix,
    const Mat<T> &prelim_ortho_matrix,
    const vector<real_t> &weights,
    real_t zero_abs_thresh,
    Actuator<T> &actuator);

/**
 *  @brief Orthogonalization with weights using a default parallelization
 *  strategy.
 *
 *  Implementation uses @link parallel::grs::Actuator @endlink for its default
 *  parallelization strategy.
 *
 *  @param mpi_comm
 *    MPI communicator. The level of thread support must be at least
 *    <tt>MPI_THREAD_FUNNELED</tt>. Number of MPI processes at each host must
 *    be the same.
 *
 *  @param max_sweeps
 *    Positive maximum number of sweeps.
 */
template <typename T>
parallel::grs::Result<T> WeightOrthogonalized(
    MPI_Comm mpi_comm,
    const Mat<T> &nonortho_matrix,
    const Mat<T> &prelim_ortho_matrix,
    const vector<real_t> &weights,
    size_t max_sweeps = 100,
    real_t zero_abs_thresh = 1.0e-5);

} // namespace linear
} // namespace math
} // namespace tanuki

#include "tanuki/math/linear/weighted_orthogonalization.hxx"

#endif
