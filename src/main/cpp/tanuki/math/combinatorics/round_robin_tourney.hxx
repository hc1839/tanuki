#ifndef TANUKI_MATH_COMBINATORICS_ROUND_ROBIN_TOURNEY_HXX
#define TANUKI_MATH_COMBINATORICS_ROUND_ROBIN_TOURNEY_HXX

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <stdexcept>
#include <utility>

namespace tanuki {
namespace math {
namespace combinatorics {

using arma::Col;
using arma::Mat;

template <typename T>
bool RoundRobinTourney<T>::Iterator::operator==(const Iterator &other) const {
  return num_competitors_ == other.num_competitors_ &&
      round_.size() == other.round_.size() &&
      std::equal(round_.begin(), round_.end(), other.round_.begin());
}

template <typename T>
bool RoundRobinTourney<T>::Iterator::operator!=(const Iterator &other) const {
  return !(*this == other);
}

template <typename T>
auto RoundRobinTourney<T>::Iterator::operator*() const -> reference {
  assert(!round_.is_empty());

  return round_;
}

template <typename T>
auto RoundRobinTourney<T>::Iterator::operator->() const -> pointer {
  return &operator*();
}

template <typename T>
auto RoundRobinTourney<T>::Iterator::operator++() -> Iterator & {
  assert(!round_.is_empty());

  if (round_.n_rows == 1) {
    round_.clear();
    return *this;
  }

  // Buffer for the two competitor indices that run off the matrix after the
  // advancement.
  auto runoff_pair = std::make_pair(
      *(round_.end_col(0) - 1),
      *round_.begin_col(1));

  // Move the competitor indices in the first column downward.
  std::move_backward(
      round_.begin() + 1,
      round_.end_col(0) - 1,
      round_.end_col(0));

  // Move the competitor indices in the second column upward.
  std::move(
      round_.begin_col(1) + 1,
      round_.end(),
      round_.begin_col(1));

  // Assign the run-offs to the new positions.
  *(round_.end() - 1) = runoff_pair.first;
  *(round_.begin() + 1) = runoff_pair.second;

  if (*(round_.begin() + 1) == 1) {
    round_.clear();
  }

  return *this;
}

template <typename T>
auto RoundRobinTourney<T>::Iterator::operator++(int) -> Iterator {
  auto old = *this;
  operator++();
  return old;
}

template <typename T>
RoundRobinTourney<T>::Iterator::Iterator(
    size_t num_competitors,
    const Mat<T> &round)
        : num_competitors_(num_competitors),
          round_(round) {
  assert(std::is_integral<T>::value);
  assert(std::is_signed<T>::value);

  if (!round_.is_empty()) {
    assert(round.n_rows >= 1);
    assert(round.n_cols == 2);
  }
}

template <typename T>
RoundRobinTourney<T>::RoundRobinTourney(size_t num_competitors)
    : num_competitors(num_competitors) {
  assert(std::is_integral<T>::value);
  assert(std::is_signed<T>::value);

  if (num_competitors == 0) {
    throw std::domain_error("Number of competitors is zero.");
  }
}

template <typename T>
auto RoundRobinTourney<T>::begin() const -> Iterator {
  auto num_pairs = std::div(
      static_cast<long long>(num_competitors),
      static_cast<long long>(2));

  // Two-column matrix representing the first round.
  Mat<T> round(num_pairs.quot + num_pairs.rem, 2);

  std::iota(round.begin(), round.end(), 0);
  std::reverse(round.begin_col(1), round.end());

  // Add a dummy competitor if the number of competitors is odd.
  if (num_pairs.rem != 0) {
    *round.begin_col(1) = -1;
  }

  return Iterator(num_competitors, round);
}

template <typename T>
auto RoundRobinTourney<T>::end() const -> Iterator {
  return Iterator(num_competitors, Mat<T>());
}

template <typename T>
size_t RoundRobinTourney<T>::num_rounds() const {
  if (num_competitors & 1) {
    return num_competitors;
  } else {
    return num_competitors - 1;
  }
}

template <typename T>
size_t RoundRobinTourney<T>::num_pairs_per_round(bool include_idle) const {
  if (num_competitors & 1) {
    return (num_competitors - 1 >> 1) + (include_idle ? 1 : 0);
  } else {
    return num_competitors >> 1;
  }
}

} // namespace combinatorics
} // namespace math
} // namespace tanuki

#endif
