// competitive-verifier: STANDALONE
#include "tree_oracle.hpp"
#include <cp/data_structure/segment_tree.hpp>
#include <cp/tree/heavy_light_decomposition.hpp>
#include <random>
#include <string>
struct Aggregate {
  std::string forward, backward;
};
Aggregate op(Aggregate a, Aggregate b) {
  return {a.forward + b.forward, b.backward + a.backward};
}
Aggregate identity() { return {}; }
int main() {
  std::mt19937 rng(1013);
  for (int trial = 0; trial < 700; ++trial) {
    int n = 1 + rng() % 65;
    std::vector<std::pair<int, int>> edges;
    for (int v = 1; v < n; ++v)
      edges.emplace_back(v, rng() % v);
    std::shuffle(edges.begin(), edges.end(), rng);
    cp::Tree tree(n);
    for (auto [u, v] : edges)
      tree.add_edge(u, v);
    int root = rng() % n;
    TreeOracle oracle(tree, root);
    cp::HeavyLightDecomposition h(tree, root);
    std::vector<std::string> labels(n), edge_labels(n - 1);
    std::vector<Aggregate> vertex_values(n), edge_values(n);
    for (int v = 0; v < n; ++v) {
      labels[v] = std::to_string(v) + ",";
      vertex_values[h.index(v)] = {labels[v], labels[v]};
    }
    for (int id = 0; id < n - 1; ++id) {
      edge_labels[id] = "e" + std::to_string(id) + ",";
      edge_values[h.edge_index(id)] = {edge_labels[id], edge_labels[id]};
    }
    cp::SegmentTree<Aggregate, op, identity> vs(vertex_values), es(edge_values);
    for (int step = 0; step < 140; ++step) {
      int u = rng() % n, v = rng() % n;
      if (step % 7 == 0) {
        labels[u] = "[" + std::to_string(step) + "]";
        vs.set(h.index(u), {labels[u], labels[u]});
      }
      auto expected = oracle.path(u, v);
      std::vector<int> got;
      std::string direct, folded;
      for (int x : expected)
        direct += labels[x];
      for (auto s : h.path(u, v)) {
        auto a = vs.prod(s.left, s.right);
        folded += s.reverse ? a.backward : a.forward;
        if (s.reverse)
          for (int i = s.right; i-- > s.left;)
            got.push_back(h.vertex(i));
        else
          for (int i = s.left; i < s.right; ++i)
            got.push_back(h.vertex(i));
      }
      assert(got == expected && direct == folded &&
             h.lca(u, v) == oracle.lca(u, v));
      std::string expected_edges, folded_edges;
      std::vector<int> children;
      for (std::size_t i = 1; i < expected.size(); ++i) {
        int child = oracle.parent[expected[i - 1]] == expected[i]
                        ? expected[i - 1]
                        : expected[i];
        children.push_back(child);
        expected_edges += edge_labels[*oracle.edge[child]];
      }
      got.clear();
      for (auto s : h.path(u, v, true)) {
        auto a = es.prod(s.left, s.right);
        folded_edges += s.reverse ? a.backward : a.forward;
        if (s.reverse)
          for (int i = s.right; i-- > s.left;)
            got.push_back(h.vertex(i));
        else
          for (int i = s.left; i < s.right; ++i)
            got.push_back(h.vertex(i));
      }
      assert(got == children && expected_edges == folded_edges);
      assert(h.distance(u, v) == static_cast<long long>(expected.size()) - 1);
    }
    for (int v = 0; v < n; ++v) {
      for (bool edge : {false, true}) {
        auto [l, r] = h.subtree(v, edge);
        std::vector<int> got;
        for (int i = l; i < r; ++i)
          got.push_back(h.vertex(i));
        std::sort(got.begin(), got.end());
        auto expected = oracle.subtree(v);
        if (edge)
          expected.erase(std::find(expected.begin(), expected.end(), v));
        assert(got == expected);
      }
    }
  }
}
