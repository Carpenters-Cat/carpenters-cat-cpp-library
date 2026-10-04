// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/find_linear_recurrence
#include <cp/math/linear_recurrence.hpp>
#include <iostream>
using M = cp::modint998244353;
int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n;
    std::cin >> n;
    std::vector<M> s(n);
    for (auto &x : s) {
        int a;
        std::cin >> a;
        x = a;
    }
    auto c = cp::berlekamp_massey(s);
    std::cout << c.size() << "\n";
    for (auto x : c)
        std::cout << x.val() << ' ';
    std::cout << '\n';
}
