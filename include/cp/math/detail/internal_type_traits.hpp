// Adapted from AtCoder Library (864245a00b00dd008d1abfdc239618fdb7d139da),
// CC0-1.0. https://github.com/atcoder/ac-library; see
// docs/third_party/acl-math.md.
#ifndef CP_ACL_INTERNAL_TYPE_TRAITS_HPP
#define CP_ACL_INTERNAL_TYPE_TRAITS_HPP

#include <cassert>
#include <numeric>
#include <type_traits>

namespace cp {

namespace acl_math_internal {

#ifndef _MSC_VER
template <class T>
using is_signed_int128 =
    typename std::conditional<std::is_same<T, __int128_t>::value ||
                                  std::is_same<T, __int128>::value,
                              std::true_type, std::false_type>::type;

template <class T>
using is_unsigned_int128 =
    typename std::conditional<std::is_same<T, __uint128_t>::value ||
                                  std::is_same<T, unsigned __int128>::value,
                              std::true_type, std::false_type>::type;

template <class T>
using make_unsigned_int128 =
    typename std::conditional<std::is_same<T, __int128_t>::value, __uint128_t,
                              unsigned __int128>;

template <class T>
using is_integral =
    typename std::conditional<std::is_integral<T>::value ||
                                  is_signed_int128<T>::value ||
                                  is_unsigned_int128<T>::value,
                              std::true_type, std::false_type>::type;

template <class T>
using is_signed_int =
    typename std::conditional<(is_integral<T>::value &&
                               std::is_signed<T>::value) ||
                                  is_signed_int128<T>::value,
                              std::true_type, std::false_type>::type;

template <class T>
using is_unsigned_int =
    typename std::conditional<(is_integral<T>::value &&
                               std::is_unsigned<T>::value) ||
                                  is_unsigned_int128<T>::value,
                              std::true_type, std::false_type>::type;

template <class T>
using to_unsigned = typename std::conditional<
    is_signed_int128<T>::value, make_unsigned_int128<T>,
    typename std::conditional<std::is_signed<T>::value, std::make_unsigned<T>,
                              std::common_type<T>>::type>::type;

#else

template <class T> using is_integral = typename std::is_integral<T>;

template <class T>
using is_signed_int =
    typename std::conditional<is_integral<T>::value && std::is_signed<T>::value,
                              std::true_type, std::false_type>::type;

template <class T>
using is_unsigned_int =
    typename std::conditional<is_integral<T>::value &&
                                  std::is_unsigned<T>::value,
                              std::true_type, std::false_type>::type;

template <class T>
using to_unsigned =
    typename std::conditional<is_signed_int<T>::value, std::make_unsigned<T>,
                              std::common_type<T>>::type;

#endif

template <class T>
using is_signed_int_t = std::enable_if_t<is_signed_int<T>::value>;

template <class T>
using is_unsigned_int_t = std::enable_if_t<is_unsigned_int<T>::value>;

template <class T> using to_unsigned_t = typename to_unsigned<T>::type;

} // namespace acl_math_internal

} // namespace cp

#endif // CP_ACL_INTERNAL_TYPE_TRAITS_HPP
