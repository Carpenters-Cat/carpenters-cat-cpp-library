// competitive-verifier: STANDALONE
#include <cassert>
#include <cp/math/factorization.hpp>
#include <random>
int main() {
    cp::PollardRho rho(76345);
    std::mt19937_64 rng(325617);
    for (std::uint64_t n = 1; n <= 10000; n++) {
        auto remaining = n;
        std::vector<std::uint64_t> want;
        for (std::uint64_t p = 2; p * p <= remaining; p++)
            while (remaining % p == 0) {
                want.push_back(p);
                remaining /= p;
            }
        if (remaining > 1)
            want.push_back(remaining);
        assert(rho.factorize(n) == want);
    }
    for (int t = 0; t < 100; t++) {
        std::uint64_t n = rng();
        if (!n)
            n = 1;
        auto factors = rho.factorize(n);
        __uint128_t product = 1;
        assert(std::is_sorted(factors.begin(), factors.end()));
        for (auto p : factors) {
            assert(cp::is_prime_u64(p));
            product *= p;
        }
        assert(product == n);
    }
    for (std::uint64_t seed : {0ULL, 1ULL, 999ULL}) {
        cp::PollardRho alternate(seed);
        assert((alternate.factorize(1000000007ULL * 1000000009ULL) ==
                std::vector<std::uint64_t>{1000000007ULL, 1000000009ULL}));
    }
}
