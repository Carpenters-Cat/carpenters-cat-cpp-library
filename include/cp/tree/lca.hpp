#pragma once
#include <algorithm>
#include <bit>
#include <cp/tree/tree.hpp>
#include <cstdint>

namespace cp {
class Lca {
public:
  explicit Lca(const Tree &tree, int root = 0) : rooted_(tree, root) {
    int n = tree.size();
    if (!n)
      return;
    up_.assign(std::bit_width(static_cast<unsigned>(n)),
               std::vector<int>(n, -1));
    up_[0] = rooted_.parent;
    for (std::size_t k = 1; k < up_.size(); ++k)
      for (int v = 0; v < n; ++v)
        if (up_[k - 1][v] >= 0)
          up_[k][v] = up_[k - 1][up_[k - 1][v]];
  }
  int size() const { return static_cast<int>(rooted_.parent.size()); }
  int root() const { return rooted_.root; }
  int parent(int v) const {
    check(v);
    return rooted_.parent[v];
  }
  int depth(int v) const {
    check(v);
    return rooted_.depth[v];
  }
  std::optional<std::size_t> parent_edge(int v) const {
    check(v);
    return rooted_.parent_edge[v];
  }
  int jump(int v, std::uint64_t steps) const {
    check(v);
    if (steps > static_cast<std::uint64_t>(depth(v)))
      return -1;
    for (std::size_t k = 0; steps; ++k, steps >>= 1)
      if (steps & 1U)
        v = up_[k][v];
    return v;
  }
  int lca(int u, int v) const {
    check(u);
    check(v);
    if (depth(u) < depth(v))
      std::swap(u, v);
    u = jump(u, depth(u) - depth(v));
    if (u == v)
      return u;
    for (std::size_t k = up_.size(); k--;)
      if (up_[k][u] != up_[k][v]) {
        u = up_[k][u];
        v = up_[k][v];
      }
    return parent(u);
  }
  std::int64_t distance(int u, int v) const {
    return std::int64_t(depth(u)) + depth(v) - 2LL * depth(lca(u, v));
  }

private:
  void check(int v) const { assert(0 <= v && v < size()); }
  RootedTree rooted_;
  std::vector<std::vector<int>> up_;
};
} // namespace cp
