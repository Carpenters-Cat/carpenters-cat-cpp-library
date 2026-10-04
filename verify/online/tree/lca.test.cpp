// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/lca
#include <cp/tree/lca.hpp>
#include <iostream>
int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  int n, q;
  std::cin >> n >> q;
  cp::Tree tree(n);
  for (int v = 1; v < n; ++v) {
    int p;
    std::cin >> p;
    tree.add_edge(p, v);
  }
  cp::Lca lca(tree);
  while (q--) {
    int u, v;
    std::cin >> u >> v;
    std::cout << lca.lca(u, v) << '\n';
  }
}
