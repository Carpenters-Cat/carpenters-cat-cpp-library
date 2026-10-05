#pragma once
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <limits>
#include <optional>
#include <vector>
namespace cp {
// Iterative DFS reverse postorder; nullopt iff a directed cycle exists.
inline std::optional<std::vector<int>>
topological_sort(const std::vector<std::vector<int>> &graph) {
    assert(graph.size() <= static_cast<std::size_t>(std::numeric_limits<int>::max()));
    const int n = static_cast<int>(graph.size());
    for (const auto &neighbors : graph)
        for (int to : neighbors)
            assert(0 <= to && to < n);
    std::vector<unsigned char> state(n, 0);
    std::vector<std::size_t> cursor(n, 0);
    std::vector<int> stack, order;
    stack.reserve(n);
    order.reserve(n);
    for (int root = 0; root < n; ++root) {
        if (state[root])
            continue;
        state[root] = 1;
        stack.push_back(root);
        while (!stack.empty()) {
            const int v = stack.back();
            if (cursor[v] < graph[v].size()) {
                const int to = graph[v][cursor[v]++];
                if (state[to] == 1)
                    return std::nullopt;
                if (state[to] == 0) {
                    state[to] = 1;
                    stack.push_back(to);
                }
            } else {
                state[v] = 2;
                order.push_back(v);
                stack.pop_back();
            }
        }
    }
    std::reverse(order.begin(), order.end());
    return order;
}
} // namespace cp
