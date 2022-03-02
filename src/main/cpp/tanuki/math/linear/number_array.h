#ifndef TANUKI_MATH_LINEAR_NUMBER_ARRAY_H
#define TANUKI_MATH_LINEAR_NUMBER_ARRAY_H

#include <vector>

#include <armadillo>
#include <avro/Decoder.hh>
#include <avro/Encoder.hh>
#include <avro/Specific.hh>
#include <avro/ValidSchema.hh>
#include <boost/multi_array.hpp>

#include "tanuki/number/types.h"

namespace tanuki {
namespace math {
namespace linear {

using std::vector;

using arma::Cube;
using arma::Mat;
using boost::const_multi_array_ref;
using boost::multi_array;
using tanuki::number::complex_t;
using tanuki::number::real_t;

/**
 *  @brief Array of numbers that are either real or complex.
 */
struct NumberArray final {
 public:
  /**
   *  @brief Avro serialization schema.
   */
  static const avro::ValidSchema &AvroSchema();

  /**
   *  @brief Number of elements determined from @link extents @endlink only.
   */
  size_t num_elems() const;

  /**
   *  @brief Whether the numbers are complex.
   */
  bool is_complex;

  /**
   *  @brief Shape of the array. Number of elements is the number of
   *  dimensions.
   */
  vector<size_t> extents;

  /**
   *  @brief Real part of the array in column-major order.
   */
  vector<real_t> real;

  /**
   *  @brief Imaginary part of the array in column-major order. It is not used
   *  if @link is_complex @endlink is <tt>false</tt>.
   */
  vector<real_t> imag;
};

/**
 *  @brief Converts from <tt>boost::const_multi_array_ref</tt> to @link
 *  NumberArray @endlink.
 */
template <typename E, size_t D>
NumberArray MultiToNumberArray(const const_multi_array_ref<E, D> &src);

/**
 *  @brief Converts from <tt>boost::multi_array</tt> to @link NumberArray
 *  @endlink.
 */
template <typename E, size_t D>
NumberArray MultiToNumberArray(const multi_array<E, D> &src);

/**
 *  @brief Converts from @link NumberArray @endlink to
 *  <tt>boost::multi_array</tt>.
 */
template <typename E, size_t D>
multi_array<E, D> NumberToMultiArray(const NumberArray &src);

template <typename T>
NumberArray ArmaMatToNumberArray(const Mat<T> &src);

/**
 *  @brief Converts from real <tt>arma::Mat</tt> to @link NumberArray @endlink.
 */
template <>
NumberArray ArmaMatToNumberArray(const Mat<real_t> &src);

/**
 *  @brief Converts from complex <tt>arma::Mat</tt> to @link NumberArray
 *  @endlink.
 */
template <>
NumberArray ArmaMatToNumberArray(const Mat<complex_t> &src);

template <typename T>
NumberArray ArmaCubeToNumberArray(const Cube<T> &src);

/**
 *  @brief Converts from real <tt>arma::Cube</tt> to @link NumberArray
 *  @endlink.
 */
template <>
NumberArray ArmaCubeToNumberArray(const Cube<real_t> &src);

/**
 *  @brief Converts from complex <tt>arma::Cube</tt> to @link NumberArray
 *  @endlink.
 */
template <>
NumberArray ArmaCubeToNumberArray(const Cube<complex_t> &src);

template <typename T>
Mat<T> NumberArrayToArmaMat(const NumberArray &src);

/**
 *  @brief Converts from @link NumberArray @endlink to real <tt>arma::Mat</tt>.
 */
template <>
Mat<real_t> NumberArrayToArmaMat(const NumberArray &src);

/**
 *  @brief Converts from @link NumberArray @endlink to complex
 *  <tt>arma::Mat</tt>.
 */
template <>
Mat<complex_t> NumberArrayToArmaMat(const NumberArray &src);

template <typename T>
Cube<T> NumberArrayToArmaCube(const NumberArray &src);

/**
 *  @brief Converts from @link NumberArray @endlink to real
 *  <tt>arma::Cube</tt>.
 */
template <>
Cube<real_t> NumberArrayToArmaCube(const NumberArray &src);

/**
 *  @brief Converts from @link NumberArray @endlink to complex
 *  <tt>arma::Cube</tt>.
 */
template <>
Cube<complex_t> NumberArrayToArmaCube(const NumberArray &src);

} // namespace linear
} // namespace math
} // namespace tanuki

namespace avro {

using tanuki::math::linear::NumberArray;

/**
 *  @brief Encoding and decoding of @link NumberArray @endlink.
 */
template <>
struct codec_traits<NumberArray> {
 public:
  static void encode(Encoder &e, const NumberArray &o);

  static void decode(Decoder &d, NumberArray &o);
};

} // namespace avro

#include "tanuki/math/linear/number_array.hxx"

#endif
