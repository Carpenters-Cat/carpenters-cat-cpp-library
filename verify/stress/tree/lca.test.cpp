// competitive-verifier: STANDALONE
#include "tree_oracle.hpp"
#include <cp/tree/lca.hpp>
#include <random>
int main() {
  std::mt19937 rng(1011);
  for (int trial = 0; trial < 600; ++trial) {
    int n = 1 + rng() % 70;
    std::vector<std::pair<int, int>> edges;
    for (int v = 1; v < n; ++v)
      edges.emplace_back(v, rng() % v);
    std::shuffle(edges.begin(), edges.end(), rng);
    cp::Tree tree(n);
    for (auto [u, v] : edges)
      tree.add_edge(u, v);
    int root = rng() % n;
    TreeOracle oracle(tree, root);
    cp::Lca lca(tree, root);
    for (int u = 0; u < n; ++u) {
      int ancestor = u;
      for (int k = 0; k <= n; ++k) {
        assert(lca.jump(u, k) == ancestor);
        if (ancestor >= 0)
          ancestor = oracle.parent[ancestor];
      }
      for (int v = 0; v < n; ++v) {
        int a = oracle.lca(u, v);
        assert(lca.lca(u, v) == a);
        assert(lca.distance(u, v) ==
               oracle.depth[u] + oracle.depth[v] - 2 * oracle.depth[a]);
      }
    }
  }
}
