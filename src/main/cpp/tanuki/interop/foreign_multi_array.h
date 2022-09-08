#ifndef TANUKI_INTEROP_FOREIGN_MULTI_ARRAY_H
#define TANUKI_INTEROP_FOREIGN_MULTI_ARRAY_H

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <limits>
#include <memory>
#include <numeric>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include <avro/Decoder.hh>
#include <avro/Encoder.hh>
#include <avro/Specific.hh>

#include "tanuki/interop/foreign_iterator.h"
#include "tanuki/memory/storage_order.h"

namespace tanuki {
namespace interop {

using std::array;
using std::unique_ptr;
using std::vector;

using tanuki::memory::StorageOrder;

template <typename T, size_t num_dims>
class ForeignMultiArrayImpl;

/**
 *  @brief Multi-array that partially satisfies C++ <tt>Container</tt> named
 *  requirement for wrapping a foreign multi-array of opaque items.
 *
 *  @tparam T
 *    Type of decorated items being iterated over. See @link ForeignIterator
 *    @endlink for the requirements.
 *
 *  @tparam num_dims
 *    Number of dimensions. It must be positive.
 */
template <typename T, size_t num_dims>
class ForeignMultiArray {
 public:
  using value_type = T;

  using reference = value_type &;

  using const_reference = const reference;

  using iterator = ForeignIterator<T>;

  using const_iterator = const iterator;

  using difference_type =
      typename std::iterator_traits<iterator>::difference_type;

  using size_type = size_t;

  using ShapeType = array<size_type, num_dims>;

  using Deleter = function<void(CSequence, const ShapeType &)>;

  /**
   *  @brief Creates an instance with a deleter that deletes the backing
   *  sequence.
   *
   *  @param seq
   *    Backing sequence to take ownership of.
   *
   *  @param seq_deleter
   *    Deleter that deletes the backing array in <tt>seq</tt>.
   */
  template <bool B = true>
  static typename std::enable_if<
      num_dims == 1 && B, unique_ptr<ForeignMultiArray<T, 1>>>::type
  Create(CSequence seq, Deleter seq_deleter) {
    auto obj = new ForeignMultiArrayImpl<T, 1>();

    obj->seq_ = seq;
    obj->shape_ = { seq.num_items };

    // Actual value does not matter.
    obj->storage_order_ = StorageOrder::ROW_MAJOR;

    obj->seq_deleter_ = seq_deleter;

    return unique_ptr<ForeignMultiArray<T, 1>>(obj);
  }

  /**
   *  @brief Creates an instance with a deleter that deletes the backing
   *  sequence.
   *
   *  @param seq
   *    Backing sequence to take ownership of.
   *
   *  @param extents
   *    Extents along each dimension.
   *
   *  @param storage_order
   *    Storage order of <tt>seq</tt>.
   *
   *  @param seq_deleter
   *    Deleter that deletes the backing array in <tt>seq</tt>.
   */
  template <bool B = true>
  static typename std::enable_if<
      num_dims != 1 && B, unique_ptr<ForeignMultiArray<T, num_dims>>>::type
  Create(
      CSequence seq,
      const ShapeType &extents,
      StorageOrder storage_order,
      Deleter seq_deleter) {
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

    auto obj = new ForeignMultiArrayImpl<T, num_dims>();

    obj->seq_ = seq;
    obj->shape_ = extents;
    obj->storage_order_ = storage_order;
    obj->seq_deleter_ = seq_deleter;

    return unique_ptr<ForeignMultiArray<T, num_dims>>(obj);
  }

  virtual ~ForeignMultiArray() = default;

  /**
   *  @brief Item at the specified index under the assumption of a flat layout
   *  in @link storage_order @endlink without bounds checking.
   */
  value_type operator[](size_type idx) const {
    return begin()[idx];
  }

  /**
   *  @brief Item at the specified indices without bounds checking.
   */
  value_type operator[](const ShapeType &idxs) const {
    return *iter(idxs);
  }

  /**
   *  @brief Item at the specified indices.
   *
   *  Bounds checking is performed if <tt>TANUKI_NO_DEBUG</tt> is not defined;
   *  otherwise it is not performed.
   *
   *  @tparam U
   *    Must be convertible to @link size_type @endlink.
   *
   *  @param idxs
   *    Indices to the item.
   *
   *  @return
   *    Item at <tt>idxs</tt>.
   */
  template <typename... U>
  value_type operator()(U... idxs) const {
#ifndef TANUKI_NO_DEBUG
    {
      const auto idxs_vec = ExpandedIndices(idxs...);

      for (size_type i = 0; i != idxs_vec.size(); ++i) {
        if (idxs_vec[i] >= shape()[i]) {
          throw std::out_of_range("Out of range for foreign multi-array.");
        }
      }
    }
#endif

    return *iter(idxs...);
  }

  iterator begin() {
    return iterator(seq().begin, 0, seq().item_size);
  }

  iterator end() {
    return iterator(
        static_cast<char *>(seq().begin) + seq().item_size * seq().num_items,
        seq().num_items,
        seq().item_size);
  }

  const_iterator begin() const {
    return iterator(seq().begin, 0, seq().item_size);
  }

  const_iterator end() const {
    return iterator(
        static_cast<char *>(seq().begin) + seq().item_size * seq().num_items,
        seq().num_items,
        seq().item_size);
  }

  const_iterator cbegin() const {
    return iterator(seq().begin, 0, seq().item_size);
  }

  const_iterator cend() const {
    return iterator(
        static_cast<char *>(seq().begin) + seq().item_size * seq().num_items,
        seq().num_items,
        seq().item_size);
  }

  size_type size() const {
    return seq().num_items;
  }

  size_type max_size() const {
    return std::numeric_limits<size_t>::max();
  }

  bool empty() const {
    return size() == 0;
  }

  value_type front() const {
    return *begin();
  }

  value_type back() const {
    auto tmp = end();
    --tmp;
    return *tmp;
  }

  /**
   *  @brief Item at the specified indices with bounds checking.
   *
   *  Bounds checking is performed regardless of <tt>TANUKI_NO_DEBUG</tt>.
   *
   *  @tparam U
   *    Must be convertible to @link size_type @endlink.
   *
   *  @param idxs
   *    Indices to the item.
   *
   *  @return
   *    Item at <tt>idxs</tt>.
   */
  template <typename... U>
  value_type at(U... idxs) const {
    const auto idxs_vec = ExpandedIndices(idxs...);

    for (size_type i = 0; i != idxs_vec.size(); ++i) {
      if (idxs_vec[i] >= shape()[i]) {
        throw std::out_of_range("Out of range for foreign multi-array.");
      }
    }

    return *iter(idxs...);
  }

  /**
   *  @brief Opaque pointer to the first item.
   */
  void *memptr() const {
    return seq().begin;
  }

  /**
   *  @brief Opaque pointer to the item at the specified indices without bounds
   *  checking.
   *
   *  @param idxs
   *    Indices to the item.
   *
   *  @return
   *    Opaque pointer to the item at <tt>idxs</tt>.
   */
  void *memptr(const ShapeType &idxs) const {
    size_type dst_idx;

    switch (storage_order()) {
      case StorageOrder::ROW_MAJOR: {
        size_type curr_stride = 1;
        dst_idx = idxs.back();

        for (size_type i = idxs.size() - 2; i != -1; --i) {
          curr_stride *= shape()[i + 1];
          dst_idx += curr_stride * idxs[i];
        }

        break;
      }
      case StorageOrder::COL_MAJOR: {
        size_type curr_stride = 1;
        dst_idx = idxs.front();

        for (size_type i = 1; i != idxs.size(); ++i) {
          curr_stride *= shape()[i - 1];
          dst_idx += curr_stride * idxs[i];
        }

        break;
      }
      default: {
        throw std::logic_error("Enumeration matching is not exhaustive.");
        break;
      }
    }

    return static_cast<char *>(seq().begin) + seq().item_size * dst_idx;
  }

  /**
   *  @brief Opaque pointer to the item at the specified indices without bounds
   *  checking.
   *
   *  @tparam U
   *    Must be convertible to @link size_type @endlink.
   *
   *  @param idxs
   *    Indices to the item.
   *
   *  @return
   *    Opaque pointer to the item at <tt>idxs</tt>.
   */
  template <typename... U>
  void *memptr(U... idxs) const {
    static_assert(sizeof...(idxs) == num_dims);

    ShapeType idxs_arr;
    auto idxs_vec = ExpandedIndices(idxs...);
    std::copy(idxs_vec.begin(), idxs_vec.end(), idxs_arr.begin());

    return memptr(idxs_arr);
  }

  /**
   *  @brief Iterator to the item at the specified indices without bounds
   *  checking.
   *
   *  @param idxs
   *    Indices to the item.
   *
   *  @return
   *    Iterator to the item at <tt>idxs</tt>.
   */
  const_iterator iter(const ShapeType &idxs) const {
    auto ptr = memptr(idxs);
    const auto idx = std::distance(
        static_cast<char *>(memptr()), static_cast<char *>(ptr)) /
            seq().item_size ;

    return iterator(ptr, idx, seq().item_size);
  }

  iterator iter(const ShapeType &idxs) {
    const auto *this_ = this;
    return const_cast<iterator>(this_->iter(idxs));
  }

  /**
   *  @brief Iterator to the item at the specified indices without bounds
   *  checking.
   *
   *  @tparam U
   *    Must be convertible to @link size_type @endlink.
   *
   *  @param idxs
   *    Indices to the item.
   *
   *  @return
   *    Iterator to the item at <tt>idxs</tt>.
   */
  template <typename... U>
  const_iterator iter(U... idxs) const {
    static_assert(sizeof...(idxs) == num_dims);

    ShapeType idxs_arr;
    auto idxs_vec = ExpandedIndices(idxs...);
    std::copy(idxs_vec.begin(), idxs_vec.end(), idxs_arr.begin());

    return iter(idxs_arr);
  }

  template <typename... U>
  iterator iter(U... idxs) {
    const auto *this_ = this;
    return const_cast<iterator>(this_->iter(idxs...));
  }

  /**
   *  @brief Backing foreign sequence.
   */
  virtual CSequence seq() const = 0;

  /**
   *  @brief Extents along each dimension.
   *
   *  The same container must be returned for each call such that its iterators
   *  remain valid throughout the lifetime of <tt>this</tt>.
   */
  virtual const ShapeType &shape() const = 0;

  /**
   *  @brief Storage order of @link seq @endlink.
   *
   *  If the array is one-dimensional, the return value is
   *  implementation-defined and should not be used in defining the storage
   *  order of other multidimensional arrays.
   */
  virtual StorageOrder storage_order() const = 0;

  /**
   *  @brief Backing foreign sequence as a @link CMultiArray @endlink.
   *
   *  Lifetime of @link CMultiArray::extents @endlink is valid only during the
   *  lifetime of <tt>this</tt>.
   */
  CMultiArray marr() const {
    return CMultiArray{
      seq().begin,
      num_dims,
      const_cast<size_type *>(shape().data()),
      seq().item_size
    };
  }

 private:
  /**
   *  @brief Expands a parameter pack of indices.
   */
  template <typename... U>
  static vector<size_type> ExpandedIndices(size_type idx, U... idxs) {
    vector<size_type> retval = { idx };

    const auto idxs_vec = ExpandedIndices(idxs...);
    retval.insert(retval.end(), idxs_vec.begin(), idxs_vec.end());

    return retval;
  }

  static vector<size_type> ExpandedIndices(size_type idx) {
    return { idx };
  }
};

/**
 *  @brief Internal implementation of @link ForeignMultiArray @endlink that
 *  deletes the backing sequence upon destruction.
 *
 *  @private
 */
template <typename T, size_t num_dims>
class ForeignMultiArrayImpl final : public ForeignMultiArray<T, num_dims> {
 public:
  using typename ForeignMultiArray<T, num_dims>::ShapeType;

  virtual ~ForeignMultiArrayImpl() {
    seq_deleter_(seq_, shape_);
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

  friend ForeignMultiArray<T, num_dims>;

 private:
  using typename ForeignMultiArray<T, num_dims>::Deleter;

  ForeignMultiArrayImpl() = default;

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

  /**
   *  @brief Deleter of the backing sequence.
   */
  Deleter seq_deleter_;
};

template <typename T>
using ForeignContainer = ForeignMultiArray<T, 1>;

} // namespace interop
} // namespace tanuki

namespace avro {

using tanuki::interop::ForeignContainer;

/**
 *  @brief Encoding and decoding of @link ForeignContainer @endlink.
 *
 *  Avro schema is <tt>array</tt>, where each item has the Avro type
 *  corresponding to <tt>T</tt>.
 *
 *  For decoding, items are decoded starting at the beginning of the foreign
 *  container, which must be allocated to a size that can contain the decoded
 *  items. For each item, the backing pointer must not be reassigned.
 *
 *  @tparam T
 *    Type of decorated items. It must be Avro encodable and decodable.
 */
template <typename T>
struct codec_traits<ForeignContainer<T>> {
 public:
  static void encode(Encoder &e, const ForeignContainer<T> &o) {
    e.arrayStart();
    if (!o.empty()) {
      e.setItemCount(o.size());
      for (const auto &item : o) {
        e.startItem();
        avro::encode(e, item);
      }
    }
    e.arrayEnd();
  }

  static void decode(Decoder &d, ForeignContainer<T> &o) {
    auto d_item_it = o.begin();
    for (size_t n = d.arrayStart(); n != 0; n = d.arrayNext()) {
      for (size_t i = 0; i != n; ++i) {
        auto d_item = *d_item_it++;
        avro::decode(d, d_item);
      }
    }
  }
};

} // namespace avro

#endif
