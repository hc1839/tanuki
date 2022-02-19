#ifndef TANUKI_INTEROP_FOREIGN_POINTER_H
#define TANUKI_INTEROP_FOREIGN_POINTER_H

#include <functional>

namespace tanuki {
namespace interop {

/**
 *  @brief Decorator to a foreign pointer that is an opaque <tt>void *</tt>.
 */
class ForeignPointer {
 public:
  /**
   *  @brief Default constructs a decorator to <tt>nullptr</tt>.
   */
  ForeignPointer() = default;

  /**
   *  @param ptr
   *    Foreign pointer to decorate.
   */
  explicit ForeignPointer(void *ptr);

  virtual ~ForeignPointer() = default;

  /**
   *  @brief Whether two foreign pointers are the same.
   */
  bool operator==(const ForeignPointer &other) const;

  bool operator!=(const ForeignPointer &other) const;

  /**
   *  @brief Deletes the foreign pointer.
   *
   *  @link ptr @endlink is <tt>nullptr</tt> after the call.
   */
  void Delete();

  /**
   *  @brief Foreign pointer.
   */
  void *ptr() const;

 protected:
  /**
   *  @brief Deleter of the foreign pointer.
   */
  virtual std::function<void(void *)> deleter() const = 0;

  /**
   *  @brief Backing data member for @link ptr @endlink.
   */
  void *ptr_ = nullptr;
};

} // namespace interop
} // namespace tanuki

#endif
