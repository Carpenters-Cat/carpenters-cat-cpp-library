// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/scc
#include <cp/graph/scc.hpp>
#include <iostream>
int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  int n, m;
  std::cin >> n >> m;
  cp::SccGraph g(n);
  for (int i = 0; i < m; ++i) {
    int a, b;
    std::cin >> a >> b;
    g.add_edge(a, b);
  }
  auto groups = g.scc();
  std::cout << groups.size() << '\n';
  for (auto &group : groups) {
    std::cout << group.size();
    for (int v : group)
      std::cout << ' ' << v;
    std::cout << '\n';
  }
}
