// competitive-verifier: STANDALONE
#include "shortest_path_oracle.hpp"
#include <cp/graph/dijkstra.hpp>
#include <cp/graph/zero_one_bfs.hpp>
#include <random>
int main() {
  std::mt19937 rng(3101);
  for (int trial = 0; trial < 1600; ++trial) {
    int n = 1 + rng() % 20;
    cp::WeightedGraph graph(n);
    for (unsigned m = rng() % 81; m--;)
      graph.add_edge(rng() % n, rng() % n, rng() % 2);
    for (int s = 0; s < n; ++s) {
      auto result = cp::zero_one_bfs(graph, s), other = cp::dijkstra(graph, s);
      assert(result.distance == other.distance && result.state == other.state);
      for (int t = 0; t < n; ++t)
        check_path(graph, s, t, result.path_to(t), result.distance[t]);
    }
  }
}
