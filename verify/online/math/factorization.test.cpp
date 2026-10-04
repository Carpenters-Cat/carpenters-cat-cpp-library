// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/factorize
#include <cp/math/factorization.hpp>
#include <iostream>
int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int q;
    std::cin >> q;
    cp::PollardRho rho;
    while (q--) {
        std::uint64_t n;
        std::cin >> n;
        auto f = rho.factorize(n);
        std::cout << f.size();
        for (auto p : f)
            std::cout << ' ' << p;
        std::cout << '\n';
    }
}
