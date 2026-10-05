// competitive-verifier: STANDALONE
#include "shortest_path_oracle.hpp"
#include <cp/graph/dijkstra.hpp>
#include <cp/graph/floyd_warshall.hpp>
#include <random>
int main() {
  std::mt19937 rng(1907);
  for (int trial = 0; trial < 900; ++trial) {
    int n = 1 + rng() % 7;
    cp::WeightedGraph graph(n);
    for (unsigned m = rng() % 21; m--;)
      graph.add_edge(rng() % n, rng() % n, rng() % 9);
    PathOracle oracle(graph);
    auto all = cp::floyd_warshall(graph);
    for (int s = 0; s < n; ++s) {
      auto result = cp::dijkstra(graph, s);
      assert(!result.has_negative_cycle());
      for (int t = 0; t < n; ++t) {
        assert(result.state[t] == oracle.state[s][t] &&
               result.distance[t] == oracle.distance[s][t]);
        assert(result.distance[t] == all.distance[s][t]);
        check_path(graph, s, t, result.path_to(t), result.distance[t]);
      }
    }
  }
}
