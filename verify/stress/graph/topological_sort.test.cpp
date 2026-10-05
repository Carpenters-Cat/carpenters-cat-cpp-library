#include <algorithm>
#include <cassert>
#include <cp/graph/topological_sort.hpp>
#include <numeric>
#include <random>
bool valid(const std::vector<std::vector<int>> &g, const std::vector<int> &order) {
    std::vector<int> pos(g.size(), -1);
    for (int i = 0; i < int(order.size()); ++i) {
        int v = order[i];
        if (v < 0 || v >= int(g.size()) || pos[v] >= 0)
            return false;
        pos[v] = i;
    }
    if (order.size() != g.size())
        return false;
    for (int v = 0; v < int(g.size()); ++v)
        for (int to : g[v])
            if (pos[v] >= pos[to])
                return false;
    return true;
}
int main() {
    std::mt19937 rng(20231024);
    for (int trial = 0; trial < 1500; ++trial) {
        int n = rng() % 8;
        std::vector<std::vector<int>> g(n);
        if (n)
            for (unsigned i = 0, m = rng() % 20; i < m; ++i)
                g[rng() % n].push_back(rng() % n);
        std::vector<int> perm(n);
        std::iota(perm.begin(), perm.end(), 0);
        bool exists = false;
        do {
            if (valid(g, perm)) {
                exists = true;
                break;
            }
        } while (std::next_permutation(perm.begin(), perm.end()));
        auto r = cp::topological_sort(g);
        assert(bool(r) == exists);
        if (r)
            assert(valid(g, *r));
    }
}
