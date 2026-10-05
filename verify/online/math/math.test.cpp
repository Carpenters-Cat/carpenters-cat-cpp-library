// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/sum_of_floor_of_linear
#include <cp/math/math.hpp>
#include <iostream>
int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  int t;
  std::cin >> t;
  while (t--) {
    long long n, m, a, b;
    std::cin >> n >> m >> a >> b;
    std::cout << cp::floor_sum(n, m, a, b) << '\n';
  }
}
