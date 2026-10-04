#pragma once
#include <algorithm>
#include <cp/data_structure/union_find.hpp>
#include <cp/graph/weighted_graph.hpp>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <vector>
namespace cp {
struct MinimumSpanningForest {
    Distance cost = 0;
    std::vector<std::size_t> edge_ids;
    int components = 0;
    bool connected() const { return components <= 1; }
};
// Each WeightedGraph record is one undirected candidate; add each edge once.
inline MinimumSpanningForest kruskal(const WeightedGraph &graph) {
    std::vector<std::size_t> ids(graph.edges().size());
    std::iota(ids.begin(), ids.end(), std::size_t{0});
    std::sort(ids.begin(), ids.end(), [&](auto a, auto b) {
        if (graph.get_edge(a).weight != graph.get_edge(b).weight)
            return graph.get_edge(a).weight < graph.get_edge(b).weight;
        return a < b;
    });
    UnionFind uf(graph.size());
    MinimumSpanningForest result;
    __int128_t total = 0;
    for (auto id : ids) {
        const auto &edge = graph.get_edge(id);
        if (uf.merge(edge.from, edge.to)) {
            result.edge_ids.push_back(id);
            total += static_cast<__int128_t>(edge.weight);
        }
    }
    if (total < std::numeric_limits<Distance>::min() ||
        total > std::numeric_limits<Distance>::max())
        throw std::overflow_error("Minimum spanning forest cost is outside int64_t");
    result.cost = static_cast<Distance>(total);
    result.components = uf.components();
    return result;
}
} // namespace cp
