#ifndef TANUKI_MATH_LINEAR_NUMBER_ARRAY_SPECIFIC_HXX
#define TANUKI_MATH_LINEAR_NUMBER_ARRAY_SPECIFIC_HXX

#include <algorithm>
#include <cassert>
#include <vector>

namespace tanuki {
namespace math {
namespace linear {

using std::vector;

using boost::fortran_storage_order;

template <size_t D>
void NumberArrayConvert<const_multi_array_ref<real_t, D>>::Encode(
    NumberArray &e, const const_multi_array_ref<real_t, D> &o) {
#ifndef TANUKI_NO_DEBUG
  assert(o.storage_order() == fortran_storage_order());
#endif

  e = {
    .is_complex = false,
    .extents = vector<size_t>(o.shape(), o.shape() + D),
    .real = vector<real_t>(o.data(), o.data() + o.num_elements()),
    .imag = vector<real_t>(0)
  };
}

template <size_t D>
void NumberArrayConvert<const_multi_array_ref<complex_t, D>>::Encode(
    NumberArray &e, const const_multi_array_ref<complex_t, D> &o) {
#ifndef TANUKI_NO_DEBUG
  assert(o.storage_order() == fortran_storage_order());
#endif

  e = {
    .is_complex = true,
    .extents = vector<size_t>(o.shape(), o.shape() + D),
    .real = vector<real_t>(o.num_elements()),
    .imag = vector<real_t>(o.num_elements())
  };

  // Copy real and imaginary parts.
  {
    auto e_real_it = e.real.begin();
    auto e_imag_it = e.imag.begin();

    const auto o_begin_it = o.data();
    const auto o_end_it = o.data() + o.num_elements();

    for (auto o_it = o_begin_it; o_it != o_end_it; ++o_it) {
      *e_real_it++ = o_it->real();
      *e_imag_it++ = o_it->imag();
    }
  }
}

template <size_t D>
void NumberArrayConvert<multi_array<real_t, D>>::Encode(
    NumberArray &e, const multi_array<real_t, D> &o) {
  NumberArrayConvert<const_multi_array_ref<real_t, D>>::Encode(
      e,
      const_multi_array_ref<real_t, D>(
          o.data(),
          vector<size_t>(o.shape(), o.shape() + D),
          fortran_storage_order()));
}

template <size_t D>
void NumberArrayConvert<multi_array<real_t, D>>::Decode(
    const NumberArray &d, multi_array<real_t, D> &o) {
#ifndef TANUKI_NO_DEBUG
  assert(!d.is_complex);
  assert(d.extents.size() == D);
  assert(d.real.size() == d.num_elems());
  assert(o.storage_order() == fortran_storage_order());
#endif

  o.resize(d.extents);
  std::copy(d.real.begin(), d.real.end(), o.data());
}

template <size_t D>
void NumberArrayConvert<multi_array<complex_t, D>>::Encode(
    NumberArray &e, const multi_array<complex_t, D> &o) {
  NumberArrayConvert<const_multi_array_ref<complex_t, D>>::Encode(
      e,
      const_multi_array_ref<complex_t, D>(
          o.data(),
          vector<size_t>(o.shape(), o.shape() + D),
          fortran_storage_order()));
}

template <size_t D>
void NumberArrayConvert<multi_array<complex_t, D>>::Decode(
    const NumberArray &d, multi_array<complex_t, D> &o) {
#ifndef TANUKI_NO_DEBUG
  assert(d.extents.size() == D);
  assert(d.real.size() == d.num_elems());

  if (d.is_complex) {
    assert(d.imag.size() == d.num_elems());
  }

  assert(o.storage_order() == fortran_storage_order());
#endif

  o.resize(d.extents);

  if (d.is_complex) {
    auto d_imag_it = d.imag.begin();
    auto o_it = o.data();

    for (const auto &real_part : d.real) {
      *o_it++ = complex_t(real_part, *d_imag_it++);
    }
  } else {
    std::copy(d.real.begin(), d.real.end(), o.data());
  }
}

} // namespace linear
} // namespace math
} // namespace tanuki

#endif
