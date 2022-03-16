#include <tanuki.h>

#include <utility>

#include <avro/Compiler.hh>
#include <avro/Decoder.hh>
#include <avro/Encoder.hh>
#include <avro/Schema.hh>
#include <avro/Specific.hh>
#include <avro/ValidSchema.hh>
#include <gtest/gtest.h>

namespace tanuki {
namespace avro_specific {

using std::pair;

/**
 *  @brief Tests Avro serialization of <tt>std::pair</tt>.
 */
TEST(AvroSpecificPair, Avro) {
  avro::ValidSchema valid_schema;

  {
    avro::RecordSchema schema("PairBoolInt");
    schema.addField("first", avro::BoolSchema());
    schema.addField("second", avro::IntSchema());
    valid_schema.setSchema(schema);
  }

  const pair<bool, int> input = { true, 5 };

  auto mem_out = avro::memoryOutputStream();
  auto encoder = avro::validatingEncoder(valid_schema, avro::binaryEncoder());
  encoder->init(*mem_out);

  avro::encode(*encoder, input);

  auto mem_in = avro::memoryInputStream(*mem_out);
  auto decoder = avro::validatingDecoder(valid_schema, avro::binaryDecoder());
  decoder->init(*mem_in);

  pair<bool, int> output;
  avro::decode(*decoder, output);

  ASSERT_EQ(output, input);
}

} // namespace avro_specific
} // namespace tanuki
