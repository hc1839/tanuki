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

#define APPROX_EQUAL_REL_TOL 1.0e-3

using std::array;
using std::complex;
using std::vector;

using arma::Cube;
using arma::Mat;
using boost::fortran_storage_order;
using boost::multi_array;

using tanuki::math::linear::DecodeFromNumberArray;
using tanuki::math::linear::EncodeToNumberArray;
using tanuki::math::linear::NumberArray;
using tanuki::number::complex_t;
using tanuki::number::real_t;

/**
 *  @brief Tests conversions between @link NumberArray @endlink and an
 *  Armadillo container.
 */
TEST(NumberArrayTestSuite, ArmadilloConversionTest) {
  // Test real matrix.
  {
    const Mat<real_t> input(12, 8, arma::fill::randu);
    const vector<size_t> shape = { input.n_rows, input.n_cols };

    const auto narr = NumberArray::Create(shape, input.begin(), input.end());

    ASSERT_FALSE(narr.is_complex);
    ASSERT_EQ(narr.extents, shape);
    ASSERT_EQ(narr.num_elems(), input.n_elem);
    ASSERT_EQ(narr.real.size(), narr.num_elems());

    Mat<real_t> output(narr.extents[0], narr.extents[1]);
    Convert(narr, output.begin());

    const bool is_equal = arma::approx_equal(
        input,
        output,
        "reldiff",
        APPROX_EQUAL_REL_TOL);

    ASSERT_TRUE(is_equal);
  }

  // Test complex matrix.
  {
    const Mat<complex_t> input(12, 8, arma::fill::randu);
    const vector<size_t> shape = { input.n_rows, input.n_cols };

    const auto narr = NumberArray::Create(shape, input.begin(), input.end());

    ASSERT_TRUE(narr.is_complex);
    ASSERT_EQ(narr.extents, shape);
    ASSERT_EQ(narr.num_elems(), input.n_elem);
    ASSERT_EQ(narr.real.size(), narr.num_elems());
    ASSERT_EQ(narr.imag.size(), narr.real.size());

    Mat<complex_t> output(narr.extents[0], narr.extents[1]);
    Convert(narr, output.begin());

    const bool is_equal = arma::approx_equal(
        input,
        output,
        "reldiff",
        APPROX_EQUAL_REL_TOL);

    ASSERT_TRUE(is_equal);
  }

  // Test real-to-complex matrix.
  {
    const Mat<real_t> input(12, 8, arma::fill::randu);
    const vector<size_t> shape = { input.n_rows, input.n_cols };

    const auto narr = NumberArray::Create(shape, input.begin(), input.end());

    Mat<complex_t> output(narr.extents[0], narr.extents[1]);
    Convert(narr, output.begin());

    // Complex version of input matrix for comparison.
    Mat<complex_t> input_complex(arma::size(input));
    std::copy(input.begin(), input.end(), input_complex.begin());

    const bool is_equal = arma::approx_equal(
        input_complex,
        output,
        "reldiff",
        APPROX_EQUAL_REL_TOL);

    ASSERT_TRUE(is_equal);
  }
}

/**
 *  @brief Tests Avro serialization of @link NumberArray @endlink.
 *
 *  @tparam T
 *    Must be @link tanuki::number::real_t @endlink or @link
 *    tanuki::number::complex_t @endlink.
 */
template <typename T>
void TEST_NumberArray_Avro() {
  const auto &schema = NumberArray::AvroSchema();

  NumberArray input;

  // Populate input number array.
  {
    const Mat<T> input_mat(12, 8, arma::fill::randu);
    input = NumberArray::Create(
        { input_mat.n_rows, input_mat.n_cols },
        input_mat.begin(), input_mat.end());
  }

  auto mem_out = avro::memoryOutputStream();
  auto encoder = avro::validatingEncoder(schema, avro::binaryEncoder());
  encoder->init(*mem_out);

  avro::encode(*encoder, input);

  auto mem_in = avro::memoryInputStream(*mem_out);
  auto decoder = avro::validatingDecoder(schema, avro::binaryDecoder());
  decoder->init(*mem_in);

  NumberArray output;
  avro::decode(*decoder, output);

  // Compare input and output number arrays.
  {
    Mat<T> input_mat(input.extents[0], input.extents[1]);
    Convert(input, input_mat.begin());

    Mat<T> output_mat(output.extents[0], output.extents[1]);
    Convert(output, output_mat.begin());

    const bool is_equal = arma::approx_equal(
        input_mat,
        output_mat,
        "reldiff",
        APPROX_EQUAL_REL_TOL);

    ASSERT_TRUE(is_equal);
  }
}

/**
 *  @brief Tests Avro serialization of @link NumberArray @endlink.
 */
TEST(NumberArrayTestSuite, Avro) {
  TEST_NumberArray_Avro<real_t>();
  TEST_NumberArray_Avro<complex_t>();
}

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
    {
      NumberArray narr;
      EncodeToNumberArray(narr, input_marr);
      avro::encode(*encoder, narr);
    }

    // Test real-to-real.
    {
      auto mem_in = avro::memoryInputStream(*mem_out);
      auto decoder = avro::validatingDecoder(schema, avro::binaryDecoder());
      decoder->init(*mem_in);

      multi_array<real_t, 2> output_marr(
          boost::extents[0][0], fortran_storage_order());

      // Convert Avro to real Boost multi-array.
      {
        NumberArray narr;
        avro::decode(*decoder, narr);
        DecodeFromNumberArray(narr, output_marr);
      }

      const Mat<real_t> output_mat(
          output_marr.data(),
          output_marr.shape()[0], output_marr.shape()[1],
          false, true);

      const bool is_equal = arma::approx_equal(
          input_mat,
          output_mat,
          "reldiff",
          APPROX_EQUAL_REL_TOL);

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

      multi_array<complex_t, 2> output_marr(
          boost::extents[0][0], fortran_storage_order());

      // Convert Avro to complex Boost multi-array.
      {
        NumberArray narr;
        avro::decode(*decoder, narr);
        DecodeFromNumberArray(narr, output_marr);
      }

      const Mat<complex_t> output_mat(
          output_marr.data(),
          output_marr.shape()[0], output_marr.shape()[1],
          false, true);

      const bool is_equal = arma::approx_equal(
          complex_input_mat,
          output_mat,
          "reldiff",
          APPROX_EQUAL_REL_TOL);

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
    {
      NumberArray narr;
      EncodeToNumberArray(narr, input_marr);
      avro::encode(*encoder, narr);
    }

    auto mem_in = avro::memoryInputStream(*mem_out);
    auto decoder = avro::validatingDecoder(schema, avro::binaryDecoder());
    decoder->init(*mem_in);

    multi_array<complex_t, 2> output_marr(
        boost::extents[0][0], fortran_storage_order());

    // Convert Avro to complex Boost multi-array.
    {
      NumberArray narr;
      avro::decode(*decoder, narr);
      DecodeFromNumberArray(narr, output_marr);
    }

    const Mat<complex_t> output_mat(
        output_marr.data(),
        output_marr.shape()[0], output_marr.shape()[1],
        false, true);

    const bool is_equal = arma::approx_equal(
        input_mat,
        output_mat,
        "reldiff",
        APPROX_EQUAL_REL_TOL);

    ASSERT_TRUE(is_equal);
  }
}

/**
 *  @brief Tests the conversion between real Armadillo matrix and @link
 *  NumberArray @endlink.
 */
TEST(NumberArrayArmaMatConversion, RealConversion) {
  const Mat<real_t> input_mat(5, 8, arma::fill::randu);

  Mat<real_t> output_mat;

  {
    NumberArray narr;
    EncodeToNumberArray(narr, input_mat);
    DecodeFromNumberArray(narr, output_mat);
  }

  const bool is_equal = arma::approx_equal(
      input_mat,
      output_mat,
      "reldiff",
      APPROX_EQUAL_REL_TOL);

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

    Mat<complex_t> output_mat;

    {
      NumberArray narr;
      EncodeToNumberArray(narr, input_mat);
      DecodeFromNumberArray(narr, output_mat);
    }

    const bool is_equal = arma::approx_equal(
        input_mat,
        output_mat,
        "reldiff",
        APPROX_EQUAL_REL_TOL);

    ASSERT_TRUE(is_equal);
  }

  // Compare real-to-complex conversion.
  {
    const Mat<real_t> input_mat_real(5, 8, arma::fill::randu);

    Mat<complex_t> output_mat;

    {
      NumberArray narr;
      EncodeToNumberArray(narr, input_mat_real);
      DecodeFromNumberArray(narr, output_mat);
    }

    // Complex version of the input matrix for comparison.
    Mat<complex_t> input_mat_complex(arma::size(input_mat_real));

    std::copy(
        input_mat_real.begin(),
        input_mat_real.end(),
        input_mat_complex.begin());

    const bool is_equal = arma::approx_equal(
        input_mat_complex,
        output_mat,
        "reldiff",
        APPROX_EQUAL_REL_TOL);

    ASSERT_TRUE(is_equal);
  }
}

/**
 *  @brief Tests the conversion between real Armadillo cube and @link
 *  NumberArray @endlink.
 */
TEST(NumberArrayArmaCubeConversion, RealConversion) {
  const Cube<real_t> input_cube(5, 8, 2, arma::fill::randu);

  Cube<real_t> output_cube;

  {
    NumberArray narr;
    EncodeToNumberArray(narr, input_cube);
    DecodeFromNumberArray(narr, output_cube);
  }

  const bool is_equal = arma::approx_equal(
      input_cube,
      output_cube,
      "reldiff",
      APPROX_EQUAL_REL_TOL);

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

    Cube<complex_t> output_cube;

    {
      NumberArray narr;
      EncodeToNumberArray(narr, input_cube);
      DecodeFromNumberArray(narr, output_cube);
    }

    const bool is_equal = arma::approx_equal(
        input_cube,
        output_cube,
        "reldiff",
        APPROX_EQUAL_REL_TOL);

    ASSERT_TRUE(is_equal);
  }

  // Compare real-to-complex conversion.
  {
    const Cube<real_t> input_cube_real(5, 8, 2, arma::fill::randu);

    Cube<complex_t> output_cube;

    {
      NumberArray narr;
      EncodeToNumberArray(narr, input_cube_real);
      DecodeFromNumberArray(narr, output_cube);
    }

    // Complex version of the input matrix for comparison.
    Cube<complex_t> input_cube_complex(arma::size(input_cube_real));

    std::copy(
        input_cube_real.begin(),
        input_cube_real.end(),
        input_cube_complex.begin());

    const bool is_equal = arma::approx_equal(
        input_cube_complex,
        output_cube,
        "reldiff",
        APPROX_EQUAL_REL_TOL);

    ASSERT_TRUE(is_equal);
  }
}
