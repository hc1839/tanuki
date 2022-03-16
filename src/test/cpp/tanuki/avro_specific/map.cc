#include <tanuki.h>

#include <map>
#include <string>

#include <avro/Compiler.hh>
#include <avro/Decoder.hh>
#include <avro/Encoder.hh>
#include <avro/Schema.hh>
#include <avro/Specific.hh>
#include <avro/ValidSchema.hh>
#include <gtest/gtest.h>

namespace tanuki {
namespace avro_specific {

using std::map;
using std::string;

/**
 *  @brief Tests Avro serialization of <tt>std::map</tt>.
 */
TEST(AvroSpecificMap, Avro) {
  avro::ValidSchema valid_schema;

  {
    avro::RecordSchema entry_schema("MapEntry");
    entry_schema.addField("key", avro::BoolSchema());
    entry_schema.addField("value", avro::IntSchema());
    valid_schema.setSchema(avro::ArraySchema(entry_schema));
  }

  const map<bool, int> input = {
    { true, 5 },
    { false, -1 }
  };

  auto mem_out = avro::memoryOutputStream();
  auto encoder = avro::validatingEncoder(valid_schema, avro::binaryEncoder());
  encoder->init(*mem_out);

  avro::encode(*encoder, input);

  auto mem_in = avro::memoryInputStream(*mem_out);
  auto decoder = avro::validatingDecoder(valid_schema, avro::binaryDecoder());
  decoder->init(*mem_in);

  map<bool, int> output;
  avro::decode(*decoder, output);

  ASSERT_EQ(output, input);
}

} // namespace avro_specific
} // namespace tanuki
