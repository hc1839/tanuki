#include "tanuki/interop/foreign_pointer.h"

namespace tanuki {
namespace interop {

ForeignPointer::ForeignPointer(void *ptr) : ptr_(ptr) {}

bool ForeignPointer::operator==(const ForeignPointer &other) const {
  return typeid(*this) == typeid(other) &&
      ptr() == other.ptr();
}

bool ForeignPointer::operator!=(const ForeignPointer &other) const {
  return !(*this == other);
}

void ForeignPointer::Delete() {
  if (ptr_ != nullptr) {
    deleter()(ptr_);
    ptr_ = nullptr;
  }
}

void *ForeignPointer::ptr() const {
  return ptr_;
}

} // namespace interop
} // namespace tanuki
