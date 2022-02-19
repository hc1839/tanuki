#ifndef TANUKI_INTEROP_C_POLY_POINTER_H
#define TANUKI_INTEROP_C_POLY_POINTER_H

/**
 *  @brief Pointer to and type code of a foreign object that is treated as
 *  polymorphic.
 */
extern "C"
typedef struct {
  /**
   *  @brief Pointer to the foreign object.
   */
  void *ptr;

  /**
   *  @brief Type code.
   *
   *  Value of the type code depends on the foreign type.
   */
  int type_code;
} CPolyPointer;

#endif
