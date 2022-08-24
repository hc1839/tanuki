#ifndef TANUKI_SCF_MI_PROJECTION_FACTORS_H
#define TANUKI_SCF_MI_PROJECTION_FACTORS_H

#include "tanuki/scf_mi/scf_mi_method.h"

namespace tanuki {
namespace scf_mi {

/**
 *  @brief Factors in the effective Hamiltonians of an SCF-MI method for each
 *  unit.
 *
 *  Factors must not be affected by a change of basis.
 *
 *  @tparam T
 *    Must be @link tanuki::number::real_t @endlink or @link
 *    tanuki::number::complex_t @endlink.
 */
template <ScfMiMethod, typename T>
struct ProjectionFactors;

} // namespace scf_mi
} // namespace tanuki

#endif
