#pragma once

#include <cp/graph/weighted_graph.hpp>

#include <functional>
#include <queue>

namespace cp {
inline ShortestPathResult dijkstra(const WeightedGraph &graph, int source) {
  using shortest_path_detail::Wide;
  int n = graph.size();
  assert(0 <= source && source < n);
  for (const auto &edge : graph.edges())
    assert(edge.weight >= 0);
  std::vector<std::optional<Wide>> distance(n);
  std::vector<int> predecessor(n, -1);
  std::vector<std::optional<std::size_t>> edge_id(n);
  using Item = std::pair<Wide, int>;
  std::priority_queue<Item, std::vector<Item>, std::greater<Item>> queue;
  distance[source] = 0;
  queue.emplace(0, source);
  while (!queue.empty()) {
    auto [current, vertex] = queue.top();
    queue.pop();
    if (current != distance[vertex])
      continue;
    for (std::size_t id : graph.outgoing(vertex)) {
      const auto &edge = graph.get_edge(id);
      Wide candidate = current + Wide(edge.weight);
      if (!distance[edge.to] || candidate < *distance[edge.to]) {
        distance[edge.to] = candidate;
        predecessor[edge.to] = vertex;
        edge_id[edge.to] = id;
        queue.emplace(candidate, edge.to);
      }
    }
  }
  return shortest_path_detail::finish(source, distance, std::move(predecessor),
                                      std::move(edge_id));
}
} // namespace cp
