#ifndef TANUKI_AVRO_SPECIFIC_ARRAY_H
#define TANUKI_AVRO_SPECIFIC_ARRAY_H

#include <array>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include <avro/Decoder.hh>
#include <avro/Encoder.hh>
#include <avro/Specific.hh>

namespace avro {

using std::array;
using std::vector;

/**
 *  @brief Encoding and decoding of <tt>std::array</tt>.
 *
 *  Avro schema is <tt>array</tt>.
 *
 *  @tparam T
 *    Type of elements.
 *
 *  @tparam num_elems
 *    Number of elements.
 */
template <typename T, size_t num_elems>
struct codec_traits<array<T, num_elems>> {
 public:
  static void encode(Encoder &e, const array<T, num_elems> &o) {
    e.arrayStart();
    if (!o.empty()) {
      e.setItemCount(o.size());
      for (const auto &item : o) {
        e.startItem();
        avro::encode(e, item);
      }
    }
    e.arrayEnd();
  }

  static void decode(Decoder &d, array<T, num_elems> &o) {
    vector<T> o_vec;
    avro::decode(d, o_vec);
    if (o_vec.size() != num_elems) {
      throw std::runtime_error("Number of items does not match.");
    }
    std::move(o_vec.begin(), o_vec.end(), o.begin());
  }
};

} // namespace avro

#endif
