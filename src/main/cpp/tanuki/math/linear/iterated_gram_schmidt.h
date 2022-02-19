#ifndef TANUKI_MATH_LINEAR_ITERATED_GRAM_SCHMIDT_H
#define TANUKI_MATH_LINEAR_ITERATED_GRAM_SCHMIDT_H

#include <cstddef>

#include <armadillo>
#include <mpi.h>

#include "tanuki/math/linear/qr_decomposition.h"
#include "tanuki/number/types.h"

namespace tanuki {
namespace math {
namespace linear {

using tanuki::number::real_t;

/**
 *  @brief Numerically stable Gram-Schmidt process with reorthogonalization.
 *
 *  The variant implemented is iterated classical Gram-Schmidt process (Bjorck
 *  1994).
 *
 *  @tparam T
 *    Type of elements in an Armadillo matrix. It must be @link
 *    tanuki::number::real_t @endlink or @link tanuki::number::complex_t
 *    @endlink.
 *
 *  @param mpi_comm
 *    MPI communicator.
 *
 *  @param matrix
 *    Matrix to decompose. Number of rows must be at least the number of
 *    columns.
 *
 *  @param reortho_thresh_factor
 *    A fixed value in the range \f$ \left( 1.2 \epsilon, 0.83 - \epsilon
 *    \right) \f$ for determining whether to reorthogonalize a column. If \f$
 *    \left| \hat{q}^{(p + 1)} \right| < \alpha \left| \hat{q}^{p} \right| \f$
 *    for the \f$ p \f$-th orthogonalization, \f$ \hat{q}^{(p + 1)} \f$ is
 *    reorthogonalized. Reorthogonalization of \f$ \hat{q} \f$ continues until
 *    the threshold is satisfied or <tt>max_reorthos</tt> is encountered,
 *    whichever comes first. A recommended value is <tt>0.5</tt> for accurate
 *    orthogonality.
 *
 *  @param max_reorthos
 *    Maximum number of reorthogonalizations of a column. One
 *    reorthogonalization is usually sufficient. However, if the matrix is
 *    numerically rank-deficient, more reorthogonalizations would be needed. If
 *    <tt>0</tt>, orthogonality is potentially lost due to numerical errors.
 *
 *  @param zero_norm_abs_thresh
 *    Absolute threshold for determining whether a vector is zero by its norm.
 *    It must be positive.
 *
 *  @return
 *    QR decomposition of <tt>matrix</tt>.
 */
template <typename T>
QrDecomposition<T> IteratedGramSchmidt(
    MPI_Comm mpi_comm,
    const arma::Mat<T> &matrix,
    real_t reortho_thresh_factor = 0.5,
    size_t max_reorthos = 1,
    real_t zero_norm_abs_thresh = 1.0e-5);

} // namespace linear
} // namespace math
} // namespace tanuki

#include "tanuki/math/linear/iterated_gram_schmidt.hxx"

#endif
