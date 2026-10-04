#pragma once
#include <cassert>
#include <cp/graph/weighted_graph.hpp>

struct PathOracle {
  using Wide = __int128_t;
  std::vector<std::vector<std::optional<Wide>>> distance;
  std::vector<std::vector<cp::DistanceState>> state;
  explicit PathOracle(const cp::WeightedGraph &graph) {
    int n = graph.size();
    distance.assign(n, std::vector<std::optional<Wide>>(n));
    state.assign(
        n, std::vector<cp::DistanceState>(n, cp::DistanceState::unreachable));
    std::vector<bool> negative(n);
    // Enumerate all simple paths and all simple cycles, independently of DP.
    for (int source = 0; source < n; ++source) {
      auto dfs = [&](auto self, int vertex, unsigned seen, Wide cost) -> void {
        if (!distance[source][vertex] || cost < *distance[source][vertex])
          distance[source][vertex] = cost;
        for (auto id : graph.outgoing(vertex)) {
          const auto &edge = graph.get_edge(id);
          Wide next = cost + edge.weight;
          if (edge.to == source && next < 0)
            negative[source] = true;
          if (!(seen >> edge.to & 1U))
            self(self, edge.to, seen | (1U << edge.to), next);
        }
      };
      dfs(dfs, source, 1U << source, 0);
    }
    std::vector<std::vector<bool>> reach(n, std::vector<bool>(n));
    for (int s = 0; s < n; ++s)
      for (int t = 0; t < n; ++t)
        reach[s][t] = distance[s][t].has_value();
    for (int s = 0; s < n; ++s)
      for (int t = 0; t < n; ++t) {
        if (distance[s][t])
          state[s][t] = cp::DistanceState::finite;
        for (int k = 0; k < n; ++k)
          if (negative[k] && reach[s][k] && reach[k][t]) {
            state[s][t] = cp::DistanceState::negative_infinity;
            distance[s][t].reset();
            break;
          }
      }
  }
};

inline void check_path(const cp::WeightedGraph &graph, int source, int target,
                       const std::optional<cp::ShortestPath> &path,
                       std::optional<cp::Distance> expected) {
  if (!expected) {
    assert(!path);
    return;
  }
  assert(path && path->vertices.front() == source &&
         path->vertices.back() == target);
  assert(path->vertices.size() == path->edges.size() + 1);
  assert(path->vertices.size() <= static_cast<std::size_t>(graph.size()));
  __int128_t cost = 0;
  for (std::size_t i = 0; i < path->edges.size(); ++i) {
    const auto &edge = graph.get_edge(path->edges[i]);
    assert(edge.from == path->vertices[i] && edge.to == path->vertices[i + 1]);
    cost += edge.weight;
  }
  assert(cost == *expected);
}
