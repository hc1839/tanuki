#ifndef TANUKI_INTEROP_SPIN_MAP_H
#define TANUKI_INTEROP_SPIN_MAP_H

#include <map>
#include <vector>

#include "tanuki/interop/c_multi_array.h"
#include "tanuki/interop/c_sequence.h"
#include "tanuki/interop/foreign_multi_array.h"
#include "tanuki/state/spin.h"

namespace tanuki {
namespace interop {

using std::map;
using std::vector;

using tanuki::state::Spin;

/**
 *  @brief Gets the items in a two-dimensional multi-array as a spin map of
 *  opaque pointers to the items.
 *
 *  @param marr
 *    Two-dimensional multi-array.
 *
 *  @param is_col_major
 *    Whether <tt>marr</tt> is in column-major order.
 *
 *  @param is_col_spin
 *    Whether spin corresponds by column in <tt>marr</tt>. If <tt>true</tt>,
 *    <tt>marr</tt> must have one or two columns. One column indicates no spin
 *    polarization, and two columns indicate spin polarization. For spin
 *    polarization, first column must correspond to @link Spin::UP @endlink,
 *    and second column must correspond to @link Spin::DOWN @endlink. If
 *    <tt>false</tt>, rows (instead of columns) are concerned.
 *
 *  @return
 *    Spin map of opaque pointers to the items in <tt>marr</tt>.
 */
map<Spin, vector<void *>> SpinMap(
    const CMultiArray &marr, bool is_col_major, bool is_col_spin);

/**
 *  @brief Gets the items in a two-dimensional multi-array as a spin map.
 *
 *  @tparam T
 *    Type of items in the multi-array. It must meet the requirements of
 *    <tt>CopyConstructible</tt>.
 *
 *  @param marr
 *    Two-dimensional multi-array.
 *
 *  @param is_col_major
 *    Whether <tt>marr</tt> is in column-major order.
 *
 *  @param is_col_spin
 *    See @link SpinMap @endlink for opaque pointers.
 *
 *  @return
 *    Spin map of the items from <tt>marr</tt>.
 */
template <typename T>
map<Spin, vector<T>> SpinMap(
    const CMultiArray &marr, bool is_col_major, bool is_col_spin);

/**
 *  @brief Gets the items in a two-dimensional multi-array as a spin map of
 *  decorated opaque items.
 *
 *  @tparam T
 *    Type of decorated opaque items. It must meet the requirements of the type
 *    of items in @link ForeignContainer @endlink.
 *
 *  @param marr
 *    Two-dimensional multi-array.
 *
 *  @param is_col_major
 *    Whether <tt>marr</tt> is in column-major order.
 *
 *  @param is_col_spin
 *    See @link SpinMap @endlink for opaque pointers.
 *
 *  @return
 *    Spin map of decorated opaque items from <tt>marr</tt>.
 */
template <typename T>
map<Spin, vector<T>> SpinMapDecorated(
    const CMultiArray &marr, bool is_col_major, bool is_col_spin);

/**
 *  @brief Spin map of @link CSequence @endlink depending on whether there is
 *  one opaque item (for @link Spin::DUAL @endlink) or two opaque items (for
 *  @link Spin::UP @endlink and @link Spin::DOWN @endlink in order).
 */
map<Spin, void *> SpinMap(const CSequence &seq);

/**
 *  @brief Spin map of @link CSequence @endlink depending on whether there is
 *  one item (for @link Spin::DUAL @endlink) or two items (for @link Spin::UP
 *  @endlink and @link Spin::DOWN @endlink in order).
 *
 *  @tparam T
 *    Type of items in the sequence. It must meet the requirements of
 *    <tt>CopyConstructible</tt>.
 */
template <typename T>
map<Spin, T> SpinMap(const CSequence &seq);

/**
 *  @brief Spin map of @link CSequence @endlink depending on whether there is
 *  one opaque item (for @link Spin::DUAL @endlink) or two opaque items (for
 *  @link Spin::UP @endlink and @link Spin::DOWN @endlink in order).
 *
 *  @tparam T
 *    Type of decorated opaque items. It must meet the requirements of the type
 *    of items in @link ForeignContainer @endlink.
 */
template <typename T>
map<Spin, T> SpinMapDecorated(const CSequence &seq);

/**
 *  @brief Spin map of @link ForeignContainer @endlink depending on whether
 *  there is one opaque item (for @link Spin::DUAL @endlink) or two opaque
 *  items (for @link Spin::UP @endlink and @link Spin::DOWN @endlink in order).
 *
 *  @tparam T
 *    Type of decorated opaque items. It must meet the requirements of the type
 *    of items in @link ForeignContainer @endlink.
 */
template <typename T>
map<Spin, T> SpinMapDecorated(const ForeignContainer<T> &ctnr);

} // namespace interop
} // namespace tanuki

#include "tanuki/interop/spin_map.hxx"

#endif
