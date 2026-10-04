// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/shortest_path
#include <cp/graph/dijkstra.hpp>
#include <iostream>
int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  int n, m, s, t;
  std::cin >> n >> m >> s >> t;
  cp::WeightedGraph graph(n);
  for (int i = 0; i < m; ++i) {
    int a, b;
    cp::Distance w;
    std::cin >> a >> b >> w;
    graph.add_edge(a, b, w);
  }
  auto result = cp::dijkstra(graph, s);
  auto path = result.path_to(t);
  if (!path) {
    std::cout << -1 << '\n';
    return 0;
  }
  std::cout << *result.distance[t] << ' ' << path->edges.size() << '\n';
  for (auto id : path->edges) {
    const auto &e = graph.get_edge(id);
    std::cout << e.from << ' ' << e.to << '\n';
  }
}
