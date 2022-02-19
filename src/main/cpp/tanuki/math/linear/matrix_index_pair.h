#ifndef TANUKI_MATH_LINEAR_MATRIX_INDEX_PAIR_H
#define TANUKI_MATH_LINEAR_MATRIX_INDEX_PAIR_H

namespace tanuki {
namespace math {
namespace linear {

/**
 *  @brief Pair of matrix indices, both of which are either rows or columns.
 *
 *  Whether the ordering of the indices is important depends on the context in
 *  which the indices are used.
 */
struct MatrixIndexPair final {
 public:
  /**
   *  @brief Type of pair by its indices.
   */
  enum class PairType : int {
    /**
     *  @brief Indices are rows.
     */
    ROWS,

    /**
     *  @brief Indices are columns.
     */
    COLUMNS
  };

  /**
   *  @brief Type of pair.
   */
  PairType type;

  /**
   *  @brief First index.
   */
  size_t first;

  /**
   *  @brief Second index.
   */
  size_t second;
};

} // namespace linear
} // namespace math
} // namespace tanuki

#endif
