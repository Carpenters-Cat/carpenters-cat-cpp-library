#pragma once
#include <cp/tree/tree.hpp>
namespace cp {
class EulerTour {
public:
  explicit EulerTour(const Tree &tree, int root = 0)
      : rooted_(tree, root), in_(tree.size()) {
    for (int i = 0; i < size(); ++i)
      in_[rooted_.order[i]] = i;
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
  int in(int v) const {
    check(v);
    return in_[v];
  }
  int out(int v) const {
    check(v);
    return in_[v] + rooted_.subtree_size[v];
  }
  std::pair<int, int> subtree(int v) const { return {in(v), out(v)}; }
  int vertex(int index) const {
    assert(0 <= index && index < size());
    return rooted_.order[index];
  }
  const std::vector<int> &order() const { return rooted_.order; }

private:
  void check(int v) const { assert(0 <= v && v < size()); }
  RootedTree rooted_;
  std::vector<int> in_;
};
} // namespace cp
