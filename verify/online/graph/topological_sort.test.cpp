// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/GRL_4_B
#include <cp/graph/topological_sort.hpp>
#include <iostream>
int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, m;
    std::cin >> n >> m;
    std::vector<std::vector<int>> g(n);
    for (int i = 0; i < m; ++i) {
        int a, b;
        std::cin >> a >> b;
        g[a].push_back(b);
    }
    auto order = cp::topological_sort(g);
    for (int v : *order)
        std::cout << v << '\n';
}
