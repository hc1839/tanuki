#include <tanuki.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#define NUM_FOREIGN_ELEMS 8

using std::array;
using std::string;
using std::uniform_int_distribution;
using std::vector;

using tanuki::interop::ForeignContainer;
using tanuki::interop::ForeignContainerRef;
using tanuki::interop::ForeignMultiArray;
using tanuki::interop::ForeignMultiArrayRef;
using tanuki::memory::StorageOrder;

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

  // Create the mock foreign multi-array.
  ForeignContainerRef<ForeignElementMockDecorator> foreign_container(
      foreign_elems_seq);

  // Test the size of the multi-array.
  ASSERT_EQ(foreign_container.size(), NUM_FOREIGN_ELEMS);
  ASSERT_EQ(
      std::distance(foreign_container.begin(), foreign_container.end()),
      foreign_elems.size());

  // Test iteration over the mock foreign elements using the iterators.
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
 *  @brief Tests transfer of ownership to @link ForeignMultiArray @endlink.
 */
TEST(ForeignMultiArrayTest, ForeignMultiArrayOwnershipTransfer) {
  const array<ForeignElementMock, 4> mocks = {
    ForeignElementMock{ 2, { 2 } },
    ForeignElementMock{ 5, { 2, 5 } },
    ForeignElementMock{ 8, { 2, 5, 8 } },
    ForeignElementMock{ 32, { 2, 5, 8, 32 } }
  };

  auto seq_generator = [&mocks]() -> CSequence {
    auto ptr = new ForeignElementMock[4];
    std::copy(mocks.begin(), mocks.end(), ptr);
    return CSequence{ ptr, 4, sizeof(ForeignElementMock) };
  };

  bool is_equal;

  // Test one-dimensional array.
  {
    auto cntr_ptr = ForeignContainer<ForeignElementMockDecorator>::Create(
        seq_generator(),
        [](CSequence o, const array<size_t, 1> &) -> void {
          delete[] static_cast<ForeignElementMock *>(o.begin);
        });

    auto &cntr = *cntr_ptr;

    for (size_t i = 0; i != mocks.size(); ++i) {
      is_equal = *cntr[i].receiver == mocks[i];
      ASSERT_TRUE(is_equal);
    }
  }

  // Test two-dimensional array in row-major order.
  {
    auto marr_ptr = ForeignMultiArray<ForeignElementMockDecorator, 2>::Create(
        seq_generator(),
        { 2, 2 },
        StorageOrder::ROW_MAJOR,
        [](CSequence o, const array<size_t, 2> &) -> void {
          delete[] static_cast<ForeignElementMock *>(o.begin);
        });

    auto &marr = *marr_ptr;

    is_equal = *marr(0, 0).receiver == mocks[0];
    ASSERT_TRUE(is_equal);

    is_equal = *marr(0, 1).receiver == mocks[1];
    ASSERT_TRUE(is_equal);

    is_equal = *marr(1, 0).receiver == mocks[2];
    ASSERT_TRUE(is_equal);

    is_equal = *marr(1, 1).receiver == mocks[3];
    ASSERT_TRUE(is_equal);

    // Test bounds checking.
    ASSERT_THROW({ marr.at(2, 2); }, std::out_of_range);
  }

  // Test two-dimensional array in column-major order.
  {
    auto marr_ptr = ForeignMultiArray<ForeignElementMockDecorator, 2>::Create(
        seq_generator(),
        { 2, 2 },
        StorageOrder::COL_MAJOR,
        [](CSequence o, const array<size_t, 2> &) -> void {
          delete[] static_cast<ForeignElementMock *>(o.begin);
        });

    auto &marr = *marr_ptr;

    is_equal = *marr(0, 0).receiver == mocks[0];
    ASSERT_TRUE(is_equal);

    is_equal = *marr(1, 0).receiver == mocks[1];
    ASSERT_TRUE(is_equal);

    is_equal = *marr(0, 1).receiver == mocks[2];
    ASSERT_TRUE(is_equal);

    is_equal = *marr(1, 1).receiver == mocks[3];
    ASSERT_TRUE(is_equal);
  }
}
