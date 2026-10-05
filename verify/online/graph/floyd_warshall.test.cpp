// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/GRL_1_C
#include <cp/graph/floyd_warshall.hpp>
#include <iostream>
int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  int n, m;
  std::cin >> n >> m;
  cp::WeightedGraph graph(n);
  for (int i = 0; i < m; ++i) {
    int a, b;
    cp::Distance w;
    std::cin >> a >> b >> w;
    graph.add_edge(a, b, w);
  }
  auto result = cp::floyd_warshall(graph);
  if (result.has_negative_cycle()) {
    std::cout << "NEGATIVE CYCLE\n";
    return 0;
  }
  for (const auto &row : result.distance) {
    for (int j = 0; j < n; ++j) {
      if (j)
        std::cout << ' ';
      if (row[j])
        std::cout << *row[j];
      else
        std::cout << "INF";
    }
    std::cout << '\n';
  }
}
