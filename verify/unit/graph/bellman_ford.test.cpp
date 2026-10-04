// competitive-verifier: STANDALONE
#include <cassert>
#include <cp/graph/bellman_ford.hpp>
#include <limits>
int main() {
  cp::WeightedGraph graph(7);
  graph.add_edge(0, 1, 3);
  auto a = graph.add_edge(0, 1, 2);
  auto b = graph.add_edge(1, 2, -5);
  graph.add_edge(2, 1, 5);
  graph.add_edge(0, 0, 0);
  graph.add_edge(3, 3, -1);
  auto result = cp::bellman_ford(graph, 0);
  assert(!result.has_negative_cycle() && result.distance[2] == -3 &&
         !result.distance[3]);
  assert((result.path_to(2)->edges == std::vector<std::size_t>{a, b}));
  graph.add_edge(2, 4, -1);
  graph.add_edge(4, 2, 0);
  graph.add_edge(4, 5, 0);
  auto negative = cp::bellman_ford(graph, 0);
  assert(negative.has_negative_cycle() && negative.distance[0] == 0);
  for (int v : {1, 2, 4, 5})
    assert(negative.state[v] == cp::DistanceState::negative_infinity &&
           !negative.path_to(v));
  assert(negative.state[3] == cp::DistanceState::unreachable);
  constexpr auto low = std::numeric_limits<cp::Distance>::min();
  constexpr auto high = std::numeric_limits<cp::Distance>::max();
  cp::WeightedGraph large(3);
  large.add_edge(0, 1, low);
  large.add_edge(1, 2, high);
  auto exact = cp::bellman_ford(large, 0);
  assert(exact.distance[1] == low && exact.distance[2] == -1);
  large.add_edge(1, 1, -1);
  auto cycle = cp::bellman_ford(large, 0);
  assert(cycle.state[1] == cp::DistanceState::negative_infinity &&
         cycle.state[2] == cp::DistanceState::negative_infinity);
  cp::WeightedGraph overflow(3);
  overflow.add_edge(0, 1, low);
  overflow.add_edge(1, 2, -1);
  bool threw = false;
  try {
    (void)cp::bellman_ford(overflow, 0);
  } catch (const std::overflow_error &) {
    threw = true;
  }
  assert(threw);
  cp::WeightedGraph single(1);
  single.add_edge(0, 0, low);
  assert(cp::bellman_ford(single, 0).state[0] ==
         cp::DistanceState::negative_infinity);
}
