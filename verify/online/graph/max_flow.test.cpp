// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/GRL_6_A
#include <cp/graph/max_flow.hpp>
#include <iostream>
int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  int n, m;
  std::cin >> n >> m;
  cp::MaxFlow<long long> g(n);
  for (int i = 0; i < m; ++i) {
    int a, b;
    long long c;
    std::cin >> a >> b >> c;
    g.add_edge(a, b, c);
  }
  std::cout << g.flow(0, n - 1) << '\n';
}
