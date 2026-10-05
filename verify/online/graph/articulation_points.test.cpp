// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/GRL_3_A
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
    for (int v : cp::lowlink(g).articulation_points)
        std::cout << v << '\n';
}
