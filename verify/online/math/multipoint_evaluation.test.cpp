// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/multipoint_evaluation
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
    int n, m;
    std::cin >> n >> m;
    F f(n);
    std::vector<M> p(m);
    for (auto &x : f) {
        int a;
        std::cin >> a;
        x = a;
    }
    for (auto &x : p) {
        int a;
        std::cin >> a;
        x = a;
    }
    print(F(cp::multipoint_evaluate(f, p)));
}
