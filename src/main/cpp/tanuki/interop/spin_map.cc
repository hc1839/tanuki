#include "tanuki/interop/spin_map.h"

#include <stdexcept>
#include <utility>

namespace tanuki {
namespace interop {

map<Spin, vector<void *>> SpinMap(
    const CMultiArray &marr, bool is_col_major, bool is_col_spin) {
  if (marr.num_dims != 2) {
    throw std::logic_error(
        "Number of dimensions of the multi-array is not 1 or 2.");
  }

  bool is_spin_polarized;

  if (is_col_spin) {
    if (marr.extents[1] != 1 && marr.extents[1] != 2) {
      throw std::logic_error(
          "Number of columns in the multi-array is not 1 or 2.");
    }
    is_spin_polarized = marr.extents[1] == 2;
  } else {
    if (marr.extents[0] != 1 && marr.extents[0] != 2) {
      throw std::logic_error(
          "Number of rows in the multi-array is not 1 or 2.");
    }
    is_spin_polarized = marr.extents[0] == 2;
  }

  // Buffer for storing pointers to the items with contiguous grouping by spin.
  // For spin polarization, front half of the buffer is for the up spin.
  vector<void *> contig;

  // Source iterators.
  const auto begin_it = static_cast<char *>(marr.begin);
  const auto end_it = begin_it + marr.item_size * marr.num_items();

  // Store pointers in the buffer.
  if (!is_spin_polarized || is_col_major == is_col_spin) {
    for (auto it = begin_it; it != end_it; it += marr.item_size) {
      contig.push_back(it);
    }
  } else {
    contig.resize(marr.num_items());

    // Destination iterators.
    auto d_up_it = contig.begin();
    auto d_down_it = contig.begin() + contig.size() / 2;

    const size_t two_item_size = marr.item_size * 2;

    for (auto it = begin_it; it != end_it; it += two_item_size) {
      *d_up_it++ = it;
      *d_down_it++ = it + marr.item_size;
    }
  }

  map<Spin, vector<void *>> retval;

  // Store pointers in the return buffer.
  if (is_spin_polarized) {
    auto &up = retval[Spin::UP];
    auto &down = retval[Spin::DOWN];

    auto up_it = contig.begin();
    auto down_it = contig.begin() + contig.size() / 2;

    while (down_it != contig.end()) {
      up.push_back(*up_it++);
      down.push_back(*down_it++);
    }
  } else {
    retval[Spin::DUAL] = std::move(contig);
  }

  return retval;
}

map<Spin, void *> SpinMap(const CSequence &seq) {
  map<Spin, void *> retval;

  switch (seq.num_items) {
    case 1: {
      retval[Spin::DUAL] = seq.begin;
      break;
    }
    case 2: {
      retval[Spin::UP] = seq.begin;
      retval[Spin::DOWN] = static_cast<char *>(seq.begin) + seq.item_size;
      break;
    }
    default: {
      throw std::logic_error(
          "Number of items in the sequence is not 1 or 2.");
      break;
    }
  }

  return retval;
}

} // namespace interop
} // namespace tanuki
