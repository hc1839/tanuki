#include <tanuki.h>

#include <algorithm>
#include <array>
#include <complex>
#include <vector>

#include <armadillo>
#include <avro/Compiler.hh>
#include <avro/Decoder.hh>
#include <avro/Encoder.hh>
#include <avro/Specific.hh>
#include <boost/multi_array.hpp>
#include <gtest/gtest.h>

#define ARMA_EQUAL_RELDIFF 1.0e-3

using std::array;
using std::complex;
using std::vector;

using arma::Cube;
using arma::Mat;
using boost::fortran_storage_order;
using boost::multi_array;

using tanuki::math::linear::ArmaCubeToNumberArray;
using tanuki::math::linear::ArmaMatToNumberArray;
using tanuki::math::linear::MultiToNumberArray;
using tanuki::math::linear::NumberArray;
using tanuki::math::linear::NumberArrayToArmaCube;
using tanuki::math::linear::NumberArrayToArmaMat;
using tanuki::math::linear::NumberToMultiArray;
using tanuki::number::complex_t;
using tanuki::number::real_t;

/**
 *  @brief Tests the conversion between real Boost multi-array and Apache Avro
 *  with a schema.
 */
TEST(NumberArrayAvroTest, RealNumberArrayAvro) {
  const auto &schema = NumberArray::AvroSchema();

  const vector<Mat<real_t>> input_mats = {
    Mat<real_t>(5, 8, arma::fill::randu),
    Mat<real_t>(8, 5, arma::fill::randu)
  };

  for (const auto &input_mat : input_mats) {
    const array<size_t, 3> box_extents = {
      input_mat.n_rows,
      input_mat.n_cols
    };

    // Copy Armadillo matrix to Boost multi-array.
    multi_array<real_t, 2> input_marr(box_extents, fortran_storage_order());
    std::copy(input_mat.begin(), input_mat.end(), input_marr.data());

    auto mem_out = avro::memoryOutputStream();
    auto encoder = avro::validatingEncoder(schema, avro::binaryEncoder());
    encoder->init(*mem_out);

    // Convert Boost multi-array to Avro.
    avro::encode(*encoder, MultiToNumberArray(input_marr));

    // Test real-to-real.
    {
      auto mem_in = avro::memoryInputStream(*mem_out);
      auto decoder = avro::validatingDecoder(schema, avro::binaryDecoder());
      decoder->init(*mem_in);

      // Convert Avro to real Boost multi-array.
      NumberArray output_narr;
      avro::decode(*decoder, output_narr);
      auto output_marr = NumberToMultiArray<real_t, 2>(output_narr);

      const Mat<real_t> output_mat(
          output_marr.data(),
          output_marr.shape()[0], output_marr.shape()[1],
          false, true);

      const bool is_equal = arma::approx_equal(
          input_mat,
          output_mat,
          "reldiff",
          ARMA_EQUAL_RELDIFF);

      ASSERT_TRUE(is_equal);
    }

    // Test real-to-complex.
    {
      // Convert input Armadillo matrix to complex.
      Mat<complex_t> complex_input_mat(arma::size(input_mat));
      std::copy(input_mat.begin(), input_mat.end(), complex_input_mat.begin());

      auto mem_in = avro::memoryInputStream(*mem_out);
      auto decoder = avro::validatingDecoder(schema, avro::binaryDecoder());
      decoder->init(*mem_in);

      // Convert Avro to complex Boost multi-array.
      NumberArray output_narr;
      avro::decode(*decoder, output_narr);
      auto output_marr = NumberToMultiArray<complex_t, 2>(output_narr);

      const Mat<complex_t> output_mat(
          output_marr.data(),
          output_marr.shape()[0], output_marr.shape()[1],
          false, true);

      const bool is_equal = arma::approx_equal(
          complex_input_mat,
          output_mat,
          "reldiff",
          ARMA_EQUAL_RELDIFF);

      ASSERT_TRUE(is_equal);
    }
  }
}

/**
 *  @brief Tests the conversion between complex Boost multi-array and Apache
 *  Avro with a schema.
 */
TEST(NumberArrayAvroTest, ComplexNumberArrayAvro) {
  const auto &schema = NumberArray::AvroSchema();

  const vector<Mat<complex_t>> input_mats = {
    Mat<complex_t>(5, 8, arma::fill::randu),
    Mat<complex_t>(8, 5, arma::fill::randu)
  };

  for (const auto &input_mat : input_mats) {
    const array<size_t, 3> box_extents = {
      input_mat.n_rows,
      input_mat.n_cols
    };

    // Copy Armadillo matrix to Boost multi-array.
    multi_array<complex_t, 2> input_marr(box_extents, fortran_storage_order());
    std::copy(input_mat.begin(), input_mat.end(), input_marr.data());

    auto mem_out = avro::memoryOutputStream();
    auto encoder = avro::validatingEncoder(schema, avro::binaryEncoder());
    encoder->init(*mem_out);

    // Convert Boost multi-array to Avro.
    avro::encode(*encoder, MultiToNumberArray(input_marr));

    auto mem_in = avro::memoryInputStream(*mem_out);
    auto decoder = avro::validatingDecoder(schema, avro::binaryDecoder());
    decoder->init(*mem_in);

    // Convert Avro to complex Boost multi-array.
    NumberArray output_narr;
    avro::decode(*decoder, output_narr);
    auto output_marr = NumberToMultiArray<complex_t, 2>(output_narr);

    const Mat<complex_t> output_mat(
        output_marr.data(),
        output_marr.shape()[0], output_marr.shape()[1],
        false, true);

    const bool is_equal = arma::approx_equal(
        input_mat,
        output_mat,
        "reldiff",
        ARMA_EQUAL_RELDIFF);

    ASSERT_TRUE(is_equal);
  }
}

/**
 *  @brief Tests the conversion between real Armadillo matrix and @link
 *  NumberArray @endlink.
 */
TEST(NumberArrayArmaMatConversion, RealConversion) {
  const Mat<real_t> input_mat(5, 8, arma::fill::randu);

  const auto output_mat = NumberArrayToArmaMat<real_t>(
      ArmaMatToNumberArray(input_mat));

  const bool is_equal = arma::approx_equal(
      input_mat,
      output_mat,
      "reldiff",
      ARMA_EQUAL_RELDIFF);

  ASSERT_TRUE(is_equal);
}

/**
 *  @brief Tests the conversion between complex Armadillo matrix and @link
 *  NumberArray @endlink.
 */
TEST(NumberArrayArmaMatConversion, ComplexConversion) {
  // Compare complex-to-complex conversion.
  {
    const Mat<complex_t> input_mat(5, 8, arma::fill::randu);

    const auto output_mat = NumberArrayToArmaMat<complex_t>(
        ArmaMatToNumberArray(input_mat));

    const bool is_equal = arma::approx_equal(
        input_mat,
        output_mat,
        "reldiff",
        ARMA_EQUAL_RELDIFF);

    ASSERT_TRUE(is_equal);
  }

  // Compare real-to-complex conversion.
  {
    const Mat<real_t> input_mat_real(5, 8, arma::fill::randu);
    Mat<complex_t> input_mat_complex(arma::size(input_mat_real));

    std::copy(
        input_mat_real.begin(),
        input_mat_real.end(),
        input_mat_complex.begin());

    const auto output_mat = NumberArrayToArmaMat<complex_t>(
        ArmaMatToNumberArray(input_mat_real));

    const bool is_equal = arma::approx_equal(
        input_mat_complex,
        output_mat,
        "reldiff",
        ARMA_EQUAL_RELDIFF);

    ASSERT_TRUE(is_equal);
  }
}

/**
 *  @brief Tests the conversion between real Armadillo cube and @link
 *  NumberArray @endlink.
 */
TEST(NumberArrayArmaCubeConversion, RealConversion) {
  const Cube<real_t> input_cube(5, 8, 2, arma::fill::randu);

  const auto output_cube = NumberArrayToArmaCube<real_t>(
      ArmaCubeToNumberArray(input_cube));

  const bool is_equal = arma::approx_equal(
      input_cube,
      output_cube,
      "reldiff",
      ARMA_EQUAL_RELDIFF);

  ASSERT_TRUE(is_equal);
}

/**
 *  @brief Tests the conversion between complex Armadillo cube and @link
 *  NumberArray @endlink.
 */
TEST(NumberArrayArmaCubeConversion, ComplexConversion) {
  // Compare complex-to-complex conversion.
  {
    const Cube<complex_t> input_cube(5, 8, 2, arma::fill::randu);

    const auto output_cube = NumberArrayToArmaCube<complex_t>(
        ArmaCubeToNumberArray(input_cube));

    const bool is_equal = arma::approx_equal(
        input_cube,
        output_cube,
        "reldiff",
        ARMA_EQUAL_RELDIFF);

    ASSERT_TRUE(is_equal);
  }

  // Compare real-to-complex conversion.
  {
    const Cube<real_t> input_cube_real(5, 8, 2, arma::fill::randu);
    Cube<complex_t> input_cube_complex(arma::size(input_cube_real));

    std::copy(
        input_cube_real.begin(),
        input_cube_real.end(),
        input_cube_complex.begin());

    const auto output_cube = NumberArrayToArmaCube<complex_t>(
        ArmaCubeToNumberArray(input_cube_real));

    const bool is_equal = arma::approx_equal(
        input_cube_complex,
        output_cube,
        "reldiff",
        ARMA_EQUAL_RELDIFF);

    ASSERT_TRUE(is_equal);
  }
}
