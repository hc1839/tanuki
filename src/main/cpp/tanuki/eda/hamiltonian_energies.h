#ifndef TANUKI_EDA_HAMILTONIAN_ENERGIES_H
#define TANUKI_EDA_HAMILTONIAN_ENERGIES_H

#include <map>
#include <string>

#include "tanuki/number/types.h"

namespace tanuki {
namespace eda {

using std::map;
using std::string;

using tanuki::number::real_t;

/**
 *  @brief Hamiltonian energies used across EDA schemes for evaluating EDA
 *  components.
 */
struct HamiltonianEnergies final {
 public:
  /**
   *  @brief Creates from a map of data-member names to energies.
   *
   *  @param init
   *    Map of data-member names to energies. It must contain the names of all
   *    data members as keys. Other keys are ignored.
   *
   *  @return
   *    @link HamiltonianEnergies @endlink populated from <tt>init</tt>.
   *
   *  @exception std::out_of_range
   *    Name of a data member does not exist as a key.
   */
  static HamiltonianEnergies Create(const map<string, real_t> &init);

  /**
   *  @brief Converts to a map of data-member names to energies.
   */
  explicit operator map<string, real_t>() const;

  /**
   *  @brief Sum of kinetic energy and energy due to Coulomb forces.
   */
  real_t kinetic_coulomb;

  /**
   *  @brief Exchange energy.
   */
  real_t exchange;

  /**
   *  @brief Correlation energy.
   */
  real_t correlation;
};

} // namespace eda
} // namespace tanuki

#endif
