#include <tanuki.h>

#include <algorithm>
#include <random>
#include <vector>

#include <gtest/gtest.h>

using std::uniform_int_distribution;
using std::vector;

using tanuki::state::Spin;
using tanuki::state::SpinRange;

TEST(SpinRangeTestSuite, SpinRangeTest) {
  std::random_device rd;
  std::mt19937 gen(rd());

  // Container of input values.
  vector<int> input(8);

  std::generate(
      input.begin(), input.end(),
      [&gen]() -> int {
        return uniform_int_distribution<>(-12, 12)(gen);
      });

  // Buffer for pointers to the input values.
  vector<const int *> input_ptrs(input.size());

  std::transform(
      input.begin(), input.end(), input_ptrs.begin(),
      [](const int &item) -> const int * { return &item; });

  SpinRange<int> spin_range;

  // Test an empty spin range.
  ASSERT_TRUE(spin_range.empty());
  ASSERT_EQ(spin_range.size(), 0);

  spin_range = SpinRange<int>(
      input_ptrs.begin(), input_ptrs.end(), Spin::DUAL);

  // Test assignment.
  ASSERT_FALSE(spin_range.empty());
  ASSERT_EQ(spin_range.size(), input.size());

  vector<int> output;

  for (const auto &item : spin_range) {
    output.push_back(item);
  }

  ASSERT_EQ(output, input);
}
