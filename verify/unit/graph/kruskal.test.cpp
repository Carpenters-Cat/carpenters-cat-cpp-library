#include <cassert>
#include <cp/graph/kruskal.hpp>
#include <limits>
int main() {
    assert(cp::kruskal(cp::WeightedGraph()).connected());
    cp::WeightedGraph g(5);
    g.add_edge(0, 0, -100);
    g.add_edge(0, 1, 4);
    auto a = g.add_edge(0, 1, -2);
    auto b = g.add_edge(1, 2, 0);
    g.add_edge(0, 2, 0);
    auto c = g.add_edge(3, 4, 3);
    auto r = cp::kruskal(g);
    assert(r.cost == 1 && r.components == 2 && !r.connected());
    assert((r.edge_ids == std::vector<std::size_t>{a, b, c}));
    assert(cp::kruskal(cp::WeightedGraph(1)).connected());
    const auto lo = std::numeric_limits<cp::Distance>::min(),
               hi = std::numeric_limits<cp::Distance>::max();
    cp::WeightedGraph wide(4);
    wide.add_edge(0, 1, lo);
    wide.add_edge(1, 2, -1);
    wide.add_edge(2, 3, hi);
    assert(cp::kruskal(wide).cost == -2);
    for (auto w : {lo, hi}) {
        cp::WeightedGraph overflow(3);
        overflow.add_edge(0, 1, w);
        overflow.add_edge(1, 2, w);
        bool caught = false;
        try {
            cp::kruskal(overflow);
        } catch (const std::overflow_error &) {
            caught = true;
        }
        assert(caught);
    }
}
