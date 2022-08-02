#ifndef TANUKI_MEMORY_STORAGE_ORDER_H
#define TANUKI_MEMORY_STORAGE_ORDER_H

namespace tanuki {
namespace memory {

/**
 *  @brief Memory layout of data.
 */
enum StorageOrder : int {
  /**
   *  @brief Row-major ordering.
   */
  ROW_MAJOR,

  /**
   *  @brief Column-major ordering.
   */
  COL_MAJOR
};

} // namespace memory
} // namespace tanuki

#endif
