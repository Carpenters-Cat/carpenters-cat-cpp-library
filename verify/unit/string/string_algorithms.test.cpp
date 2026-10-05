// competitive-verifier: STANDALONE
#include <cassert>
#include <cp/string/string_algorithms.hpp>
#include <string>
#include <vector>
int main() {
  std::string s = "banana";
  auto sa = cp::suffix_array(s);
  assert((sa == std::vector<int>{5, 3, 1, 0, 4, 2}));
  assert((cp::lcp_array(s, sa) == std::vector<int>{1, 3, 0, 0, 2}));
  assert((cp::z_algorithm(std::string("aaaaa")) ==
          std::vector<int>{5, 4, 3, 2, 1}));
  assert(cp::suffix_array(std::string()).empty());
  assert(cp::lcp_array(std::string(), {}).empty());
  assert(cp::z_algorithm(std::string()).empty());
  assert((cp::suffix_array(std::vector<int>{-1, 9, -1}) ==
          std::vector<int>{2, 0, 1}));
  assert((cp::suffix_array(std::vector<int>{0, 2, 0}, 2) ==
          std::vector<int>{2, 0, 1}));
  assert((cp::suffix_array(std::vector<std::string>{"b", "a"}) ==
          std::vector<int>{1, 0}));
  std::string bytes;
  bytes += char(255);
  bytes += char(128);
  bytes += char(0);
  assert((cp::suffix_array(bytes) == std::vector<int>{2, 1, 0}));
  assert(cp::lcp_array(std::string("x"), {0}).empty());
}
