#include "tanuki/interop/c_multi_array.h"

#include <numeric>

std::vector<size_t> CMultiArray::shape() const {
  return std::vector<size_t>(extents, extents + num_dims);
}

size_t CMultiArray::num_items() const {
  const auto extents_vec = shape();

  return std::accumulate(
      extents_vec.begin(), extents_vec.end(), 1,
      [](const size_t &acc, const size_t &item) -> size_t {
        return acc * item;
      });
}
