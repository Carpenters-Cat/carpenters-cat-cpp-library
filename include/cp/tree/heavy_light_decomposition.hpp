#pragma once
#include <algorithm>
#include <cp/tree/tree.hpp>
#include <cstdint>

namespace cp {
struct PathSegment {
  int left, right;
  bool reverse;
};

class HeavyLightDecomposition {
public:
  explicit HeavyLightDecomposition(const Tree &tree, int root = 0)
      : rooted_(tree, root), head_(tree.size()), in_(tree.size()),
        order_(tree.size()), edge_child_(tree.edges().size()) {
    int n = size();
    if (!n)
      return;
    std::vector<int> heavy(n, -1);
    for (int v = 0; v < n; ++v)
      for (auto arc : tree.neighbors(v))
        if (rooted_.parent[arc.to] == v) {
          if (heavy[v] < 0 ||
              rooted_.subtree_size[arc.to] > rooted_.subtree_size[heavy[v]])
            heavy[v] = arc.to;
          edge_child_[arc.edge_id] = arc.to;
        }
    std::vector<std::pair<int, int>> stack{{root, root}};
    int timer = 0;
    while (!stack.empty()) {
      auto [v, head] = stack.back();
      stack.pop_back();
      head_[v] = head;
      in_[v] = timer;
      order_[timer++] = v;
      const auto &neighbors = tree.neighbors(v);
      for (auto it = neighbors.rbegin(); it != neighbors.rend(); ++it)
        if (rooted_.parent[it->to] == v && it->to != heavy[v])
          stack.emplace_back(it->to, it->to);
      if (heavy[v] >= 0)
        stack.emplace_back(heavy[v], head);
    }
  }
  int size() const { return static_cast<int>(in_.size()); }
  int root() const { return rooted_.root; }
  int parent(int v) const {
    check(v);
    return rooted_.parent[v];
  }
  int depth(int v) const {
    check(v);
    return rooted_.depth[v];
  }
  int index(int v) const {
    check(v);
    return in_[v];
  }
  int vertex(int index) const {
    assert(0 <= index && index < size());
    return order_[index];
  }
  int head(int v) const {
    check(v);
    return head_[v];
  }
  int edge_index(std::size_t id) const {
    assert(id < edge_child_.size());
    return index(edge_child_[id]);
  }
  std::pair<int, int> subtree(int v, bool edge_values = false) const {
    check(v);
    return {in_[v] + int(edge_values), in_[v] + rooted_.subtree_size[v]};
  }
  int lca(int u, int v) const {
    check(u);
    check(v);
    while (head_[u] != head_[v]) {
      if (depth(head_[u]) < depth(head_[v]))
        std::swap(u, v);
      u = parent(head_[u]);
    }
    return depth(u) < depth(v) ? u : v;
  }
  std::int64_t distance(int u, int v) const {
    return std::int64_t(depth(u)) + depth(v) - 2LL * depth(lca(u, v));
  }
  std::vector<PathSegment> path(int u, int v, bool edge_values = false) const {
    check(u);
    check(v);
    std::vector<PathSegment> first, last;
    while (head_[u] != head_[v]) {
      if (depth(head_[u]) >= depth(head_[v])) {
        first.push_back({in_[head_[u]], in_[u] + 1, true});
        u = parent(head_[u]);
      } else {
        last.push_back({in_[head_[v]], in_[v] + 1, false});
        v = parent(head_[v]);
      }
    }
    if (depth(u) >= depth(v)) {
      int left = in_[v] + int(edge_values);
      if (left < in_[u] + 1)
        first.push_back({left, in_[u] + 1, true});
    } else {
      int left = in_[u] + int(edge_values);
      if (left < in_[v] + 1)
        last.push_back({left, in_[v] + 1, false});
    }
    first.insert(first.end(), last.rbegin(), last.rend());
    return first;
  }

private:
  void check(int v) const { assert(0 <= v && v < size()); }
  RootedTree rooted_;
  std::vector<int> head_, in_, order_, edge_child_;
};
} // namespace cp
