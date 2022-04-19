#ifndef TANUKI_STATE_SPIN_RANGE_H
#define TANUKI_STATE_SPIN_RANGE_H

#include <cstddef>
#include <iterator>
#include <type_traits>
#include <vector>

#include "tanuki/state/spin.h"

namespace tanuki {
namespace state {

using std::vector;

/**
 *  @brief Range of <tt>const</tt> elements that are associated by an effective
 *  spin enumerator.
 *
 *  Effective spin enumerator is defined as a @link Spin @endlink enumerator
 *  that associates with the <tt>const</tt> value associated by @link
 *  Spin::DUAL @endlink for when a species is not spin-polarized. For instance,
 *  if a species is not spin-polarized, the @link Spin::UP @endlink and @link
 *  Spin::DOWN @endlink enumerators associate with the same <tt>const</tt>
 *  value that is associated by @link Spin::DUAL @endlink. Without the
 *  "effective" qualifier, a spin enumerator is to be taken exactly as one of
 *  the @link Spin @endlink enumerators without the aforementioned
 *  interpretation.
 *
 *  @tparam T
 *    Type of elements in the spin range.
 */
template <typename T>
class SpinRange final {
 public:
  class Iterator;

  using value_type = T;

  using reference = value_type &;

  using const_reference = const reference;

  using iterator = Iterator;

  using const_iterator = const iterator;

  using difference_type =
      typename std::iterator_traits<iterator>::difference_type;

  using size_type = size_t;

  /**
   *  @brief Iterator over the elements in the spin range.
   */
  class Iterator final {
   public:
    using iterator_category = std::forward_iterator_tag;

    using value_type = T;

    using pointer = value_type *;

    using reference = value_type &;

    using difference_type = long;

    bool operator==(const Iterator &other) const;

    bool operator!=(const Iterator &other) const;

    const T &operator*() const;

    const T *operator->() const;

    Iterator &operator++();

    Iterator operator++(int);

    void swap(Iterator &other);

    friend SpinRange<T>;

   private:
    /**
     *  @param ptr
     *    Backing pointer to the pointer to the element. It must be contiguous
     *    with other backing pointers for the same spin range.
     */
    Iterator(const T **ptr);

    /**
     *  @brief Backing pointer to the pointer to the element.
     *
     *  It is contiguous with other backing pointers for the same spin range.
     */
    const T **ptr_;
  };

  /**
   *  @brief Constructs an empty spin range.
   */
  SpinRange() = default;

  /**
   *  @brief Eagerly constructs a spin range from a range of pointers.
   *
   *  Pointers in the input range are stored in a backing container without
   *  taking ownership.
   *
   *  @tparam InputIt
   *    Must meet the requirements of <tt>LegacyInputIterator</tt>  and have a
   *    dereferenced type that is convertible to <tt>const T *</tt>.
   */
  template <typename InputIt,
      typename std::enable_if<
          std::is_convertible<
              typename std::iterator_traits<InputIt>::value_type,
              const T *>::value,
          bool>::type = true>
  SpinRange(InputIt first, InputIt last, Spin eff_spin);

  /**
   *  @brief Whether the spin range is not empty.
   */
  operator bool() const;

  /**
   *  @brief Iterator to the beginning of the spin range.
   */
  iterator begin() const;

  /**
   *  @brief Iterator to past-the-end of the spin range.
   */
  iterator end() const;

  /**
   *  @brief Whether the spin range is empty.
   */
  bool empty() const;

  /**
   *  @brief Number of elements in the spin range.
   */
  size_type size() const;

  /**
   *  @brief Effective spin enumerator.
   */
  Spin eff_spin() const;

 private:
  /**
   *  @brief Backing data member for @link eff_spin @endlink.
   */
  Spin eff_spin_;

  /**
   *  @brief Backing sequence of pointers to the <tt>const</tt> elements in the
   *  spin range.
   */
  vector<const T *> seq_;
};

template <typename T>
void swap(
    typename SpinRange<T>::Iterator &a,
    typename SpinRange<T>::Iterator &b);

} // namespace state
} // namespace tanuki

#include "tanuki/state/spin_range.hxx"

#endif
