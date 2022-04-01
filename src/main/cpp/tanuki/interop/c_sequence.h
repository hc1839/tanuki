#ifndef TANUKI_INTEROP_C_SEQUENCE_H
#define TANUKI_INTEROP_C_SEQUENCE_H

#include <cstddef>

/**
 *  @brief Data structure for passing an array of elements with foreign
 *  languages.
 *
 *  It is the one-dimensional case of @link CMultiArray @endlink.
 */
extern "C"
typedef struct {
 public:
  /**
   *  @brief Number of elements.
   */
  size_t num_elems;

  /**
   *  @brief Pointer to the beginning of the array.
   */
  void *ptr;
} CSequence;

#endif
