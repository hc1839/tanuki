#ifndef TANUKI_SCF_MI_SCF_MI_METHOD_H
#define TANUKI_SCF_MI_SCF_MI_METHOD_H

#include <string>

#include <avro/Decoder.hh>
#include <avro/Encoder.hh>
#include <avro/Specific.hh>
#include <avro/ValidSchema.hh>

#include "tanuki/enumeration/name.h"

namespace tanuki {
namespace scf_mi {

/**
 *  @brief SCF-MI method.
 */
enum class ScfMiMethod : int {
  /**
   *  @brief LMO SCF MI (Stoll 1980).
   */
  LMO,

  /**
   *  @brief LP SCF MI (Nagata 2001).
   */
  LP
};

/**
 *  @brief Avro serialization schema for @link ScfMiMethod @endlink.
 */
extern const avro::ValidSchema &ScfMiMethodAvroSchema();

} // namespace scf_mi
} // namespace tanuki

namespace avro {

using tanuki::scf_mi::ScfMiMethod;

/**
 *  @brief Encoding and decoding of @link ScfMiMethod @endlink.
 */
template <>
struct codec_traits<ScfMiMethod> {
 public:
  static void encode(Encoder &e, const ScfMiMethod &o);

  static void decode(Decoder &d, ScfMiMethod &o);
};

} // namespace avro

template <>
std::string EnumNameOf(tanuki::scf_mi::ScfMiMethod enumerator);

template <>
tanuki::scf_mi::ScfMiMethod EnumValueOf(const std::string &name);

#endif
