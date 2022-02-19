#ifndef TANUKI_INTEROP_C_COMPLEX_NUMBER_H
#define TANUKI_INTEROP_C_COMPLEX_NUMBER_H

#include "tanuki/number/types.h"

using tanuki::number::real_t;

/**
 *  @brief Complex number for interoperating with foreign languages.
 */
extern "C"
typedef struct {
  /**
   *  @brief Real part.
   */
  real_t real;

  /**
   *  @brief Imaginary part.
   */
  real_t imag;
} CComplexNumber;

#endif
