#ifndef CP_MATH_PRIMALITY_HPP
#define CP_MATH_PRIMALITY_HPP
#include <array>
#include <bit>
#include <cassert>
#include <concepts>
#include <cstdint>
#include <initializer_list>
#include <limits>
namespace cp {
inline std::uint64_t mul_mod_u64(std::uint64_t a, std::uint64_t b, std::uint64_t modulus) {
    assert(modulus != 0);
    return static_cast<std::uint64_t>(static_cast<__uint128_t>(a) * b % modulus);
}
inline std::uint64_t pow_mod_u64(std::uint64_t a, std::uint64_t exponent, std::uint64_t modulus) {
    assert(modulus != 0);
    std::uint64_t result = 1 % modulus;
    a %= modulus;
    while (exponent) {
        if (exponent & 1)
            result = mul_mod_u64(result, a, modulus);
        a = mul_mod_u64(a, a, modulus);
        exponent >>= 1;
    }
    return result;
}
inline bool is_prime_u64(std::uint64_t n) {
    if (n < 2)
        return false;
    for (std::uint64_t p : {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37})
        if (n % p == 0)
            return n == p;
    int s = std::countr_zero(n - 1);
    std::uint64_t d = (n - 1) >> s;
    // Jim Sinclair's deterministic full-uint64 set, recorded at miller-rabin.appspot.com.
    constexpr std::array<std::uint64_t, 7> bases{2, 325, 9375, 28178, 450775, 9780504, 1795265022};
    for (auto a : bases) {
        if (a % n == 0)
            continue;
        std::uint64_t x = pow_mod_u64(a, d, n);
        if (x == 1 || x == n - 1)
            continue;
        bool passed = false;
        for (int r = 1; r < s; ++r) {
            x = mul_mod_u64(x, x, n);
            if (x == n - 1) {
                passed = true;
                break;
            }
        }
        if (!passed)
            return false;
    }
    return true;
}
template <std::integral Integer> inline bool is_prime(Integer n) {
    static_assert(std::numeric_limits<Integer>::digits <= 64);
    if constexpr (std::signed_integral<Integer>) {
        if (n < 2)
            return false;
    }
    return is_prime_u64(static_cast<std::uint64_t>(n));
}
} // namespace cp
#endif
