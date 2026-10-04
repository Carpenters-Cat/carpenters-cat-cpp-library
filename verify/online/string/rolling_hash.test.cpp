// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/zalgorithm
#include <cp/string/rolling_hash.hpp>
#include <iostream>
#include <string>
int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::string s;
  std::cin >> s;
  cp::RollingHash hash(s, 26);
  for (std::size_t i = 0; i < s.size(); ++i)
    std::cout << (i ? " " : "") << hash.lcp(hash, 0, i);
  std::cout << '\n';
}
