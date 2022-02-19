#ifndef TANUKI_MATH_COMBINATORICS_COMBINATIONS_HXX
#define TANUKI_MATH_COMBINATORICS_COMBINATIONS_HXX

#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <utility>

namespace tanuki {
namespace math {
namespace combinatorics {

using std::shared_ptr;
using std::unique_ptr;
using std::vector;

template <typename T>
Combinations<T>::Iterator::Iterator(const Iterator &other)
    : pool_(other.pool_),
      combo_(other.combo_) {
  if (other.bitmask_ != nullptr) {
    bitmask_.reset(new vector<bool>(*other.bitmask_));
  }
}

template <typename T>
Combinations<T>::Iterator::Iterator(Iterator &&other)
    : pool_(std::move(other.pool_)),
      combo_(std::move(other.combo_)) {
  bitmask_ = std::move(other.bitmask_);
}

template <typename T>
auto Combinations<T>::Iterator::operator=(const Iterator &other) -> Iterator & {
  if (this == &other) {
    return *this;
  }

  pool_ = other.pool_;
  combo_ = other.combo_;

  if (other.bitmask_ != nullptr) {
    bitmask_.reset(new vector<bool>(*other.bitmask_));
  } else {
    bitmask_.reset(nullptr);
  }

  return *this;
}

template <typename T>
auto Combinations<T>::Iterator::operator=(Iterator &&other) -> Iterator & {
  if (this == &other) {
    return *this;
  }

  pool_ = std::move(other.pool_);
  combo_ = std::move(other.combo_);

  bitmask_ = std::move(other.bitmask_);

  return *this;
}

template <typename T>
bool Combinations<T>::Iterator::operator==(const Iterator &other) const {
  if (bitmask_ != nullptr && other.bitmask_ != nullptr) {
    return pool_ == other.pool_ && *bitmask_ == *other.bitmask_;
  } else {
    return pool_ == other.pool_ && bitmask_ == other.bitmask_;
  }
}

template <typename T>
bool Combinations<T>::Iterator::operator!=(const Iterator &other) const {
  return !(*this == other);
}

template <typename T>
auto Combinations<T>::Iterator::operator*() const -> reference {
  if (bitmask_ != nullptr) {
    return combo_;
  } else {
    assert(false);
  }
}

template <typename T>
auto Combinations<T>::Iterator::operator->() const -> pointer {
  return &operator*();
}

template <typename T>
auto Combinations<T>::Iterator::operator++() -> Iterator & {
  assert(bitmask_ != nullptr);

  auto is_new = std::prev_permutation(bitmask_->begin(), bitmask_->end());

  // Remove the old combination.
  combo_.clear();

  if (is_new) {
    // Create the new combination.
    for (size_t i = 0; i != bitmask_->size(); ++i) {
      if ((*bitmask_)[i]) {
        combo_.push_back((*pool_)[i]);
      }
    }
  } else {
    bitmask_.reset(nullptr);
  }

  return *this;
}

template <typename T>
auto Combinations<T>::Iterator::operator++(int) -> Iterator {
  auto old = *this;
  operator++();
  return old;
}

template <typename T>
Combinations<T>::Iterator::Iterator(
    shared_ptr<vector<T>> pool,
    unique_ptr<vector<bool>> bitmask)
        : pool_(pool),
          bitmask_(std::move(bitmask)) {
  assert(pool_ != nullptr);

  if (bitmask_ != nullptr) {
    assert(bitmask_->size() == pool_->size());

    // Create and store the combination.
    for (size_t i = 0; i != bitmask_->size(); ++i) {
      if ((*bitmask_)[i]) {
        combo_.push_back((*pool_)[i]);
      }
    }
  }
}

template <typename T>
Combinations<T>::Combinations(const std::vector<T> &pool, size_t k)
    : pool_(new vector<T>(pool)),
      n(pool.size()),
      k(k) {
  if (k > pool_->size()) {
    throw std::domain_error("Number is too large.");
  }
}

template <typename T>
auto Combinations<T>::begin() const -> Iterator {
  unique_ptr<vector<bool>> init_bitmask(new vector<bool>(k, true));
  init_bitmask->resize(pool_->size(), false);

  return Iterator(pool_, std::move(init_bitmask));
}

template <typename T>
auto Combinations<T>::end() const -> Iterator {
  return Iterator(pool_, unique_ptr<vector<bool>>());
}

template <typename T>
size_t Combinations<T>::size() const {
  return CountCombinations(n, k);
}

} // namespace combinatorics
} // namespace math
} // namespace tanuki

#endif
