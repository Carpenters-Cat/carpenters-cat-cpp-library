// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/polynomial_taylor_shift
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
    int n, c;
    std::cin >> n >> c;
    F f(n);
    for (auto &x : f) {
        int a;
        std::cin >> a;
        x = a;
    }
    print(f.taylor_shift(M(c)));
}
