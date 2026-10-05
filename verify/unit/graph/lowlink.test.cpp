#include <cassert>
#include <cp/graph/lowlink.hpp>
int main() {
    assert(cp::lowlink(cp::UndirectedGraph()).components == 0);
    cp::UndirectedGraph g(7);
    g.add_edge(0, 1);
    g.add_edge(0, 1);
    g.add_edge(1, 1);
    auto a = g.add_edge(1, 2);
    g.add_edge(3, 4);
    g.add_edge(4, 5);
    g.add_edge(5, 3);
    auto r = cp::lowlink(g);
    assert(r.components == 3 && (r.bridges == std::vector<std::size_t>{a}));
    assert((r.articulation_points == std::vector<int>{1}));
    assert((r.order == std::vector<int>{0, 1, 2, 3, 4, 5, 6}));
    assert((r.low == std::vector<int>{0, 0, 2, 3, 3, 3, 6}));
    cp::UndirectedGraph root(3);
    root.add_edge(0, 1);
    root.add_edge(0, 2);
    assert((cp::lowlink(root).articulation_points == std::vector<int>{0}));
    constexpr int n = 200000;
    cp::UndirectedGraph path(n);
    for (int v = 0; v + 1 < n; ++v)
        path.add_edge(v, v + 1);
    auto p = cp::lowlink(path);
    assert(p.components == 1 && p.bridges.size() == n - 1 && p.articulation_points.size() == n - 2);
    for (int v = 0; v < n; ++v)
        assert(p.order[v] == v && p.low[v] == v);
    for (int v = 1; v + 1 < n; ++v)
        assert(p.articulation_points[v - 1] == v);
}
