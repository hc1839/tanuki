#ifndef TANUKI_MATH_LINEAR_TRIANGULAR_MATRIX_H
#define TANUKI_MATH_LINEAR_TRIANGULAR_MATRIX_H

#include <armadillo>
#include <mpi.h>

namespace tanuki {
namespace math {
namespace linear {

using arma::Mat;

/**
 *  @brief Solves a system of linear equations, \f$ \mathbf{L} \mathbf{x} =
 *  \mathbf{b} \f$, using forward substitution.
 *
 *  @tparam T
 *    Type of matrix elements.
 *
 *  @param mpi_comm
 *    MPI communicator.
 *
 *  @param lower_coeffs
 *    Lower triangular matrix of coefficients, \f$ \mathbf{L} \f$. It must be a
 *    square matrix. Elements above the diagonal are assumed to be zero. If
 *    they are not, the computed solution will be invalid.
 *
 *  @param constants
 *    Constants, \f$ \mathbf{b} \f$, containing one or many columns. Number of
 *    rows must be the same as the number of rows/columns in
 *    <tt>lower_coeffs</tt>.
 *
 *  @return
 *    Solution, \f$ \mathbf{x} \f$.
 */
template <typename T>
Mat<T> ForwardSubstitute(
    MPI_Comm mpi_comm,
    const Mat<T> &lower_coeffs,
    const Mat<T> &constants);

/**
 *  @brief Solves a system of linear equations, \f$ \mathbf{U} \mathbf{x} =
 *  \mathbf{b} \f$, using back substitution.
 *
 *  @tparam T
 *    Type of matrix elements.
 *
 *  @param mpi_comm
 *    MPI communicator.
 *
 *  @param upper_coeffs
 *    Upper triangular matrix of coefficients, \f$ \mathbf{U} \f$. It must be a
 *    square matrix. Elements below the diagonal are assumed to be zero. If
 *    they are not, the computed solution will be invalid.
 *
 *  @param constants
 *    Constants, \f$ \mathbf{b} \f$, containing one or many columns. Number of
 *    rows must be the same as the number of rows/columns in
 *    <tt>upper_coeffs</tt>.
 *
 *  @return
 *      Solution, \f$ \mathbf{x} \f$.
 */
template <typename T>
Mat<T> BackSubstitute(
    MPI_Comm mpi_comm,
    const Mat<T> &upper_coeffs,
    const Mat<T> &constants);

} // namespace linear
} // namespace math
} // namespace tanuki

#include "tanuki/math/linear/triangular_matrix.hxx"

#endif
