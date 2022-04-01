#ifndef TANUKI_INTEROP_FOREIGN_FORWARD_ITERATOR_HXX
#define TANUKI_INTEROP_FOREIGN_FORWARD_ITERATOR_HXX

#include <cassert>
#include <cstdlib>
#include <stdexcept>
#include <utility>

using std::swap;

namespace tanuki {
namespace interop {

template <typename T, typename D>
ForeignForwardIterator<T, D>::ForeignForwardIterator(
    const COpaqueContainer &container, size_t index)
        : container_(container),
          index_(index) {
  if (index != container.arr.num_elems()) {
    this->item_ = static_cast<pointer>(std::malloc(sizeof(value_type)));
    if (this->item_ == nullptr) {
      throw std::runtime_error("malloc failed.");
    }
    new(this->item_) T(
        static_cast<char *>(container.arr.ptr) + container.elem_size * index);
  }
}

template <typename T, typename D>
ForeignForwardIterator<T, D>::ForeignForwardIterator(
    const ForeignForwardIterator<T, D> &other)
        : container_(other.container_),
          index_(other.index_) {
  if (index_ != this->container_.arr.num_elems()) {
    this->item_ = static_cast<pointer>(std::malloc(sizeof(value_type)));
    if (this->item_ == nullptr) {
      throw std::runtime_error("malloc failed.");
    }
    new(this->item_) T(
        static_cast<char *>(this->container_.arr.ptr) +
            this->container_.elem_size * this->index_);
  }
}

template <typename T, typename D>
ForeignForwardIterator<T, D> &ForeignForwardIterator<T, D>::operator=(
    const ForeignForwardIterator<T, D> &other) {
  if (this == &other) {
    return *this;
  }

  new(this) ForeignForwardIterator(other);
}

template <typename T, typename D>
ForeignForwardIterator<T, D>::~ForeignForwardIterator() {
  if (item_ != nullptr) {
    item_->~value_type();
    std::free(item_);
  }
}

template <typename T, typename D>
bool ForeignForwardIterator<T, D>::operator==(
    const ForeignForwardIterator<T, D> &other) const {
  return container_.arr.ptr == other.container_.arr.ptr &&
      container_.elem_size == other.container_.elem_size &&
      index_ == other.index_;
}

template <typename T, typename D>
bool ForeignForwardIterator<T, D>::operator!=(
    const ForeignForwardIterator<T, D> &other) const {
  return !(*this == other);
}

template <typename T, typename D>
auto ForeignForwardIterator<T, D>::operator*() -> reference {
  return *item_;
}

template <typename T, typename D>
auto ForeignForwardIterator<T, D>::operator*() const -> const reference {
  return const_cast<const reference>(*item_);
}

template <typename T, typename D>
auto ForeignForwardIterator<T, D>::operator->() const -> pointer {
  return &operator*();
}

template <typename T, typename D>
ForeignForwardIterator<T, D> &ForeignForwardIterator<T, D>::operator++() {
  assert(item_);

  ++index_;
  item_->~value_type();

  if (index_ < container_.arr.num_elems()) {
    void *opaque_ptr = static_cast<char *>(container_.arr.ptr) +
        container_.elem_size * index_;
    new(item_) T(opaque_ptr);
  } else if (index_ == container_.arr.num_elems()) {
    std::free(item_);
    item_ = nullptr;
  }

  return *this;
}

template <typename T, typename D>
ForeignForwardIterator<T, D> ForeignForwardIterator<T, D>::operator++(int) {
  auto old = *this;
  operator++();

  return old;
}

template <typename T, typename D>
void ForeignForwardIterator<T, D>::swap(ForeignForwardIterator<T, D> &other) {
  if (this == &other) {
    return;
  }

  std::swap(container_, other.container_);
  std::swap(index_, other.index_);
  std::swap(item_, other.item_);
}

template <typename T, typename D>
void swap(ForeignForwardIterator<T, D> &a, ForeignForwardIterator<T, D> &b) {
  a.swap(b);
}

} // namespace interop
} // namespace tanuki

#endif
