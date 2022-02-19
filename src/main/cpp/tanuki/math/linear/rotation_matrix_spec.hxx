#ifndef TANUKI_MATH_LINEAR_ROTATION_MATRIX_SPEC_HXX
#define TANUKI_MATH_LINEAR_ROTATION_MATRIX_SPEC_HXX

#include <algorithm>
#include <stdexcept>

namespace tanuki {
namespace math {
namespace linear {

using arma::Mat;
using arma::SpMat;

template <typename T>
void SetGivensRotation(
    const RotationMatrixSpec &spec,
    size_t row1,
    size_t row2,
    Mat<T> &matrix) {
  if (!matrix.is_square() || matrix.n_cols < 2) {
    throw std::length_error(
        "Output matrix for the Givens rotation is not a square matrix "
        "or does not have at least two rows/columns.");
  }

  if (row2 == row1) {
    throw std::invalid_argument("Row indices are the same.");
  }

  auto lesser_row_index = std::min(row1, row2);
  auto greater_row_index = std::max(row1, row2);

  if (greater_row_index >= matrix.n_cols) {
    throw std::domain_error(
        "Indices are out-of-range with respect to the size of the matrix.");
  }

  // Set the diagonal elements.
  matrix(lesser_row_index, lesser_row_index) = spec.cosine;
  matrix(greater_row_index, greater_row_index) = spec.cosine;

  // Set the off-diagonal elements.
  matrix(lesser_row_index, greater_row_index) = -spec.sine;
  matrix(greater_row_index, lesser_row_index) = spec.sine;
}

} // namespace linear
} // namespace math
} // namespace tanuki

#endif
