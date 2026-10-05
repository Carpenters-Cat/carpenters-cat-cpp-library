#pragma once

#include <cassert>
#include <cstddef>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

namespace cp {

struct TreeArc {
  int to;
  std::size_t edge_id;
};

class Tree {
public:
  explicit Tree(int n = 0) {
    assert(n >= 0);
    adjacency_.resize(n);
  }
  int size() const { return static_cast<int>(adjacency_.size()); }
  std::size_t add_edge(int u, int v) {
    assert(0 <= u && u < size() && 0 <= v && v < size());
    std::size_t id = edges_.size();
    edges_.emplace_back(u, v);
    adjacency_[u].push_back({v, id});
    adjacency_[v].push_back({u, id});
    return id;
  }
  const std::vector<TreeArc> &neighbors(int vertex) const {
    assert(0 <= vertex && vertex < size());
    return adjacency_[vertex];
  }
  const std::vector<std::pair<int, int>> &edges() const { return edges_; }

private:
  std::vector<std::vector<TreeArc>> adjacency_;
  std::vector<std::pair<int, int>> edges_;
};

// Validates and roots a connected, acyclic tree without recursion.
struct RootedTree {
  int root = -1;
  std::vector<int> parent, depth, order, subtree_size;
  std::vector<std::optional<std::size_t>> parent_edge;

  explicit RootedTree(const Tree &tree, int root_vertex = 0) {
    int n = tree.size();
    if (n == 0) {
      if (root_vertex != 0)
        throw std::invalid_argument("Empty tree uses root argument 0");
      return;
    }
    if (root_vertex < 0 || root_vertex >= n ||
        tree.edges().size() != static_cast<std::size_t>(n - 1))
      throw std::invalid_argument("Expected a valid root and n-1 tree edges");
    root = root_vertex;
    parent.assign(n, -2);
    depth.assign(n, 0);
    subtree_size.assign(n, 1);
    parent_edge.resize(n);
    parent[root] = -1;
    order.reserve(n);
    order.push_back(root);
    std::vector<std::pair<int, std::size_t>> stack{{root, 0}};
    while (!stack.empty()) {
      auto &[v, next] = stack.back();
      if (next == tree.neighbors(v).size()) {
        if (parent[v] >= 0)
          subtree_size[parent[v]] += subtree_size[v];
        stack.pop_back();
        continue;
      }
      auto arc = tree.neighbors(v)[next++];
      if (parent_edge[v] == arc.edge_id)
        continue;
      if (parent[arc.to] != -2)
        throw std::invalid_argument("Input contains a cycle");
      parent[arc.to] = v;
      parent_edge[arc.to] = arc.edge_id;
      depth[arc.to] = depth[v] + 1;
      order.push_back(arc.to);
      stack.emplace_back(arc.to, 0);
    }
    if (order.size() != static_cast<std::size_t>(n))
      throw std::invalid_argument("Input is disconnected");
  }
};
} // namespace cp
