// competitive-verifier: STANDALONE
#include "shortest_path_oracle.hpp"
#include <cp/graph/bellman_ford.hpp>
#include <random>
int main() {
  std::mt19937 rng(7192);
  for (int trial = 0; trial < 1400; ++trial) {
    int n = 1 + rng() % 7;
    cp::WeightedGraph graph(n);
    for (unsigned m = rng() % 22; m--;)
      graph.add_edge(rng() % n, rng() % n, int(rng() % 13) - 6);
    PathOracle oracle(graph);
    for (int s = 0; s < n; ++s) {
      auto result = cp::bellman_ford(graph, s);
      for (int t = 0; t < n; ++t) {
        assert(result.state[t] == oracle.state[s][t] &&
               result.distance[t] == oracle.distance[s][t]);
        check_path(graph, s, t, result.path_to(t), result.distance[t]);
      }
    }
  }
}
