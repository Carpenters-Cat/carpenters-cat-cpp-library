// competitive-verifier: STANDALONE
#include <cassert>
#include <cp/tree/tree.hpp>
int main() {
  cp::Tree empty;
  cp::RootedTree re(empty);
  assert(re.root == -1 && re.order.empty());
  cp::Tree tree(5);
  tree.add_edge(0, 2);
  tree.add_edge(0, 1);
  tree.add_edge(2, 4);
  tree.add_edge(2, 3);
  cp::RootedTree r(tree);
  assert((r.order == std::vector<int>{0, 2, 4, 3, 1}));
  assert(r.parent[0] == -1 && r.parent[4] == 2 && r.depth[4] == 2 &&
         r.subtree_size[2] == 3);
  assert(r.parent_edge[4] == 2);
  for (int which = 0; which < 4; ++which) {
    cp::Tree invalid(4);
    if (which == 0) {
      invalid.add_edge(0, 1);
      invalid.add_edge(1, 2);
    }
    if (which == 1) {
      invalid.add_edge(0, 1);
      invalid.add_edge(0, 1);
      invalid.add_edge(2, 3);
    }
    if (which == 2) {
      invalid.add_edge(0, 0);
      invalid.add_edge(1, 2);
      invalid.add_edge(2, 3);
    }
    if (which == 3) {
      invalid.add_edge(0, 1);
      invalid.add_edge(1, 2);
      invalid.add_edge(2, 0);
    }
    bool threw = false;
    try {
      cp::RootedTree bad(invalid);
    } catch (const std::invalid_argument &) {
      threw = true;
    }
    assert(threw);
  }
  bool invalid_root = false;
  try {
    cp::RootedTree bad(tree, 5);
  } catch (const std::invalid_argument &) {
    invalid_root = true;
  }
  assert(invalid_root);
}
