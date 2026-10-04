// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/zalgorithm
#include <cp/string/string_algorithms.hpp>
#include <iostream>
#include <string>
int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::string s;
  std::cin >> s;
  auto z = cp::z_algorithm(s);
  for (std::size_t i = 0; i < z.size(); ++i)
    std::cout << (i ? " " : "") << z[i];
  std::cout << '\n';
}
