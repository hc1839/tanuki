#include <tanuki.h>

#include <map>
#include <random>
#include <stdexcept>
#include <string>

#include <gtest/gtest.h>

namespace tanuki {
namespace eda {

using std::map;
using std::string;
using std::uniform_real_distribution;

using tanuki::number::real_t;

/**
 *  @brief Tests conversions between @link HamiltonianEnergies @endlink and
 *  <tt>std::map</tt>.
 */
TEST(HamiltonianEnergies, MapConversion) {
  std::random_device rd;
  std::mt19937 gen(rd());

  uniform_real_distribution<> energy_distrib(-32.0, 32.0);

  // Test valid input.
  {
    const auto input = HamiltonianEnergies::Create({
      { "kinetic_coulomb", energy_distrib(gen) },
      { "exchange", energy_distrib(gen) },
      { "correlation", energy_distrib(gen) }
    });

    const auto output = HamiltonianEnergies::Create(
        static_cast<map<string, real_t>>(input));

    ASSERT_DOUBLE_EQ(output.kinetic_coulomb, input.kinetic_coulomb);
    ASSERT_DOUBLE_EQ(output.exchange, input.exchange);
    ASSERT_DOUBLE_EQ(output.correlation, input.correlation);
  }

  // Test invalid input.
  {
    const map<string, real_t> input = {
      { "kinetic_coulomb", energy_distrib(gen) },
      { "exchange", energy_distrib(gen) }
    };

    ASSERT_THROW(HamiltonianEnergies::Create(input), std::out_of_range);
  }
}

} // namespace eda
} // namespace tanuki
