// competitive-verifier: STANDALONE
#include <cassert>
#include <cp/math/primality.hpp>
#include <limits>
int main() {
    assert(!cp::is_prime(-59) && !cp::is_prime(0) && !cp::is_prime(1));
    assert(cp::is_prime(2) && cp::is_prime(37) && cp::is_prime(97));
    for (std::uint64_t n : {4ULL, 341ULL, 561ULL, 1105ULL, 1729ULL, 3215031751ULL,
                            341550071728321ULL, 3825123056546413051ULL})
        assert(!cp::is_prime_u64(n));
    assert(cp::is_prime_u64(18446744073709551557ULL));
    assert(cp::is_prime_u64(2305843009213693951ULL));
    auto maximum = std::numeric_limits<std::uint64_t>::max();
    assert(!cp::is_prime_u64(maximum));
    assert(cp::mul_mod_u64(maximum - 1, maximum - 1, maximum) == 1);
    assert(cp::pow_mod_u64(maximum, 0, 1) == 0 && cp::pow_mod_u64(0, 0, 17) == 1);
}
