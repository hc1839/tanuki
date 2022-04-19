#ifndef TANUKI_STATE_SPIN_RANGE_TRANSFORMER_H
#define TANUKI_STATE_SPIN_RANGE_TRANSFORMER_H

#include <map>
#include <set>
#include <vector>

#include "tanuki/state/spin.h"
#include "tanuki/state/spin_range.h"

namespace tanuki {
namespace state {

using std::map;
using std::set;
using std::vector;

/**
 *  @brief Eager transformer of a range to spin ranges for effective spin
 *  enumerators.
 *
 *  For the definition of an effective spin enumerator, see @link SpinRange
 *  @endlink.
 *
 *  @tparam T
 *    Type of elements in a spin range.
 */
template <typename T>
class SpinRangeTransformer final {
 public:
  /**
   *  @tparam ForwardIt
   *    Must meet the requirements of <tt>LegacyForwardIterator</tt>.
   *
   *  @tparam F
   *    Transformation function.
   *
   *  @param first
   *    Beginning of the range to transform.
   *
   *  @param last
   *    Past-the-end of the range to transform.
   *
   *  @param fun
   *    Transformation function that is eagerly applied to the elements in
   *    <tt>[first, last)</tt>. It must return a <tt>const T *</tt> given an
   *    element in <tt>[first, last)</tt> and a spin enumerator, respectively.
   *    The spin enumerator that is passed to the transformation function is
   *    exact and not effective. The transformation function must meet the
   *    requirement where it returns a non-<tt>nullptr</tt> given @link
   *    Spin::DUAL @endlink if and only if it returns <tt>nullptr</tt> given
   *    other spin enumerators.
   */
  template <typename ForwardIt, typename F>
  SpinRangeTransformer(ForwardIt first, ForwardIt last, F fun);

  /**
   *  @brief Creates a transform of the underlying range for the specified
   *  effective spin enumerator.
   *
   *  @param eff_spin
   *    Effective spin enumerator. If @link eff_spins @endlink contains one
   *    element (which is @link Spin::DUAL @endlink), it can be any spin
   *    enumerator. Otherwise, it must be a spin enumerator that is in @link
   *    eff_spins @endlink.
   *
   *  @return
   *    Spin range for <tt>eff_spin</tt> with elements that resulted from the
   *    transformation.
   */
  SpinRange<T> CreateTransform(Spin eff_spin) const;

  /**
   *  @brief Effective spin enumerators for transforming the underlying range.
   *
   *  @return
   *    Set of effective spin enumerators. If all of the elements in the spin
   *    range are associated by @link Spin::DUAL @endlink, the return set
   *    contains only @link Spin::DUAL @endlink, otherwise the return set
   *    contains exactly @link Spin::UP @endlink and @link Spin::DOWN @endlink.
   */
  set<Spin> eff_spins() const;

 private:
  /**
   *  @brief Backing data member for @link eff_spins @endlink.
   */
  set<Spin> eff_spins_;

  /**
   *  @brief Map of transforms for constructing @link SpinRange @endlink when
   *  given an effective spin enumerator.
   */
  map<Spin, vector<const T *>> transforms_;
};

} // namespace state
} // namespace tanuki

#include "tanuki/state/spin_range_transformer.hxx"

#endif
