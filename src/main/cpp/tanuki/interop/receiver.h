#ifndef TANUKI_INTEROP_RECEIVER_H
#define TANUKI_INTEROP_RECEIVER_H

#include <functional>

#include "tanuki/interop/c_poly_pointer.h"
#include "tanuki/memory/smart_ptr.h"

namespace tanuki {
namespace interop {

using std::function;

using tanuki::memory::UniquePtr;

/**
 *  @brief Interface of C++ receiver for receiving calls from a foreign
 *  language.
 */
class Receiver {
 public:
  virtual ~Receiver() = default;

  /**
   *  @brief Creates a foreign forwarder to <tt>this</tt> as the receiver.
   */
  UniquePtr<CPolyPointer> CreateForwarder() const;

 protected:
  using ForwarderFactory = function<CPolyPointer(void *)>;

  using ForwarderDeleter = function<void(CPolyPointer)>;

  /**
   *  @brief Factory function that creates a foreign forwarder given
   *  <tt>this</tt> as the receiver.
   *
   *  It should be overridden by a descendant subclass as needed in order to
   *  construct a foreign forwarder of the valid type.
   */
  virtual ForwarderFactory forwarder_factory() const = 0;

  /**
   *  @brief Deleter function that deletes a foreign forwarder without deleting
   *  the receiver.
   *
   *  It should usually not be overridden more than once, since the foreign
   *  forwarder should be responsible for its polymorphic destruction.
   */
  virtual ForwarderDeleter forwarder_deleter() const = 0;
};

} // namespace interop
} // namespace tanuki

#endif
