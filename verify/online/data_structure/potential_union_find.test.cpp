// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/DSL_1_B
#include <cp/data_structure/potential_union_find.hpp>
#include <iostream>
int main() {
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n, q; std::cin >> n >> q;
    cp::PotentialUnionFind<long long> uf(n);
    while (q--) {
        int type, a, b; std::cin >> type >> a >> b;
        if (type == 0) { long long d; std::cin >> d; uf.merge(a, b, d); }
        else {
            const auto d = uf.difference(a, b);
            if (d) std::cout << *d << '\n';
            else std::cout << "?\n";
        }
    }
}
