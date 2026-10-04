// competitive-verifier: STANDALONE
#include <algorithm>
#include <cassert>
#include <cp/graph/min_cost_flow.hpp>
#include <random>
#include <vector>
int main() {
  std::mt19937 rng(991);
  for (int trial = 0; trial < 800; ++trial) {
    int n = 2 + rng() % 3, m = rng() % 8;
    cp::MinCostFlow<int, int> g(n);
    for (int i = 0; i < m; ++i)
      g.add_edge(rng() % n, rng() % n, rng() % 3, rng() % 6);
    auto edges = g.edges();
    std::vector<int> best(20, 100000), balance(n);
    auto enumerate = [&](auto self, int idx, int cost) -> void {
      if (idx == m) {
        if (balance[0] < 0 || balance[n - 1] != -balance[0])
          return;
        for (int v = 1; v < n - 1; ++v)
          if (balance[v])
            return;
        best[balance[0]] = std::min(best[balance[0]], cost);
        return;
      }
      auto e = edges[idx];
      for (int f = 0; f <= e.cap; ++f) {
        balance[e.from] += f;
        balance[e.to] -= f;
        self(self, idx + 1, cost + f * e.cost);
        balance[e.from] -= f;
        balance[e.to] += f;
      }
    };
    enumerate(enumerate, 0, 0);
    auto slope = g.slope(0, n - 1);
    int maximum = 0;
    for (int f = 0; f < 20; ++f)
      if (best[f] < 100000)
        maximum = f;
    assert(slope.front() == std::pair(0, 0) && slope.back().first == maximum);
    for (std::size_t i = 1; i < slope.size(); ++i) {
      auto [f0, c0] = slope[i - 1];
      auto [f1, c1] = slope[i];
      assert((c1 - c0) % (f1 - f0) == 0);
      int unit = (c1 - c0) / (f1 - f0);
      for (int f = f0; f <= f1; ++f)
        assert(best[f] == c0 + (f - f0) * unit);
      if (i > 1) {
        auto [fp, cp] = slope[i - 2];
        assert((c0 - cp) / (f0 - fp) < unit);
      }
    }
    int total = 0;
    std::fill(balance.begin(), balance.end(), 0);
    for (auto e : g.edges()) {
      assert(0 <= e.flow && e.flow <= e.cap);
      balance[e.from] += e.flow;
      balance[e.to] -= e.flow;
      total += e.flow * e.cost;
    }
    assert(total == best[maximum] && balance[0] == maximum &&
           balance[n - 1] == -maximum);
    for (int v = 1; v < n - 1; ++v)
      assert(balance[v] == 0);
  }
}
