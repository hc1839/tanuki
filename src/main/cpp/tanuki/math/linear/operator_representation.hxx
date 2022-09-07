#ifndef TANUKI_MATH_LINEAR_OPERATOR_REPRESENTATION_HXX
#define TANUKI_MATH_LINEAR_OPERATOR_REPRESENTATION_HXX

#include "tanuki/math/linear/matrix_product.h"
#include "tanuki/math/linear/triangular_matrix.h"

namespace tanuki {
namespace math {
namespace linear {

template <typename T>
Mat<T> CreateOperatorBraKetMatrix(
    MPI_Comm mpi_comm,
    const Mat<T> &op_mat_rep,
    const Mat<T> &basis,
    bool is_hermitian) {
  const Mat<T> basis_t(basis.t());

  const auto y = ForwardSubstitute(mpi_comm, basis_t, op_mat_rep);
  const auto op_t = ForwardSubstitute(mpi_comm, basis_t, Mat<T>(y.t()));

  return is_hermitian ? op_t : Mat<T>(op_t.t());
}

} // namespace linear
} // namespace math
} // namespace tanuki

#endif
