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

using tanuki::interop::ForeignContainer;
using tanuki::interop::ForeignContainerRef;

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

  CSequence foreign_elems_seq = {
    .begin = foreign_elems.data(),
    .num_items = foreign_elems.size(),
    .item_size = sizeof(ForeignElementMock)
  };

  // Create the mock foreign container.
  ForeignContainerRef<ForeignElementMockDecorator> foreign_container(
      foreign_elems_seq);

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

  // Test random access of the mock foreign elements.
  {
    auto input_begin_it = foreign_elems.begin();

    for (size_t i = 0; i != foreign_elems.size(); ++i) {
      ASSERT_EQ(input_begin_it[i], *foreign_container[i].receiver);
    }
  }
}

/**
 *  @brief Tests transfer of ownership to @link ForeignContainer @endlink.
 */
TEST(ForeignContainerTest, ForeignContainerOwnershipTransfer) {
  CSequence seq = {
    new ForeignElementMock[3]{
      { 2, { 2 } },
      { 5, { 2, 5 } },
      { 8, { 2, 5, 8 } }
    },
    3,
    sizeof(ForeignElementMock)
  };

  auto cntr_ptr = ForeignContainer<ForeignElementMockDecorator>::Create(
      seq,
      [](CSequence o) -> void {
        delete[] static_cast<ForeignElementMock *>(o.begin);
      });

  auto &cntr = *cntr_ptr;
  bool is_equal;

  is_equal = *cntr[0].receiver == ForeignElementMock{ 2, { 2 } };
  ASSERT_TRUE(is_equal);

  is_equal = *cntr[1].receiver == ForeignElementMock{ 5, { 2, 5 } };
  ASSERT_TRUE(is_equal);

  is_equal = *cntr[2].receiver == ForeignElementMock{ 8, { 2, 5, 8 } };
  ASSERT_TRUE(is_equal);
}
