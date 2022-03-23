#include "tanuki/math/linear/number_array_specific.h"

#include <algorithm>
#include <cassert>
#include <vector>

namespace tanuki {
namespace math {
namespace linear {

using std::vector;

void NumberArrayConvert<Mat<real_t>>::Encode(
    NumberArray &e, const Mat<real_t> &o) {
  e = {
    .is_complex = false,
    .extents = { o.n_rows, o.n_cols },
    .real = vector<real_t>(o.begin(), o.end()),
    .imag = vector<real_t>(0)
  };
}

void NumberArrayConvert<Mat<real_t>>::Decode(
    const NumberArray &d, Mat<real_t> &o) {
#ifndef TANUKI_NO_DEBUG
  assert(!d.is_complex);
  assert(d.extents.size() == 2);
  assert(d.real.size() == d.num_elems());
#endif

  o = Mat<real_t>(d.extents[0], d.extents[1]);

  std::copy(d.real.begin(), d.real.end(), o.begin());
}

void NumberArrayConvert<Mat<complex_t>>::Encode(
    NumberArray &e, const Mat<complex_t> &o) {
  e = {
    .is_complex = true,
    .extents = { o.n_rows, o.n_cols },
    .real = vector<real_t>(o.n_elem),
    .imag = vector<real_t>(o.n_elem)
  };

  // Copy real and imaginary parts.
  {
    auto e_real_it = e.real.begin();
    auto e_imag_it = e.imag.begin();

    for (const auto &o_item : o) {
      *e_real_it++ = o_item.real();
      *e_imag_it++ = o_item.imag();
    }
  }
}

void NumberArrayConvert<Mat<complex_t>>::Decode(
    const NumberArray &d, Mat<complex_t> &o) {
#ifndef TANUKI_NO_DEBUG
  assert(d.extents.size() == 2);
  assert(d.real.size() == d.num_elems());

  if (d.is_complex) {
    assert(d.imag.size() == d.num_elems());
  }
#endif

  o = Mat<complex_t>(d.extents[0], d.extents[1]);

  // Copy real and imaginary parts.
  if (d.is_complex) {
    auto d_real_it = d.real.begin();
    auto d_imag_it = d.imag.begin();

    for (auto &o_item : o) {
      o_item = complex_t(*d_real_it++, *d_imag_it++);
    }
  } else {
    std::copy(d.real.begin(), d.real.end(), o.begin());
  }
}

void NumberArrayConvert<Cube<real_t>>::Encode(
    NumberArray &e, const Cube<real_t> &o) {
  e = {
    .is_complex = false,
    .extents = { o.n_rows, o.n_cols, o.n_slices },
    .real = vector<real_t>(o.begin(), o.end()),
    .imag = vector<real_t>(0)
  };
}

void NumberArrayConvert<Cube<real_t>>::Decode(
    const NumberArray &d, Cube<real_t> &o) {
#ifndef TANUKI_NO_DEBUG
  assert(!d.is_complex);
  assert(d.extents.size() == 3);
  assert(d.real.size() == d.num_elems());
#endif

  o = Cube<real_t>(d.extents[0], d.extents[1], d.extents[2]);

  std::copy(d.real.begin(), d.real.end(), o.begin());
}

void NumberArrayConvert<Cube<complex_t>>::Encode(
    NumberArray &e, const Cube<complex_t> &o) {
  e = {
    .is_complex = true,
    .extents = { o.n_rows, o.n_cols, o.n_slices },
    .real = vector<real_t>(o.n_elem),
    .imag = vector<real_t>(o.n_elem)
  };

  // Copy real and imaginary parts.
  {
    auto e_real_it = e.real.begin();
    auto e_imag_it = e.imag.begin();

    for (const auto &o_item : o) {
      *e_real_it++ = o_item.real();
      *e_imag_it++ = o_item.imag();
    }
  }
}

void NumberArrayConvert<Cube<complex_t>>::Decode(
    const NumberArray &d, Cube<complex_t> &o) {
#ifndef TANUKI_NO_DEBUG
  assert(d.extents.size() == 3);
  assert(d.real.size() == d.num_elems());

  if (d.is_complex) {
    assert(d.imag.size() == d.num_elems());
  }
#endif

  o = Cube<complex_t>(d.extents[0], d.extents[1], d.extents[2]);

  // Copy real and imaginary parts.
  if (d.is_complex) {
    auto d_real_it = d.real.begin();
    auto d_imag_it = d.imag.begin();

    for (auto &o_item : o) {
      o_item = complex_t(*d_real_it++, *d_imag_it++);
    }
  } else {
    std::copy(d.real.begin(), d.real.end(), o.begin());
  }
}

} // namespace linear
} // namespace math
} // namespace tanuki
