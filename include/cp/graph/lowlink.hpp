#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <optional>
#include <vector>
namespace cp {
struct UndirectedEdge {
    int from, to;
};
class UndirectedGraph {
  public:
    explicit UndirectedGraph(int n = 0) {
        assert(n >= 0);
        adjacency_.resize(static_cast<std::size_t>(n));
    }
    int size() const { return static_cast<int>(adjacency_.size()); }
    std::size_t add_edge(int from, int to) {
        assert(0 <= from && from < size() && 0 <= to && to < size());
        const auto id = edges_.size();
        edges_.push_back({from, to});
        adjacency_[from].push_back(id);
        adjacency_[to].push_back(id);
        return id;
    }
    const std::vector<UndirectedEdge> &edges() const { return edges_; }
    const UndirectedEdge &get_edge(std::size_t id) const {
        assert(id < edges_.size());
        return edges_[id];
    }
    const std::vector<std::size_t> &incident(int vertex) const {
        assert(0 <= vertex && vertex < size());
        return adjacency_[vertex];
    }

  private:
    std::vector<UndirectedEdge> edges_;
    std::vector<std::vector<std::size_t>> adjacency_;
};
struct LowlinkResult {
    std::vector<int> order, low;
    std::vector<std::size_t> bridges;
    std::vector<int> articulation_points;
    int components = 0;
};
inline LowlinkResult lowlink(const UndirectedGraph &graph) {
    const int n = graph.size();
    LowlinkResult result;
    result.order.assign(n, -1);
    result.low.assign(n, -1);
    std::vector<int> parent(n, -1), children(n, 0), stack;
    std::vector<std::optional<std::size_t>> parent_edge(n);
    std::vector<std::size_t> cursor(n, 0);
    std::vector<bool> articulation(n, false), bridge(graph.edges().size(), false);
    stack.reserve(n);
    int timer = 0;
    for (int root = 0; root < n; ++root) {
        if (result.order[root] >= 0)
            continue;
        ++result.components;
        result.order[root] = result.low[root] = timer++;
        stack.push_back(root);
        while (!stack.empty()) {
            const int v = stack.back();
            if (cursor[v] < graph.incident(v).size()) {
                const auto id = graph.incident(v)[cursor[v]++];
                if (parent_edge[v] && id == *parent_edge[v])
                    continue;
                const auto &edge = graph.get_edge(id);
                const int to = edge.from == v ? edge.to : edge.from;
                if (result.order[to] < 0) {
                    parent[to] = v;
                    parent_edge[to] = id;
                    ++children[v];
                    result.order[to] = result.low[to] = timer++;
                    stack.push_back(to);
                } else {
                    result.low[v] = std::min(result.low[v], result.order[to]);
                }
            } else {
                stack.pop_back();
                if (parent[v] >= 0) {
                    const int p = parent[v];
                    result.low[p] = std::min(result.low[p], result.low[v]);
                    if (result.low[v] > result.order[p])
                        bridge[*parent_edge[v]] = true;
                    if (parent[p] >= 0 && result.low[v] >= result.order[p])
                        articulation[p] = true;
                } else if (children[v] > 1) {
                    articulation[v] = true;
                }
            }
        }
    }
    for (std::size_t id = 0; id < bridge.size(); ++id)
        if (bridge[id])
            result.bridges.push_back(id);
    for (int v = 0; v < n; ++v)
        if (articulation[v])
            result.articulation_points.push_back(v);
    return result;
}
} // namespace cp
