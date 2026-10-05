#pragma once
#include <algorithm>
#include <cp/tree/tree.hpp>
struct TreeOracle {
  std::vector<int> parent, depth, order;
  std::vector<std::optional<std::size_t>> edge;
  TreeOracle(const cp::Tree &tree, int root)
      : parent(tree.size(), -2), depth(tree.size()), edge(tree.size()) {
    parent[root] = -1;
    order.push_back(root);
    for (std::size_t i = 0; i < order.size(); ++i)
      for (auto arc : tree.neighbors(order[i]))
        if (parent[arc.to] == -2) {
          parent[arc.to] = order[i];
          depth[arc.to] = depth[order[i]] + 1;
          edge[arc.to] = arc.edge_id;
          order.push_back(arc.to);
        }
  }
  int lca(int u, int v) const {
    while (depth[u] > depth[v])
      u = parent[u];
    while (depth[v] > depth[u])
      v = parent[v];
    while (u != v) {
      u = parent[u];
      v = parent[v];
    }
    return u;
  }
  std::vector<int> path(int u, int v) const {
    int a = lca(u, v);
    std::vector<int> first, last;
    while (u != a) {
      first.push_back(u);
      u = parent[u];
    }
    first.push_back(a);
    while (v != a) {
      last.push_back(v);
      v = parent[v];
    }
    first.insert(first.end(), last.rbegin(), last.rend());
    return first;
  }
  std::vector<int> subtree(int v) const {
    std::vector<int> result;
    for (int u = 0; u < static_cast<int>(parent.size()); ++u) {
      for (int p = u; p >= 0; p = parent[p])
        if (p == v) {
          result.push_back(u);
          break;
        }
    }
    return result;
  }
};
