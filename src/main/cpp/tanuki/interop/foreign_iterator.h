#ifndef TANUKI_INTEROP_FOREIGN_ITERATOR_H
#define TANUKI_INTEROP_FOREIGN_ITERATOR_H

#include <cstddef>
#include <iterator>

#include "tanuki/interop/c_sequence.h"

namespace tanuki {
namespace interop {

/**
 *  @brief Iterator over decorated opaque items of constant size.
 *
 *  @tparam T
 *    Type of decorated items being iterated over. It must
 *      - have a constructor, <tt>T(void *)</tt>, that wraps the given pointer
 *        to the opaque item,
 *      - not take ownership of the opaque pointer,
 *      - maintain the same opaque pointer throughout its lifetime, and
 *      - meet the requirements of <tt>Destructible</tt>.
 *
 *  @tparam D
 *    <tt>std::iterator_traits::difference_type</tt>.
 */
template <typename T, typename D = long>
class ForeignIterator final {
 public:
  using iterator_category = std::forward_iterator_tag;

  using value_type = T;

  using pointer = value_type *;

  using reference = value_type &;

  using difference_type = D;

  ForeignIterator() = default;

  /**
   *  @param seq
   *    Foreign sequence.
   *
   *  @param index
   *    Zero-based index of the item. Valid index ranges from zero to the
   *    number of items, inclusive, where the latter represents the end
   *    iterator.
   */
  ForeignIterator(CSequence seq, size_t index);

  ForeignIterator(const ForeignIterator<T, D> &other);

  ForeignIterator &operator=(const ForeignIterator<T, D> &other);

  ~ForeignIterator();

  bool operator==(const ForeignIterator<T, D> &other) const;

  bool operator!=(const ForeignIterator<T, D> &other) const;

  reference operator*();

  const reference operator*() const;

  pointer operator->() const;

  ForeignIterator<T, D> &operator++();

  ForeignIterator<T, D> operator++(int);

  void swap(ForeignIterator<T, D> &other);

 private:
  /**
   *  @brief Foreign sequence.
   */
  CSequence seq_;

  /**
   *  @brief Zero-based index of the item.
   */
  size_t index_;

  /**
   *  @brief Placeholder for the decorated opaque item, or <tt>nullptr</tt> if
   *  <tt>this</tt> is an end iterator.
   */
  pointer item_ = nullptr;
};

template <typename T, typename D>
void swap(ForeignIterator<T, D> &a, ForeignIterator<T, D> &b);

} // namespace interop
} // namespace tanuki

#include "tanuki/interop/foreign_iterator.hxx"

#endif
