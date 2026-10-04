// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/unionfind
#include <cp/data_structure/union_find.hpp>

#include <iostream>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, q;
    std::cin >> n >> q;
    cp::UnionFind uf(n);
    for (int i = 0; i < q; ++i) {
        int type, a, b;
        std::cin >> type >> a >> b;
        if (type == 0) uf.merge(a, b);
        else std::cout << uf.same(a, b) << '\n';
    }
}
