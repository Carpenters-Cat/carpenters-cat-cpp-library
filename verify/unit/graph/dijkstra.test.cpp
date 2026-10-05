// competitive-verifier: STANDALONE
#include <cassert>
#include <cp/graph/dijkstra.hpp>
#include <limits>
int main() {
  cp::WeightedGraph graph(5);
  graph.add_edge(0, 0, 0);
  graph.add_edge(0, 1, 8);
  auto id = graph.add_edge(0, 1, 0);
  graph.add_edge(1, 0, 0);
  auto next = graph.add_edge(1, 2, 2);
  graph.add_edge(0, 2, 7);
  auto result = cp::dijkstra(graph, 0);
  assert(result.distance[0] == 0 && result.distance[1] == 0 &&
         result.distance[2] == 2);
  assert(result.state[4] == cp::DistanceState::unreachable &&
         !result.path_to(4));
  assert((result.path_to(2)->edges == std::vector<std::size_t>{id, next}));
  constexpr auto high = std::numeric_limits<cp::Distance>::max();
  cp::WeightedGraph large(3);
  large.add_edge(0, 1, high);
  large.add_edge(1, 2, high);
  large.add_edge(0, 2, 1);
  auto exact = cp::dijkstra(large, 0);
  assert(exact.distance[1] == high && exact.distance[2] == 1);
  cp::WeightedGraph overflow(3);
  overflow.add_edge(0, 1, high);
  overflow.add_edge(1, 2, 1);
  bool threw = false;
  try {
    (void)cp::dijkstra(overflow, 0);
  } catch (const std::overflow_error &) {
    threw = true;
  }
  assert(threw);
  const int n = 200000;
  cp::WeightedGraph chain(n);
  for (int i = 1; i < n; ++i)
    chain.add_edge(i - 1, i, 0);
  auto deep = cp::dijkstra(chain, 0);
  assert(deep.path_to(n - 1)->vertices.size() == static_cast<std::size_t>(n));
}
