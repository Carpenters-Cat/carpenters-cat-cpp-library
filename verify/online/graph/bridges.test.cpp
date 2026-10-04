// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/GRL_3_B
#include <cp/graph/lowlink.hpp>
#include <iostream>
int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, m;
    std::cin >> n >> m;
    cp::UndirectedGraph g(n);
    for (int i = 0; i < m; ++i) {
        int a, b;
        std::cin >> a >> b;
        g.add_edge(a, b);
    }
    std::vector<std::pair<int, int>> edges;
    for (auto id : cp::lowlink(g).bridges) {
        auto e = g.get_edge(id);
        edges.emplace_back(std::min(e.from, e.to), std::max(e.from, e.to));
    }
    std::sort(edges.begin(), edges.end());
    for (auto [a, b] : edges)
        std::cout << a << ' ' << b << '\n';
}
