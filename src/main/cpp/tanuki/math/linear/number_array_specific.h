#ifndef TANUKI_MATH_LINEAR_NUMBER_ARRAY_SPECIFIC_H
#define TANUKI_MATH_LINEAR_NUMBER_ARRAY_SPECIFIC_H

#include <cstddef>

#include <armadillo>
#include <boost/multi_array.hpp>

#include "tanuki/math/linear/number_array.h"
#include "tanuki/number/types.h"

namespace tanuki {
namespace math {
namespace linear {

using arma::Cube;
using arma::Mat;
using boost::const_multi_array_ref;
using boost::multi_array;
using tanuki::number::complex_t;
using tanuki::number::real_t;

/**
 *  @brief Conversion from <tt>boost::const_multi_array_ref</tt> of real
 *  numbers in column-major order to @link NumberArray @endlink.
 *
 *  @tparam D
 *    Number of dimensions.
 */
template <size_t D>
struct NumberArrayConvert<const_multi_array_ref<real_t, D>> final {
 public:
  static void Encode(
      NumberArray &e, const const_multi_array_ref<real_t, D> &o);
};

/**
 *  @brief Conversion from <tt>boost::const_multi_array_ref</tt> of complex
 *  numbers in column-major order to @link NumberArray @endlink.
 *
 *  @tparam D
 *    Number of dimensions.
 */
template <size_t D>
struct NumberArrayConvert<const_multi_array_ref<complex_t, D>> final {
 public:
  static void Encode(
      NumberArray &e, const const_multi_array_ref<complex_t, D> &o);
};

/**
 *  @brief Conversions between <tt>boost::multi_array</tt> of real numbers in
 *  column-major order and @link NumberArray @endlink.
 *
 *  @tparam D
 *    Number of dimensions.
 */
template <size_t D>
struct NumberArrayConvert<multi_array<real_t, D>> final {
 public:
  static void Encode(NumberArray &e, const multi_array<real_t, D> &o);

  static void Decode(const NumberArray &e, multi_array<real_t, D> &o);
};

/**
 *  @brief Conversions between <tt>boost::multi_array</tt> of complex numbers
 *  in column-major order and @link NumberArray @endlink.
 *
 *  @tparam D
 *    Number of dimensions.
 */
template <size_t D>
struct NumberArrayConvert<multi_array<complex_t, D>> final {
 public:
  static void Encode(NumberArray &e, const multi_array<complex_t, D> &o);

  static void Decode(const NumberArray &e, multi_array<complex_t, D> &o);
};

/**
 *  @brief Conversions between <tt>arma::Mat</tt> of real numbers and @link
 *  NumberArray @endlink.
 */
template <>
struct NumberArrayConvert<Mat<real_t>> final {
 public:
  static void Encode(NumberArray &e, const Mat<real_t> &o);

  static void Decode(const NumberArray &e, Mat<real_t> &o);
};

/**
 *  @brief Conversions between <tt>arma::Mat</tt> of complex numbers and @link
 *  NumberArray @endlink.
 */
template <>
struct NumberArrayConvert<Mat<complex_t>> final {
 public:
  static void Encode(NumberArray &e, const Mat<complex_t> &o);

  static void Decode(const NumberArray &e, Mat<complex_t> &o);
};

/**
 *  @brief Conversions between <tt>arma::Cube</tt> of real numbers and @link
 *  NumberArray @endlink.
 */
template <>
struct NumberArrayConvert<Cube<real_t>> final {
 public:
  static void Encode(NumberArray &e, const Cube<real_t> &o);

  static void Decode(const NumberArray &e, Cube<real_t> &o);
};

/**
 *  @brief Conversions between <tt>arma::Cube</tt> of complex numbers and @link
 *  NumberArray @endlink.
 */
template <>
struct NumberArrayConvert<Cube<complex_t>> final {
 public:
  static void Encode(NumberArray &e, const Cube<complex_t> &o);

  static void Decode(const NumberArray &e, Cube<complex_t> &o);
};

} // namespace linear
} // namespace math
} // namespace tanuki

#include "tanuki/math/linear/number_array_specific.hxx"

#endif
