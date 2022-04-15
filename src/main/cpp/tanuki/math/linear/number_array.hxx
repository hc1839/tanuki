#ifndef TANUKI_MATH_LINEAR_NUMBER_ARRAY_HXX
#define TANUKI_MATH_LINEAR_NUMBER_ARRAY_HXX

#include <algorithm>
#include <cassert>
#include <stdexcept>

namespace tanuki {
namespace math {
namespace linear {

template <
    typename InputIt,
    typename std::enable_if<
        std::is_floating_point<
            typename std::iterator_traits<InputIt>::value_type>::value,
        bool>::type>
NumberArray NumberArray::Create(
    const vector<size_t> &extents, InputIt first, InputIt last) {
  NumberArray retval;

  retval.is_complex = false;
  retval.extents = extents;

  for (auto it = first; it != last; ++it) {
    retval.real.push_back(*it);
  }

  if (retval.real.size() != retval.num_elems()) {
    throw std::length_error("Invalid size of the range of numbers.");
  }

  return retval;
}

template <
    typename InputIt,
    typename std::enable_if<
        std::is_same<
            typename std::iterator_traits<InputIt>::value_type,
            complex_t>::value,
        bool>::type>
NumberArray NumberArray::Create(
    const vector<size_t> &extents, InputIt first, InputIt last) {
  NumberArray retval;

  retval.is_complex = true;
  retval.extents = extents;

  for (auto it = first; it != last; ++it) {
    retval.real.push_back(it->real());
    retval.imag.push_back(it->imag());
  }

  if (retval.real.size() != retval.num_elems()) {
    throw std::length_error("Invalid size of the range of numbers.");
  }

  return retval;
}

template <
    typename OutputIt,
    typename std::enable_if<
        std::is_floating_point<
            typename std::iterator_traits<OutputIt>::value_type>::value,
        bool>::type>
void Convert(const NumberArray &src, OutputIt d_first) {
  if (src.is_complex) {
    throw std::range_error(
        "Number array is complex, but destination range is real.");
  }

  if (src.real.size() != src.num_elems()) {
    throw std::length_error("Inconsistent size of the number array.");
  }

  std::copy(src.real.begin(), src.real.end(), d_first);
}

template <
    typename OutputIt,
    typename std::enable_if<
        std::is_same<
            typename std::iterator_traits<OutputIt>::value_type,
            complex_t>::value,
        bool>::type>
void Convert(const NumberArray &src, OutputIt d_first) {
  if (src.real.size() != src.num_elems() ||
      (src.is_complex && src.imag.size() != src.real.size())) {
    throw std::length_error("Inconsistent size of the number array.");
  }

  if (src.is_complex) {
    auto real_it = src.real.begin();
    auto imag_it = src.imag.begin();
    auto d_it = d_first;

    while (real_it != src.real.end() && imag_it != src.imag.end()) {
      *d_it++ = complex_t(*real_it++, *imag_it++);
    }
  } else {
    std::copy(src.real.begin(), src.real.end(), d_first);
  }
}

} // namespace linear
} // namespace math
} // namespace tanuki

#endif
