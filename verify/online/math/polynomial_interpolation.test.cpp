// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/polynomial_interpolation
#include <cp/math/formal_power_series.hpp>
#include <iostream>
using M = cp::modint998244353;
using F = cp::FPS<M>;
void print(const F &f) {
    for (auto x : f)
        std::cout << x.val() << ' ';
    std::cout << '\n';
}
int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n;
    std::cin >> n;
    std::vector<M> p(n), v(n);
    for (auto &x : p) {
        int a;
        std::cin >> a;
        x = a;
    }
    for (auto &x : v) {
        int a;
        std::cin >> a;
        x = a;
    }
    print(cp::interpolate(p, v));
}
