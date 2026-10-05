// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/vertex_set_path_composite
#include <cp/data_structure/segment_tree.hpp>
#include <cp/math/modint.hpp>
#include <cp/tree/heavy_light_decomposition.hpp>
#include <iostream>
using Mint = cp::modint998244353;
struct Affine {
  Mint a, b;
};
Affine then(Affine first, Affine second) {
  return {second.a * first.a, second.a * first.b + second.b};
}
struct Aggregate {
  Affine forward, backward;
};
Aggregate op(Aggregate left, Aggregate right) {
  return {then(left.forward, right.forward),
          then(right.backward, left.backward)};
}
Aggregate identity() { return {{1, 0}, {1, 0}}; }
int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  int n, q;
  std::cin >> n >> q;
  std::vector<Affine> values(n);
  for (auto &f : values) {
    long long a, b;
    std::cin >> a >> b;
    f = {a, b};
  }
  cp::Tree tree(n);
  for (int i = 1; i < n; ++i) {
    int u, v;
    std::cin >> u >> v;
    tree.add_edge(u, v);
  }
  cp::HeavyLightDecomposition h(tree);
  std::vector<Aggregate> ordered(n);
  for (int v = 0; v < n; ++v)
    ordered[h.index(v)] = {values[v], values[v]};
  cp::SegmentTree<Aggregate, op, identity> segment(ordered);
  while (q--) {
    int type;
    std::cin >> type;
    if (type == 0) {
      int p;
      long long a, b;
      std::cin >> p >> a >> b;
      Affine f{a, b};
      segment.set(h.index(p), {f, f});
    } else {
      int u, v;
      long long x;
      std::cin >> u >> v >> x;
      Affine total{1, 0};
      for (auto s : h.path(u, v)) {
        auto part = segment.prod(s.left, s.right);
        total = then(total, s.reverse ? part.backward : part.forward);
      }
      std::cout << (total.a * x + total.b).val() << '\n';
    }
  }
}
