#ifndef TANUKI_INTEROP_C_SEQUENCE_HXX
#define TANUKI_INTEROP_C_SEQUENCE_HXX

#include <algorithm>
#include <iterator>

template <typename OutputIt>
OutputIt Copy(const CSequence &src, OutputIt dst) {
  // Type of elements being copied.
  using T = typename std::iterator_traits<OutputIt>::value_type;

  std::copy(
      static_cast<T *>(src.begin),
      static_cast<T *>(src.begin) + src.num_items,
      dst);

  return dst;
}

#endif
