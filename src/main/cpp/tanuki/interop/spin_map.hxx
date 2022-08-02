#ifndef TANUKI_INTEROP_SPIN_MAP_HXX
#define TANUKI_INTEROP_SPIN_MAP_HXX

#include <algorithm>

namespace tanuki {
namespace interop {

template <typename T>
map<Spin, vector<T>> SpinMap(
    const CMultiArray &marr, bool is_col_major, bool is_col_spin) {
  map<Spin, vector<T>> retval;

  for (const auto &entry : SpinMap(marr, is_col_major, is_col_spin)) {
    const auto &spin = entry.first;

    for (auto ptr : entry.second) {
      retval[spin].push_back(*static_cast<const T *>(ptr));
    }
  }

  return retval;
}

template <typename T>
map<Spin, vector<T>> SpinMapDecorated(
    const CMultiArray &marr, bool is_col_major, bool is_col_spin) {
  map<Spin, vector<T>> retval;
  auto ptr_map = SpinMap(marr, is_col_major, is_col_spin);

  for (auto &entry : ptr_map) {
    const auto &spin = entry.first;

    for (auto ptr : entry.second) {
      retval[spin].push_back(T(ptr));
    }
  }

  return retval;
}

template <typename T>
map<Spin, T> SpinMap(const CSequence &seq) {
  map<Spin, T> retval;

  for (const auto &entry : SpinMap(seq)) {
    const auto &spin = entry.first;
    retval[spin] = *static_cast<const T *>(entry.second);
  }

  return retval;
}

template <typename T>
map<Spin, T> SpinMapDecorated(const CSequence &seq) {
  map<Spin, T> retval;
  auto ptr_map = SpinMap(seq);

  for (auto &entry : ptr_map) {
    const auto &spin = entry.first;
    retval[spin] = T(entry.second);
  }

  return retval;
}

template <typename T>
map<Spin, T> SpinMapDecorated(const ForeignContainer<T> &ctnr) {
  return SpinMapDecorated<T>(ctnr.seq());
}

} // namespace interop
} // namespace tanuki

#endif
