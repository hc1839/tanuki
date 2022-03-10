#include <tanuki.h>

#include <map>
#include <string>
#include <vector>

#include <avro/Compiler.hh>
#include <avro/Decoder.hh>
#include <avro/Encoder.hh>
#include <avro/Specific.hh>
#include <gtest/gtest.h>

namespace tanuki {
namespace scf_mi {

using std::map;
using std::string;
using std::vector;

/**
 *  @brief Tests Avro serialization of @link ScfMiMethod @endlink.
 */
TEST(ScfMiMethod, Avro) {
  const auto &schema = ScfMiMethodAvroSchema();

  const vector<ScfMiMethod> inputs = {
    ScfMiMethod::LMO
  };

  for (const auto &input : inputs) {
    auto mem_out = avro::memoryOutputStream();
    auto encoder = avro::validatingEncoder(schema, avro::binaryEncoder());
    encoder->init(*mem_out);

    avro::encode(*encoder, input);

    auto mem_in = avro::memoryInputStream(*mem_out);
    auto decoder = avro::validatingDecoder(schema, avro::binaryDecoder());
    decoder->init(*mem_in);

    ScfMiMethod output;
    avro::decode(*decoder, output);

    ASSERT_EQ(output, input);
  }
}

/**
 *  @brief Tests mapping between @link ScfMiMethod @endlink and its name.
 */
TEST(ScfMiMethod, Name) {
  const map<ScfMiMethod, string> inputs = {
    { ScfMiMethod::LMO, "LMO" }
  };

  for (const auto &input : inputs) {
    const auto &enumerator = input.first;
    const auto &name = input.second;

    ASSERT_EQ(enumerator, EnumValueOf<ScfMiMethod>(name));
    ASSERT_EQ(name, EnumNameOf(enumerator));
  }

  ASSERT_ANY_THROW(EnumValueOf<ScfMiMethod>("_"));
}

} // namespace scf_mi
} // namespace tanuki
