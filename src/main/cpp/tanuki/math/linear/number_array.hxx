#ifndef TANUKI_MATH_LINEAR_NUMBER_ARRAY_HXX
#define TANUKI_MATH_LINEAR_NUMBER_ARRAY_HXX

namespace tanuki {
namespace math {
namespace linear {

template <typename T>
void EncodeToNumberArray(NumberArray &e, const T &o) {
  NumberArrayConvert<T>::Encode(e, o);
}

template <typename T>
void DecodeFromNumberArray(const NumberArray &d, T &o) {
  NumberArrayConvert<T>::Decode(d, o);
}

} // namespace linear
} // namespace math
} // namespace tanuki

#endif
