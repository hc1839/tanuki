#ifndef TANUKI_INTEROP_FOREIGN_CONTAINER_HXX
#define TANUKI_INTEROP_FOREIGN_CONTAINER_HXX

#include <limits>

namespace tanuki {
namespace interop {

template <typename T, typename S>
auto ForeignContainer<T, S>::begin() -> iterator {
  return iterator(container(), 0);
}

template <typename T, typename S>
auto ForeignContainer<T, S>::end() -> iterator {
  return iterator(container(), size());
}

template <typename T, typename S>
auto ForeignContainer<T, S>::begin() const -> const_iterator {
  return iterator(container(), 0);
}

template <typename T, typename S>
auto ForeignContainer<T, S>::end() const -> const_iterator {
  return iterator(container(), size());
}

template <typename T, typename S>
auto ForeignContainer<T, S>::cbegin() const -> const_iterator {
  return iterator(container(), 0);
}

template <typename T, typename S>
auto ForeignContainer<T, S>::cend() const -> const_iterator {
  return iterator(container(), size());
}

template <typename T, typename S>
auto ForeignContainer<T, S>::size() const -> size_type {
  return container().arr.num_elems;
}

template <typename T, typename S>
auto ForeignContainer<T, S>::max_size() const -> size_type {
  return std::numeric_limits<size_t>::max();
}

template <typename T, typename S>
bool ForeignContainer<T, S>::empty() const {
  return size() == 0;
}

} // namespace interop
} // namespace tanuki

#endif
