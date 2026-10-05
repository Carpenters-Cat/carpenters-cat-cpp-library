// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/exp_of_formal_power_series
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
    F f(n);
    for (auto &x : f) {
        int a;
        std::cin >> a;
        x = a;
    }
    print(f.exp(n));
}
