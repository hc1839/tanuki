#ifndef TANUKI_MATH_LINEAR_NUMBER_ARRAY_H
#define TANUKI_MATH_LINEAR_NUMBER_ARRAY_H

#include <cstddef>
#include <iterator>
#include <type_traits>
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
   *  @brief Creates from a range of real numbers.
   *
   *  @tparam InputIt
   *    Must meet the requirements of <tt>LegacyInputIterator</tt> and have a
   *    dereferenced type that is floating-point.
   *
   *  @param extents
   *    Extents along each dimension of the number array.
   *
   *  @param first
   *    Beginning of the range of numbers to create from.
   *
   *  @param last
   *    Past-the-end of the range of numbers to create from.
   *
   *  @exception std::length_error
   *    Size of <tt>[first, last)</tt> is not equal to the product of the
   *    extents.
   */
  template <
      typename InputIt,
      typename std::enable_if<
          std::is_floating_point<
              typename std::iterator_traits<InputIt>::value_type>::value,
          bool>::type = true>
  static NumberArray Create(
      const vector<size_t> &extents, InputIt first, InputIt last);

  /**
   *  @brief Creates from a range of complex numbers.
   *
   *  @tparam InputIt
   *    Must meet the requirements of <tt>LegacyInputIterator</tt> and have a
   *    dereferenced type that is @link complex_t @endlink.
   *
   *  @param extents
   *    Extents along each dimension of the number array.
   *
   *  @param first
   *    Beginning of the range of numbers to create from.
   *
   *  @param last
   *    Past-the-end of the range of numbers to create from.
   *
   *  @exception std::length_error
   *    Size of <tt>[first, last)</tt> is not equal to the product of the
   *    extents.
   */
  template <
      typename InputIt,
      typename std::enable_if<
          std::is_same<
              typename std::iterator_traits<InputIt>::value_type,
              complex_t>::value,
          bool>::type = true>
  static NumberArray Create(
      const vector<size_t> &extents, InputIt first, InputIt last);

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
   *  @brief Real part of the array.
   */
  vector<real_t> real;

  /**
   *  @brief Imaginary part of the array. It is not used if @link is_complex
   *  @endlink is <tt>false</tt>.
   */
  vector<real_t> imag;
};

/**
 *  @brief Converts @link NumberArray @endlink to a range of real numbers.
 *
 *  @tparam OutputIt
 *    Must meet the requirements of <tt>LegacyOutputIterator</tt> and have a
 *    dereferenced type that is floating-point.
 *
 *  @param src
 *    Source @link NumberArray @endlink.
 *
 *  @param d_first
 *    Beginning of the destination range.
 *
 *  @exception std::range_error
 *    <tt>this</tt> is complex.
 *
 *  @exception std::length_error
 *    Size of @link real @endlink is not equal to @link num_elems @endlink.
 */
template <
    typename OutputIt,
    typename std::enable_if<
        std::is_floating_point<
            typename std::iterator_traits<OutputIt>::value_type>::value,
        bool>::type = true>
void Convert(const NumberArray &src, OutputIt d_first);

/**
 *  @brief Converts @link NumberArray @endlink to a range of complex numbers.
 *
 *  @tparam OutputIt
 *    Must meet the requirements of <tt>LegacyOutputIterator</tt> and have a
 *    dereferenced type that is @link complex_t @endlink.
 *
 *  @param src
 *    Source @link NumberArray @endlink.
 *
 *  @param d_first
 *    Beginning of the destination range.
 *
 *  @exception std::length_error
 *    Size of @link real @endlink or @link imag @endlink is not equal to @link
 *    num_elems @endlink.
 */
template <
    typename OutputIt,
    typename std::enable_if<
        std::is_same<
            typename std::iterator_traits<OutputIt>::value_type,
            complex_t>::value,
        bool>::type = true>
void Convert(const NumberArray &src, OutputIt d_first);

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
