#ifndef TANUKI_INTEROP_C_SEQUENCE_H
#define TANUKI_INTEROP_C_SEQUENCE_H

#include <cstddef>

/**
 *  @brief Data structure for passing a foreign sequence where each item has
 *  the same size.
 */
extern "C"
typedef struct {
 public:
  /**
   *  @brief Pointer to the beginning of the sequence.
   */
  void *begin;

  /**
   *  @brief Number of items in the sequence.
   */
  size_t num_items;

  /**
   *  @brief Size in bytes of each item.
   */
  size_t item_size;
} CSequence;

/**
 *  @brief Copies @link CSequence @endlink to a new location.
 *
 *  @tparam OutputIt
 *    Must meet the requirements of <tt>LegacyOutputIterator</tt>.
 *
 *  @param src
 *    @link CSequence @endlink to copy.
 *
 *  @param dst
 *    Beginning of the destination range.
 *
 *  @return
 *    Iterator to past-the-end element in the destination range.
 */
template <typename OutputIt>
OutputIt Copy(const CSequence &src, OutputIt dst);

#include "tanuki/interop/c_sequence.hxx"

#endif
