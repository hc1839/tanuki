#include <tanuki.h>

#include <algorithm>
#include <map>
#include <set>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

using std::map;
using std::pair;
using std::set;
using std::vector;

using tanuki::state::Spin;
using tanuki::state::SpinRangeTransformer;

/**
 *  @brief Tests spin-range transformer for a spin range that is not
 *  spin-polarized.
 */
TEST(SpinRangeTransformerTestSuite, SpinRangeTransformerDualTest) {
  // Underlying range.
  const vector<map<Spin, int>> base = {
    { { Spin::DUAL, 2 } },
    { { Spin::DUAL, 5 } },
    { { Spin::DUAL, 8 } }
  };

  SpinRangeTransformer<int> transformer(
      base.begin(), base.end(),
      [](const map<Spin, int> &item, Spin spin) -> const int * {
        const auto entry_it = item.find(spin);
        return entry_it != item.end() ? &entry_it->second : nullptr;
      });

  const auto eff_spins = transformer.eff_spins();

  ASSERT_EQ(eff_spins.size(), 1);
  ASSERT_EQ(eff_spins, set<Spin>({ Spin::DUAL }));

  // Test effective up spin.
  {
    const auto spin_range = transformer.CreateTransform(Spin::UP);
    const vector<int> output(spin_range.begin(), spin_range.end());

    ASSERT_EQ(output, vector<int>({ 2, 5, 8 }));
  }

  // Test effective down spin.
  {
    const auto spin_range = transformer.CreateTransform(Spin::DOWN);
    const vector<int> output(spin_range.begin(), spin_range.end());

    ASSERT_EQ(output, vector<int>({ 2, 5, 8 }));
  }
}

/**
 *  @brief Tests spin-range transformer for a spin range that uniformly
 *  spin-polarized.
 */
TEST(SpinRangeTransformerTestSuite, SpinRangeTransformerUniformTest) {
  // Underlying range.
  const vector<map<Spin, int>> base = {
    { { Spin::UP, 2 }, { Spin::DOWN, 5 } },
    { { Spin::UP, 8 }, { Spin::DOWN, 12 } },
    { { Spin::UP, 16 }, { Spin::DOWN, 32 } }
  };

  SpinRangeTransformer<int> transformer(
      base.begin(), base.end(),
      [](const map<Spin, int> &item, Spin spin) -> const int * {
        const auto entry_it = item.find(spin);
        return entry_it != item.end() ? &entry_it->second : nullptr;
      });

  const auto eff_spins = transformer.eff_spins();

  ASSERT_EQ(eff_spins.size(), 2);
  ASSERT_EQ(eff_spins, set<Spin>({ Spin::UP, Spin::DOWN }));

  // Test up spin.
  {
    const auto spin_range = transformer.CreateTransform(Spin::UP);
    const vector<int> output(spin_range.begin(), spin_range.end());

    ASSERT_EQ(output, vector<int>({ 2, 8, 16 }));
  }

  // Test down spin.
  {
    const auto spin_range = transformer.CreateTransform(Spin::DOWN);
    const vector<int> output(spin_range.begin(), spin_range.end());

    ASSERT_EQ(output, vector<int>({ 5, 12, 32 }));
  }
}

/**
 *  @brief Tests spin-range transformer for a spin range that is spin-polarized
 *  without being uniform in spin polarity.
 */
TEST(SpinRangeTransformerTestSuite, SpinRangeTransformerNonUniformTest) {
  // Underlying range.
  const vector<map<Spin, int>> base = {
    { { Spin::UP, 2 }, { Spin::DOWN, 5 } },
    { { Spin::DUAL, 8 } },
    { { Spin::UP, 12 }, { Spin::DOWN, 32 } }
  };

  SpinRangeTransformer<int> transformer(
      base.begin(), base.end(),
      [](const map<Spin, int> &item, Spin spin) -> const int * {
        const auto entry_it = item.find(spin);
        return entry_it != item.end() ? &entry_it->second : nullptr;
      });

  const auto eff_spins = transformer.eff_spins();

  ASSERT_EQ(eff_spins.size(), 2);
  ASSERT_EQ(eff_spins, set<Spin>({ Spin::UP, Spin::DOWN }));

  // Test up spin.
  {
    const auto spin_range = transformer.CreateTransform(Spin::UP);
    const vector<int> output(spin_range.begin(), spin_range.end());

    ASSERT_EQ(output, vector<int>({ 2, 8, 12 }));
  }

  // Test down spin.
  {
    const auto spin_range = transformer.CreateTransform(Spin::DOWN);
    const vector<int> output(spin_range.begin(), spin_range.end());

    ASSERT_EQ(output, vector<int>({ 5, 8, 32 }));
  }
}
