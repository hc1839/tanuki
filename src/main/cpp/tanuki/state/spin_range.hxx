#ifndef TANUKI_STATE_SPIN_RANGE_HXX
#define TANUKI_STATE_SPIN_RANGE_HXX

#include <utility>

using std::swap;

namespace tanuki {
namespace state {

template <typename T>
bool SpinRange<T>::Iterator::operator==(const Iterator &other) const {
  return ptr_ == other.ptr_;
}

template <typename T>
bool SpinRange<T>::Iterator::operator!=(const Iterator &other) const {
  return !(*this == other);
}

template <typename T>
const T &SpinRange<T>::Iterator::operator*() const {
  return const_cast<const reference>(**ptr_);
}

template <typename T>
const T *SpinRange<T>::Iterator::operator->() const {
  return &operator*();
}

template <typename T>
auto SpinRange<T>::Iterator::operator++() -> Iterator & {
  ++ptr_;

  return *this;
}

template <typename T>
auto SpinRange<T>::Iterator::operator++(int) -> Iterator {
  auto old = *this;
  operator++();

  return old;
}

template <typename T>
void SpinRange<T>::Iterator::swap(Iterator &other) {
  swap(ptr_, other.ptr_);
}

template <typename T>
SpinRange<T>::Iterator::Iterator(const T **ptr) : ptr_(ptr) {}

template <typename T>
void swap(
    typename SpinRange<T>::Iterator &a,
    typename SpinRange<T>::Iterator &b) {
  a.swap(b);
}

template <typename T>
template <typename InputIt,
    typename std::enable_if<
        std::is_convertible<
            typename std::iterator_traits<InputIt>::value_type,
            const T *>::value,
        bool>::type>
SpinRange<T>::SpinRange(InputIt first, InputIt last, Spin eff_spin)
    : eff_spin_(eff_spin) {
  for (auto it = first; it != last; ++it) {
    seq_.push_back(*it);
  }
}

template <typename T>
SpinRange<T>::operator bool() const {
  return !empty();
}

template <typename T>
auto SpinRange<T>::begin() const -> iterator {
  return Iterator(const_cast<const T **>(seq_.data()));
}

template <typename T>
auto SpinRange<T>::end() const -> iterator {
  return Iterator(const_cast<const T **>(seq_.data()) + seq_.size());
}

template <typename T>
bool SpinRange<T>::empty() const {
  return seq_.empty();
}

template <typename T>
auto SpinRange<T>::size() const -> size_type {
  return seq_.size();
}

template <typename T>
Spin SpinRange<T>::eff_spin() const {
  return eff_spin_;
}

} // namespace state
} // namespace tanuki

#endif
