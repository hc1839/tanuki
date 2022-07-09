#ifndef TANUKI_MATH_LINEAR_OPERATOR_REPRESENTATION_H
#define TANUKI_MATH_LINEAR_OPERATOR_REPRESENTATION_H

#include <armadillo>
#include <mpi.h>

namespace tanuki {
namespace math {
namespace linear {

using arma::Mat;

/**
 *  @brief Creates an operator in bra-ket matrix using the matrix
 *  representation of the operator in the specified basis functions.
 *
 *  @tparam T
 *    Type of elements in an Armadillo matrix. It must be @link
 *    tanuki::number::real_t @endlink or @link tanuki::number::complex_t
 *    @endlink.
 *
 *  @param op_mat_rep
 *    Matrix representation of the operator in <tt>basis</tt>.
 *
 *  @param basis
 *    Basis functions as a ket matrix. It is the upper triangular matrix from
 *    the Cholesky decomposition of the basis overlap matrix.
 *
 *  @param is_hermitian
 *    Whether the operator is Hermitian. If <tt>false</tt>, an additional
 *    conjugate transposition is performed so that the valid form of the
 *    operator can be returned. Such additional conjugate transposition is
 *    unnecessary for a Hermitian operator.
 *
 *  @param comm
 *    MPI communicator.
 *
 *  @return
 *    Operator in bra-ket matrix. It is \f$ \hat{\mathcal{O}} \f$ in \f$
 *    \boldsymbol{\Phi}^{\dagger} \hat{\mathcal{O}} \boldsymbol{\Phi} =
 *    \mathbf{O} \f$, where \f$ \boldsymbol{\Phi} \f$ is <tt>basis</tt>, and
 *    \f$ \mathbf{O} \f$ is <tt>op_mat_rep</tt>.
 */
template <typename T>
Mat<T> CreateOperatorBraKetMatrix(
    const Mat<T> &op_mat_rep,
    const Mat<T> &basis,
    bool is_hermitian,
    MPI_Comm comm = MPI_COMM_WORLD);

/**
 *  @brief Creates the matrix representation of an operator in the specified
 *  basis functions using the bra-ket matrix of the operator.
 *
 *  @tparam T
 *    Type of elements in an Armadillo matrix. It must be @link
 *    tanuki::number::real_t @endlink or @link tanuki::number::complex_t
 *    @endlink.
 *
 *  @param op_braketmat
 *    Operator in bra-ket matrix.
 *
 *  @param basis
 *    Basis functions as a ket matrix. It is the upper triangular matrix from
 *    the Cholesky decomposition of the basis overlap matrix.
 *
 *  @param comm
 *    MPI communicator.
 *
 *  @return
 *    Matrix representation of the operator. It is \f$
 *    \boldsymbol{\Phi}^{\dagger} \hat{\mathcal{O}} \boldsymbol{\Phi} \f$,
 *    where \f$ \boldsymbol{\Phi} \f$ is <tt>basis</tt>, and \f$
 *    \hat{\mathcal{O}} \f$ is <tt>op_braketmat</tt>.
 */
template <typename T>
Mat<T> CreateOperatorMatrixRep(
    const Mat<T> &op_braketmat,
    const Mat<T> &basis,
    MPI_Comm comm = MPI_COMM_WORLD);

} // namespace linear
} // namespace math
} // namespace tanuki

#include "tanuki/math/linear/operator_representation.hxx"

#endif
