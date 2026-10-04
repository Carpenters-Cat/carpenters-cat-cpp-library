#include <bit>
#include <cassert>
#include <cp/graph/kruskal.hpp>
#include <optional>
#include <random>
// Independent label propagation over each chosen edge subset.
std::vector<int> labels(const cp::WeightedGraph &g, unsigned mask) {
    std::vector<int> label(g.size());
    std::iota(label.begin(), label.end(), 0);
    for (int pass = 0; pass < g.size(); ++pass)
        for (std::size_t id = 0; id < g.edges().size(); ++id)
            if (mask & (1U << id)) {
                auto e = g.get_edge(id);
                int x = std::min(label[e.from], label[e.to]);
                label[e.from] = label[e.to] = x;
            }
    return label;
}
int main() {
    std::mt19937 rng(20231023);
    for (int trial = 0; trial < 2000; ++trial) {
        int n = 1 + rng() % 6, m = rng() % 11;
        cp::WeightedGraph g(n);
        for (int i = 0; i < m; ++i)
            g.add_edge(rng() % n, rng() % n, int(rng() % 13) - 6);
        auto all = labels(g, (1U << m) - 1);
        int components = 0;
        for (int v = 0; v < n; ++v)
            if (all[v] == v)
                ++components;
        std::optional<long long> best;
        for (unsigned mask = 0; mask < (1U << m); ++mask) {
            if (std::popcount(mask) != n - components || labels(g, mask) != all)
                continue;
            long long cost = 0;
            for (int id = 0; id < m; ++id)
                if (mask & (1U << id))
                    cost += g.get_edge(id).weight;
            if (!best || cost < *best)
                best = cost;
        }
        auto r = cp::kruskal(g);
        assert(best && r.cost == *best && r.components == components);
        unsigned chosen = 0;
        long long cost = 0;
        for (auto id : r.edge_ids) {
            assert(id < g.edges().size() && !(chosen & (1U << id)));
            chosen |= 1U << id;
            cost += g.get_edge(id).weight;
        }
        assert(std::popcount(chosen) == n - components && labels(g, chosen) == all &&
               cost == r.cost);
    }
}
