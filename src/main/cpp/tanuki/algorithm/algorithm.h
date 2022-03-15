#ifndef TANUKI_ALGORITHM_ALGORITHM_H
#define TANUKI_ALGORITHM_ALGORITHM_H

namespace tanuki {
namespace algorithm {

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

} // namespace algorithm
} // namespace tanuki

#include "tanuki/algorithm/algorithm.hxx"

#endif
