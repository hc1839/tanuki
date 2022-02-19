#ifndef TANUKI_MATH_LINEAR_QR_DECOMPOSITION_H
#define TANUKI_MATH_LINEAR_QR_DECOMPOSITION_H

#include <cstddef>

#include <armadillo>

namespace tanuki {
namespace math {
namespace linear {

/**
 *  @brief QR decomposition of a matrix.
 *
 *  @tparam T
 *    Type of elements in an Armadillo matrix.
 */
template <typename T>
struct QrDecomposition final {
 public:
  /**
   *  @brief Square matrix, Q, of the decomposition.
   */
  arma::Mat<T> q;

  /**
   *  @brief Upper triangular matrix, R, of the decomposition with zero rows at
   *  the bottom for any padding.
   */
  arma::Mat<T> r;
};

} // namespace linear
} // namespace math
} // namespace tanuki

#endif
