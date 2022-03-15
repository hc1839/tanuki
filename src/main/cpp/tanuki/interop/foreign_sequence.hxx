#ifndef TANUKI_INTEROP_FOREIGN_SEQUENCE_HXX
#define TANUKI_INTEROP_FOREIGN_SEQUENCE_HXX

#include <utility>

namespace tanuki {
namespace interop {

template <typename T>
ForeignSequence<T>::ForeignSequence(void *ptr) : ForeignPointer(ptr) {}

} // namespace interop
} // namespace tanuki

#endif
