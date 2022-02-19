#include "tanuki/math/linear/rotation_matrix_spec.h"

#include <algorithm>
#include <stdexcept>

namespace tanuki {
namespace math {
namespace linear {

using arma::Mat;
using arma::SpMat;

RotationMatrixSpec CreateIdentityRotation() {
  return {
    .cosine = 1.0,
    .sine = 0.0
  };
}

Mat<real_t> CreateGivensRotation(
    const RotationMatrixSpec &spec,
    size_t size,
    size_t row1,
    size_t row2) {
  Mat<real_t> retval(size, size, arma::fill::eye);
  SetGivensRotation(spec, row1, row2, retval);
  return retval;
}

SpMat<real_t> CreateGivensRotationSp(
    const RotationMatrixSpec &spec,
    size_t size,
    size_t row1,
    size_t row2) {
  if (size < 2) {
    throw std::length_error(
        "Size of Givens rotation matrix to create is not at least 2.");
  }

  if (row2 == row1) {
    throw std::invalid_argument("Row indices are the same.");
  }

  auto lesser_row_index = std::min(row1, row2);
  auto greater_row_index = std::max(row1, row2);

  if (greater_row_index >= size) {
    throw std::domain_error(
        "Indices are out-of-range with respect to the size of the matrix.");
  }

  SpMat<real_t> retval(size, size);
  retval.eye();

  // Set the diagonal elements.
  retval(lesser_row_index, lesser_row_index) = spec.cosine;
  retval(greater_row_index, greater_row_index) = spec.cosine;

  // Set the off-diagonal elements.
  retval(lesser_row_index, greater_row_index) = -spec.sine;
  retval(greater_row_index, lesser_row_index) = spec.sine;

  return retval;
}

} // namespace linear
} // namespace math
} // namespace tanuki
