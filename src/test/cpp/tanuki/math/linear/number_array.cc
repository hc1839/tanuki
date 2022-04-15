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

namespace tanuki {
namespace math {
namespace linear {

using std::array;
using std::complex;
using std::vector;

using arma::Cube;
using arma::Mat;
using boost::fortran_storage_order;
using boost::multi_array;

using tanuki::number::complex_t;
using tanuki::number::real_t;

/**
 *  @brief Tests conversions between @link NumberArray @endlink and an
 *  Armadillo container.
 */
TEST(NumberArray, ArmadilloConversion) {
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

} // namespace linear
} // namespace math
} // namespace tanuki
