// competitive-verifier: STANDALONE
#include <cp/data_structure/potential_union_find.hpp>
#include <algorithm>
#include <cassert>
#include <optional>
#include <queue>
#include <random>
#include <utility>
#include <vector>
using Graph = std::vector<std::vector<std::pair<int, long long>>>;
std::vector<std::optional<long long>> explore(const Graph& graph, int start) {
    std::vector<std::optional<long long>> distance(graph.size());
    distance[start] = 0;
    std::queue<int> queue; queue.push(start);
    while (!queue.empty()) {
        const int v = queue.front(); queue.pop();
        for (const auto& [to, weight] : graph[v]) {
            if (!distance[to]) { distance[to] = *distance[v] + weight; queue.push(to); }
            else assert(*distance[to] == *distance[v] + weight);
        }
    }
    return distance;
}
int main() {
    using UF = cp::PotentialUnionFind<long long>;
    using Result = UF::MergeResult;
    std::mt19937 random(20261014);
    for (int trial = 0; trial < 300; ++trial) {
        const int n = 1 + random() % 40;
        UF uf(n); Graph graph(n);
        int components = n;
        for (int step = 0; step < 400; ++step) {
            const int a = random() % n, b = random() % n;
            const auto before = explore(graph, a);
            long long delta = static_cast<int>(random() % 101) - 50;
            if (before[b] && random() % 3 == 0) delta = *before[b];
            const Result expected = !before[b] ? Result::merged
                : *before[b] == delta ? Result::already_consistent : Result::contradiction;
            assert(uf.merge(a, b, delta) == expected);
            if (expected == Result::merged) {
                graph[a].push_back({b, delta}); graph[b].push_back({a, -delta});
                --components;
            }
            const auto after = explore(graph, a);
            assert(uf.components() == components);
            assert(uf.size(a) == std::count_if(after.begin(), after.end(), [](const auto& x) { return x.has_value(); }));
            for (int v = 0; v < n; ++v) {
                assert(uf.same(a, v) == after[v].has_value());
                assert(uf.difference(a, v) == after[v]);
            }
            const int root = uf.leader(a);
            assert(uf.potential(a) == *explore(graph, root)[a]);
        }
    }
}
