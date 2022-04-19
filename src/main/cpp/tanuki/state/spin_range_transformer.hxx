#ifndef TANUKI_STATE_SPIN_RANGE_TRANSFORMER_HXX
#define TANUKI_STATE_SPIN_RANGE_TRANSFORMER_HXX

#include <algorithm>
#include <cassert>
#include <iterator>

namespace tanuki {
namespace state {

template <typename T>
template <typename ForwardIt, typename F>
SpinRangeTransformer<T>::SpinRangeTransformer(
    ForwardIt first, ForwardIt last, F fun) {
  using BaseElem = typename std::iterator_traits<ForwardIt>::value_type;

  const bool is_spin_polarized = std::any_of(
      first, last,
      [fun](const BaseElem &item) -> bool {
        return fun(item, Spin::DUAL) == nullptr;
      });

  // Whether spin polarities are uniform for efficiency.
  const bool is_uniform = !is_spin_polarized || std::all_of(
      first, last,
      [fun](const BaseElem &item) -> bool {
        return fun(item, Spin::DUAL) == nullptr;
      });

  const size_t num_elems = std::distance(first, last);

  if (is_spin_polarized) {
    this->eff_spins_ = { Spin::UP, Spin::DOWN };
    this->transforms_[Spin::UP].reserve(num_elems);
    this->transforms_[Spin::DOWN].reserve(num_elems);

    if (is_uniform) {
      for (auto it = first; it != last; ++it) {
        this->transforms_[Spin::UP].push_back(fun(*it, Spin::UP));
        this->transforms_[Spin::DOWN].push_back(fun(*it, Spin::DOWN));
      }
    } else {
      for (auto it = first; it != last; ++it) {
        const auto *dual_value = fun(*it, Spin::DUAL);

        if (dual_value == nullptr) {
          this->transforms_[Spin::UP].push_back(fun(*it, Spin::UP));
          this->transforms_[Spin::DOWN].push_back(fun(*it, Spin::DOWN));
        } else {
          this->transforms_[Spin::UP].push_back(dual_value);
          this->transforms_[Spin::DOWN].push_back(dual_value);
        }
      }
    }
  } else {
    this->eff_spins_ = { Spin::DUAL };
    this->transforms_[Spin::DUAL].reserve(num_elems);

    for (auto it = first; it != last; ++it) {
      this->transforms_[Spin::DUAL].push_back(fun(*it, Spin::DUAL));
    }
  }
}

template <typename T>
SpinRange<T> SpinRangeTransformer<T>::CreateTransform(Spin eff_spin) const {
  const vector<const T *> *transform;

  if (eff_spins_.size() == 2) {
#ifndef TANUKI_NO_DEBUG
    assert(eff_spin != Spin::DUAL);
#endif
    transform = &transforms_.find(eff_spin)->second;
  } else {
    transform = &transforms_.find(Spin::DUAL)->second;
  }

  return SpinRange<T>(transform->begin(), transform->end(), eff_spin);
}

template <typename T>
set<Spin> SpinRangeTransformer<T>::eff_spins() const {
  return eff_spins_;
}

} // namespace state
} // namespace tanuki

#endif
