#ifndef TANUKI_AVRO_SPECIFIC_SET_H
#define TANUKI_AVRO_SPECIFIC_SET_H

#include <set>
#include <stdexcept>
#include <utility>

#include <avro/Decoder.hh>
#include <avro/Encoder.hh>
#include <avro/Specific.hh>

namespace avro {

using std::set;

/**
 *  @brief Encoding and decoding of <tt>std::set</tt>.
 *
 *  Avro schema is <tt>array</tt> of unique items where ordering is not
 *  important.
 *
 *  @tparam Key
 *    Type of elements.
 */
template <typename Key>
struct codec_traits<set<Key>> {
 public:
  static void encode(Encoder &e, const set<Key> &o) {
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

  static void decode(Decoder &d, set<Key> &o) {
    o.clear();
    for (size_t n = d.arrayStart(); n != 0; n = d.arrayNext()) {
      for (size_t i = 0; i != n; ++i) {
        Key item;
        avro::decode(d, item);
        if (!o.insert(std::move(item)).second) {
          throw std::runtime_error("Items are not unique.");
        }
      }
    }
  }
};

} // namespace avro

#endif
