#ifndef TANUKI_INTEROP_FOREIGN_FORWARDER_H
#define TANUKI_INTEROP_FOREIGN_FORWARDER_H

#include <memory>

#include "tanuki/interop/foreign_pointer.h"

namespace tanuki {
namespace interop {

/**
 *  @brief Wrapper to a foreign sender in the forwarding pattern that forwards
 *  to a C++ receiver.
 *
 *  Foreign forwarder does not take ownership of the C++ receiver.
 *
 *  @tparam R
 *    C++ receiver.
 */
template <typename R>
class ForeignForwarder : public ForeignPointer {
 public:
  /**
   *  @brief Default constructs a wrapper to <tt>nullptr</tt>.
   */
  ForeignForwarder() = default;

  /**
   *  @brief Constructs a wrapper to a specified foreign forwarder.
   *
   *  See @link ForeignPointer @endlink for <tt>forwarder_type_code</tt>.
   */
  explicit ForeignForwarder(void *forwarder, int forwarder_type_code = 0);

  virtual ~ForeignForwarder() = default;

  /**
   *  @brief Foreign forwarder.
   *
   *  It serves to rename @link ptr @endlink by delegating to it.
   */
  void *forwarder() const;

  /**
   *  @brief Pointer to the receiver.
   */
  virtual R *receiver() const = 0;
};

} // namespace interop
} // namespace tanuki

#include "tanuki/interop/foreign_forwarder.hxx"

#endif
