// competitive-verifier: STANDALONE
#include <cassert>
#include <cp/tree/heavy_light_decomposition.hpp>
std::vector<int> expand(const cp::HeavyLightDecomposition &h, int u, int v,
                        bool edge = false) {
  std::vector<int> result;
  for (auto s : h.path(u, v, edge)) {
    if (s.reverse)
      for (int i = s.right; i-- > s.left;)
        result.push_back(h.vertex(i));
    else
      for (int i = s.left; i < s.right; ++i)
        result.push_back(h.vertex(i));
  }
  return result;
}
int main() {
  cp::HeavyLightDecomposition empty(cp::Tree{});
  assert(empty.size() == 0 && empty.root() == -1);
  cp::Tree one(1);
  cp::HeavyLightDecomposition single(one);
  assert(single.path(0, 0, true).empty() &&
         expand(single, 0, 0) == std::vector<int>{0});
  assert(single.subtree(0, true) == std::pair(1, 1));
  cp::Tree t(6);
  t.add_edge(0, 1);
  t.add_edge(0, 2);
  t.add_edge(1, 3);
  t.add_edge(1, 4);
  t.add_edge(2, 5);
  cp::HeavyLightDecomposition h(t);
  assert((expand(h, 3, 5) == std::vector<int>{3, 1, 0, 2, 5}));
  assert((expand(h, 3, 5, true) == std::vector<int>{3, 1, 2, 5}));
  assert((expand(h, 0, 5, true) == std::vector<int>{2, 5}));
  assert((expand(h, 5, 0, true) == std::vector<int>{5, 2}));
  assert(h.edge_index(4) == h.index(5) && h.lca(3, 5) == 0 &&
         h.distance(3, 5) == 4);
  const int n = 200000;
  cp::Tree chain(n);
  for (int v = 1; v < n; ++v)
    chain.add_edge(v - 1, v);
  cp::HeavyLightDecomposition deep(chain);
  assert(deep.path(n - 1, 0).size() == 1 &&
         expand(deep, n - 1, 0).size() == static_cast<std::size_t>(n));
}
