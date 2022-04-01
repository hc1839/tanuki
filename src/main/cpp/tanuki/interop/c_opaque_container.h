#ifndef TANUKI_INTEROP_C_OPAQUE_CONTAINER_H
#define TANUKI_INTEROP_C_OPAQUE_CONTAINER_H

#include <cstddef>

#include "tanuki/interop/c_multi_array.h"

/**
 *  @brief Data structure for passing a foreign array of opaque elements of
 *  constant size.
 */
extern "C"
typedef struct {
 public:
  /**
   *  @brief Foreign array.
   */
  CMultiArray arr;

  /**
   *  @brief Size in bytes of each element.
   */
  size_t elem_size;
} COpaqueContainer;

#endif
