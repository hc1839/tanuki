#include "tanuki/eda/hamiltonian_energies.h"

namespace tanuki {
namespace eda {

HamiltonianEnergies HamiltonianEnergies::Create(
    const map<string, real_t> &init) {
  HamiltonianEnergies retval;

  retval.kinetic_coulomb = init.at("kinetic_coulomb");
  retval.exchange = init.at("exchange");
  retval.correlation = init.at("correlation");

  return retval;
}

HamiltonianEnergies::operator map<string, real_t>() const {
  return {
    { "kinetic_coulomb", kinetic_coulomb },
    { "exchange", exchange },
    { "correlation", correlation }
  };
}

} // namespace eda
} // namespace tanuki
