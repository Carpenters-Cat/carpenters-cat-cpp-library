// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/ALDS1_11_C
#include <cp/graph/zero_one_bfs.hpp>
#include <iostream>
int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  int n;
  std::cin >> n;
  cp::WeightedGraph graph(n);
  for (int i = 0; i < n; ++i) {
    int from, k;
    std::cin >> from >> k;
    while (k--) {
      int to;
      std::cin >> to;
      graph.add_edge(from - 1, to - 1, 1);
    }
  }
  auto result = cp::zero_one_bfs(graph, 0);
  for (int v = 0; v < n; ++v)
    std::cout << v + 1 << ' ' << result.distance[v].value_or(-1) << '\n';
}
