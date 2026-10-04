#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

namespace cp {

using Distance = std::int64_t;
enum class DistanceState { unreachable, finite, negative_infinity };

struct WeightedEdge {
  int from, to;
  Distance weight;
};

class WeightedGraph {
public:
  explicit WeightedGraph(int n = 0) {
    assert(n >= 0);
    adjacency_.resize(n);
  }
  int size() const { return static_cast<int>(adjacency_.size()); }
  std::size_t add_edge(int from, int to, Distance weight) {
    assert(0 <= from && from < size());
    assert(0 <= to && to < size());
    std::size_t id = edges_.size();
    edges_.push_back({from, to, weight});
    adjacency_[from].push_back(id);
    return id;
  }
  const std::vector<WeightedEdge> &edges() const { return edges_; }
  const WeightedEdge &get_edge(std::size_t id) const {
    assert(id < edges_.size());
    return edges_[id];
  }
  const std::vector<std::size_t> &outgoing(int vertex) const {
    assert(0 <= vertex && vertex < size());
    return adjacency_[vertex];
  }

private:
  std::vector<WeightedEdge> edges_;
  std::vector<std::vector<std::size_t>> adjacency_;
};

struct ShortestPath {
  std::vector<int> vertices;
  std::vector<std::size_t> edges;
};

struct ShortestPathResult {
  int source;
  std::vector<std::optional<Distance>> distance;
  std::vector<DistanceState> state;
  std::vector<int> predecessor_vertex;
  std::vector<std::optional<std::size_t>> predecessor_edge;

  bool has_negative_cycle() const {
    return std::find(state.begin(), state.end(),
                     DistanceState::negative_infinity) != state.end();
  }

  std::optional<ShortestPath> path_to(int target) const {
    assert(0 <= target && target < static_cast<int>(state.size()));
    if (state[target] != DistanceState::finite)
      return std::nullopt;
    ShortestPath path;
    for (int vertex = target;; vertex = predecessor_vertex[vertex]) {
      assert(path.vertices.size() < state.size());
      path.vertices.push_back(vertex);
      if (vertex == source)
        break;
      assert(predecessor_vertex[vertex] >= 0 && predecessor_edge[vertex]);
      path.edges.push_back(*predecessor_edge[vertex]);
    }
    std::reverse(path.vertices.begin(), path.vertices.end());
    std::reverse(path.edges.begin(), path.edges.end());
    return path;
  }
};

namespace shortest_path_detail {
// GCC/Clang wide integer; supported on the project's macOS/Linux targets.
using Wide = __int128_t;
inline Distance narrow(Wide value) {
  if (value < std::numeric_limits<Distance>::min() ||
      value > std::numeric_limits<Distance>::max())
    throw std::overflow_error("Finite shortest distance is outside int64_t");
  return static_cast<Distance>(value);
}
inline ShortestPathResult
finish(int source, const std::vector<std::optional<Wide>> &distance,
       std::vector<int> predecessor_vertex,
       std::vector<std::optional<std::size_t>> predecessor_edge,
       const std::vector<bool> &negative = {}) {
  int n = static_cast<int>(distance.size());
  ShortestPathResult result{source,
                            {},
                            {},
                            std::move(predecessor_vertex),
                            std::move(predecessor_edge)};
  result.distance.resize(n);
  result.state.assign(n, DistanceState::unreachable);
  for (int v = 0; v < n; ++v) {
    if (!negative.empty() && negative[v]) {
      result.state[v] = DistanceState::negative_infinity;
      result.predecessor_vertex[v] = -1;
      result.predecessor_edge[v].reset();
    } else if (distance[v]) {
      result.state[v] = DistanceState::finite;
      result.distance[v] = narrow(*distance[v]);
    }
  }
  return result;
}
} // namespace shortest_path_detail
} // namespace cp
