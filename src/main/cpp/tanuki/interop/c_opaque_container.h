#ifndef TANUKI_INTEROP_C_OPAQUE_CONTAINER_H
#define TANUKI_INTEROP_C_OPAQUE_CONTAINER_H

#include <cstddef>

#include "tanuki/interop/c_sequence.h"

/**
 *  @brief Data structure for passing a foreign sequence of opaque elements of
 *  constant size.
 */
extern "C"
typedef struct {
 public:
  /**
   *  @brief Foreign array.
   */
  CSequence arr;

  /**
   *  @brief Size in bytes of each element.
   */
  size_t elem_size;
} COpaqueContainer;

#endif
