// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/range_affine_range_sum
#include <cp/data_structure/lazy_segment_tree.hpp>
#include <iostream>
#include <vector>
constexpr long long mod = 998244353;
struct S { long long sum; int length; };
struct F { long long a, b; };
S op(S x, S y) { return {(x.sum + y.sum) % mod, x.length + y.length}; }
S e() { return {0, 0}; }
S mapping(F f, S x) { return {(f.a * x.sum + f.b * x.length) % mod, x.length}; }
F composition(F f, F g) { return {f.a * g.a % mod, (f.a * g.b + f.b) % mod}; }
F id() { return {1, 0}; }
int main() {
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n, q; std::cin >> n >> q;
    std::vector<S> values(n);
    for (auto& x : values) { std::cin >> x.sum; x.length = 1; }
    cp::LazySegmentTree<S, op, e, F, mapping, composition, id> tree(values);
    while (q--) {
        int type, l, r; std::cin >> type >> l >> r;
        if (type == 0) { F f; std::cin >> f.a >> f.b; tree.apply(l, r, f); }
        else std::cout << tree.prod(l, r).sum << '\n';
    }
}
