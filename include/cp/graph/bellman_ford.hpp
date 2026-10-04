#pragma once

#include <cp/graph/weighted_graph.hpp>

namespace cp {
inline ShortestPathResult bellman_ford(const WeightedGraph &graph, int source) {
  using shortest_path_detail::Wide;
  int n = graph.size();
  assert(0 <= source && source < n);
  std::vector<std::optional<Wide>> distance(n);
  std::vector<int> predecessor(n, -1);
  std::vector<std::optional<std::size_t>> edge_id(n);
  std::vector<bool> negative(n);
  distance[source] = 0;
  auto next = distance;
  std::vector<int> updated_round(n, -1);
  // Synchronous relaxation limits every intermediate walk to at most n edges.
  for (int round = 0; round < n; ++round) {
    std::vector<int> changed;
    for (std::size_t id = 0; id < graph.edges().size(); ++id) {
      const auto &edge = graph.get_edge(id);
      if (!distance[edge.from])
        continue;
      Wide candidate = *distance[edge.from] + Wide(edge.weight);
      if (!next[edge.to] || candidate < *next[edge.to]) {
        next[edge.to] = candidate;
        predecessor[edge.to] = edge.from;
        edge_id[edge.to] = id;
        if (updated_round[edge.to] != round) {
          updated_round[edge.to] = round;
          changed.push_back(edge.to);
        }
        if (round == n - 1)
          negative[edge.to] = true;
      }
    }
    for (int vertex : changed)
      distance[vertex] = next[vertex];
    if (changed.empty())
      break;
  }
  std::vector<int> queue;
  for (int v = 0; v < n; ++v)
    if (negative[v])
      queue.push_back(v);
  for (std::size_t index = 0; index < queue.size(); ++index) {
    int vertex = queue[index];
    for (std::size_t id : graph.outgoing(vertex)) {
      int to = graph.get_edge(id).to;
      if (!negative[to]) {
        negative[to] = true;
        queue.push_back(to);
      }
    }
  }
  return shortest_path_detail::finish(source, distance, std::move(predecessor),
                                      std::move(edge_id), negative);
}
} // namespace cp
