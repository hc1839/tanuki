#include <tanuki.h>

#include <algorithm>
#include <array>
#include <random>

#include <avro/Compiler.hh>
#include <avro/Decoder.hh>
#include <avro/Encoder.hh>
#include <avro/Schema.hh>
#include <avro/Specific.hh>
#include <avro/ValidSchema.hh>
#include <gtest/gtest.h>

namespace tanuki {
namespace avro_specific {

using std::array;
using std::uniform_int_distribution;

/**
 *  @brief Tests Avro serialization of <tt>std::array</tt>.
 */
TEST(AvroSpecificArray, Avro) {
  avro::ValidSchema valid_schema;

  {
    const avro::IntSchema int_schema;
    const avro::ArraySchema array_schema(int_schema);
    valid_schema.setSchema(array_schema);
  }

  array<int, 8> input;

  std::random_device rd;
  std::mt19937 gen(rd());

  std::generate(
      input.begin(), input.end(),
      [&gen]() -> int { return uniform_int_distribution<>(-32, 32)(gen); });

  auto mem_out = avro::memoryOutputStream();
  auto encoder = avro::validatingEncoder(valid_schema, avro::binaryEncoder());
  encoder->init(*mem_out);

  avro::encode(*encoder, input);

  // Test expected array size.
  {
    auto mem_in = avro::memoryInputStream(*mem_out);
    auto decoder = avro::validatingDecoder(valid_schema, avro::binaryDecoder());
    decoder->init(*mem_in);

    array<int, 8> output;
    avro::decode(*decoder, output);

    ASSERT_EQ(output, input);
  }

  // Test unexpected array size.
  {
    auto mem_in = avro::memoryInputStream(*mem_out);
    auto decoder = avro::validatingDecoder(valid_schema, avro::binaryDecoder());
    decoder->init(*mem_in);

    array<int, 12> output;
    ASSERT_ANY_THROW(avro::decode(*decoder, output));
  }
}

} // namespace avro_specific
} // namespace tanuki
