#ifndef TANUKI_MATH_LINEAR_NUMBER_ARRAY_H
#define TANUKI_MATH_LINEAR_NUMBER_ARRAY_H

#include <vector>

#include <avro/Decoder.hh>
#include <avro/Encoder.hh>
#include <avro/Specific.hh>
#include <avro/ValidSchema.hh>

#include "tanuki/number/types.h"

namespace tanuki {
namespace math {
namespace linear {

using std::vector;

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
 *  @brief Custom conversions with @link NumberArray @endlink that is called by
 *  @link EncodeToNumberArray @endlink and @link DecodeFromNumberArray
 *  @endlink.
 *
 *  @tparam T
 *    Type being converted.
 */
template <typename T>
struct NumberArrayConvert final {
 public:
  /**
   *  @brief Converts to @link NumberArray @endlink.
   *
   *  It is called by @link EncodeToNumberArray @endlink.
   */
  static void Encode(NumberArray &e, const T &o);

  /**
   *  @brief Converts from @link NumberArray @endlink.
   *
   *  It is called by @link DecodeFromNumberArray @endlink.
   */
  static void Decode(const NumberArray &e, T &o);
};

/**
 *  @brief Converts to @link NumberArray @endlink.
 *
 *  To implement custom conversion from a type, define @link
 *  NumberArrayConvert::Encode @endlink.
 *
 *  @tparam T
 *    Type being converted from.
 */
template <typename T>
void EncodeToNumberArray(NumberArray &e, const T &o);

/**
 *  @brief Converts from @link NumberArray @endlink.
 *
 *  To implement custom conversion to a type, define @link
 *  NumberArrayConvert::Decode @endlink.
 *
 *  @tparam T
 *    Type being converted to.
 */
template <typename T>
void DecodeFromNumberArray(const NumberArray &d, T &o);

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
