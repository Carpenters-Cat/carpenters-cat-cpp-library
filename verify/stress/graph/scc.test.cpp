// competitive-verifier: STANDALONE
#include <cassert>
#include <cp/graph/scc.hpp>
#include <random>
#include <vector>
int main() {
  std::mt19937 rng(192);
  for (int trial = 0; trial < 1200; ++trial) {
    int n = rng() % 20;
    cp::SccGraph g(n);
    std::vector<std::vector<bool>> reach(n, std::vector<bool>(n));
    for (int i = 0; i < n; ++i)
      reach[i][i] = true;
    if (n)
      for (unsigned m = rng() % 80; m--;) {
        int a = rng() % n, b = rng() % n;
        g.add_edge(a, b);
        reach[a][b] = true;
      }
    for (int k = 0; k < n; ++k)
      for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
          reach[i][j] = reach[i][j] || (reach[i][k] && reach[k][j]);
    auto [count, ids] = g.scc_ids();
    auto groups = g.scc();
    assert(groups.size() == static_cast<std::size_t>(count));
    std::vector<bool> seen(n);
    for (int id = 0; id < count; ++id)
      for (int v : groups[id]) {
        assert(!seen[v] && ids[v] == id);
        seen[v] = true;
      }
    for (int i = 0; i < n; ++i)
      for (int j = 0; j < n; ++j) {
        assert((ids[i] == ids[j]) == (reach[i][j] && reach[j][i]));
        if (reach[i][j])
          assert(ids[i] <= ids[j]);
      }
    for (bool v : seen)
      assert(v);
  }
}
