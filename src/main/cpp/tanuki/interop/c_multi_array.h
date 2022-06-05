#ifndef TANUKI_INTEROP_C_MULTI_ARRAY_H
#define TANUKI_INTEROP_C_MULTI_ARRAY_H

#include <cstddef>
#include <vector>

#include "tanuki/interop/c_sequence.h"

/**
 *  @brief Data structure for passing a multidimensional array of items with
 *  foreign languages.
 */
extern "C"
typedef struct {
  /**
   *  @brief Pointer to the beginning of the array.
   */
  void *begin;

  /**
   *  @brief Number of dimensions of the array.
   */
  size_t num_dims;

  /**
   *  @brief Array with @link num_dims @endlink elements that specifies the
   *  extent of each dimension.
   */
  size_t *extents;

  /**
   *  @brief Size in bytes of each item.
   */
  size_t item_size;

  /**
   *  @brief Copy of @link extents @endlink as an <tt>std::vector</tt>
   *  determined from @link num_dims @endlink and @link extents @endlink only.
   */
  std::vector<size_t> shape() const;

  /**
   *  @brief Number of items in the array determined from @link shape @endlink
   *  only.
   */
  size_t num_items() const;
} CMultiArray;

/**
 *  @brief Copies @link CMultiArray @endlink to a new location.
 *
 *  @tparam OutputIt
 *    Must meet the requirements of <tt>LegacyOutputIterator</tt>.
 *
 *  @param src
 *    @link CMultiArray @endlink to copy.
 *
 *  @param dst
 *    Beginning of the destination range.
 *
 *  @return
 *    Iterator to past-the-end element in the destination range.
 */
template <typename OutputIt>
OutputIt Copy(const CMultiArray &src, OutputIt dst);

#include "tanuki/interop/c_multi_array.hxx"

#endif
