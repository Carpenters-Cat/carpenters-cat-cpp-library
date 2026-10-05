// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/enumerate_primes
#include <cp/math/sieve.hpp>
#include <iostream>
int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, a, b;
    std::cin >> n >> a >> b;
    auto p = cp::enumerate_primes(n);
    std::size_t count = b < p.size() ? (p.size() - 1 - b) / a + 1 : 0;
    std::cout << p.size() << ' ' << count << '\n';
    for (std::size_t i = b; i < p.size(); i += a)
        std::cout << p[i] << ' ';
    std::cout << '\n';
}
