// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/enumerate_palindromes
#include <cp/string/manacher.hpp>
#include <iostream>
#include <string>
int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::string s;
  std::cin >> s;
  cp::Manacher result(s);
  for (int i = 0; i < result.size(); ++i) {
    if (i)
      std::cout << ' ';
    auto [l, r] = result.odd_interval(i);
    std::cout << r - l;
    if (i + 1 < result.size()) {
      auto [a, b] = result.even_interval(i + 1);
      std::cout << ' ' << b - a;
    }
  }
  std::cout << '\n';
}
