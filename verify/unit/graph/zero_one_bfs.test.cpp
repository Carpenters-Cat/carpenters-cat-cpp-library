// competitive-verifier: STANDALONE
#include <cassert>
#include <cp/graph/zero_one_bfs.hpp>
int main() {
  cp::WeightedGraph graph(5);
  graph.add_edge(0, 1, 1);
  auto a = graph.add_edge(0, 2, 0);
  auto b = graph.add_edge(2, 1, 0);
  graph.add_edge(1, 2, 0);
  graph.add_edge(1, 1, 0);
  auto c = graph.add_edge(1, 3, 1);
  auto result = cp::zero_one_bfs(graph, 0);
  assert(result.distance[1] == 0 && result.distance[3] == 1);
  assert((result.path_to(3)->edges == std::vector<std::size_t>{a, b, c}));
  assert(!result.distance[4] && !result.path_to(4));
  assert(result.path_to(0)->vertices == std::vector<int>{0});
  const int n = 200000;
  cp::WeightedGraph zero(n), one(n);
  for (int i = 1; i < n; ++i) {
    zero.add_edge(i - 1, i, 0);
    one.add_edge(i - 1, i, 1);
  }
  auto rz = cp::zero_one_bfs(zero, 0), ro = cp::zero_one_bfs(one, 0);
  assert(rz.distance[n - 1] == 0 && ro.distance[n - 1] == n - 1);
  assert(rz.path_to(n - 1)->edges.size() == static_cast<std::size_t>(n - 1));
}
