#ifndef TANUKI_ALGORITHM_ALGORITHM_HXX
#define TANUKI_ALGORITHM_ALGORITHM_HXX

namespace tanuki {
namespace algorithm {

template <typename InputIt1, typename InputIt2, typename BinaryPredicate>
bool CompareRangeAllOf(
    InputIt1 first1, InputIt1 last1,
    InputIt2 first2, InputIt2 last2,
    BinaryPredicate p) {
  auto range1_it = first1;
  auto range2_it = first2;

  while (range1_it != last1 && range2_it != last2) {
    if (!p(*range1_it++, *range2_it++)) {
      return false;
    }
  }

  return true;
}

} // namespace algorithm
} // namespace tanuki

#endif
