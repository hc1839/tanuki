#ifndef TANUKI_MATH_LINEAR_EIGEN_H
#define TANUKI_MATH_LINEAR_EIGEN_H

#include <functional>

#include <armadillo>

#include "tanuki/number/types.h"

namespace tanuki {
namespace math {
namespace linear {

using std::function;

using arma::Col;
using arma::Mat;

using tanuki::number::complex_t;
using tanuki::number::real_t;

/**
 *  @brief Type of function that solves a generalized eigenvalue equation.
 *
 *  @tparam T
 *    Type of numbers in eigenvectors.
 *
 *  Generalized eigenvalue equation takes on the form, \f$ \mathbf{A}
 *  \mathbf{X} = \mathbf{B} \mathbf{X} \mathbf{D} \f$, where \f$ \mathbf{D} \f$
 *  is the diagonal matrix of real eigenvalues, \f$ \mathbf{X} \f$ is the
 *  square matrix with eigenvectors as columns, \f$ \mathbf{A} \f$ is
 *  Hermitian, and \f$ \mathbf{B} \f$ is Hermitian and positive definite. The
 *  arguments of the function are
 *    1. [out] eigenvalues as a real column vector whose elements are the
 *       diagonal elements of \f$ \mathbf{D} \f$,
 *    2. [out] eigenvectors as columns in \f$ \mathbf{X} \f$,
 *    3. [in] \f$ \mathbf{A} \f$, and
 *    4. [in] \f$ \mathbf{B} \f$.
 */
template <typename T>
using EigSolver = function<
    void(Col<real_t> &, Mat<T> &, const Mat<T> &, const Mat<T> &)>;

} // namespace linear
} // namespace math
} // namespace tanuki

#endif
