#ifndef TANUKI_INTEROP_C_MULTI_ARRAY_H
#define TANUKI_INTEROP_C_MULTI_ARRAY_H

#include <cstddef>
#include <vector>

/**
 *  @brief Data structure for passing a multidimensional array of elements with
 *  foreign languages.
 */
extern "C"
typedef struct {
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
   *  @brief Pointer to the beginning of the array, or <tt>nullptr</tt> if the
   *  array is empty.
   */
  void *ptr;

  /**
   *  @brief Copy of @link extents @endlink as an <tt>std::vector</tt>
   *  determined from @link num_dims @endlink and @link extents @endlink only.
   */
  std::vector<size_t> shape() const;

  /**
   *  @brief Number of elements determined from @link shape @endlink only.
   */
  size_t num_elems() const;
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
