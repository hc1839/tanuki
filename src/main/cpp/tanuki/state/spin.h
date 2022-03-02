#ifndef TANUKI_STATE_SPIN_H
#define TANUKI_STATE_SPIN_H

#include <string>

#include <avro/Decoder.hh>
#include <avro/Encoder.hh>
#include <avro/Specific.hh>
#include <avro/ValidSchema.hh>

#include "tanuki/enumeration/name.h"

namespace tanuki {
namespace state {

/**
 *  @brief Enumeration of up/down, up, and down spin directions.
 */
enum class Spin : int {
  /**
   *  @brief One of the same up or down direction in a system that is not spin
   *  polarized.
   */
  DUAL,

  /**
   *  @brief Up direction in a system that is spin polarized.
   */
  UP,

  /**
   *  @brief Down direction in a system that is spin polarized.
   */
  DOWN
};

/**
 *  @brief Avro serialization schema for @link Spin @endlink.
 */
extern const avro::ValidSchema &SpinAvroSchema();

} // namespace state
} // namespace tanuki

namespace avro {

using tanuki::state::Spin;

/**
 *  @brief Encoding and decoding of @link Spin @endlink.
 */
template <>
struct codec_traits<Spin> {
 public:
  static void encode(Encoder &e, const Spin &o);

  static void decode(Decoder &d, Spin &o);
};

} // namespace avro

template <>
std::string EnumNameOf(tanuki::state::Spin enumerator);

template <>
tanuki::state::Spin EnumValueOf(const std::string &name);

#endif
