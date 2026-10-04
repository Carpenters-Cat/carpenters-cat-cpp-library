// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/binomial_coefficient_prime_mod
#include <cp/math/combinatorics.hpp>
#include <iostream>
int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int t, p;
    std::cin >> t >> p;
    using M = cp::DynamicModint<112>;
    M::set_mod(p);
    std::vector<std::pair<int, int>> queries(t);
    int maximum = 0;
    for (auto &[n, k] : queries) {
        std::cin >> n >> k;
        maximum = std::max(maximum, n);
    }
    cp::Combinatorics<M> c(std::min(maximum, p - 1));
    // The API is for n<p; Lucas's theorem in this driver covers the judge's small primes.
    for (auto [n, k] : queries) {
        M answer = 1;
        do {
            answer *= c.nCr(n % p, k % p);
            n /= p;
            k /= p;
        } while (n || k);
        std::cout << answer.val() << '\n';
    }
}
