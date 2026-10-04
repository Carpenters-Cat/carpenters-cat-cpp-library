// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/GRL_6_B
#include <cp/graph/min_cost_flow.hpp>
#include <iostream>
int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  int n, m;
  long long f;
  std::cin >> n >> m >> f;
  cp::MinCostFlow<long long, long long> g(n);
  for (int i = 0; i < m; ++i) {
    int a, b;
    long long cap, cost;
    std::cin >> a >> b >> cap >> cost;
    g.add_edge(a, b, cap, cost);
  }
  auto [sent, cost] = g.flow(0, n - 1, f);
  std::cout << (sent == f ? cost : -1) << '\n';
}
