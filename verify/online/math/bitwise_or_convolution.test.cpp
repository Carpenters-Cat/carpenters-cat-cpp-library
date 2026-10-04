// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/bitwise_and_convolution
// De Morgan's bijection: complemented-index OR equals complemented-index AND.
#include <cp/math/bitwise_convolution.hpp>
#include <cp/math/modint.hpp>
#include <algorithm>
#include <iostream>
#include <vector>

int main() {
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int log_n; std::cin >> log_n;
    const int n = 1 << log_n;
    using Mint = cp::modint998244353;
    std::vector<Mint> a(n), b(n);
    int x;
    for (auto& v : a) { std::cin >> x; v = x; }
    for (auto& v : b) { std::cin >> x; v = x; }
    std::reverse(a.begin(), a.end()); std::reverse(b.begin(), b.end());
    auto c = cp::or_convolution(a, b);
    std::reverse(c.begin(), c.end());
    for (int i = 0; i < n; ++i) std::cout << c[i].val() << (i + 1 == n ? '\n' : ' ');
}
