#include <cassert>
#include <cp/graph/lowlink.hpp>
#include <functional>
#include <numeric>
#include <random>
int components(const cp::UndirectedGraph &g, int removed_vertex = -1, int removed_edge = -1) {
    std::vector<std::vector<int>> adj(g.size());
    for (int id = 0; id < int(g.edges().size()); ++id) {
        auto e = g.get_edge(id);
        if (id == removed_edge || e.from == removed_vertex || e.to == removed_vertex)
            continue;
        adj[e.from].push_back(e.to);
        adj[e.to].push_back(e.from);
    }
    std::vector<bool> seen(g.size());
    int count = 0;
    for (int v = 0; v < g.size(); ++v)
        if (v != removed_vertex && !seen[v]) {
            ++count;
            seen[v] = true;
            std::vector<int> queue{v};
            for (std::size_t i = 0; i < queue.size(); ++i)
                for (int to : adj[queue[i]])
                    if (!seen[to]) {
                        seen[to] = true;
                        queue.push_back(to);
                    }
        }
    return count;
}
// Build the DFS tree recursively on tiny graphs, then enumerate every subtree
// vertex and its non-tree edges directly instead of propagating low values.
void check_low_values(const cp::UndirectedGraph &g, const cp::LowlinkResult &result) {
    const int n = g.size();
    std::vector<int> order(n, -1), exit(n, -1);
    std::vector<std::optional<std::size_t>> parent_edge(n);
    int timer = 0;
    std::function<void(int)> visit = [&](int v) {
        order[v] = timer++;
        for (auto id : g.incident(v)) {
            if (parent_edge[v] && id == *parent_edge[v])
                continue;
            auto e = g.get_edge(id);
            int to = e.from == v ? e.to : e.from;
            if (order[to] < 0) {
                parent_edge[to] = id;
                visit(to);
            }
        }
        exit[v] = timer;
    };
    for (int v = 0; v < n; ++v)
        if (order[v] < 0)
            visit(v);
    assert(order == result.order);
    for (int v = 0; v < n; ++v) {
        int low = order[v];
        for (int u = 0; u < n; ++u)
            if (order[v] <= order[u] && order[u] < exit[v])
                for (auto id : g.incident(u)) {
                    if (parent_edge[u] && id == *parent_edge[u])
                        continue;
                    auto e = g.get_edge(id);
                    int to = e.from == u ? e.to : e.from;
                    low = std::min(low, order[to]);
                }
        assert(low == result.low[v]);
    }
}
int main() {
    std::mt19937 rng(20231025);
    for (int trial = 0; trial < 5000; ++trial) {
        int n = rng() % 9;
        cp::UndirectedGraph g(n);
        if (n)
            for (unsigned i = 0, m = rng() % 16; i < m; ++i)
                g.add_edge(rng() % n, rng() % n);
        auto r = cp::lowlink(g);
        check_low_values(g, r);
        int base = components(g);
        assert(base == r.components);
        std::vector<std::size_t> bridges;
        std::vector<int> arts;
        for (int id = 0; id < int(g.edges().size()); ++id)
            if (components(g, -1, id) > base)
                bridges.push_back(id);
        for (int v = 0; v < n; ++v)
            if (components(g, v) > base)
                arts.push_back(v);
        assert(bridges == r.bridges && arts == r.articulation_points);
        auto order = r.order;
        std::sort(order.begin(), order.end());
        for (int v = 0; v < n; ++v)
            assert(order[v] == v && 0 <= r.low[v] && r.low[v] <= r.order[v]);
    }
}
