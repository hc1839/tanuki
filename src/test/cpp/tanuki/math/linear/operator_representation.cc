#include <tanuki.h>

#include <complex>

#include <armadillo>
#include <gtest/gtest.h>
#include <mpi.h>

#define APPROX_EQUAL_REL_TOL 1.0e-3

namespace tanuki {
namespace math {
namespace linear {

using std::complex;

using arma::Mat;

using tanuki::number::complex_t;
using tanuki::number::real_t;
using tanuki::parallel::mpi::MpiBasicDatatype;

/**
 *  @brief Tests creating a non-Hermitian operator in bra-ket matrix.
 *
 *  @tparam T
 *    Must be @link tanuki::number::real_t @endlink or @link
 *    tanuki::number::complex_t @endlink.
 */
template <typename T>
void TEST_OperatorRepresentation_BraKet(size_t mat_size) {
  Mat<T> basis_ketmat;

  {
    Mat<T> basis_vecs(mat_size, mat_size, arma::fill::randu);

    MPI_Bcast(
        basis_vecs.memptr(),
        basis_vecs.n_elem,
        MpiBasicDatatype<T>(),
        0,
        MPI_COMM_WORLD);

    const Mat<T> basis_overlap(basis_vecs.t() * basis_vecs);
    basis_ketmat = CholeskyDecomposition<T>(basis_overlap).lt();
  }

  const Mat<T> input_op_mat_rep(mat_size, mat_size, arma::fill::randu);

  const auto op_braketmat = CreateOperatorBraKetMatrix(
      MPI_COMM_WORLD, input_op_mat_rep, basis_ketmat, false);

  const auto output_op_mat_rep = MatrixProduct(
      MPI_COMM_WORLD, Mat<T>(basis_ketmat.t()), op_braketmat, basis_ketmat);

  const bool is_equal = arma::approx_equal(
      input_op_mat_rep,
      output_op_mat_rep,
      "reldiff",
      APPROX_EQUAL_REL_TOL);

  ASSERT_TRUE(is_equal);
}

/**
 *  @brief Tests creating a non-Hermitian operator in bra-ket matrix.
 */
TEST(OperatorRepresentation, BraKet) {
  TEST_OperatorRepresentation_BraKet<real_t>(8);
  TEST_OperatorRepresentation_BraKet<complex_t>(8);
}

} // namespace linear
} // namespace math
} // namespace tanuki
