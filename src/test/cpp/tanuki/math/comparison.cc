#include <tanuki.h>

#include <algorithm>
#include <random>

#include <armadillo>
#include <gtest/gtest.h>

#define APPROX_EQUAL_RELDIFF 1.0e-3
#define APPROX_EQUAL_ABSDIFF 1.0e-3

namespace tanuki {
namespace math {

using arma::Mat;

using tanuki::algorithm::CompareRangeAllOf;
using tanuki::number::complex_t;
using tanuki::number::real_t;

/**
 *  @brief Tests the whether two real matrices have approximately equal
 *  corresponding elements.
 */
TEST(ApproxEqualRangeTest, RealMatrix) {
  std::random_device rd;
  std::mt19937 gen(rd());

  const Mat<real_t> input_mat(12, 8, arma::fill::randu);

  // Adding small numbers to the input matrix.
  {
    // Distribution of small numbers.
    std::uniform_real_distribution<> distrib(-1.0e-12, 1.0e-12);

    Mat<real_t> output_mat(arma::size(input_mat));
    std::transform(
        input_mat.begin(), input_mat.end(), output_mat.begin(),
        [&gen, &distrib](const real_t &item) -> real_t {
          return item + distrib(gen);
        });

    // Relative tolerance.
    const bool approx_equal_rel = CompareRangeAllOf(
        input_mat.begin(), input_mat.end(),
        output_mat.begin(), output_mat.end(),
        [](const real_t &a, const real_t &b) -> bool {
          return ApproxEqualRel(a, b, APPROX_EQUAL_RELDIFF);
        });

    ASSERT_TRUE(approx_equal_rel);

    // Absolute tolerance.
    const bool approx_equal_abs = CompareRangeAllOf(
        input_mat.begin(), input_mat.end(),
        output_mat.begin(), output_mat.end(),
        [](const real_t &a, const real_t &b) -> bool {
          return ApproxEqualAbs(a, b, APPROX_EQUAL_ABSDIFF);
        });

    ASSERT_TRUE(approx_equal_abs);
  }

  // Adding not-so-small numbers to the input matrix.
  {
    // Distribution of not-so-small numbers.
    std::uniform_real_distribution<> distrib(-1.0e-2, 1.0e-2);

    Mat<real_t> output_mat(arma::size(input_mat));
    std::transform(
        input_mat.begin(), input_mat.end(), output_mat.begin(),
        [&gen, &distrib](const real_t &item) -> real_t {
          return item + distrib(gen);
        });

    // Relative tolerance.
    const bool approx_equal_rel = CompareRangeAllOf(
        input_mat.begin(), input_mat.end(),
        output_mat.begin(), output_mat.end(),
        [](const real_t &a, const real_t &b) -> bool {
          return ApproxEqualRel(a, b, APPROX_EQUAL_RELDIFF);
        });

    ASSERT_FALSE(approx_equal_rel);

    // Absolute tolerance.
    const bool approx_equal_abs = CompareRangeAllOf(
        input_mat.begin(), input_mat.end(),
        output_mat.begin(), output_mat.end(),
        [](const real_t &a, const real_t &b) -> bool {
          return ApproxEqualAbs(a, b, APPROX_EQUAL_ABSDIFF);
        });

    ASSERT_FALSE(approx_equal_abs);
  }
}

/**
 *  @brief Tests the whether two complex matrices have approximately equal
 *  corresponding elements.
 */
TEST(ApproxEqualRangeTest, ComplexMatrix) {
  std::random_device rd;
  std::mt19937 gen(rd());

  const Mat<complex_t> input_mat(12, 8, arma::fill::randu);

  // Adding small numbers to the input matrix.
  {
    // Distribution of small numbers.
    std::uniform_real_distribution<> distrib(-1.0e-12, 1.0e-12);

    Mat<complex_t> output_mat(arma::size(input_mat));
    std::transform(
        input_mat.begin(), input_mat.end(), output_mat.begin(),
        [&gen, &distrib](const complex_t &item) -> complex_t {
          return item + complex_t(distrib(gen), distrib(gen));
        });

    // Relative tolerance.
    const bool approx_equal_rel = CompareRangeAllOf(
        input_mat.begin(), input_mat.end(),
        output_mat.begin(), output_mat.end(),
        [](const complex_t &a, const complex_t &b) -> bool {
          return ApproxEqualRel(a, b, APPROX_EQUAL_RELDIFF);
        });

    ASSERT_TRUE(approx_equal_rel);

    // Absolute tolerance.
    const bool approx_equal_abs = CompareRangeAllOf(
        input_mat.begin(), input_mat.end(),
        output_mat.begin(), output_mat.end(),
        [](const complex_t &a, const complex_t &b) -> bool {
          return ApproxEqualAbs(a, b, APPROX_EQUAL_ABSDIFF);
        });

    ASSERT_TRUE(approx_equal_abs);
  }

  // Adding not-so-small numbers to the input matrix.
  {
    // Distribution of not-so-small numbers.
    std::uniform_real_distribution<> distrib(-1.0e-2, 1.0e-2);

    Mat<complex_t> output_mat(arma::size(input_mat));
    std::transform(
        input_mat.begin(), input_mat.end(), output_mat.begin(),
        [&gen, &distrib](const complex_t &item) -> complex_t {
          return item + complex_t(distrib(gen), distrib(gen));
        });

    // Relative tolerance.
    const bool approx_equal_rel = CompareRangeAllOf(
        input_mat.begin(), input_mat.end(),
        output_mat.begin(), output_mat.end(),
        [](const complex_t &a, const complex_t &b) -> bool {
          return ApproxEqualRel(a, b, APPROX_EQUAL_RELDIFF);
        });

    ASSERT_FALSE(approx_equal_rel);

    // Absolute tolerance.
    const bool approx_equal_abs = CompareRangeAllOf(
        input_mat.begin(), input_mat.end(),
        output_mat.begin(), output_mat.end(),
        [](const complex_t &a, const complex_t &b) -> bool {
          return ApproxEqualAbs(a, b, APPROX_EQUAL_ABSDIFF);
        });

    ASSERT_FALSE(approx_equal_abs);
  }
}

} // namespace math
} // namespace tanuki
