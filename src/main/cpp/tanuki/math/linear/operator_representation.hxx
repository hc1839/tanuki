#ifndef TANUKI_MATH_LINEAR_OPERATOR_REPRESENTATION_HXX
#define TANUKI_MATH_LINEAR_OPERATOR_REPRESENTATION_HXX

#include "tanuki/math/linear/matrix_product.h"
#include "tanuki/math/linear/triangular_matrix.h"

namespace tanuki {
namespace math {
namespace linear {

template <typename T>
Mat<T> CreateOperatorBraKetMatrix(
    const Mat<T> &op_mat_rep,
    const Mat<T> &basis,
    bool is_hermitian,
    MPI_Comm comm) {
  const Mat<T> basis_t(basis.t());

  const auto y = ForwardSubstitute(basis_t, op_mat_rep, comm);
  const auto op_t = ForwardSubstitute(basis_t, Mat<T>(y.t()), comm);

  return is_hermitian ? op_t : Mat<T>(op_t.t());
}

template <typename T>
Mat<T> CreateOperatorMatrixRep(
    const Mat<T> &op_braketmat,
    const Mat<T> &basis,
    MPI_Comm comm) {
  return MatrixProduct(comm, Mat<T>(basis.t()), op_braketmat, basis);
}

} // namespace linear
} // namespace math
} // namespace tanuki

#endif
