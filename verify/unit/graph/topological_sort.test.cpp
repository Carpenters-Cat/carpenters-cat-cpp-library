#include <cassert>
#include <cp/graph/topological_sort.hpp>
int main() {
    assert(cp::topological_sort({})->empty());
    assert((cp::topological_sort({{1}, {}, {}}) == std::vector<int>{2, 0, 1}));
    assert((cp::topological_sort({{1, 1}, {2}, {}, {}}) == std::vector<int>{3, 0, 1, 2}));
    assert(!cp::topological_sort({{0}}));
    assert(!cp::topological_sort({{1}, {0}, {}}));
    std::vector<std::vector<int>> path(200000);
    for (int v = 0; v + 1 < int(path.size()); ++v)
        path[v].push_back(v + 1);
    auto order = cp::topological_sort(path);
    assert(order && order->size() == path.size());
    for (int v = 0; v < int(path.size()); ++v)
        assert((*order)[v] == v);
}
