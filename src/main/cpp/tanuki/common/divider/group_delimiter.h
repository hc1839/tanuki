#ifndef TANUKI_COMMON_DIVIDER_GROUP_DELIMITER_H
#define TANUKI_COMMON_DIVIDER_GROUP_DELIMITER_H

#include <cstddef>
#include <utility>
#include <vector>

namespace tanuki {
namespace common {
namespace divider {

/**
 *  @brief Determines the end (exclusive) index of the specified group in an
 *  interval.
 *
 *  An interval encompasses a contiguous sequence of elements to be subdivided
 *  into groups.
 *
 *  @param interval_start_index
 *    Index of the first element in the interval.
 *
 *  @param interval_end_index
 *    Index of the element following the last element in the interval. It must
 *    be no less than <tt>interval_start_index</tt>.
 *
 *  @param num_groups
 *    Number of groups. It must be positive.
 *
 *  @param group_index
 *    Index of the group to determine the end index. First group in the
 *    interval has the index <tt>0</tt>.
 *
 *  @return
 *    Pair of start (inclusive) and end (exclusive) element indices for the
 *    group specified by <tt>group_index</tt>.
 */
std::pair<size_t, size_t> GroupIndexRange(
    size_t interval_start_index,
    size_t interval_end_index,
    size_t num_groups,
    size_t group_index);

/**
 *  @brief Returns a list of the start indices for each group in an interval
 *  and the interval's end (exclusive) index.
 *
 *  An interval encompasses a contiguous sequence of elements to be
 *  subdivided into groups.
 *
 *  @param interval_start_index
 *    Index of the first element in the interval.
 *
 *  @param interval_end_index
 *    Index of the element following the last element in the interval. It must
 *    be no less than <tt>interval_start_index</tt>.
 *
 *  @param num_groups
 *    Number of groups. It must be positive.
 *
 *  @return
 *    List of <tt>num_groups + 1</tt> indices, where the first element is
 *    <tt>interval_start_index</tt>, and the last element is
 *    <tt>interval_end_index</tt>.
 */
std::vector<size_t> GroupIndices(
    size_t interval_start_index,
    size_t interval_end_index,
    size_t num_groups);

/**
 *  @brief Returns a list of the number of elements for each group in an
 *  interval.
 *
 *  An interval encompasses a contiguous sequence of elements to be subdivided
 *  into groups.
 *
 *  @param interval_start_index
 *    Index of the first element in the interval.
 *
 *  @param interval_end_index
 *    Index of the element following the last element in the interval. It must
 *    be no less than <tt>interval_start_index</tt>.
 *
 *  @param num_groups
 *    Number of groups. It must be positive.
 *
 *  @return
 *    List of group sizes.
 */
std::vector<size_t> GroupSizes(
    size_t interval_start_index,
    size_t interval_end_index,
    size_t num_groups);

/**
 *  @brief Returns a list of the start indices for each non-overlap group of
 *  consecutive integers.
 *
 *  @tparam InputIt
 *    <tt>LegacyInputIterator</tt> that can be dereferenced to an integral
 *    type. If not integral, behavior is undefined.
 *
 *  @param input
 *    Input list of integers.
 *
 *  @return
 *    List of start indices and past-the-end index as the last element.
 */
template <typename InputIt>
std::vector<size_t> GroupConsecutive(InputIt first, InputIt last);

} // namespace divider
} // namespace common
} // namespace tanuki

#include "tanuki/common/divider/group_delimiter.hxx"

#endif
