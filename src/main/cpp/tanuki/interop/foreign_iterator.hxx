#ifndef TANUKI_INTEROP_FOREIGN_ITERATOR_HXX
#define TANUKI_INTEROP_FOREIGN_ITERATOR_HXX

#include <cassert>
#include <cstdlib>
#include <stdexcept>
#include <utility>

using std::swap;

namespace tanuki {
namespace interop {

template <typename T, typename D>
ForeignIterator<T, D>::ForeignIterator(void *ptr, size_t item_size)
    : ptr_(ptr),
      item_size_(item_size) {
  this->item_ = unique_ptr<value_type, DecoratorDeleter>(
      static_cast<pointer>(std::malloc(sizeof(value_type))),
      [](pointer item) -> void {
        item->~value_type();
        std::free(item);
      });

  if (!this->item_) {
    throw std::runtime_error("malloc failed.");
  }

  new(this->item_.get()) T(this->ptr_);
}

template <typename T, typename D>
ForeignIterator<T, D>::ForeignIterator(const ForeignIterator<T, D> &other)
    : ForeignIterator(other.ptr_, other.item_size_) {}

template <typename T, typename D>
ForeignIterator<T, D> &ForeignIterator<T, D>::operator=(
    const ForeignIterator<T, D> &other) {
  if (this == &other) {
    return *this;
  }

  new(this) ForeignIterator(other);
}

template <typename T, typename D>
bool ForeignIterator<T, D>::operator==(
    const ForeignIterator<T, D> &rhs) const {
  return ptr_ == rhs.ptr_ &&
      item_size_ == rhs.item_size_;
}

template <typename T, typename D>
bool ForeignIterator<T, D>::operator!=(
    const ForeignIterator<T, D> &rhs) const {
  return !(*this == rhs);
}

template <typename T, typename D>
auto ForeignIterator<T, D>::operator*() const -> value_type {
  return *item_;
}

template <typename T, typename D>
auto ForeignIterator<T, D>::operator->() const -> pointer {
  return item_.get();
}

template <typename T, typename D>
ForeignIterator<T, D> &ForeignIterator<T, D>::operator++() {
  ptr_ = static_cast<char *>(ptr_) + item_size_;
  item_->~value_type();
  new(item_.get()) T(ptr_);
}

template <typename T, typename D>
ForeignIterator<T, D> ForeignIterator<T, D>::operator++(int) {
  auto old = *this;
  operator++();
  return old;
}

template <typename T, typename D>
ForeignIterator<T, D> &ForeignIterator<T, D>::operator--() {
  ptr_ = static_cast<char *>(ptr_) - item_size_;
  item_->~value_type();
  new(item_.get()) T(ptr_);
}

template <typename T, typename D>
ForeignIterator<T, D> ForeignIterator<T, D>::operator--(int) {
  auto old = *this;
  operator--();
  return old;
}

template <typename T, typename D>
void ForeignIterator<T, D>::swap(ForeignIterator<T, D> &other) {
  if (this == &other) {
    return;
  }

  std::swap(ptr_, other.ptr_);
  std::swap(item_size_, other.item_size_);
  std::swap(item_, other.item_);
}

template <typename T, typename D>
void swap(ForeignIterator<T, D> &a, ForeignIterator<T, D> &b) {
  a.swap(b);
}

} // namespace interop
} // namespace tanuki

#endif
