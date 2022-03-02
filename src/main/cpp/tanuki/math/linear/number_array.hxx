#ifndef TANUKI_MATH_LINEAR_NUMBER_ARRAY_HXX
#define TANUKI_MATH_LINEAR_NUMBER_ARRAY_HXX

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <vector>

namespace tanuki {
namespace math {
namespace linear {

using boost::fortran_storage_order;
using tanuki::number::complex_t;

template <typename E, size_t D>
class NumberMultiArrayConversion;

/**
 *  @brief Helper class for converting between @link NumberArray @endlink and
 *  real Boost multi-array in column-major order.
 *
 *  @tparam D
 *    Number of dimensions.
 */
template <size_t D>
class NumberMultiArrayConversion<real_t, D> final {
 public:
  static NumberArray MultiToNumberArray(
      const const_multi_array_ref<real_t, D> &src) {
#ifndef TANUKI_NO_DEBUG
    assert(src.storage_order() == fortran_storage_order());
#endif

    return {
      .is_complex = false,
      .extents = vector<size_t>(src.shape(), src.shape() + D),
      .real = vector<real_t>(src.data(), src.data() + src.num_elements())
    };
  }

  static NumberArray MultiToNumberArray(const multi_array<real_t, D> &src) {
    return MultiToNumberArray(
        const_multi_array_ref<real_t, D>(
            src.data(),
            vector<size_t>(src.shape(), src.shape() + D),
            fortran_storage_order()));
  }

  static multi_array<real_t, D> NumberToMultiArray(const NumberArray &src) {
#ifndef TANUKI_NO_DEBUG
    assert(!src.is_complex);
    assert(src.extents.size() == D);
    assert(src.real.size() == src.num_elems());
#endif

    multi_array<real_t, D> retval(src.extents, fortran_storage_order());
    std::copy(src.real.begin(), src.real.end(), retval.data());

    return retval;
  }
};

/**
 *  @brief Helper class for converting between @link NumberArray @endlink and
 *  complex Boost multi-array in column-major order.
 *
 *  @tparam D
 *    Number of dimensions.
 */
template <size_t D>
class NumberMultiArrayConversion<complex_t, D> final {
 public:
  static NumberArray MultiToNumberArray(
      const const_multi_array_ref<complex_t, D> &src) {
#ifndef TANUKI_NO_DEBUG
    assert(src.storage_order() == fortran_storage_order());
#endif

    NumberArray retval = {
      .is_complex = true,
      .extents = vector<size_t>(src.shape(), src.shape() + D),
      .real = vector<real_t>(src.num_elements()),
      .imag = vector<real_t>(src.num_elements())
    };

    std::transform(
        src.data(), src.data() + src.num_elements(), retval.real.begin(),
        [](const complex_t &item) -> real_t {
          return item.real();
        });

    std::transform(
        src.data(), src.data() + src.num_elements(), retval.imag.begin(),
        [](const complex_t &item) -> real_t {
          return item.imag();
        });

    return retval;
  }

  static NumberArray MultiToNumberArray(const multi_array<complex_t, D> &src) {
    return MultiToNumberArray(
        const_multi_array_ref<complex_t, D>(
            src.data(),
            vector<size_t>(src.shape(), src.shape() + D),
            fortran_storage_order()));
  }

  static multi_array<complex_t, D> NumberToMultiArray(const NumberArray &src) {
#ifndef TANUKI_NO_DEBUG
    assert(src.extents.size() == D);

    {
      const size_t num_elems = src.num_elems();

      assert(src.real.size() == num_elems);

      if (src.is_complex) {
        assert(src.imag.size() == num_elems);
      }
    }
#endif

    multi_array<complex_t, D> retval(src.extents, fortran_storage_order());

    if (src.is_complex) {
      auto imag_part_it = src.imag.begin();
      auto dst_it = retval.data();

      for (const auto &real_part : src.real) {
        *dst_it++ = complex_t(real_part, *imag_part_it++);
      }
    } else {
      std::copy(src.real.begin(), src.real.end(), retval.data());
    }

    return retval;
  }
};

template <typename E, size_t D>
NumberArray MultiToNumberArray(const const_multi_array_ref<E, D> &src) {
  return NumberMultiArrayConversion<E, D>::MultiToNumberArray(src);
}

template <typename E, size_t D>
NumberArray MultiToNumberArray(const multi_array<E, D> &src) {
  return NumberMultiArrayConversion<E, D>::MultiToNumberArray(src);
}

template <typename E, size_t D>
multi_array<E, D> NumberToMultiArray(const NumberArray &src) {
  return NumberMultiArrayConversion<E, D>::NumberToMultiArray(src);
}

} // namespace linear
} // namespace math
} // namespace tanuki

#endif
