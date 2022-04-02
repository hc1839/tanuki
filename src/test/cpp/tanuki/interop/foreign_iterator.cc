#include <tanuki.h>

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <random>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#define NUM_FOREIGN_ELEMS 8

using std::string;
using std::uniform_int_distribution;
using std::vector;

using tanuki::interop::ForeignIterator;

/**
 *  @brief Mock data class for foreign elements.
 */
struct ForeignElementMock final {
 public:
  bool operator==(const ForeignElementMock &other) const = default;

  size_t id;

  vector<int> codes;
};

/**
 *  @brief Decorator to @link ForeignElementMock @endlink.
 */
struct ForeignElementMockDecorator final {
 public:
  explicit ForeignElementMockDecorator(void *receiver)
      : receiver(static_cast<ForeignElementMock *>(receiver)) {}

  ForeignElementMock *receiver;
};

/**
 *  @brief Tests @link ForeignIterator @endlink.
 */
TEST(ForeignIteratorTest, ForeignIteratorAccess) {
  std::random_device rd;
  std::mt19937 gen(rd());

  // Sequence of mock foreign elements.
  vector<ForeignElementMock> foreign_elems(NUM_FOREIGN_ELEMS);

  // Populate the mock foreign elements with random data.
  for (auto &item : foreign_elems) {
    item.id = uniform_int_distribution<>(2, 32)(gen);

    item.codes.resize(uniform_int_distribution<>(8, 12)(gen));
    std::generate(
        item.codes.begin(), item.codes.end(),
        [&gen]() -> int { return uniform_int_distribution<>(-32, 32)(gen); });
  }

  COpaqueContainer foreign_elems_ctnr = {
    .arr = {
      .num_elems = foreign_elems.size(),
      .ptr = foreign_elems.data()
    },
    .elem_size = sizeof(ForeignElementMock)
  };

  // Create foreign iterators to the decorated mock elements.
  ForeignIterator<ForeignElementMockDecorator> output_begin_it(
      foreign_elems_ctnr, 0);
  ForeignIterator<ForeignElementMockDecorator> output_end_it(
      foreign_elems_ctnr, foreign_elems.size());

  // Test the distance from the beginning iterator to the end iterator.
  ASSERT_EQ(
      std::distance(output_begin_it, output_end_it), foreign_elems.size());

  // Test iteration over the mock foreign elements using the foreign iterators.
  {
    // Iterator to the mock foreign elements stored in the original sequence.
    auto input_it = foreign_elems.begin();

    for (auto output_it = output_begin_it;
         output_it != output_end_it;
         ++output_it, ++input_it) {
      ASSERT_EQ(*input_it, *output_it->receiver);
    }
  }
}
