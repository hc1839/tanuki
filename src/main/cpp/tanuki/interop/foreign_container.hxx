#ifndef TANUKI_INTEROP_FOREIGN_CONTAINER_HXX
#define TANUKI_INTEROP_FOREIGN_CONTAINER_HXX

#include <algorithm>
#include <limits>
#include <stdexcept>

namespace tanuki {
namespace interop {

template <typename T, typename S>
auto ForeignContainer<T, S>::operator[](size_type idx) const -> value_type {
  return begin()[idx];
}

template <typename T, typename S>
auto ForeignContainer<T, S>::begin() -> iterator {
  return iterator(seq().begin, 0, seq().item_size);
}

template <typename T, typename S>
auto ForeignContainer<T, S>::end() -> iterator {
  return iterator(
      static_cast<char *>(seq().begin) + seq().item_size * seq().num_items,
      seq().num_items,
      seq().item_size);
}

template <typename T, typename S>
auto ForeignContainer<T, S>::begin() const -> const_iterator {
  return iterator(seq().begin, 0, seq().item_size);
}

template <typename T, typename S>
auto ForeignContainer<T, S>::end() const -> const_iterator {
  return iterator(
      static_cast<char *>(seq().begin) + seq().item_size * seq().num_items,
      seq().num_items,
      seq().item_size);
}

template <typename T, typename S>
auto ForeignContainer<T, S>::cbegin() const -> const_iterator {
  return iterator(seq().begin, 0, seq().item_size);
}

template <typename T, typename S>
auto ForeignContainer<T, S>::cend() const -> const_iterator {
  return iterator(
      static_cast<char *>(seq().begin) + seq().item_size * seq().num_items,
      seq().num_items,
      seq().item_size);
}

template <typename T, typename S>
auto ForeignContainer<T, S>::size() const -> size_type {
  return seq().num_items;
}

template <typename T, typename S>
auto ForeignContainer<T, S>::max_size() const -> size_type {
  return std::numeric_limits<size_t>::max();
}

template <typename T, typename S>
bool ForeignContainer<T, S>::empty() const {
  return size() == 0;
}

template <typename T, typename S>
auto ForeignContainer<T, S>::front() const -> value_type {
  return *begin();
}

template <typename T, typename S>
auto ForeignContainer<T, S>::back() const -> value_type {
  auto tmp = end();
  --tmp;
  return *tmp;
}

template <typename T, typename S>
auto ForeignContainer<T, S>::at(size_type idx) const -> value_type {
  if (idx >= size()) {
    throw std::out_of_range("Out of range for foreign container.");
  }

  return operator[](idx);
}

} // namespace interop
} // namespace tanuki

#endif
