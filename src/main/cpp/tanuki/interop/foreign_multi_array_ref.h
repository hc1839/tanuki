#ifndef TANUKI_INTEROP_FOREIGN_MULTI_ARRAY_REF_H
#define TANUKI_INTEROP_FOREIGN_MULTI_ARRAY_REF_H

#include <numeric>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "tanuki/interop/c_sequence.h"
#include "tanuki/interop/foreign_multi_array.h"
#include "tanuki/memory/storage_order.h"

namespace tanuki {
namespace interop {

using std::swap;

using tanuki::memory::StorageOrder;

/**
 *  @brief Reference to a foreign multi-array without taking ownership.
 *
 *  Copy/move construction and operation replace the current pointer with the
 *  one from the other instance. The foreign array itself is not copied.
 *
 *  @tparam T
 *    Type of decorated items being iterated over. See @link ForeignIterator
 *    @endlink for the requirements.
 *
 *  @tparam num_dims
 *    Number of dimensions. It must be positive.
 */
template <typename T, size_t num_dims>
class ForeignMultiArrayRef final : public ForeignMultiArray<T, num_dims> {
 public:
  using typename ForeignMultiArray<T, num_dims>::ShapeType;

  ForeignMultiArrayRef() = default;

  /**
   *  @param seq
   *    Backing sequence. Ownership is not transferred.
   */
  template <bool B = true>
  ForeignMultiArrayRef(
      CSequence seq,
      typename std::enable_if<num_dims == 1 && B, bool>::type = true)
          : seq_(seq),
            shape_({ seq.num_items }),
            storage_order_(StorageOrder::ROW_MAJOR) {}

  /**
   *  @param seq
   *    Backing sequence. Ownership is not transferred.
   *
   *  @param extents
   *    Extents along each dimension.
   *
   *  @param storage_order
   *    Storage order of <tt>seq</tt>.
   */
  template <bool B = true>
  ForeignMultiArrayRef(
      CSequence seq, const ShapeType &extents, StorageOrder storage_order,
      typename std::enable_if<num_dims != 1 && B, bool>::type = true)
          : seq_(seq),
            shape_(extents),
            storage_order_(storage_order) {
#ifndef TANUKI_NO_DEBUG
    {
      const size_t num_items = std::accumulate(
          extents.begin(), extents.end(), 1,
          [](const size_t &acc, const size_t &item) -> size_t {
            return acc * item;
          });

      if (num_items != seq.num_items) {
        throw std::logic_error("Mismatch in the number of items.");
      }
    }
#endif
  }

  virtual ~ForeignMultiArrayRef() = default;

  void swap(ForeignMultiArrayRef &other) {
    if (this == &other) {
      return;
    }

    std::swap(seq_, other.seq_);
    std::swap(shape_, other.shape_);
    std::swap(storage_order_, other.storage_order_);
  }

  CSequence seq() const override final {
    return seq_;
  }

  const ShapeType &shape() const override final {
    return shape_;
  }

  StorageOrder storage_order() const override final {
    return storage_order_;
  }

 private:
  /**
   *  @brief Backing data member for @link seq @endlink.
   */
  CSequence seq_;

  /**
   *  @brief Backing data member for @link shape @endlink.
   */
  ShapeType shape_;

  /**
   *  @brief Backing data member for @link storage_order @endlink.
   */
  StorageOrder storage_order_;
};

template <typename T, size_t num_dims>
void swap(
    ForeignMultiArrayRef<T, num_dims> &a,
    ForeignMultiArrayRef<T, num_dims> &b) {
  a.swap(b);
}

template <typename T>
using ForeignContainerRef = ForeignMultiArrayRef<T, 1>;

} // namespace interop
} // namespace tanuki

#endif
