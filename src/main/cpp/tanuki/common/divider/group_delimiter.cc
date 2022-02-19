#include "tanuki/common/divider/group_delimiter.h"

#include <cassert>
#include <cstdlib>
#include <utility>

namespace tanuki {
namespace common {
namespace divider {

using std::div;
using std::make_pair;
using std::pair;
using std::vector;

/**
 *  @brief Determines the end (exclusive) index of the specified group in an
 *  interval with a known index of the first element in the group.
 *
 *  An interval encompasses a contiguous sequence of elements to be subdivided
 *  into groups.
 *
 *  It is meant to be called iteratively starting from the first group with
 *  index <tt>0</tt>.
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
 *  @param group_start_index
 *    Index of the first element in the group specified by
 *    <tt>group_index</tt>.
 *
 *  @return
 *    Index of the element following the last element in the group specified by
 *    <tt>group_index</tt>.
 */
size_t DelimitationEndIndex(
    size_t interval_start_index,
    size_t interval_end_index,
    size_t num_groups,
    size_t group_index,
    size_t group_start_index) {
  assert(interval_end_index >= interval_start_index);
  assert(num_groups > 0);
  assert(group_index >= 0 && group_index < num_groups);

  if (group_index == 0) {
    assert(group_start_index == interval_start_index);
  }

  if (interval_end_index == interval_start_index) {
    return interval_end_index;
  }

  // Minimum group size and the remainder.
  auto min_group_size_info = div(
      static_cast<long long>(interval_end_index - interval_start_index),
      static_cast<long long>(num_groups));

  // Size of the specified group.
  size_t group_size = min_group_size_info.quot;

  if (min_group_size_info.rem != 0 && group_index < min_group_size_info.rem) {
    ++group_size;
  }

  return group_start_index + group_size;
}

pair<size_t, size_t> GroupIndexRange(
    size_t interval_start_index,
    size_t interval_end_index,
    size_t num_groups,
    size_t group_index) {
  size_t start_index = interval_start_index;

  for (size_t i = 0; i != group_index; ++i) {
    start_index = DelimitationEndIndex(
        interval_start_index,
        interval_end_index,
        num_groups,
        i,
        start_index);
  }

  size_t end_index = DelimitationEndIndex(
      interval_start_index,
      interval_end_index,
      num_groups,
      group_index,
      start_index);

  return make_pair(start_index, end_index);
}

vector<size_t> GroupIndices(
    size_t interval_start_index,
    size_t interval_end_index,
    size_t num_groups) {
  vector<size_t> retval(1, interval_start_index);

  for (size_t i = 1; i != num_groups; ++i) {
    auto start_index = DelimitationEndIndex(
        interval_start_index,
        interval_end_index,
        num_groups,
        i - 1,
        retval.back());

    retval.push_back(start_index);
  }

  retval.push_back(interval_end_index);

  return retval;
}

vector<size_t> GroupSizes(
    size_t interval_start_index,
    size_t interval_end_index,
    size_t num_groups) {
  size_t start_index = interval_start_index;
  vector<size_t> retval;

  for (size_t i = 0; i != num_groups; ++i) {
    auto end_index = DelimitationEndIndex(
        interval_start_index,
        interval_end_index,
        num_groups,
        i,
        start_index);

    retval.push_back(end_index - start_index);
    start_index = end_index;
  }

  return retval;
}

} // namespace divider
} // namespace common
} // namespace tanuki
