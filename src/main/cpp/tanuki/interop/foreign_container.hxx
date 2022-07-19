#ifndef TANUKI_INTEROP_FOREIGN_CONTAINER_HXX
#define TANUKI_INTEROP_FOREIGN_CONTAINER_HXX

#include <algorithm>
#include <limits>
#include <stdexcept>

namespace tanuki {
namespace interop {

/**
 *  @brief Internal implementation of @link ForeignContainer @endlink that
 *  deletes the backing sequence upon destruction.
 *
 *  @private
 */
template <typename T, typename S = size_t>
class ForeignContainerImpl final : public ForeignContainer<T, S> {
 public:
  virtual ~ForeignContainerImpl() {
    seq_deleter_(seq_);
  }

  CSequence seq() const override final {
    return seq_;
  }

  friend unique_ptr<ForeignContainer<T, S>> ForeignContainer<T, S>::Create(
      CSequence, function<void(CSequence)>);

 private:
  /**
   *  See @link ForeignContainer::Create @endlink.
   */
  ForeignContainerImpl(CSequence seq, function<void(CSequence)> seq_deleter)
      : seq_(seq),
        seq_deleter_(seq_deleter) {}

  /**
   *  @brief Backing data member for @link seq @endlink.
   */
  CSequence seq_;

  /**
   *  @brief Deleter of the backing sequence.
   */
  function<void(CSequence)> seq_deleter_;
};

template <typename T, typename S>
unique_ptr<ForeignContainer<T, S>> ForeignContainer<T, S>::Create(
    CSequence seq, function<void(CSequence)> seq_deleter) {
  return unique_ptr<ForeignContainer<T, S>>(
      new ForeignContainerImpl<T, S>(seq, seq_deleter));
}

template <typename T, typename S>
auto ForeignContainer<T, S>::operator[](size_type idx) const -> value_type {
  return begin()[idx];
}

template <typename T, typename S>
auto ForeignContainer<T, S>::begin() -> iterator {
  return iterator(seq().begin, 0, seq().item_size);
}

template <typename T, typename S>
auto ForeignContainer<T, S>::end() -> iterator {
  return iterator(
      static_cast<char *>(seq().begin) + seq().item_size * seq().num_items,
      seq().num_items,
      seq().item_size);
}

template <typename T, typename S>
auto ForeignContainer<T, S>::begin() const -> const_iterator {
  return iterator(seq().begin, 0, seq().item_size);
}

template <typename T, typename S>
auto ForeignContainer<T, S>::end() const -> const_iterator {
  return iterator(
      static_cast<char *>(seq().begin) + seq().item_size * seq().num_items,
      seq().num_items,
      seq().item_size);
}

template <typename T, typename S>
auto ForeignContainer<T, S>::cbegin() const -> const_iterator {
  return iterator(seq().begin, 0, seq().item_size);
}

template <typename T, typename S>
auto ForeignContainer<T, S>::cend() const -> const_iterator {
  return iterator(
      static_cast<char *>(seq().begin) + seq().item_size * seq().num_items,
      seq().num_items,
      seq().item_size);
}

template <typename T, typename S>
auto ForeignContainer<T, S>::size() const -> size_type {
  return seq().num_items;
}

template <typename T, typename S>
auto ForeignContainer<T, S>::max_size() const -> size_type {
  return std::numeric_limits<size_t>::max();
}

template <typename T, typename S>
bool ForeignContainer<T, S>::empty() const {
  return size() == 0;
}

template <typename T, typename S>
auto ForeignContainer<T, S>::front() const -> value_type {
  return *begin();
}

template <typename T, typename S>
auto ForeignContainer<T, S>::back() const -> value_type {
  auto tmp = end();
  --tmp;
  return *tmp;
}

template <typename T, typename S>
auto ForeignContainer<T, S>::at(size_type idx) const -> value_type {
  if (idx >= size()) {
    throw std::out_of_range("Out of range for foreign container.");
  }

  return operator[](idx);
}

} // namespace interop
} // namespace tanuki

#endif
