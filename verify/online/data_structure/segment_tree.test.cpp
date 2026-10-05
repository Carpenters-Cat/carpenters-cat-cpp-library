// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/point_set_range_composite
#include <cp/data_structure/segment_tree.hpp>
#include <iostream>
#include <vector>
constexpr long long mod = 998244353;
struct Affine { long long a, b; };
Affine op(Affine left, Affine right) {
    return {left.a * right.a % mod, (left.b * right.a + right.b) % mod};
}
Affine e() { return {1, 0}; }
int main() {
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n, q; std::cin >> n >> q;
    std::vector<Affine> values(n);
    for (auto& f : values) std::cin >> f.a >> f.b;
    cp::SegmentTree<Affine, op, e> tree(values);
    while (q--) {
        int type; std::cin >> type;
        if (type == 0) {
            int p; Affine f; std::cin >> p >> f.a >> f.b; tree.set(p, f);
        } else {
            int l, r; long long x; std::cin >> l >> r >> x;
            const auto f = tree.prod(l, r);
            std::cout << (f.a * x + f.b) % mod << '\n';
        }
    }
}
