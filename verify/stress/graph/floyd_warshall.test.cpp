// competitive-verifier: STANDALONE
#include "shortest_path_oracle.hpp"
#include <cp/graph/bellman_ford.hpp>
#include <cp/graph/floyd_warshall.hpp>
#include <random>
int main() {
  std::mt19937 rng(42010);
  for (int trial = 0; trial < 1400; ++trial) {
    int n = 1 + rng() % 7;
    cp::WeightedGraph graph(n);
    for (unsigned m = rng() % 22; m--;)
      graph.add_edge(rng() % n, rng() % n, int(rng() % 13) - 6);
    PathOracle oracle(graph);
    auto result = cp::floyd_warshall(graph);
    cp::DistanceMatrix matrix(n, std::vector<std::optional<cp::Distance>>(n));
    for (const auto &edge : graph.edges())
      if (!matrix[edge.from][edge.to] ||
          edge.weight < *matrix[edge.from][edge.to])
        matrix[edge.from][edge.to] = edge.weight;
    auto from_matrix = cp::floyd_warshall(matrix);
    assert(result.distance == from_matrix.distance &&
           result.state == from_matrix.state);
    for (int s = 0; s < n; ++s) {
      auto single = cp::bellman_ford(graph, s);
      for (int t = 0; t < n; ++t) {
        assert(result.state[s][t] == oracle.state[s][t] &&
               result.distance[s][t] == oracle.distance[s][t]);
        assert(result.state[s][t] == single.state[t] &&
               result.distance[s][t] == single.distance[t]);
        check_path(graph, s, t, result.path(s, t), result.distance[s][t]);
        auto path = from_matrix.path(s, t);
        if (!result.distance[s][t]) {
          assert(!path);
          continue;
        }
        assert(path && path->vertices.front() == s &&
               path->vertices.back() == t);
        __int128_t cost = 0;
        for (std::size_t k = 0; k < path->edges.size(); ++k) {
          int from = path->vertices[k], to = path->vertices[k + 1];
          assert(path->edges[k] == static_cast<std::size_t>(from * n + to));
          assert(matrix[from][to]);
          cost += *matrix[from][to];
        }
        assert(cost == *result.distance[s][t]);
      }
    }
  }
}
