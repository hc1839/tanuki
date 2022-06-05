#ifndef TANUKI_INTEROP_C_MULTI_ARRAY_HXX
#define TANUKI_INTEROP_C_MULTI_ARRAY_HXX

#include <algorithm>
#include <iterator>

template <typename OutputIt>
OutputIt Copy(const CMultiArray &src, OutputIt dst) {
  // Type of elements being copied.
  using T = typename std::iterator_traits<OutputIt>::value_type;

  std::copy(
      static_cast<T *>(src.begin),
      static_cast<T *>(src.begin) + src.num_items(),
      dst);

  return dst;
}

#endif
