// Adapted from AtCoder Library (864245a00b00dd008d1abfdc239618fdb7d139da),
// CC0-1.0. https://github.com/atcoder/ac-library; see
// docs/third_party/acl-math.md.
#ifndef CP_ACL_INTERNAL_BITOP_HPP
#define CP_ACL_INTERNAL_BITOP_HPP

#ifdef _MSC_VER
#include <intrin.h>
#endif

#if __cplusplus >= 202002L
#include <bit>
#endif

namespace cp {

namespace acl_math_internal {

#if __cplusplus >= 202002L

using std::bit_ceil;

#else

// @return same with std::bit::bit_ceil
unsigned int bit_ceil(unsigned int n) {
  unsigned int x = 1;
  while (x < (unsigned int)(n))
    x *= 2;
  return x;
}

#endif

// @param n `1 <= n`
// @return same with std::bit::countr_zero
inline int countr_zero(unsigned int n) {
#ifdef _MSC_VER
  unsigned long index;
  _BitScanForward(&index, n);
  return index;
#else
  return __builtin_ctz(n);
#endif
}

// @param n `1 <= n`
// @return same with std::bit::countr_zero
constexpr int countr_zero_constexpr(unsigned int n) {
  int x = 0;
  while (!(n & (1 << x)))
    x++;
  return x;
}

} // namespace acl_math_internal

} // namespace cp

#endif // CP_ACL_INTERNAL_BITOP_HPP
