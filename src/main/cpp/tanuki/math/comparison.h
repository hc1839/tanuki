#ifndef TANUKI_MATH_COMPARISON_H
#define TANUKI_MATH_COMPARISON_H

#include "tanuki/number/types.h"

namespace tanuki {
namespace math {

using tanuki::number::real_t;

/**
 *  @brief Whether two numbers are approximately equal within a relative
 *  tolerance.
 *
 *  @tparam T
 *    Type of number. Supported types are <tt>tanuki::number::real_t</tt> and
 *    <tt>tanuki::number::complex_t</tt>. Other types are undefined.
 *
 *  @param a
 *    First number.
 *
 *  @param b
 *    Second number.
 *
 *  @param tol
 *    Relative tolerance.
 *
 *  @return
 *    For real numbers, returns <tt>true</tt> if \f$ \left| a - b \right| /
 *    \max(\left| a \right|, \left| b \right|) \leq \mathrm{tol} \f$. For
 *    complex numbers, corresponding real and imaginary parts are compared as
 *    for real numbers, and returns <tt>true</tt> if the corresponding real and
 *    imaginary parts are both approximately equal.
 */
template <typename T>
bool ApproxEqualRel(T a, T b, real_t tol);

/**
 *  @brief Whether two numbers are approximately equal within an absolute
 *  tolerance.
 *
 *  @tparam T
 *    Type of number. Supported types are <tt>tanuki::number::real_t</tt> and
 *    <tt>tanuki::number::complex_t</tt>. Other types are undefined.
 *
 *  @param a
 *    First number.
 *
 *  @param b
 *    Second number.
 *
 *  @param tol
 *    Absolute tolerance.
 *
 *  @return
 *    For real numbers, returns <tt>true</tt> if \f$ \left| a - b \right| \leq
 *    \mathrm{tol} \f$. For complex numbers, corresponding real and imaginary
 *    parts are compared as for real numbers, and returns <tt>true</tt> if the
 *    corresponding real and imaginary parts are both approximately equal.
 */
template <typename T>
bool ApproxEqualAbs(T a, T b, real_t tol);

} // namespace math
} // namespace tanuki

#endif
