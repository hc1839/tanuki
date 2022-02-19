#include "tanuki/math/combinatorics/combinations.h"

#include <cmath>

namespace tanuki {
namespace math {
namespace combinatorics {

size_t CountCombinations(size_t n, size_t k) {
  auto retval = std::tgamma(n + 1) /
      (std::tgamma(k + 1) * std::tgamma(n - k + 1));

  return static_cast<size_t>(std::lround(retval));
}

} // namespace combinatorics
} // namespace math
} // namespace tanuki
