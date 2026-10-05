#pragma once

#include <cassert>
#include <utility>
#include <vector>

namespace cp {

// Iterative Kosaraju: component IDs increase along edges between components.
class SccGraph {
public:
  explicit SccGraph(int n = 0) : n_(n) {
    assert(n >= 0);
    graph_.resize(n);
    reverse_.resize(n);
  }

  void add_edge(int from, int to) {
    assert(0 <= from && from < n_);
    assert(0 <= to && to < n_);
    graph_[from].push_back(to);
    reverse_[to].push_back(from);
  }

  std::pair<int, std::vector<int>> scc_ids() const {
    std::vector<bool> seen(n_);
    std::vector<int> order;
    order.reserve(n_);
    std::vector<std::pair<int, std::size_t>> stack;
    for (int root = 0; root < n_; ++root) {
      if (seen[root])
        continue;
      seen[root] = true;
      stack.emplace_back(root, 0);
      while (!stack.empty()) {
        auto &[vertex, next] = stack.back();
        if (next == graph_[vertex].size()) {
          order.push_back(vertex);
          stack.pop_back();
        } else {
          int to = graph_[vertex][next++];
          if (!seen[to]) {
            seen[to] = true;
            stack.emplace_back(to, 0);
          }
        }
      }
    }
    std::vector<int> ids(n_, -1), pending;
    int count = 0;
    for (auto it = order.rbegin(); it != order.rend(); ++it) {
      if (ids[*it] != -1)
        continue;
      ids[*it] = count;
      pending.push_back(*it);
      while (!pending.empty()) {
        int vertex = pending.back();
        pending.pop_back();
        for (int to : reverse_[vertex]) {
          if (ids[to] == -1) {
            ids[to] = count;
            pending.push_back(to);
          }
        }
      }
      ++count;
    }
    return {count, std::move(ids)};
  }

  std::vector<std::vector<int>> scc() const {
    auto [count, ids] = scc_ids();
    std::vector<std::vector<int>> groups(count);
    for (int i = 0; i < n_; ++i)
      groups[ids[i]].push_back(i);
    return groups;
  }

private:
  int n_;
  std::vector<std::vector<int>> graph_, reverse_;
};

} // namespace cp
