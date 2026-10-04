// competitive-verifier: STANDALONE
#include <cassert>
#include <cp/graph/min_cost_flow.hpp>
#include <utility>
#include <vector>
int main() {
  cp::MinCostFlow<int, long long> empty;
  assert(empty.edges().empty());
  cp::MinCostFlow<int, long long> g(3);
  int id = g.add_edge(0, 1, 2, 1);
  g.add_edge(1, 2, 2, 3);
  g.add_edge(0, 2, 1, 4);
  g.add_edge(0, 2, 2, 7);
  g.add_edge(1, 1, 10, 0);
  auto s = g.slope(0, 2);
  assert(
      (s == std::vector<std::pair<int, long long>>{{0, 0}, {3, 12}, {5, 26}}));
  auto e = g.get_edge(id);
  assert(e.from == 0 && e.to == 1 && e.flow == 2 && e.cap == 2 && e.cost == 1);
  cp::MinCostFlow<long long, long long> big(2);
  big.add_edge(0, 1, 1000000000LL, 1000000000LL);
  assert((big.flow(0, 1) == std::pair<long long, long long>{
                                1000000000LL, 1000000000000000000LL}));
  cp::MinCostFlow<int, int> no_path(2), zero(2), limited(2);
  assert((no_path.flow(0, 1) == std::pair<int, int>{0, 0}));
  zero.add_edge(0, 1, 3, 0);
  assert((zero.slope(0, 1, 0) == std::vector<std::pair<int, int>>{{0, 0}}));
  limited.add_edge(0, 1, 3, 2);
  assert((limited.flow(0, 1, 2) == std::pair<int, int>{2, 4}));
}
