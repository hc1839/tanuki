#ifndef TANUKI_COMMON_DIVIDER_GROUP_DELIMITER_HXX
#define TANUKI_COMMON_DIVIDER_GROUP_DELIMITER_HXX

namespace tanuki {
namespace common {
namespace divider {

using std::vector;

template <typename InputIt>
vector<size_t> GroupConsecutive(InputIt first, InputIt last) {
  if (last == first) {
    return vector<size_t>({ 0, 0 });
  }

  vector<size_t> retval(1, 0);

  auto second = first;
  ++second;

  size_t idx = 1;

  for (auto it = second; it != last; ++it) {
    auto prev = it;
    --prev;

    if (*it - *prev != 1) {
      retval.push_back(idx);
    }

    ++idx;
  }

  retval.push_back(idx);

  return retval;
}

} // namespace divider
} // namespace common
} // namespace tanuki

#endif
