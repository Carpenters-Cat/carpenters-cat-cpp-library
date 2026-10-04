// competitive-verifier: STANDALONE
#include "tree_oracle.hpp"
#include <cp/tree/rerooting.hpp>
#include <random>
#include <string>
struct SumState {
  long long count, sum;
};
struct Encoded {
  std::string value;
};
int main() {
  std::mt19937 rng(1015);
  for (int trial = 0; trial < 900; ++trial) {
    int n = 1 + rng() % 15;
    std::vector<std::pair<int, int>> edges;
    for (int v = 1; v < n; ++v)
      edges.emplace_back(v, rng() % v);
    std::shuffle(edges.begin(), edges.end(), rng);
    cp::Tree tree(n);
    for (auto [u, v] : edges)
      tree.add_edge(u, v);
    std::vector<int> weights(n - 1);
    for (int &w : weights)
      w = int(rng() % 11) - 5;
    auto merge = [](SumState a, SumState b) {
      return SumState{a.count + b.count, a.sum + b.sum};
    };
    auto finish = [](SumState a, int) { return SumState{a.count + 1, a.sum}; };
    auto lift = [&](SumState a, int from, int to, std::size_t id) {
      return SumState{a.count, a.sum + a.count * (weights[id] + from - to)};
    };
    auto sum = cp::rerooting(tree, SumState{0, 0}, merge, lift, finish);
    for (int root = 0; root < n; ++root) {
      TreeOracle oracle(tree, root);
      std::vector<long long> distance(n);
      long long expected = 0;
      for (int v : oracle.order)
        if (oracle.parent[v] >= 0) {
          distance[v] = distance[oracle.parent[v]] + weights[*oracle.edge[v]] +
                        v - oracle.parent[v];
          expected += distance[v];
        }
      assert(sum[root].count == n && sum[root].sum == expected);
    }
    // Distinct State/Result types and directed edge transformations. The
    // parent contribution must remain in its original adjacency position.
    auto concat = [](const std::string &a, const std::string &b) {
      return a + b;
    };
    auto encode = [](const std::string &value, int v) {
      return Encoded{"(" + std::to_string(v) + ":" + value + ")"};
    };
    auto edge = [&](const Encoded &value, int from, int to, std::size_t id) {
      auto [u, v] = tree.edges()[id];
      assert((u == from && v == to) || (u == to && v == from));
      return "[" + std::to_string(from) + ">" + std::to_string(to) + "/" +
             std::to_string(id) + value.value + "]";
    };
    auto ordered = cp::rerooting(tree, std::string{}, concat, edge, encode);
    for (int root = 0; root < n; ++root) {
      auto naive = [&](auto self, int v, int parent) -> Encoded {
        std::string aggregate;
        for (auto arc : tree.neighbors(v))
          if (arc.to != parent)
            aggregate += edge(self(self, arc.to, v), arc.to, v, arc.edge_id);
        return encode(aggregate, v);
      };
      assert(ordered[root].value == naive(naive, root, -1).value);
    }
  }
}
