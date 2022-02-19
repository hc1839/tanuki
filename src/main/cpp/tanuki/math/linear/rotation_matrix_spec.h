#ifndef TANUKI_MATH_LINEAR_ROTATION_MATRIX_SPEC_H
#define TANUKI_MATH_LINEAR_ROTATION_MATRIX_SPEC_H

#include <cstddef>

#include <armadillo>

#include "tanuki/number/types.h"

namespace tanuki {
namespace math {
namespace linear {

using tanuki::number::real_t;

/**
 *  @brief Specification of a rotation matrix using the cosine and sine of an
 *  angle.
 */
struct RotationMatrixSpec final {
 public:
  /**
   *  @brief Cosine of the rotation angle.
   */
  real_t cosine;

  /**
   *  @brief Sine of the rotation angle.
   */
  real_t sine;
};

/**
 *  @brief Creates a specification for an identity rotation matrix.
 */
RotationMatrixSpec CreateIdentityRotation();

/**
 *  @brief Sets the four elements of a Givens rotation matrix from a
 *  specification.
 *
 *  Starting with an identity, it is useful in successively building a
 *  cumulative Givens rotation matrix from non-conflicting rotations.
 *
 *  @tparam T
 *    Type of matrix elements.
 *
 *  @param spec
 *    Specification of the rotation.
 *
 *  @param row1
 *    Index of one of the rows to rotate. If it is greater than or equal to the
 *    number of rows/columns in <tt>matrix</tt>, <tt>std::domain_error</tt> is
 *    thrown.
 *
 *  @param row2
 *    Index of the other row to rotate. If it is greater than or equal to the
 *    number of rows/columns in <tt>matrix</tt>, <tt>std::domain_error</tt> is
 *    thrown. If it is the same as <tt>row1</tt>,
 *    <tt>std::invalid_argument</tt> is thrown.
 *
 *  @param matrix
 *    Output matrix. If it is not a square matrix and does not have at least
 *    two rows/columns, <tt>std::length_error</tt> is thrown. Only the four
 *    elements at the intersection of <tt>row1</tt> and <tt>row2</tt> are set.
 */
template <typename T>
void SetGivensRotation(
    const RotationMatrixSpec &spec,
    size_t row1,
    size_t row2,
    arma::Mat<T> &matrix);

/**
 *  @brief Creates a Givens rotation matrix from a specification.
 *
 *  @param spec
 *    Specification of the rotation matrix.
 *
 *  @param size
 *    Size of the Givens rotation matrix. If it is not at least <tt>2</tt>,
 *    <tt>std::length_error</tt> is thrown.
 *
 *  @param row1
 *    Index of one of the rows to rotate. If it is greater than or equal to
 *    <tt>size</tt>, <tt>std::domain_error</tt> is thrown.
 *
 *  @param row2
 *    Index of the other row to rotate. If it is greater than or equal to
 *    <tt>size</tt>, <tt>std::domain_error</tt> is thrown. If it is the same as
 *    <tt>row1</tt>, <tt>std::invalid_argument</tt> is thrown.
 *
 *  @return
 *    <tt>size</tt>-by-<tt>size</tt> Givens rotation matrix.
 */
arma::Mat<real_t> CreateGivensRotation(
    const RotationMatrixSpec &spec,
    size_t size,
    size_t row1,
    size_t row2);

/**
 *  @brief Same as @link CreateGivensRotation @endlink but as a sparse matrix.
 */
arma::SpMat<real_t> CreateGivensRotationSp(
    const RotationMatrixSpec &spec,
    size_t size,
    size_t row1,
    size_t row2);

} // namespace linear
} // namespace math
} // namespace tanuki

#include "tanuki/math/linear/rotation_matrix_spec.hxx"

#endif
