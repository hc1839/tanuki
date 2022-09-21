#ifndef TANUKI_MATH_LINEAR_EQUATION_SYSTEM_H
#define TANUKI_MATH_LINEAR_EQUATION_SYSTEM_H

#include <armadillo>
#include <mpi.h>

#include "tanuki/math/linear/iterated_gram_schmidt.h"
#include "tanuki/math/linear/matrix_product.h"
#include "tanuki/math/linear/triangular_matrix.h"

namespace tanuki {
namespace math {
namespace linear {

using arma::Mat;

/**
 *  @brief Solves a system of linear equations, \f$ \mathbf{A} \mathbf{x} =
 *  \mathbf{b} \f$.
 *
 *  Current implementation employs QR decomposition using @link
 *  IteratedGramSchmidt @endlink with default values.
 *
 *  @tparam T
 *    Type of matrix elements.
 *
 *  @param mpi_comm
 *    MPI communicator.
 *
 *  @param coeffs
 *    Matrix of coefficients, \f$ \mathbf{A} \f$. It must be a square matrix.
 *
 *  @param constants
 *    Constants, \f$ \mathbf{b} \f$, containing one or many columns. Number of
 *    rows must be the same as the number of rows/columns in <tt>coeffs</tt>.
 *
 *  @return
 *    Solution, \f$ \mathbf{x} \f$.
 */
template <typename T>
Mat<T> EquationSystemSolution(
    MPI_Comm mpi_comm, const Mat<T> &coeffs, const Mat<T> &constants) {
  const auto qr = IteratedGramSchmidt(mpi_comm, coeffs);
  const auto rx = MatrixProduct(mpi_comm, Mat<T>(qr.q.t()), constants);

  return BackSubstitute(mpi_comm, qr.r, rx);
}

} // namespace linear
} // namespace math
} // namespace tanuki

#endif
