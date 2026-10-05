// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/point_add_range_sum
#include <cp/data_structure/fenwick_tree.hpp>
#include <iostream>
#include <vector>
int main() {
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n, q; std::cin >> n >> q;
    std::vector<long long> values(n);
    for (auto& x : values) std::cin >> x;
    cp::FenwickTree<long long> tree(values);
    while (q--) {
        int type, l; long long r; std::cin >> type >> l >> r;
        if (type == 0) tree.add(l, r);
        else std::cout << tree.sum(l, static_cast<int>(r)) << '\n';
    }
}
