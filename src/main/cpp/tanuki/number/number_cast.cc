#include "tanuki/number/number_cast.h"

namespace tanuki {
namespace number {

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
