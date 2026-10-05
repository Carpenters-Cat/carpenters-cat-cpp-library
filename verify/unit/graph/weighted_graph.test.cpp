// competitive-verifier: STANDALONE
#include <cassert>
#include <cp/graph/weighted_graph.hpp>
int main() {
  cp::WeightedGraph empty;
  assert(empty.size() == 0 && empty.edges().empty());
  cp::WeightedGraph graph(3);
  auto id = graph.add_edge(0, 1, -2);
  auto parallel = graph.add_edge(0, 1, 4);
  auto self = graph.add_edge(2, 2, 0);
  assert(id == 0 && parallel == 1 && self == 2);
  assert((graph.outgoing(0) == std::vector<std::size_t>{0, 1}));
  assert(graph.get_edge(id).from == 0 && graph.get_edge(id).to == 1 &&
         graph.get_edge(id).weight == -2);
  cp::ShortestPathResult result{0,
                                {0, -2, std::nullopt},
                                {cp::DistanceState::finite,
                                 cp::DistanceState::finite,
                                 cp::DistanceState::unreachable},
                                {-1, 0, -1},
                                {std::nullopt, id, std::nullopt}};
  auto path = result.path_to(1);
  assert((path->vertices == std::vector<int>{0, 1}) &&
         path->edges == std::vector<std::size_t>{id});
  assert(result.path_to(0)->edges.empty() && !result.path_to(2));
  result.state[1] = cp::DistanceState::negative_infinity;
  result.distance[1].reset();
  assert(result.has_negative_cycle() && !result.path_to(1));
}
