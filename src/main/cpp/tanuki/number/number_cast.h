#ifndef TANUKI_NUMBER_NUMBER_CAST_H
#define TANUKI_NUMBER_NUMBER_CAST_H

#include "tanuki/number/types.h"

namespace tanuki {
namespace number {

/**
 *  @brief Casts a complex number.
 *
 *  @tparam T
 *    Destination type. It must be @link real_t @endlink or @link complex_t
 *    @endlink. If @link real_t @endlink, the imaginary part is dropped. If
 *    @link complex_t @endlink, the same complex number is returned.
 */
template <typename T>
T NumberCast(const complex_t &o);

/**
 *  @brief Casts a complex number to a real number by dropping the imaginary
 *  part.
 */
template <>
real_t NumberCast(const complex_t &o) {
  return o.real();
}

/**
 *  @brief Identity casts a complex number.
 */
template <>
complex_t NumberCast(const complex_t &o) {
  return o;
}

} // namespace number
} // namespace tanuki

#endif
