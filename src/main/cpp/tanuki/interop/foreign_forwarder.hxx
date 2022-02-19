#ifndef TANUKI_INTEROP_FOREIGN_FORWARDER_HXX
#define TANUKI_INTEROP_FOREIGN_FORWARDER_HXX

#include <utility>

namespace tanuki {
namespace interop {

template <typename R>
ForeignForwarder<R>::ForeignForwarder(
    void *forwarder, int forwarder_type_code)
        : ForeignPointer(forwarder, forwarder_type_code) {}

template <typename R>
void *ForeignForwarder<R>::forwarder() const {
  return ptr();
}

} // namespace interop
} // namespace tanuki

#endif
