#ifndef TANUKI_INTEROP_FOREIGN_CONTAINER_REF_HXX
#define TANUKI_INTEROP_FOREIGN_CONTAINER_REF_HXX

#include <utility>

using std::swap;

namespace tanuki {
namespace interop {

template <typename T, typename S>
ForeignContainerRef<T, S>::ForeignContainerRef(
    const COpaqueContainer &container)
        : container_(container) {}

template <typename T, typename S>
void ForeignContainerRef<T, S>::swap(ForeignContainerRef &other) {
  if (this == &other) {
    return;
  }

  std::swap(container_, other.container_);
}

template <typename T, typename S>
COpaqueContainer ForeignContainerRef<T, S>::container() const {
  return container_;
}

template <typename T, typename S>
void swap(ForeignContainerRef<T, S> &a, ForeignContainerRef<T, S> &b) {
  a.swap(b);
}

} // namespace interop
} // namespace tanuki

#endif
