#ifndef TANUKI_INTEROP_FOREIGN_CONTAINER_REF_H
#define TANUKI_INTEROP_FOREIGN_CONTAINER_REF_H

#include "tanuki/interop/foreign_container.h"

namespace tanuki {
namespace interop {

/**
 *  @brief Reference to a foreign container without taking ownership.
 *
 *  Copy/move construction and operation replace the current pointer with the
 *  one from the other instance. The foreign array itself is not copied.
 *
 *  @tparam T
 *    Type of decorated elements being iterated over. See @link
 *    ForeignForwardIterator @endlink for the requirements.
 *
 *  @tparam S
 *    <tt>size_type</tt> in the <tt>Container</tt> named requirement.
 */
template <typename T, typename S = size_t>
class ForeignContainerRef final : public ForeignContainer<T, S> {
 public:
  ForeignContainerRef() = default;

  /**
   *  @param container
   *    Data of the pointer to the foreign array.
   */
  ForeignContainerRef(const COpaqueContainer &container);

  virtual ~ForeignContainerRef() = default;

  void swap(ForeignContainerRef &other);

  COpaqueContainer container() const override final;

 private:
  /**
   *  @brief Backing data member for @link container @endlink.
   */
  COpaqueContainer container_;
};

template <typename T, typename S>
void swap(ForeignContainerRef<T, S> &a, ForeignContainerRef<T, S> &b);

} // namespace interop
} // namespace tanuki

#include "tanuki/interop/foreign_container_ref.hxx"

#endif
