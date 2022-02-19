#ifndef TANUKI_MATH_LINEAR_INDEXED_VECTOR_PAIR_H
#define TANUKI_MATH_LINEAR_INDEXED_VECTOR_PAIR_H

#include <armadillo>

#include "tanuki/math/linear/matrix_index_pair.h"

namespace tanuki {
namespace math {
namespace linear {

/**
 *  @brief Pair of vectors with their indices.
 *
 *  Regardless of whether the indices refer to rows or columns in the original
 *  matrix, a vector is stored as a column.
 *
 *  @tparam T
 *    Type of elements in an Armadillo matrix.
 */
template <typename T>
struct IndexedVectorPair final {
 public:
  /**
   *  @brief Pair of indices.
   */
  MatrixIndexPair indices;

  /**
   *  @brief Vector corresponding to the first index.
   */
  const arma::Col<T> &first;

  /**
   *  @brief Vector corresponding to the second index.
   */
  const arma::Col<T> &second;
};

} // namespace linear
} // namespace math
} // namespace tanuki

#endif
