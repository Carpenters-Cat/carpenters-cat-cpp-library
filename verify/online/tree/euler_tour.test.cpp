// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/vertex_add_subtree_sum
#include <cp/data_structure/fenwick_tree.hpp>
#include <cp/tree/euler_tour.hpp>
#include <iostream>
int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  int n, q;
  std::cin >> n >> q;
  std::vector<long long> values(n);
  for (auto &x : values)
    std::cin >> x;
  cp::Tree tree(n);
  for (int v = 1; v < n; ++v) {
    int p;
    std::cin >> p;
    tree.add_edge(p, v);
  }
  cp::EulerTour tour(tree);
  std::vector<long long> ordered(n);
  for (int v = 0; v < n; ++v)
    ordered[tour.in(v)] = values[v];
  cp::FenwickTree<long long> sums(ordered);
  while (q--) {
    int type, v;
    std::cin >> type >> v;
    if (type == 0) {
      long long x;
      std::cin >> x;
      sums.add(tour.in(v), x);
    } else {
      auto [l, r] = tour.subtree(v);
      std::cout << sums.sum(l, r) << '\n';
    }
  }
}
