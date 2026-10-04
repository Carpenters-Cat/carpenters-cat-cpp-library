// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/kth_term_of_linearly_recurrent_sequence
#include <cp/math/linear_recurrence.hpp>
#include <iostream>
using M = cp::modint998244353;
int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int d;
    std::uint64_t k;
    std::cin >> d >> k;
    std::vector<M> a(d), c(d);
    for (auto &x : a) {
        int v;
        std::cin >> v;
        x = v;
    }
    for (auto &x : c) {
        int v;
        std::cin >> v;
        x = v;
    }
    std::cout << cp::linear_recurrence_nth(a, c, k).val() << "\n";
}
