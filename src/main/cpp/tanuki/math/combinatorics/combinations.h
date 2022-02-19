#ifndef TANUKI_MATH_COMBINATORICS_COMBINATIONS_H
#define TANUKI_MATH_COMBINATORICS_COMBINATIONS_H

#include <cstddef>
#include <iterator>
#include <memory>
#include <vector>

namespace tanuki {
namespace math {
namespace combinatorics {

/**
 *  @brief Creates combinations <tt>(n, k)</tt> of <tt>k</tt> elements in a set
 *  of <tt>n</tt> elements.
 *
 *  @tparam T
 *    Type of elements.
 */
template <typename T>
class Combinations {
 public:
  class Iterator;

  /**
   *  @briefs Iterator to the beginning of the iteration over the combinations.
   */
  Iterator begin() const;

  /**
   *  @briefs Past-the-end iterator over the combinations.
   */
  Iterator end() const;

  /**
   *  @brief Iterator over combinations.
   */
  class Iterator final {
    friend Iterator Combinations<T>::begin() const;

    friend Iterator Combinations<T>::end() const;

   public:
    using difference_type = size_t;

    using value_type = std::vector<T>;

    using pointer = const value_type *;

    using reference = const value_type &;

    using iterator_category = std::input_iterator_tag;

    Iterator(const Iterator &other);

    Iterator(Iterator &&other);

    Iterator &operator=(const Iterator &other);

    Iterator &operator=(Iterator &&other);

    bool operator==(const Iterator &other) const;

    bool operator!=(const Iterator &other) const;

    reference operator*() const;

    pointer operator->() const;

    Iterator &operator++();

    Iterator operator++(int);

   private:
    /**
     *  @param pool
     *    Pool of all elements that participate in the generation of
     *    combinations.
     *
     *  @param bitmask
     *    Mask that determines whether the corresponding element from
     *    <tt>pool</tt> is in the combination, or <tt>nullptr</tt> if at the
     *    end of iteration. If non-null, it must have the same number of
     *    elements as <tt>pool</tt>.
     */
    Iterator(
        std::shared_ptr<std::vector<T>> pool,
        std::unique_ptr<std::vector<bool>> bitmask);

    /**
     *  @brief Pool of all elements that participate in the generation of
     *  combinations.
     */
    std::shared_ptr<std::vector<T>> pool_;

    /**
     *  @brief Bitmask that determines whether the corresponding element from
     *  @link pool_ @endlink is in <tt>this</tt> combination, or
     *  <tt>nullptr</tt> if at the end of iteration.
     */
    std::unique_ptr<std::vector<bool>> bitmask_;

    /**
     *  @brief Combination.
     */
    std::vector<T> combo_;
  };

  /**
   *  @param pool
   *    Pool of elements from which to generate combinations.
   *
   *  @param k
   *    Number of elements in each combination. If it is greater than the
   *    number of elements in <tt>pool</tt>, <tt>std::domain_error</tt> is
   *    thrown.
   */
  Combinations(const std::vector<T> &pool, size_t k);

  virtual ~Combinations() = default;

  /**
   *  @brief Number of combinations.
   */
  size_t size() const;

  /**
   *  @brief Number of elements from which the combinations are generated.
   */
  const size_t n;

  /**
   *  @brief Number of elements in each combination.
   */
  const size_t k;

 private:
  /**
   *  @brief Pool of elements from which to generate combinations.
   */
  std::shared_ptr<std::vector<T>> pool_;
};

/**
 *  @brief Number of <tt>k</tt>-combinations from a set of <tt>n</tt> elements.
 */
size_t CountCombinations(size_t n, size_t k);

} // namespace combinatorics
} // namespace math
} // namespace tanuki

#include "tanuki/math/combinatorics/combinations.hxx"

#endif
