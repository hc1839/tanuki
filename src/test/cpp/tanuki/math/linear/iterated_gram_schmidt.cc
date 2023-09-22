#include <tanuki.h>

#include <cstddef>

#include <armadillo>
#include <gtest/gtest.h>
#include <mpi.h>

#define APPROX_EQUAL_ABS_TOL 1.0e-6
#define APPROX_EQUAL_REL_TOL 1.0e-3

namespace tanuki {
namespace math {
namespace linear {

using arma::Mat;

using tanuki::number::complex_t;
using tanuki::number::real_t;

/**
 *  @brief Tests iterated Gram-Schmidt process as a method of QR decomposition.
 *
 *  @tparam T
 *    Must be @link tanuki::number::real_t @endlink or @link
 *    tanuki::number::complex_t @endlink.
 */
template <typename T>
void TEST_IteratedGramSchmidt_Qr(size_t num_rows, size_t num_cols) {
  Mat<T> a(num_rows, num_cols, arma::fill::randu);
  MPI_Bcast(a.memptr(), a.n_elem, MpiBasicDatatype<T>(), 0, MPI_COMM_WORLD);

  const auto qr = IteratedGramSchmidt(MPI_COMM_WORLD, a);

  ASSERT_TRUE(qr.q.is_square());

  // Test that Q is orthonormal.
  {
    const bool is_q_ortho = arma::approx_equal(
        qr.q.t() * qr.q,
        Mat<T>(arma::size(qr.q), arma::fill::eye),
        "absdiff",
        APPROX_EQUAL_ABS_TOL);

    ASSERT_TRUE(is_q_ortho);
  }

  // Test that the product of decomposition factors gives the original matrix.
  {
    const bool is_decompose_equal = arma::approx_equal(
        qr.q * qr.r,
        a,
        "reldiff",
        APPROX_EQUAL_REL_TOL);

    ASSERT_TRUE(is_decompose_equal);
  }
}

/**
 *  @brief Tests iterated Gram-Schmidt process as a method of QR decomposition.
 */
TEST(IteratedGramSchmidt, Qr) {
  // Test square matrices.
  TEST_IteratedGramSchmidt_Qr<real_t>(8, 8);
  TEST_IteratedGramSchmidt_Qr<complex_t>(8, 8);

  // Test rectangular matrices.
  TEST_IteratedGramSchmidt_Qr<real_t>(8, 5);
  TEST_IteratedGramSchmidt_Qr<complex_t>(8, 5);
}

} // namespace linear
} // namespace math
} // namespace tanuki
