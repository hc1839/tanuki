#include <tanuki.h>

#include <cstddef>
#include <limits>
#include <stdexcept>

#include <avro/Compiler.hh>
#include <avro/Decoder.hh>
#include <avro/Encoder.hh>
#include <avro/Schema.hh>
#include <avro/Specific.hh>
#include <avro/ValidSchema.hh>
#include <gtest/gtest.h>

namespace tanuki {
namespace avro_specific {

/**
 *  @brief Tests Avro serialization of <tt>size_t</tt>.
 */
TEST(AvroSpecificSizeT, Avro) {
  avro::ValidSchema valid_schema;

  {
    avro::LongSchema long_schema;
    valid_schema.setSchema(long_schema);
  }

  // Test valid input.
  {
    const size_t input = 32;

    auto mem_out = avro::memoryOutputStream();
    auto encoder = avro::validatingEncoder(valid_schema, avro::binaryEncoder());
    encoder->init(*mem_out);

    avro::encode(*encoder, input);

    auto mem_in = avro::memoryInputStream(*mem_out);
    auto decoder = avro::validatingDecoder(valid_schema, avro::binaryDecoder());
    decoder->init(*mem_in);

    size_t output;
    avro::decode(*decoder, output);

    ASSERT_EQ(output, input);
  }

  // Test invalid input.
  {
    const size_t input =
        static_cast<size_t>(std::numeric_limits<long>::max()) + 1;

    auto mem_out = avro::memoryOutputStream();
    auto encoder = avro::validatingEncoder(valid_schema, avro::binaryEncoder());
    encoder->init(*mem_out);

    ASSERT_THROW(avro::encode(*encoder, input), std::range_error);
  }

  // Test invalid output.
  {
    const long input = -1;

    auto mem_out = avro::memoryOutputStream();
    auto encoder = avro::validatingEncoder(valid_schema, avro::binaryEncoder());
    encoder->init(*mem_out);

    avro::encode(*encoder, input);

    auto mem_in = avro::memoryInputStream(*mem_out);
    auto decoder = avro::validatingDecoder(valid_schema, avro::binaryDecoder());
    decoder->init(*mem_in);

    size_t output;
    ASSERT_THROW(avro::decode(*decoder, output), std::range_error);
  }
}

} // namespace avro_specific
} // namespace tanuki
