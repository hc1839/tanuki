#include "tanuki/avro_specific/size_t.h"

#include <limits>

#include <avro/Specific.hh>

namespace avro {

void codec_traits<size_t>::encode(Encoder &e, const size_t &o) {
  if (o > std::numeric_limits<long>::max()) {
    throw std::range_error("Value to encode is outside valid range.");
  }
  avro::encode(e, static_cast<long>(o));
}

void codec_traits<size_t>::decode(Decoder &d, size_t &o) {
  long o_long;
  avro::decode(d, o_long);
  if (o_long < 0) {
    throw std::range_error("Decoded value is outside valid range.");
  }
  o = o_long;
}

} // namespace avro
