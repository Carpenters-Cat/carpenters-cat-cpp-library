#pragma once

#include <cp/graph/weighted_graph.hpp>

namespace cp {
using DistanceMatrix = std::vector<std::vector<std::optional<Distance>>>;

struct AllPairsShortestPathResult {
  DistanceMatrix distance;
  std::vector<std::vector<DistanceState>> state;
  std::vector<std::vector<int>> predecessor_vertex;
  std::vector<std::vector<std::optional<std::size_t>>> predecessor_edge;
  bool has_negative_cycle() const {
    for (std::size_t v = 0; v < state.size(); ++v)
      if (state[v][v] == DistanceState::negative_infinity)
        return true;
    return false;
  }
  std::optional<ShortestPath> path(int source, int target) const {
    int n = static_cast<int>(state.size());
    assert(0 <= source && source < n && 0 <= target && target < n);
    if (state[source][target] != DistanceState::finite)
      return std::nullopt;
    ShortestPath result;
    for (int v = target;; v = predecessor_vertex[source][v]) {
      assert(result.vertices.size() < state.size());
      result.vertices.push_back(v);
      if (v == source)
        break;
      assert(predecessor_vertex[source][v] >= 0 && predecessor_edge[source][v]);
      result.edges.push_back(*predecessor_edge[source][v]);
    }
    std::reverse(result.vertices.begin(), result.vertices.end());
    std::reverse(result.edges.begin(), result.edges.end());
    return result;
  }
};

namespace shortest_path_detail {
inline AllPairsShortestPathResult floyd_impl(
    const DistanceMatrix &matrix,
    const std::vector<std::vector<std::optional<std::size_t>>> &initial_edge) {
  assert(matrix.size() <=
         static_cast<std::size_t>(std::numeric_limits<int>::max()));
  int n = static_cast<int>(matrix.size());
  std::vector<std::vector<std::optional<Wide>>> distance(
      n, std::vector<std::optional<Wide>>(n));
  std::vector<std::vector<bool>> reach(n, std::vector<bool>(n));
  AllPairsShortestPathResult result;
  result.distance = matrix;
  result.state.assign(
      n, std::vector<DistanceState>(n, DistanceState::unreachable));
  result.predecessor_vertex.assign(n, std::vector<int>(n, -1));
  result.predecessor_edge = initial_edge;
  for (int i = 0; i < n; ++i) {
    assert(matrix[i].size() == matrix.size());
    for (int j = 0; j < n; ++j)
      if (matrix[i][j]) {
        distance[i][j] = Wide(*matrix[i][j]);
        reach[i][j] = true;
        result.predecessor_vertex[i][j] = i;
      }
    reach[i][i] = true;
    if (!distance[i][i] || *distance[i][i] >= 0) {
      distance[i][i] = 0;
      result.predecessor_vertex[i][i] = -1;
      result.predecessor_edge[i][i].reset();
    }
  }
  for (int k = 0; k < n; ++k)
    for (int i = 0; i < n; ++i)
      if (reach[i][k])
        for (int j = 0; j < n; ++j)
          if (reach[k][j])
            reach[i][j] = true;
  std::vector<int> negative_pivots;
  for (int k = 0; k < n; ++k) {
    // Once a negative cycle is exposed, do not repeatedly relax through it.
    // Any pair connected through this pivot has undefined shortest distance.
    if (*distance[k][k] < 0) {
      negative_pivots.push_back(k);
      continue;
    }
    for (int i = 0; i < n; ++i)
      if (distance[i][k]) {
        for (int j = 0; j < n; ++j)
          if (distance[k][j]) {
            Wide candidate = *distance[i][k] + *distance[k][j];
            if (!distance[i][j] || candidate < *distance[i][j]) {
              distance[i][j] = candidate;
              result.predecessor_vertex[i][j] = result.predecessor_vertex[k][j];
              result.predecessor_edge[i][j] = result.predecessor_edge[k][j];
            }
          }
      }
  }
  std::vector<std::vector<bool>> negative(n, std::vector<bool>(n));
  for (int k : negative_pivots)
    for (int i = 0; i < n; ++i)
      if (reach[i][k])
        for (int j = 0; j < n; ++j)
          if (reach[k][j])
            negative[i][j] = true;
  for (int i = 0; i < n; ++i)
    for (int j = 0; j < n; ++j) {
      result.distance[i][j].reset();
      if (negative[i][j]) {
        result.state[i][j] = DistanceState::negative_infinity;
        result.predecessor_vertex[i][j] = -1;
        result.predecessor_edge[i][j].reset();
      } else if (distance[i][j]) {
        result.state[i][j] = DistanceState::finite;
        result.distance[i][j] = narrow(*distance[i][j]);
      }
    }
  return result;
}
} // namespace shortest_path_detail

inline AllPairsShortestPathResult floyd_warshall(const DistanceMatrix &matrix) {
  std::size_t n = matrix.size();
  assert(n <= static_cast<std::size_t>(std::numeric_limits<int>::max()));
  std::vector<std::vector<std::optional<std::size_t>>> ids(
      n, std::vector<std::optional<std::size_t>>(n));
  for (std::size_t i = 0; i < n; ++i) {
    assert(matrix[i].size() == n);
    for (std::size_t j = 0; j < n; ++j)
      if (matrix[i][j])
        ids[i][j] = i * n + j;
  }
  return shortest_path_detail::floyd_impl(matrix, ids);
}
inline AllPairsShortestPathResult floyd_warshall(const WeightedGraph &graph) {
  int n = graph.size();
  DistanceMatrix matrix(n, std::vector<std::optional<Distance>>(n));
  std::vector<std::vector<std::optional<std::size_t>>> ids(
      n, std::vector<std::optional<std::size_t>>(n));
  for (std::size_t id = 0; id < graph.edges().size(); ++id) {
    const auto &edge = graph.get_edge(id);
    if (!matrix[edge.from][edge.to] ||
        edge.weight < *matrix[edge.from][edge.to]) {
      matrix[edge.from][edge.to] = edge.weight;
      ids[edge.from][edge.to] = id;
    }
  }
  return shortest_path_detail::floyd_impl(matrix, ids);
}
} // namespace cp
