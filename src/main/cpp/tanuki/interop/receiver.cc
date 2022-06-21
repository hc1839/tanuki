#include "tanuki/interop/receiver.h"

namespace tanuki {
namespace interop {

auto Receiver::CreateForwarder() const -> UniquePtr<CPolyPointer> {
  auto this_ = const_cast<Receiver *>(this);
  auto forwarder_deleter_fn = forwarder_deleter();

  return UniquePtr<CPolyPointer>(
      new CPolyPointer(forwarder_factory()(this_)),
      [forwarder_deleter_fn](CPolyPointer *ptr) -> void {
        forwarder_deleter_fn(*ptr);
        delete ptr;
      });
}

} // namespace interop
} // namespace tanuki
