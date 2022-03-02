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
namespace state {

using std::map;
using std::string;
using std::vector;

/**
 *  @brief Tests Avro serialization of @link Spin @endlink.
 */
TEST(Spin, Avro) {
  const auto &schema = SpinAvroSchema();

  const vector<Spin> inputs = {
    Spin::DUAL,
    Spin::UP,
    Spin::DOWN
  };

  for (const auto &input : inputs) {
    auto mem_out = avro::memoryOutputStream();
    auto encoder = avro::validatingEncoder(schema, avro::binaryEncoder());
    encoder->init(*mem_out);

    avro::encode(*encoder, input);

    auto mem_in = avro::memoryInputStream(*mem_out);
    auto decoder = avro::validatingDecoder(schema, avro::binaryDecoder());
    decoder->init(*mem_in);

    Spin output;
    avro::decode(*decoder, output);

    ASSERT_EQ(output, input);
  }
}

/**
 *  @brief Tests mapping between @link Spin @endlink and its name.
 */
TEST(Spin, Name) {
  const map<Spin, string> inputs = {
    { Spin::DUAL, "DUAL" },
    { Spin::UP, "UP" },
    { Spin::DOWN, "DOWN" }
  };

  for (const auto &input : inputs) {
    const auto &enumerator = input.first;
    const auto &name = input.second;

    ASSERT_EQ(enumerator, EnumValueOf<Spin>(name));
    ASSERT_EQ(name, EnumNameOf(enumerator));
  }

  ASSERT_ANY_THROW(EnumValueOf<Spin>("_"));
}

} // namespace state
} // namespace tanuki
