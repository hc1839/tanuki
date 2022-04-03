#ifndef TANUKI_INTEROP_FOREIGN_CONTAINER_REF_H
#define TANUKI_INTEROP_FOREIGN_CONTAINER_REF_H

#include "tanuki/interop/c_sequence.h"
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
 *    Type of decorated items being iterated over. See @link ForeignIterator
 *    @endlink for the requirements.
 *
 *  @tparam S
 *    <tt>size_type</tt> in the <tt>Container</tt> named requirement.
 */
template <typename T, typename S = size_t>
class ForeignContainerRef final : public ForeignContainer<T, S> {
 public:
  ForeignContainerRef() = default;

  /**
   *  @param seq
   *    Foreign sequence.
   */
  ForeignContainerRef(CSequence seq);

  virtual ~ForeignContainerRef() = default;

  void swap(ForeignContainerRef &other);

  CSequence seq() const override final;

 private:
  /**
   *  @brief Backing data member for @link seq @endlink.
   */
  CSequence seq_;
};

template <typename T, typename S>
void swap(ForeignContainerRef<T, S> &a, ForeignContainerRef<T, S> &b);

} // namespace interop
} // namespace tanuki

#include "tanuki/interop/foreign_container_ref.hxx"

#endif
