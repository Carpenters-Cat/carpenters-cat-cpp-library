// competitive-verifier: STANDALONE
#include "tree_oracle.hpp"
#include <cp/tree/euler_tour.hpp>
#include <random>
int main() {
  std::mt19937 rng(1012);
  for (int trial = 0; trial < 900; ++trial) {
    int n = 1 + rng() % 100;
    std::vector<std::pair<int, int>> edges;
    for (int v = 1; v < n; ++v)
      edges.emplace_back(v, rng() % v);
    std::shuffle(edges.begin(), edges.end(), rng);
    cp::Tree tree(n);
    for (auto [u, v] : edges)
      tree.add_edge(u, v);
    int root = rng() % n;
    TreeOracle oracle(tree, root);
    cp::EulerTour tour(tree, root);
    std::vector<int> expected;
    auto dfs = [&](auto self, int v, int p) -> void {
      expected.push_back(v);
      for (auto arc : tree.neighbors(v))
        if (arc.to != p)
          self(self, arc.to, v);
    };
    dfs(dfs, root, -1);
    assert(tour.order() == expected);
    for (int v = 0; v < n; ++v) {
      assert(tour.parent(v) == oracle.parent[v] &&
             tour.depth(v) == oracle.depth[v]);
      auto [l, r] = tour.subtree(v);
      std::vector<int> got;
      for (int i = l; i < r; ++i)
        got.push_back(tour.vertex(i));
      std::sort(got.begin(), got.end());
      assert(got == oracle.subtree(v) && tour.vertex(tour.in(v)) == v);
    }
  }
}
