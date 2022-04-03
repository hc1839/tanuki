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
ForeignIterator<T, D>::ForeignIterator(CSequence seq, size_t index)
    : seq_(seq),
      index_(index) {
  if (index != seq.num_items) {
    this->item_ = static_cast<pointer>(std::malloc(sizeof(value_type)));
    if (this->item_ == nullptr) {
      throw std::runtime_error("malloc failed.");
    }
    new(this->item_) T(static_cast<char *>(seq.begin) + seq.item_size * index);
  }
}

template <typename T, typename D>
ForeignIterator<T, D>::ForeignIterator(const ForeignIterator<T, D> &other)
    : seq_(other.seq_),
      index_(other.index_) {
  if (index_ != this->seq_.num_items) {
    this->item_ = static_cast<pointer>(std::malloc(sizeof(value_type)));
    if (this->item_ == nullptr) {
      throw std::runtime_error("malloc failed.");
    }
    new(this->item_) T(
        static_cast<char *>(this->seq_.begin) +
            this->seq_.item_size * this->index_);
  }
}

template <typename T, typename D>
ForeignIterator<T, D> &ForeignIterator<T, D>::operator=(
    const ForeignIterator<T, D> &other) {
  if (this == &other) {
    return *this;
  }

  new(this) ForeignIterator(other);
}

template <typename T, typename D>
ForeignIterator<T, D>::~ForeignIterator() {
  if (item_ != nullptr) {
    item_->~value_type();
    std::free(item_);
  }
}

template <typename T, typename D>
bool ForeignIterator<T, D>::operator==(
    const ForeignIterator<T, D> &other) const {
  return seq_.begin == other.seq_.begin &&
      seq_.item_size == other.seq_.item_size &&
      index_ == other.index_;
}

template <typename T, typename D>
bool ForeignIterator<T, D>::operator!=(
    const ForeignIterator<T, D> &other) const {
  return !(*this == other);
}

template <typename T, typename D>
auto ForeignIterator<T, D>::operator*() -> reference {
  return *item_;
}

template <typename T, typename D>
auto ForeignIterator<T, D>::operator*() const -> const reference {
  return const_cast<const reference>(*item_);
}

template <typename T, typename D>
auto ForeignIterator<T, D>::operator->() const -> pointer {
  return &operator*();
}

template <typename T, typename D>
ForeignIterator<T, D> &ForeignIterator<T, D>::operator++() {
  assert(item_);

  ++index_;
  item_->~value_type();

  if (index_ < seq_.num_items) {
    new(item_) T(static_cast<char *>(seq_.begin) + seq_.item_size * index_);
  } else if (index_ == seq_.num_items) {
    std::free(item_);
    item_ = nullptr;
  }

  return *this;
}

template <typename T, typename D>
ForeignIterator<T, D> ForeignIterator<T, D>::operator++(int) {
  auto old = *this;
  operator++();

  return old;
}

template <typename T, typename D>
void ForeignIterator<T, D>::swap(ForeignIterator<T, D> &other) {
  if (this == &other) {
    return;
  }

  std::swap(seq_, other.seq_);
  std::swap(index_, other.index_);
  std::swap(item_, other.item_);
}

template <typename T, typename D>
void swap(ForeignIterator<T, D> &a, ForeignIterator<T, D> &b) {
  a.swap(b);
}

} // namespace interop
} // namespace tanuki

#endif
