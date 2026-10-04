// competitive-verifier: STANDALONE
#include <cassert>
#include <cp/graph/max_flow.hpp>
#include <limits>
int main() {
  cp::MaxFlow<long long> empty;
  assert(empty.edges().empty());
  cp::MaxFlow<long long> g(4);
  int self = g.add_edge(0, 0, 9);
  int a = g.add_edge(0, 1, 3);
  g.add_edge(0, 1, 2);
  g.add_edge(1, 3, 4);
  g.add_edge(0, 2, 2);
  g.add_edge(2, 3, 2);
  assert(g.flow(0, 3, 0) == 0);
  assert(g.flow(0, 3, 2) == 2);
  assert(g.flow(0, 3) == 4);
  assert(g.flow(0, 3) == 0);
  assert(g.get_edge(self).flow == 0);
  assert(g.get_edge(a).cap == 3);
  auto cut = g.min_cut(0);
  assert(cut[0] && cut[1] && !cut[2] && !cut[3]);
  g.change_edge(self, 10, 5);
  assert(g.get_edge(self).cap == 10 && g.get_edge(self).flow == 5);
  cp::MaxFlow<unsigned long long> large(2);
  large.add_edge(0, 1, std::numeric_limits<unsigned long long>::max());
  assert(large.flow(0, 1) == std::numeric_limits<unsigned long long>::max());
  cp::MaxFlow<int> changed(2);
  int id = changed.add_edge(0, 1, 3);
  changed.change_edge(id, 5, 0);
  assert(changed.flow(0, 1) == 5);
}
