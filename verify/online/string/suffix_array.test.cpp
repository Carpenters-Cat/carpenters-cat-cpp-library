// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/suffixarray
#include <cp/string/string_algorithms.hpp>
#include <iostream>
#include <string>
int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::string s;
  std::cin >> s;
  auto sa = cp::suffix_array(s);
  for (std::size_t i = 0; i < sa.size(); ++i)
    std::cout << (i ? " " : "") << sa[i];
  std::cout << '\n';
}
