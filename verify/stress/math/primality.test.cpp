// competitive-verifier: STANDALONE
#include <cassert>
#include <cp/math/primality.hpp>
#include <cp/math/sieve.hpp>
#include <random>
std::uint64_t add(std::uint64_t a, std::uint64_t b, std::uint64_t m) {
    return a >= m - b ? a - (m - b) : a + b;
}
std::uint64_t multiply(std::uint64_t a, std::uint64_t b, std::uint64_t m) {
    a %= m;
    std::uint64_t r = 0;
    while (b) {
        if (b & 1)
            r = add(r, a, m);
        a = add(a, a, m);
        b >>= 1;
    }
    return r;
}
int main() {
    cp::PrimeSieve s(100000);
    for (int n = 0; n <= 100000; n++)
        assert(cp::is_prime(n) == s.is_prime(n));
    std::mt19937_64 rng(439876);
    for (int t = 0; t < 1000; t++) {
        auto a = rng(), b = rng(), m = rng();
        if (!m)
            m = 1;
        assert(cp::mul_mod_u64(a, b, m) == multiply(a, b, m));
        int e = rng() % 100;
        std::uint64_t want = 1 % m;
        for (int i = 0; i < e; i++)
            want = multiply(want, a, m);
        assert(cp::pow_mod_u64(a, e, m) == want);
    }
}
