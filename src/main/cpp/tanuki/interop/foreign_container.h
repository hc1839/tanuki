#ifndef TANUKI_INTEROP_FOREIGN_CONTAINER_H
#define TANUKI_INTEROP_FOREIGN_CONTAINER_H

#include <cstddef>
#include <iterator>

#include "tanuki/interop/foreign_iterator.h"

namespace tanuki {
namespace interop {

/**
 *  @brief Container that partially satisfies C++ <tt>Container</tt> named
 *  requirement for wrapping a foreign array of opaque items.
 *
 *  @tparam T
 *    Type of decorated items being iterated over. See @link ForeignIterator
 *    @endlink for the requirements.
 *
 *  @tparam S
 *    <tt>size_type</tt> in the <tt>Container</tt> named requirement.
 */
template <typename T, typename S = size_t>
class ForeignContainer {
 public:
  using value_type = T;

  using reference = value_type &;

  using const_reference = const reference;

  using iterator = ForeignIterator<T>;

  using const_iterator = const iterator;

  using difference_type =
      typename std::iterator_traits<iterator>::difference_type;

  using size_type = S;

  virtual ~ForeignContainer() = default;

  iterator begin();

  iterator end();

  const_iterator begin() const;

  const_iterator end() const;

  const_iterator cbegin() const;

  const_iterator cend() const;

  size_type size() const;

  size_type max_size() const;

  bool empty() const;

  /**
   *  @brief Foreign sequence backing <tt>this</tt> container.
   */
  virtual CSequence seq() const = 0;
};

} // namespace interop
} // namespace tanuki

#include "tanuki/interop/foreign_container.hxx"

#endif
