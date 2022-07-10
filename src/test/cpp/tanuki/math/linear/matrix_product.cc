#include <tanuki.h>

#include <algorithm>
#include <cstddef>
#include <random>
#include <utility>

#include <armadillo>
#include <gtest/gtest.h>
#include <mpi.h>

#define APPROX_EQUAL_REL_TOL 1.0e-3

namespace tanuki {
namespace math {
namespace linear {

using arma::Col;
using arma::Mat;

using tanuki::number::complex_t;
using tanuki::number::real_t;
using tanuki::parallel::mpi::MpiBasicDatatype;

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

/**
 *  @brief Tests creating a ket matrix of orbitals multiplied by a real
 *  diagonal matrix of weights using @link DuoProduct @endlink.
 *
 *  @tparam T
 *    Must be @link tanuki::number::real_t @endlink or @link
 *    tanuki::number::complex_t @endlink.
 */
template <typename T>
void TEST_DuoProduct_WeightedOrbs(size_t num_orbs) {
  Mat<T> orbs;

  {
    Mat<T> orbs_buf(num_orbs, num_orbs, arma::fill::randu);

    MPI_Bcast(
        orbs_buf.memptr(),
        orbs_buf.n_elem,
        MpiBasicDatatype<T>(),
        0,
        MPI_COMM_WORLD);

    const Mat<T> orb_overlap(orbs_buf.t() * orbs_buf);
    orbs = CholeskyDecomposition<T>(orb_overlap).lt();
  }

  Col<real_t> weights;

  {
    Col<real_t> weights_buf(num_orbs, arma::fill::randu);

    MPI_Bcast(
        weights_buf.memptr(),
        weights_buf.size(),
        MpiBasicDatatype<real_t>(),
        0,
        MPI_COMM_WORLD);

    weights = std::move(weights_buf);
  }

  const auto weighted_orbs = DuoProduct(MPI_COMM_WORLD, orbs, weights.begin());

  // Diagonal matrix of weights.
  Mat<T> weight_diagmat(weights.size(), weights.size(), arma::fill::zeros);

  for (size_t j = 0; j != weights.size(); ++j) {
    weight_diagmat(j, j) = weights[j];
  }

  const bool is_equal = arma::approx_equal(
      weighted_orbs,
      orbs * weight_diagmat,
      "reldiff",
      APPROX_EQUAL_REL_TOL);

  ASSERT_TRUE(is_equal);
}

/**
 *  @brief Tests creating a ket matrix of orbitals multiplied by a real
 *  diagonal matrix of weights using @link DuoProduct @endlink.
 */
TEST(DuoProduct, WeightedOrbs) {
  TEST_DuoProduct_WeightedOrbs<real_t>(8);
  TEST_DuoProduct_WeightedOrbs<complex_t>(8);
}

/**
 *  @brief Tests creating a first-order density operator in bra-ket matrix
 *  using @link TrioProduct @endlink.
 *
 *  @tparam T
 *    Must be @link tanuki::number::real_t @endlink or @link
 *    tanuki::number::complex_t @endlink.
 */
template <typename T>
void TEST_TrioProduct_DensityOperator(size_t num_orbs) {
  Mat<T> mos;

  {
    Mat<T> mos_buf(num_orbs, num_orbs, arma::fill::randu);

    MPI_Bcast(
        mos_buf.memptr(),
        mos_buf.n_elem,
        MpiBasicDatatype<T>(),
        0,
        MPI_COMM_WORLD);

    const Mat<T> mo_overlap(mos_buf.t() * mos_buf);
    mos = CholeskyDecomposition<T>(mo_overlap).lt();
  }

  Col<real_t> occs(num_orbs);

  {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> occ_distrib(0.0, 1.0);

    std::generate(
        occs.begin(), occs.end(),
        [&gen, &occ_distrib]() -> real_t { return occ_distrib(gen); });

    MPI_Bcast(
        occs.memptr(),
        occs.size(),
        MpiBasicDatatype<real_t>(),
        0,
        MPI_COMM_WORLD);
  }

  const auto density_operator = TrioProduct(MPI_COMM_WORLD, mos, occs.begin());

  // Diagonal matrix of occupation numbers.
  Mat<T> occ_diagmat(occs.size(), occs.size(), arma::fill::zeros);

  for (size_t j = 0; j != occs.size(); ++j) {
    occ_diagmat(j, j) = occs[j];
  }

  const bool is_equal = arma::approx_equal(
      density_operator,
      mos * occ_diagmat * mos.t(),
      "reldiff",
      APPROX_EQUAL_REL_TOL);

  ASSERT_TRUE(is_equal);
}

/**
 *  @brief Tests creating a first-order density operator in bra-ket matrix
 *  using @link TrioProduct @endlink.
 */
TEST(TrioProduct, DensityOperator) {
  TEST_TrioProduct_DensityOperator<real_t>(8);
  TEST_TrioProduct_DensityOperator<complex_t>(8);
}

} // namespace linear
} // namespace math
} // namespace tanuki
