#ifndef TANUKI_MEMORY_SMART_PTR_H
#define TANUKI_MEMORY_SMART_PTR_H

#include <functional>
#include <memory>

namespace tanuki {
namespace memory {

/**
 *  @brief Unique pointer with a user-defined deleter.
 */
template <typename T>
using UniquePtr = std::unique_ptr<T, std::function<void(T *)>>;

} // namespace memory
} // namespace tanuki

#endif
