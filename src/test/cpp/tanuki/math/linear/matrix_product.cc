#include <tanuki.h>

#include <cstddef>

#include <armadillo>
#include <gtest/gtest.h>
#include <mpi.h>

#define APPROX_EQUAL_REL_TOL 1.0e-3

namespace tanuki {
namespace math {
namespace linear {

using arma::Mat;

using tanuki::number::complex_t;
using tanuki::number::real_t;

/**
 *  @brief Tests calculating a matrix product.
 *
 *  @tparam T
 *    Must be @link tanuki::number::real_t @endlink or @link
 *    tanuki::number::complex_t @endlink.
 */
template <typename T>
void TEST_MatrixProduct_Calculate(size_t mat_size) {
  Mat<T> a(mat_size, mat_size, arma::fill::randu);
  MPI_Bcast(a.memptr(), a.n_elem, MpiBasicDatatype<T>(), 0, MPI_COMM_WORLD);

  Mat<T> b(mat_size, mat_size, arma::fill::randu);
  MPI_Bcast(b.memptr(), b.n_elem, MpiBasicDatatype<T>(), 0, MPI_COMM_WORLD);

  Mat<T> c(mat_size, mat_size, arma::fill::randu);
  MPI_Bcast(c.memptr(), c.n_elem, MpiBasicDatatype<T>(), 0, MPI_COMM_WORLD);

  const bool is_equal = arma::approx_equal(
      MatrixProduct(MPI_COMM_WORLD, a, b, c),
      a * b * c,
      "reldiff",
      APPROX_EQUAL_REL_TOL);

  ASSERT_TRUE(is_equal);
}

/**
 *  @brief Tests calculating a matrix product.
 */
TEST(MatrixProduct, Calculate) {
  TEST_MatrixProduct_Calculate<real_t>(8);
  TEST_MatrixProduct_Calculate<complex_t>(8);
}

} // namespace linear
} // namespace math
} // namespace tanuki
