// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/convolution_mod
#include <cp/math/convolution.hpp>
#include <iostream>
int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  int n, m;
  std::cin >> n >> m;
  std::vector<int> a(n), b(m);
  for (auto &x : a)
    std::cin >> x;
  for (auto &x : b)
    std::cin >> x;
  auto c = cp::convolution(a, b);
  for (int x : c)
    std::cout << x << ' ';
  std::cout << '\n';
}
