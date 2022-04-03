#ifndef TANUKI_INTEROP_FOREIGN_CONTAINER_REF_HXX
#define TANUKI_INTEROP_FOREIGN_CONTAINER_REF_HXX

#include <utility>

using std::swap;

namespace tanuki {
namespace interop {

template <typename T, typename S>
ForeignContainerRef<T, S>::ForeignContainerRef(CSequence seq)
    : seq_(seq) {}

template <typename T, typename S>
void ForeignContainerRef<T, S>::swap(ForeignContainerRef &other) {
  if (this == &other) {
    return;
  }

  std::swap(seq_, other.seq_);
}

template <typename T, typename S>
CSequence ForeignContainerRef<T, S>::seq() const {
  return seq_;
}

template <typename T, typename S>
void swap(ForeignContainerRef<T, S> &a, ForeignContainerRef<T, S> &b) {
  a.swap(b);
}

} // namespace interop
} // namespace tanuki

#endif
