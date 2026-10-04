// competitive-verifier: STANDALONE
#include <cassert>
#include <cp/tree/euler_tour.hpp>
int main() {
  cp::EulerTour empty(cp::Tree{});
  assert(empty.root() == -1 && empty.order().empty());
  cp::Tree one(1);
  cp::EulerTour single(one);
  assert(single.subtree(0) == std::pair(0, 1));
  cp::Tree tree(5);
  tree.add_edge(0, 2);
  tree.add_edge(0, 1);
  tree.add_edge(2, 4);
  tree.add_edge(2, 3);
  cp::EulerTour tour(tree);
  assert((tour.order() == std::vector<int>{0, 2, 4, 3, 1}));
  assert(tour.subtree(2) == std::pair(1, 4) && tour.parent(4) == 2 &&
         tour.depth(4) == 2);
  cp::EulerTour rerooted(tree, 2);
  assert((rerooted.order() == std::vector<int>{2, 0, 1, 4, 3}));
  const int n = 200000;
  cp::Tree chain(n);
  for (int v = 1; v < n; ++v)
    chain.add_edge(v - 1, v);
  cp::EulerTour deep(chain);
  for (int v = 0; v < n; ++v)
    assert(deep.in(v) == v && deep.out(v) == n && deep.vertex(v) == v);
}
