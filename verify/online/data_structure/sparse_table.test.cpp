// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/staticrmq
#include <cp/data_structure/sparse_table.hpp>
#include <algorithm>
#include <iostream>
#include <vector>
int minimum(int a, int b) { return std::min(a, b); }
int main() {
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n, q; std::cin >> n >> q;
    std::vector<int> values(n);
    for (int& x : values) std::cin >> x;
    cp::SparseTable<int, minimum> table(values);
    while (q--) { int l, r; std::cin >> l >> r; std::cout << *table.prod(l, r) << '\n'; }
}
