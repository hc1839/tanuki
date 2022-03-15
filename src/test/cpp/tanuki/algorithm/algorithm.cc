#include <tanuki.h>

#include <algorithm>
#include <random>
#include <vector>

#include <gtest/gtest.h>

#define APPROX_EQUAL_RELDIFF 1.0e-3
#define APPROX_EQUAL_ABSDIFF 1.0e-3

using std::vector;

using tanuki::algorithm::CompareRangeAllOf;

/**
 *  @brief Tests the whether two lists of integers have equal corresponding
 *  elements.
 */
TEST(EqualRangeTest, IntegerList) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(-32, 32);

  // First list of integers.
  vector<int> first_list(32);
  std::generate(
      first_list.begin(), first_list.end(),
      [&gen, &distrib]() -> int { return distrib(gen); });

  // Second list of integers that is a copy.
  const auto second_list = first_list;

  const bool is_equal = CompareRangeAllOf(
      first_list.begin(), first_list.end(),
      second_list.begin(), second_list.end(),
      [](const int &a, const int &b) -> bool {
        return a == b;
      });

  ASSERT_TRUE(is_equal);
}
