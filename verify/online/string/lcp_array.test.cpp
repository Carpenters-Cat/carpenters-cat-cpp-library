// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/number_of_substrings
#include <cp/string/string_algorithms.hpp>
#include <iostream>
#include <string>
int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::string s;
  std::cin >> s;
  long long n = s.size(), answer = n * (n + 1) / 2;
  for (int length : cp::lcp_array(s, cp::suffix_array(s)))
    answer -= length;
  std::cout << answer << '\n';
}
