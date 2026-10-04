// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/tree_path_composite_sum
#include <cp/math/modint.hpp>
#include <cp/tree/rerooting.hpp>
#include <iostream>
using Mint = cp::modint998244353;
struct State {
  Mint sum, count;
};
int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  int n;
  std::cin >> n;
  std::vector<Mint> value(n), b(n - 1), c(n - 1);
  for (auto &v : value) {
    long long x;
    std::cin >> x;
    v = x;
  }
  cp::Tree tree(n);
  for (int id = 0; id < n - 1; ++id) {
    int u, v;
    long long x, y;
    std::cin >> u >> v >> x >> y;
    tree.add_edge(u, v);
    b[id] = x;
    c[id] = y;
  }
  auto result = cp::rerooting(
      tree, State{0, 0},
      [](State a, State b) { return State{a.sum + b.sum, a.count + b.count}; },
      [&](State state, int, int, std::size_t id) {
        return State{b[id] * state.sum + c[id] * state.count, state.count};
      },
      [&](State state, int v) {
        return State{state.sum + value[v], state.count + 1};
      });
  for (int v = 0; v < n; ++v)
    std::cout << (v ? " " : "") << result[v].sum.val();
  std::cout << '\n';
}
