// competitive-verifier: STANDALONE
#include <algorithm>
#include <cassert>
#include <cp/string/string_algorithms.hpp>
#include <numeric>
#include <random>
#include <string>
#include <vector>
int main() {
  std::mt19937 rng(112);
  for (int trial = 0; trial < 1500; ++trial) {
    int n = rng() % 201;
    std::vector<int> s(n);
    for (int &x : s)
      x = rng() % 9;
    std::vector<int> expected(n);
    std::iota(expected.begin(), expected.end(), 0);
    std::sort(expected.begin(), expected.end(), [&](int a, int b) {
      return std::lexicographical_compare(s.begin() + a, s.end(), s.begin() + b,
                                          s.end());
    });
    auto sa = cp::suffix_array(s, 8);
    assert(sa == expected && cp::suffix_array(s) == expected);
    std::string bytes;
    for (int x : s)
      bytes += char(x + 247);
    assert(cp::suffix_array(bytes) == expected);
    auto lcp = cp::lcp_array(s, sa), z = cp::z_algorithm(s);
    for (int i = 0; i < n; ++i) {
      int length = 0;
      while (i + length < n && s[length] == s[i + length])
        ++length;
      assert(z[i] == length);
      if (i) {
        length = 0;
        int a = sa[i - 1], b = sa[i];
        while (a + length < n && b + length < n &&
               s[a + length] == s[b + length])
          ++length;
        assert(lcp[i - 1] == length);
      }
    }
    assert(cp::lcp_array(bytes, sa) == lcp && cp::z_algorithm(bytes) == z);
  }
  const int n = 300000;
  std::string repeated(n, 'a');
  auto sa = cp::suffix_array(repeated), lcp = cp::lcp_array(repeated, sa),
       z = cp::z_algorithm(repeated);
  for (int i = 0; i < n; ++i) {
    assert(sa[i] == n - i - 1 && z[i] == n - i);
    if (i)
      assert(lcp[i - 1] == i);
  }
}
