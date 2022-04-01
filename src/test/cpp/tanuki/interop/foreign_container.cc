#include <tanuki.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <random>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#define NUM_FOREIGN_ELEMS 8

using std::array;
using std::string;
using std::uniform_int_distribution;
using std::vector;

using tanuki::interop::ForeignContainerRef;
using tanuki::interop::ForeignForwardIterator;

/**
 *  @brief Mock data class for foreign elements.
 */
struct ForeignElementMock final {
 public:
  bool operator==(const ForeignElementMock &other) const {
    return id == other.id && codes == other.codes;
  }

  bool operator!=(const ForeignElementMock &other) const {
    return !(*this == other);
  }

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
 *  @brief Tests access to @link ForeignContainer @endlink.
 */
TEST(ForeignContainerTest, ForeignContainerAccess) {
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

  array<size_t, 1> foreign_elems_shape = { foreign_elems.size() };

  COpaqueContainer foreign_elems_ctnr = {
    .arr = {
      .num_dims = foreign_elems_shape.size(),
      .extents = foreign_elems_shape.data(),
      .ptr = foreign_elems.data()
    },
    .elem_size = sizeof(ForeignElementMock)
  };

  // Create the mock foreign container.
  ForeignContainerRef<ForeignElementMockDecorator> foreign_container(
      foreign_elems_ctnr);

  // Test the size of the container.
  ASSERT_EQ(foreign_container.size(), NUM_FOREIGN_ELEMS);
  ASSERT_EQ(
      std::distance(foreign_container.begin(), foreign_container.end()),
      foreign_elems.size());

  // Test iteration over the mock foreign elements using the container
  // iterators.
  {
    // Iterator to the mock foreign elements stored in the original sequence.
    auto input_it = foreign_elems.begin();

    for (const auto &output : foreign_container) {
      ASSERT_EQ(*input_it++, *output.receiver);
    }
  }
}
