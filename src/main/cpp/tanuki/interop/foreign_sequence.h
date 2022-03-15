#ifndef TANUKI_INTEROP_FOREIGN_SEQUENCE_H
#define TANUKI_INTEROP_FOREIGN_SEQUENCE_H

#include <functional>

#include "tanuki/interop/c_poly_pointer.h"
#include "tanuki/interop/foreign_pointer.h"

namespace tanuki {
namespace interop {

/**
 *  @brief Wrapper to a foreign sequence of items as an opaque <tt>void *</tt>.
 *
 *  Indices are zero-based.
 *
 *  @tparam T
 *    Type of items.
 */
template <typename T>
class ForeignSequence : public ForeignPointer {
 public:
  /**
   *  @brief Default constructs a wrapper to <tt>nullptr</tt>.
   */
  ForeignSequence() = default;

  /**
   *  @brief Constructs a wrapper to a specified foreign sequence.
   */
  explicit ForeignSequence(void *ptr);

  virtual ~ForeignSequence() = default;

  /**
   *  @brief Item at the specified <tt>index</tt>.
   */
  virtual T Item(size_t index) = 0;

  virtual const T Item(size_t index) const = 0;

  /**
   *  @brief Sets an item at the specified <tt>index</tt>.
   */
  virtual void SetItem(size_t index, const T &item) = 0;

  virtual size_t Count() const = 0;
};

} // namespace interop
} // namespace tanuki

#include "tanuki/interop/foreign_sequence.hxx"

#endif
