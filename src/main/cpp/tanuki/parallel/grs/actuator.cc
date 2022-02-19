#include "tanuki/parallel/grs/actuator.h"

#include <cassert>
#include <cmath>

namespace tanuki {
namespace parallel {
namespace grs {

real_t GrsOneSidedRelaxParam(size_t n) {
  assert(n > 0);

  return std::fmax(0.0, 1.0 - 2.2919 * std::pow(n, -0.3382));
}

real_t GrsTwoSidedRelaxParam(size_t n) {
  assert(n > 0);

  return std::fmax(0.0, 1.0 - 2.9267 * std::pow(n, -0.4284));
}

} // namespace grs
} // namespace parallel
} // namespace tanuki
