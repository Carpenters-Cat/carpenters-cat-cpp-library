// competitive-verifier: STANDALONE
#include <algorithm>
#include <cassert>
#include <cp/graph/max_flow.hpp>
#include <random>
#include <vector>
int main() {
  std::mt19937 rng(713);
  for (int trial = 0; trial < 1500; ++trial) {
    int n = 2 + rng() % 7, m = rng() % 25;
    cp::MaxFlow<int> g(n);
    for (int i = 0; i < m; ++i)
      g.add_edge(rng() % n, rng() % n, rng() % 5);
    auto edges = g.edges();
    int best = 100000;
    for (int mask = 0; mask < (1 << n); ++mask) {
      if (!(mask & 1) || (mask & (1 << (n - 1))))
        continue;
      int cut = 0;
      for (auto e : edges)
        if ((mask >> e.from & 1) && !(mask >> e.to & 1))
          cut += e.cap;
      best = std::min(best, cut);
    }
    int partial = g.flow(0, n - 1, rng() % 5);
    assert(partial + g.flow(0, n - 1) == best);
    auto reachable = g.min_cut(0);
    int cut = 0;
    std::vector<int> balance(n);
    for (auto e : g.edges()) {
      assert(0 <= e.flow && e.flow <= e.cap);
      balance[e.from] += e.flow;
      balance[e.to] -= e.flow;
      if (reachable[e.from] && !reachable[e.to])
        cut += e.cap;
    }
    assert(cut == best && balance[0] == best && balance[n - 1] == -best);
    for (int v = 1; v < n - 1; ++v)
      assert(balance[v] == 0);
  }
}
