#ifndef TANUKI_INTEROP_FOREIGN_ITERATOR_H
#define TANUKI_INTEROP_FOREIGN_ITERATOR_H

#include <cstddef>
#include <functional>
#include <iterator>
#include <memory>

#include "tanuki/interop/c_sequence.h"

namespace tanuki {
namespace interop {

using std::function;
using std::unique_ptr;

/**
 *  @brief Iterator over decorated opaque items of constant size.
 *
 *  @tparam T
 *    Type of decorated opaque items being iterated over. It must
 *      - have a constructor, <tt>T(void *)</tt>, that wraps the given pointer
 *        to the opaque item,
 *      - meet the requirements of <tt>CopyConstructible</tt> without copying
 *        the opaque item,
 *      - meet the requirements of <tt>Destructible</tt> without deleting the
 *        opaque item,
 *      - not take ownership of the pointer, and
 *      - maintain the same pointer throughout its lifetime.
 *
 *  @tparam D
 *    <tt>std::iterator_traits::difference_type</tt>.
 */
template <typename T, typename D = long>
class ForeignIterator final {
 public:
  using iterator_category = std::random_access_iterator_tag;

  using value_type = T;

  using pointer = value_type *;

  using reference = value_type &;

  using difference_type = D;

  ForeignIterator() = default;

  /**
   *  @param ptr
   *    Pointer to the opaque item.
   *
   *  @param item_index
   *    Zero-based index of the opaque item.
   *
   *  @param item_size
   *    Size in bytes of each item in the range that the item pointed by
   *    <tt>ptr</tt> is in.
   */
  ForeignIterator(void *ptr, size_t item_index, size_t item_size);

  ForeignIterator(const ForeignIterator<T, D> &other);

  ForeignIterator &operator=(const ForeignIterator<T, D> &other);

  ForeignIterator<T, D> &operator+=(difference_type rhs);

  ForeignIterator<T, D> &operator-=(difference_type rhs);

  bool operator==(const ForeignIterator<T, D> &rhs) const;

  bool operator!=(const ForeignIterator<T, D> &rhs) const;

  value_type operator*() const;

  pointer operator->() const;

  ForeignIterator<T, D> &operator++();

  ForeignIterator<T, D> operator++(int);

  ForeignIterator<T, D> &operator--();

  ForeignIterator<T, D> operator--(int);

  ForeignIterator<T, D> operator-(difference_type rhs) const;

  difference_type operator-(const ForeignIterator<T, D> &rhs) const;

  value_type operator[](difference_type idx) const;

  void swap(ForeignIterator<T, D> &other);

 private:
  using DecoratorDeleter = function<void(pointer)>;

  /**
   *  @brief Pointer to the opaque item.
   */
  void *ptr_;

  /**
   *  @brief Zero-based index of the opaque item.
   */
  size_t item_index_;

  /**
   *  @brief Size in bytes of each item in the range that the item pointed by
   *  @link ptr_ @endlink is in.
   */
  size_t item_size_;

  /**
   *  @brief Decorated opaque item.
   */
  unique_ptr<value_type, DecoratorDeleter> item_;
};

template <typename T, typename D>
ForeignIterator<T, D> operator+(ForeignIterator<T, D> lhs, D rhs);

template <typename T, typename D>
ForeignIterator<T, D> operator+(D lhs, ForeignIterator<T, D> rhs);

template <typename T, typename D>
bool operator<(
    const ForeignIterator<T, D> &lhs, const ForeignIterator<T, D> &rhs);

template <typename T, typename D>
bool operator>(
    const ForeignIterator<T, D> &lhs, const ForeignIterator<T, D> &rhs);

template <typename T, typename D>
bool operator<=(
    const ForeignIterator<T, D> &lhs, const ForeignIterator<T, D> &rhs);

template <typename T, typename D>
bool operator>=(
    const ForeignIterator<T, D> &lhs, const ForeignIterator<T, D> &rhs);

template <typename T, typename D>
void swap(ForeignIterator<T, D> &a, ForeignIterator<T, D> &b);

} // namespace interop
} // namespace tanuki

#include "tanuki/interop/foreign_iterator.hxx"

#endif
