#ifndef TANUKI_AVRO_SPECIFIC_MAP_ENTRY_H
#define TANUKI_AVRO_SPECIFIC_MAP_ENTRY_H

#include <map>
#include <stdexcept>
#include <utility>

#include <avro/Decoder.hh>
#include <avro/Encoder.hh>
#include <avro/Specific.hh>

#include "tanuki/avro_specific/pair.h"

namespace avro {

using std::map;
using std::pair;

/**
 *  @brief Encoding and decoding of <tt>std::map</tt> that has a generic key
 *  type.
 *
 *  Avro schema is <tt>array</tt> of <tt>record</tt>s, where each
 *  <tt>record</tt> must contain exactly two fields. First field is the key,
 *  and second field is the value associated by the key. Keys must be unique.
 *
 *  @tparam Key
 *    Key type.
 *
 *  @tparam T
 *    Mapped type.
 */
template <typename Key, typename T>
struct codec_traits<map<Key, T>> {
 public:
  static void encode(Encoder &e, const map<Key, T> &o) {
    e.arrayStart();
    if (!o.empty()) {
      e.setItemCount(o.size());
      for (const auto &entry : o) {
        e.startItem();
        avro::encode(e, entry);
      }
    }
    e.arrayEnd();
  }

  static void decode(Decoder &d, map<Key, T> &o) {
    o.clear();
    for (size_t n = d.arrayStart(); n != 0; n = d.arrayNext()) {
      for (size_t i = 0; i != n; ++i) {
        pair<Key, T> entry;
        avro::decode(d, entry);
        if (!o.insert(std::move(entry)).second) {
          throw std::runtime_error("Keys are not unique.");
        }
      }
    }
  }
};

} // namespace avro

#endif
