#ifndef TANUKI_AVRO_SPECIFIC_ARMADILLO_H
#define TANUKI_AVRO_SPECIFIC_ARMADILLO_H

#include <stdexcept>

#include <armadillo>
#include <avro/Decoder.hh>
#include <avro/Encoder.hh>
#include <avro/Specific.hh>

#include "tanuki/math/linear/number_array.h"

namespace avro {

using arma::Col;
using arma::Cube;
using arma::Mat;

using tanuki::math::linear::NumberArray;

/**
 *  @brief Encoding and decoding of <tt>arma::Col</tt>.
 *
 *  Avro schema is <tt>tanuki.math.linear.NumberArray</tt>.
 *
 *  @tparam T
 *    Must be @link tanuki::number::real_t @endlink or @link
 *    tanuki::number::complex_t @endlink.
 */
template <typename T>
struct codec_traits<Col<T>> {
 public:
  static void encode(Encoder &e, const Col<T> &o) {
    const auto narr = NumberArray::Create({ o.n_elem }, o.begin(), o.end());
    avro::encode(e, narr);
  }

  static void decode(Decoder &d, Col<T> &o) {
    NumberArray narr;
    avro::decode(d, narr);
    if (narr.extents.size() != 1) {
      throw std::runtime_error("Number of dimensions is not 1.");
    }
    o.set_size(narr.num_elems());
    Convert(narr, o.begin());
  }
};

/**
 *  @brief Encoding and decoding of <tt>arma::Mat</tt>.
 *
 *  Avro schema is <tt>tanuki.math.linear.NumberArray</tt> with elements stored
 *  in column-major order.
 *
 *  @tparam T
 *    Must be @link tanuki::number::real_t @endlink or @link
 *    tanuki::number::complex_t @endlink.
 */
template <typename T>
struct codec_traits<Mat<T>> {
 public:
  static void encode(Encoder &e, const Mat<T> &o) {
    const auto narr = NumberArray::Create(
        { o.n_rows, o.n_cols }, o.begin(), o.end());
    avro::encode(e, narr);
  }

  static void decode(Decoder &d, Mat<T> &o) {
    NumberArray narr;
    avro::decode(d, narr);
    if (narr.extents.size() != 2) {
      throw std::runtime_error("Number of dimensions is not 2.");
    }
    o.set_size(narr.extents[0], narr.extents[1]);
    Convert(narr, o.begin());
  }
};

/**
 *  @brief Encoding and decoding of <tt>arma::Cube</tt>.
 *
 *  Avro schema is <tt>tanuki.math.linear.NumberArray</tt> with elements stored
 *  in column-major order.
 *
 *  @tparam T
 *    Must be @link tanuki::number::real_t @endlink or @link
 *    tanuki::number::complex_t @endlink.
 */
template <typename T>
struct codec_traits<Cube<T>> {
 public:
  static void encode(Encoder &e, const Cube<T> &o) {
    const auto narr = NumberArray::Create(
        { o.n_rows, o.n_cols, o.n_slices }, o.begin(), o.end());
    avro::encode(e, narr);
  }

  static void decode(Decoder &d, Cube<T> &o) {
    NumberArray narr;
    avro::decode(d, narr);
    if (narr.extents.size() != 3) {
      throw std::runtime_error("Number of dimensions is not 3.");
    }
    o.set_size(narr.extents[0], narr.extents[1], narr.extents[2]);
    Convert(narr, o.begin());
  }
};

} // namespace avro

#endif
