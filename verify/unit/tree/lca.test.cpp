// competitive-verifier: STANDALONE
#include <cassert>
#include <cp/tree/lca.hpp>
#include <limits>
int main() {
  cp::Lca empty(cp::Tree{});
  assert(empty.size() == 0 && empty.root() == -1);
  cp::Tree one(1);
  cp::Lca single(one);
  assert(single.lca(0, 0) == 0 && single.distance(0, 0) == 0 &&
         single.jump(0, 0) == 0);
  assert(single.jump(0, 1) == -1 && single.parent(0) == -1 &&
         !single.parent_edge(0));
  cp::Tree star(6);
  for (int v = 1; v < 6; ++v)
    star.add_edge(0, v);
  cp::Lca lca(star, 3);
  assert(lca.root() == 3 && lca.lca(1, 2) == 0 && lca.lca(3, 1) == 3);
  assert(lca.distance(1, 2) == 2 && lca.jump(2, 2) == 3 &&
         lca.jump(2, 3) == -1);
  assert(lca.jump(2, std::numeric_limits<std::uint64_t>::max()) == -1);
  const int n = 200000;
  cp::Tree chain(n);
  for (int v = 1; v < n; ++v)
    chain.add_edge(v - 1, v);
  cp::Lca deep(chain, n - 1);
  assert(deep.jump(0, n - 1) == n - 1 && deep.jump(0, n) == -1 &&
         deep.lca(0, 17) == 17 && deep.distance(0, n - 1) == n - 1);
}
