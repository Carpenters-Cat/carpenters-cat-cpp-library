// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/GRL_1_B
#include <cp/graph/bellman_ford.hpp>
#include <iostream>
int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  int n, m, s;
  std::cin >> n >> m >> s;
  cp::WeightedGraph graph(n);
  for (int i = 0; i < m; ++i) {
    int a, b;
    cp::Distance w;
    std::cin >> a >> b >> w;
    graph.add_edge(a, b, w);
  }
  auto result = cp::bellman_ford(graph, s);
  if (result.has_negative_cycle()) {
    std::cout << "NEGATIVE CYCLE\n";
    return 0;
  }
  for (auto value : result.distance) {
    if (value)
      std::cout << *value << '\n';
    else
      std::cout << "INF\n";
  }
}
