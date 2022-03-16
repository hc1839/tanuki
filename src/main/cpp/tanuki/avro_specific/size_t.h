#ifndef TANUKI_AVRO_SPECIFIC_SIZE_T_H
#define TANUKI_AVRO_SPECIFIC_SIZE_T_H

#include <cstddef>

#include <avro/Decoder.hh>
#include <avro/Encoder.hh>
#include <avro/Specific.hh>

namespace avro {

/**
 *  @brief Encoding and decoding of <tt>size_t</tt> with range checking.
 *
 *  Avro schema is <tt>long</tt>.
 */
template <>
struct codec_traits<size_t> {
 public:
  /**
   *  @exception std::range_error
   *    Value of <tt>o</tt> is greater than the maximum value representable by
   *    C++ <tt>long</tt>.
   */
  static void encode(Encoder &e, const size_t &o);

  /**
   *  @exception std::range_error
   *    Decoded <tt>long</tt> is negative.
   */
  static void decode(Decoder &d, size_t &o);
};

} // namespace avro

#endif
