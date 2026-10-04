// competitive-verifier: STANDALONE
#include <cassert>
#include <cp/graph/scc.hpp>
#include <vector>
int main() {
  assert(cp::SccGraph().scc().empty());
  cp::SccGraph g(5);
  g.add_edge(0, 1);
  g.add_edge(1, 0);
  g.add_edge(1, 2);
  g.add_edge(2, 3);
  g.add_edge(3, 2);
  g.add_edge(3, 3);
  g.add_edge(1, 2);
  auto [count, ids] = g.scc_ids();
  assert(count == 3 && ids[0] == ids[1] && ids[2] == ids[3]);
  assert(ids[1] < ids[2] && ids[4] != ids[0] && ids[4] != ids[2]);
  assert(g.scc() == g.scc());
  g.add_edge(3, 0);
  assert(g.scc_ids().first == 2);
  const int n = 300000;
  cp::SccGraph chain(n);
  for (int i = 1; i < n; ++i)
    chain.add_edge(i - 1, i);
  auto [nc, ci] = chain.scc_ids();
  assert(nc == n);
  for (int i = 0; i < n; ++i)
    assert(ci[i] == i);
  chain.add_edge(n - 1, 0);
  assert(chain.scc().front().size() == static_cast<std::size_t>(n));
}
