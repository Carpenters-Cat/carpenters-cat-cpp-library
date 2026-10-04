// competitive-verifier: STANDALONE
#include <cassert>
#include <cp/graph/two_sat.hpp>
#include <random>
#include <tuple>
#include <vector>
int main() {
  std::mt19937 rng(456);
  for (int trial = 0; trial < 1800; ++trial) {
    int n = 1 + rng() % 8, m = rng() % 35;
    cp::TwoSat sat(n);
    std::vector<std::tuple<int, bool, int, bool>> clauses;
    for (int i = 0; i < m; ++i) {
      int a = rng() % n, b = rng() % n;
      bool f = rng() % 2, g = rng() % 2;
      sat.add_clause(a, f, b, g);
      clauses.emplace_back(a, f, b, g);
    }
    bool exists = false;
    for (int mask = 0; mask < (1 << n); ++mask) {
      bool ok = true;
      for (auto [a, f, b, g] : clauses)
        if (bool(mask >> a & 1) != f && bool(mask >> b & 1) != g)
          ok = false;
      exists |= ok;
    }
    assert(sat.satisfiable() == exists);
    if (exists) {
      auto answer = sat.answer();
      for (auto [a, f, b, g] : clauses)
        assert(answer[a] == f || answer[b] == g);
    }
  }
}
