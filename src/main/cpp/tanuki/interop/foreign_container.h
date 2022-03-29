#ifndef TANUKI_INTEROP_FOREIGN_CONTAINER_H
#define TANUKI_INTEROP_FOREIGN_CONTAINER_H

#include <cstddef>
#include <iterator>

#include "tanuki/interop/c_multi_array.h"

namespace tanuki {
namespace interop {

/**
 *  @brief Container that partially satisfies C++ <tt>Container</tt> named
 *  requirement for storing foreign objects.
 *
 *  @tparam Iter
 *    <tt>LegacyForwardIterator</tt> over the foreign objects in the container.
 *
 *  @tparam SizeType
 *    <tt>size_type</tt> in the <tt>Container</tt> named requirement.
 */
template <typename Iter, typename SizeType = size_t>
class ForeignContainer {
 public:
  using value_type = typename std::iterator_traits<Iter>::value_type;

  using reference = value_type &;

  using const_reference = const reference;

  using iterator = Iter;

  using const_iterator = const iterator;

  using difference_type = typename std::iterator_traits<Iter>::difference_type;

  using size_type = SizeType;

  virtual ~ForeignContainer() = default;

  virtual iterator begin() = 0;

  virtual iterator end() = 0;

  virtual const_iterator begin() const = 0;

  virtual const_iterator end() const = 0;

  virtual const_iterator cbegin() const = 0;

  virtual const_iterator cend() const = 0;

  virtual size_type size() const = 0;

  virtual size_type max_size() const = 0;

  virtual bool empty() const = 0;

  /**
   *  @brief Pointer to the array of foreign objects.
   */
  virtual CMultiArray ptr() const = 0;
};

} // namespace interop
} // namespace tanuki

#endif
