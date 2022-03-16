#include <tanuki.h>

#include <random>
#include <set>

#include <avro/Compiler.hh>
#include <avro/Decoder.hh>
#include <avro/Encoder.hh>
#include <avro/Schema.hh>
#include <avro/Specific.hh>
#include <avro/ValidSchema.hh>
#include <gtest/gtest.h>

namespace tanuki {
namespace avro_specific {

using std::set;
using std::uniform_int_distribution;

/**
 *  @brief Tests Avro serialization of <tt>std::set</tt>.
 */
TEST(AvroSpecificSet, Avro) {
  avro::ValidSchema valid_schema;

  {
    const avro::IntSchema int_schema;
    const avro::ArraySchema array_schema(int_schema);
    valid_schema.setSchema(array_schema);
  }

  set<int> input;

  std::random_device rd;
  std::mt19937 gen(rd());

  while (input.size() != 8) {
    input.insert(uniform_int_distribution<>(-32, 32)(gen));
  }

  auto mem_out = avro::memoryOutputStream();
  auto encoder = avro::validatingEncoder(valid_schema, avro::binaryEncoder());
  encoder->init(*mem_out);

  avro::encode(*encoder, input);

  auto mem_in = avro::memoryInputStream(*mem_out);
  auto decoder = avro::validatingDecoder(valid_schema, avro::binaryDecoder());
  decoder->init(*mem_in);

  set<int> output;
  avro::decode(*decoder, output);

  ASSERT_EQ(output, input);
}

} // namespace avro_specific
} // namespace tanuki
