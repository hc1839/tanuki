#ifndef TANUKI_MATH_COMBINATORICS_ROUND_ROBIN_TOURNEY_H
#define TANUKI_MATH_COMBINATORICS_ROUND_ROBIN_TOURNEY_H

#include <cstddef>

#include <armadillo>

namespace tanuki {
namespace math {
namespace combinatorics {

/**
 *  @brief Creates rounds of a round-robin tournament as rows of a two-column
 *  matrix.
 *
 *  In a set of \f$ n \f$ competitors, each competitor is assigned an integer
 *  in the range \f$ [0, n) \f$. The circle method is used to create the
 *  rounds. For each round, a matrix is constructed such that each row
 *  represents pair of competitor indices. Iteration over the rounds are
 *  deterministic in order.
 *
 *  If there is an odd number of competitors, an index of \f$ -1 \f$ is added
 *  to the set of competitors as a dummy competitor. For any round, a
 *  competitor is idle when paired with \f$ -1 \f$.
 *
 *  @tparam
 *    Type of competitor indices. It must be integral and signed.
 */
template <typename T>
class RoundRobinTourney {
 public:
  class Iterator;

  /**
   *  @briefs Iterator to the beginning of the iteration over the rounds.
   */
  Iterator begin() const;

  /**
   *  @briefs Past-the-end iterator over the rounds.
   */
  Iterator end() const;

  /**
   *  @brief Iterator over the rounds of a round-robin tournament.
   */
  class Iterator final {
    friend Iterator RoundRobinTourney<T>::begin() const;

    friend Iterator RoundRobinTourney<T>::end() const;

   public:
    using difference_type = size_t;

    using value_type = arma::Mat<T>;

    using pointer = const value_type *;

    using reference = const value_type &;

    using iterator_category = std::input_iterator_tag;

    bool operator==(const Iterator &other) const;

    bool operator!=(const Iterator &other) const;

    reference operator*() const;

    pointer operator->() const;

    Iterator &operator++();

    Iterator operator++(int);

   private:
    /**
     *  @param num_competitors
     *      Positive number of competitors.
     *
     *  @param round
     *    Current round in the round-robin tournament, or empty if at the end
     *    of iteration. If not empty, it must have at least two rows and
     *    exactly two columns.
     */
    Iterator(size_t num_competitors, const arma::Mat<T> &round);

    /**
     *  @brief Positive number of competitors.
     */
    size_t num_competitors_;

    /**
     *  @brief Current round in the round-robin tournament, or empty if at the
     *  end of iteration.
     */
    arma::Mat<T> round_;
  };

  /**
   *  @param num_competitors
   *    Positive number of competitors.
   */
  RoundRobinTourney(size_t num_competitors);

  virtual ~RoundRobinTourney() = default;

  /**
   *  @brief Number of rounds in the round-robin tournament.
   */
  size_t num_rounds() const;

  /**
   *  @brief Number of pairs in each round.
   *
   *  @param include_idle
   *    Whether to count the idle competitor as a pair.
   */
  size_t num_pairs_per_round(bool include_idle) const;

  /**
   *  @brief Positive number of competitors.
   */
  const size_t num_competitors;
};

} // namespace combinatorics
} // namespace math
} // namespace tanuki

#include "tanuki/math/combinatorics/round_robin_tourney.hxx"

#endif
