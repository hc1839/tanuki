#ifndef TANUKI_ALGORITHM_ALGORITHM_H
#define TANUKI_ALGORITHM_ALGORITHM_H

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <numeric>
#include <vector>

namespace tanuki {
namespace algorithm {

using std::vector;

/**
 *  @brief Determines whether all corresponding elements in two ranges of equal
 *  distance satisfy a predicate.
 *
 *  Behavior is undefined if the distances of the two ranges are not equal.
 *
 *  @tparam InputIt1
 *    Type of iterator for the first range. It must meets the requirements of
 *    <tt>LegacyInputIterator</tt>.
 *
 *  @tparam InputIt2
 *    Type of iterator for the second range. It must meets the requirements of
 *    <tt>LegacyInputIterator</tt>.
 *
 *  @tparam BinaryPredicate
 *    Function type that accepts two arguments whose types are convertible from
 *    the first and second ranges, respectively, and returns a value that is
 *    convertible to a <tt>bool</tt>.
 *
 *  @param first1
 *    Iterator to the beginning of the first range.
 *
 *  @param last1
 *    Iterator to the end of the first range.
 *
 *  @param first2
 *    Iterator to the beginning of the second range.
 *
 *  @param last2
 *    Iterator to the end of the second range.
 *
 *  @param p
 *    Binary function that returns <tt>true</tt> if two corresponding elements
 *    satisfy the predicate.
 *
 *  @return
 *    Whether all corresponding elements in the two ranges satisfy the
 *    predicate.
 */
template <typename InputIt1, typename InputIt2, typename BinaryPredicate>
bool CompareRangeAllOf(
    InputIt1 first1, InputIt1 last1,
    InputIt2 first2, InputIt2 last2,
    BinaryPredicate p);

/**
 *  @brief Stable sorts zero-based indices of the elements in a range using a
 *  comparison function.
 *
 *  The range itself is not sorted.
 *
 *  @tparam RandomIt
 *    Must meet the same requirements as <tt>RandomIt</tt> in
 *    <tt>std::stable_sort</tt>.
 *
 *  @tparam OutputIt
 *    Must meet the requirements of <tt>LegacyOutputIterator</tt>.
 *
 *  @tparam Compare
 *    Must meet the same requirements as <tt>Compare</tt> in
 *    <tt>std::stable_sort</tt>.
 *
 *  @param first
 *    Beginning of the range to sort by index.
 *
 *  @param last
 *    Past-the-end of the range to sort by index.
 *
 *  @param indices_first
 *    Beginning of the destination range of sorted zero-based indices. Behavior
 *    is undefined if the destination range has fewer elements than there are
 *    in <tt>[first, last)</tt>.
 *
 *  @param comp
 *    Comparison function.
 */
template <typename RandomIt, typename OutputIt, typename Compare>
void StableIndexSort(
    RandomIt first, RandomIt last, OutputIt indices_first, Compare comp) {
  vector<size_t> sort_idxs(std::distance(first, last));
  std::iota(sort_idxs.begin(), sort_idxs.end(), 0);

  std::stable_sort(
      sort_idxs.begin(), sort_idxs.end(),
      [first, comp](const size_t &a, const size_t &b) -> bool {
        return comp(first[a], first[b]);
      });

  std::move(sort_idxs.begin(), sort_idxs.end(), indices_first);
}

/**
 *  @brief Stable sorts zero-based indices of the elements in a range using
 *  <tt>operator&lt;</tt>.
 *
 *  The range itself is not sorted.
 *
 *  @tparam RandomIt
 *    Must meet the same requirements as <tt>RandomIt</tt> in
 *    <tt>std::stable_sort</tt>.
 *
 *  @tparam OutputIt
 *    Must meet the requirements of <tt>LegacyOutputIterator</tt>.
 *
 *  @param first
 *    Beginning of the range to sort by index.
 *
 *  @param last
 *    Past-the-end of the range to sort by index.
 *
 *  @param indices_first
 *    Beginning of the destination range of sorted zero-based indices. Behavior
 *    is undefined if the destination range has fewer elements than there are
 *    in <tt>[first, last)</tt>.
 */
template <typename RandomIt, typename OutputIt>
void StableIndexSort(RandomIt first, RandomIt last, OutputIt indices_first) {
  using T = typename std::iterator_traits<RandomIt>::value_type;

  StableIndexSort(
      first, last, indices_first,
      [](const T &a, const T &b) -> bool { return a < b; });
}

} // namespace algorithm
} // namespace tanuki

#include "tanuki/algorithm/algorithm.hxx"

#endif
