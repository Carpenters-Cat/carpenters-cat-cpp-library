// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/minimum_spanning_tree
#include <cp/graph/kruskal.hpp>
#include <iostream>
int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, m;
    std::cin >> n >> m;
    cp::WeightedGraph g(n);
    for (int i = 0; i < m; ++i) {
        int a, b;
        cp::Distance w;
        std::cin >> a >> b >> w;
        g.add_edge(a, b, w);
    }
    auto r = cp::kruskal(g);
    std::cout << r.cost << '\n';
    for (auto id : r.edge_ids)
        std::cout << id << ' ';
    std::cout << '\n';
}
