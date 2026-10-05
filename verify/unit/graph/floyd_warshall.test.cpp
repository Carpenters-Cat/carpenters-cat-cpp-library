// competitive-verifier: STANDALONE
#include <cassert>
#include <cp/graph/floyd_warshall.hpp>
#include <limits>
int main() {
  auto empty = cp::floyd_warshall(cp::WeightedGraph());
  assert(empty.distance.empty() && !empty.has_negative_cycle());
  cp::DistanceMatrix matrix(4, std::vector<std::optional<cp::Distance>>(4));
  matrix[0][1] = 2;
  matrix[1][2] = -5;
  matrix[2][1] = 5;
  matrix[3][3] = 9;
  auto result = cp::floyd_warshall(matrix);
  assert(result.distance[0][2] == -3 && result.distance[3][3] == 0 &&
         !result.has_negative_cycle());
  assert(!result.path(3, 0) && result.path(0, 0)->edges.empty());
  assert((result.path(0, 2)->edges == std::vector<std::size_t>{1, 6}));
  matrix[2][1] = 4;
  auto negative = cp::floyd_warshall(matrix);
  assert(negative.has_negative_cycle() &&
         negative.state[0][2] == cp::DistanceState::negative_infinity);
  assert(negative.distance[0][0] == 0 &&
         negative.state[3][1] == cp::DistanceState::unreachable);
  assert(!negative.path(0, 2));
  constexpr auto low = std::numeric_limits<cp::Distance>::min();
  constexpr auto high = std::numeric_limits<cp::Distance>::max();
  cp::WeightedGraph large(3);
  auto id = large.add_edge(0, 1, high);
  large.add_edge(1, 2, high);
  large.add_edge(0, 2, 1);
  auto exact = cp::floyd_warshall(large);
  assert(exact.distance[0][1] == high && exact.distance[0][2] == 1);
  assert(exact.path(0, 1)->edges == std::vector<std::size_t>{id});
  cp::WeightedGraph overflow(3);
  overflow.add_edge(0, 1, low);
  overflow.add_edge(1, 2, -1);
  bool threw = false;
  try {
    (void)cp::floyd_warshall(overflow);
  } catch (const std::overflow_error &) {
    threw = true;
  }
  assert(threw);
  cp::WeightedGraph dense(100);
  for (int i = 0; i < 100; ++i)
    for (int j = 0; j < 100; ++j)
      if (i != j)
        dense.add_edge(i, j, low);
  auto huge_cycles = cp::floyd_warshall(dense);
  for (const auto &row : huge_cycles.state)
    for (auto state : row)
      assert(state == cp::DistanceState::negative_infinity);
}
