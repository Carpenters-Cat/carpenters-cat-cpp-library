// competitive-verifier: STANDALONE
#include <cassert>
#include <cp/math/sieve.hpp>
int main() {
    cp::PrimeSieve zero(0);
    assert(zero.primes().empty() && !zero.is_prime(0));
    assert(cp::enumerate_primes(0).empty() && cp::enumerate_primes(1).empty());
    cp::PrimeSieve one(1);
    assert(one.factorize(1).empty() && one.smallest_prime_factor(1) == 1);
    bool rejected = false;
    try {
        one.factorize(0);
    } catch (const std::invalid_argument &) {
        rejected = true;
    }
    assert(rejected);
    cp::PrimeSieve s(101);
    assert(s.is_prime(2) && s.is_prime(101) && !s.is_prime(100));
    assert(s.smallest_prime_factor(100) == 2 && s.smallest_prime_factor(49) == 7);
    assert((s.factorize(72) == std::vector<std::pair<int, int>>{{2, 3}, {3, 2}}));
    assert(s.prime_table()[101] && s.smallest_factors()[101] == 101);
    assert(cp::enumerate_primes(101) == s.primes());
    cp::PrimeSieve blocks(131073);
    assert(cp::enumerate_primes(131073) == blocks.primes());
}
