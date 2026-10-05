// competitive-verifier: STANDALONE
#include <cassert>
#include <cp/math/sieve.hpp>
#include <random>
int main() {
    cp::PrimeSieve sieve(1000000);
    std::mt19937 rng(427613);
    for (int t = 0; t < 10000; t++) {
        int n = rng() % 1000001, remaining = n;
        std::vector<std::pair<int, int>> want;
        bool prime = n >= 2;
        for (int p = 2; 1LL * p * p <= n; p++)
            if (n % p == 0) {
                prime = false;
                break;
            }
        assert(sieve.is_prime(n) == prime);
        if (!n)
            continue;
        for (int p = 2; 1LL * p * p <= remaining; p++)
            if (remaining % p == 0) {
                int e = 0;
                while (remaining % p == 0) {
                    remaining /= p;
                    e++;
                }
                want.emplace_back(p, e);
            }
        if (remaining > 1)
            want.emplace_back(remaining, 1);
        assert(sieve.factorize(n) == want);
    }
    assert(cp::enumerate_primes(1000000) == sieve.primes());
}
