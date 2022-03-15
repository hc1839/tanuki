#ifndef TANUKI_MATH_COMPARISON_HXX
#define TANUKI_MATH_COMPARISON_HXX

#include <algorithm>
#include <cmath>

#include "tanuki/number/types.h"

namespace tanuki {
namespace math {

using tanuki::number::complex_t;
using tanuki::number::real_t;

template <>
bool ApproxEqualRel(real_t a, real_t b, real_t tol) {
  if (a == 0.0 && b == 0.0) {
    return true;
  } else {
    return std::abs(a - b) / std::max(std::abs(a), std::abs(b)) <= tol;
  }
}

template <>
bool ApproxEqualRel(complex_t a, complex_t b, real_t tol) {
  return ApproxEqualRel(a.real(), b.real(), tol) &&
      ApproxEqualRel(a.imag(), b.imag(), tol);
}

template <>
bool ApproxEqualAbs(real_t a, real_t b, real_t tol) {
  return std::abs(a - b) <= tol;
}

template <>
bool ApproxEqualAbs(complex_t a, complex_t b, real_t tol) {
  return ApproxEqualAbs(a.real(), b.real(), tol) &&
      ApproxEqualAbs(a.imag(), b.imag(), tol);
}

} // namespace math
} // namespace tanuki

#endif
