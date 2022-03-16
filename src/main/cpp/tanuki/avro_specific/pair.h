#ifndef TANUKI_AVRO_SPECIFIC_PAIR_H
#define TANUKI_AVRO_SPECIFIC_PAIR_H

#include <utility>

#include <avro/Decoder.hh>
#include <avro/Encoder.hh>
#include <avro/Specific.hh>

namespace avro {

using std::pair;

/**
 *  @brief Encoding and decoding of <tt>std::pair</tt>.
 *
 *  Avro schema is <tt>record</tt> that contains exactly two fields. First
 *  field is first element, and second field is second element.
 *
 *  @tparam T1
 *    Type of first element.
 *
 *  @tparam T2
 *    Type of second element.
 */
template <typename T1, typename T2>
struct codec_traits<pair<T1, T2>> {
 public:
  static void encode(Encoder &e, const pair<T1, T2> &o) {
    avro::encode(e, o.first);
    avro::encode(e, o.second);
  }

  static void decode(Decoder &d, pair<T1, T2> &o) {
    avro::decode(d, o.first);
    avro::decode(d, o.second);
  }
};

} // namespace avro

#endif
