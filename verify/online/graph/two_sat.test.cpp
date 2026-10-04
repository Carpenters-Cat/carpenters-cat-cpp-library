// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/two_sat
#include <cp/graph/two_sat.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::string p, cnf;
  int n, m;
  std::cin >> p >> cnf >> n >> m;
  cp::TwoSat sat(n);
  for (int i = 0; i < m; ++i) {
    int a, b, zero;
    std::cin >> a >> b >> zero;
    sat.add_clause(std::abs(a) - 1, a > 0, std::abs(b) - 1, b > 0);
  }
  if (!sat.satisfiable()) {
    std::cout << "s UNSATISFIABLE\n";
    return 0;
  }
  std::cout << "s SATISFIABLE\nv";
  auto answer = sat.answer();
  for (int i = 0; i < n; ++i)
    std::cout << ' ' << (answer[i] ? i + 1 : -i - 1);
  std::cout << " 0\n";
}
