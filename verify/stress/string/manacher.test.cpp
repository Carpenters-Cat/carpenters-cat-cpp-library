// competitive-verifier: STANDALONE
#include <cassert>
#include <cp/string/manacher.hpp>
#include <random>
#include <string>
int main() {
  std::mt19937 rng(27);
  for (int trial = 0; trial < 1600; ++trial) {
    int n = rng() % 81;
    std::string s(n, ' ');
    for (char &ch : s)
      ch = char(rng() % (trial % 2 ? 256 : 4));
    cp::Manacher result(s);
    for (int center = 0; center < n; ++center) {
      int radius = 1;
      while (center - radius >= 0 && center + radius < n &&
             s[center - radius] == s[center + radius])
        ++radius;
      assert(result.odd_radius(center) == radius);
      assert(result.odd_interval(center) ==
             std::pair(center - radius + 1, center + radius));
    }
    for (int gap = 0; gap <= n; ++gap) {
      int radius = 0;
      while (gap - radius - 1 >= 0 && gap + radius < n &&
             s[gap - radius - 1] == s[gap + radius])
        ++radius;
      assert(result.even_radius(gap) == radius &&
             result.even_interval(gap) ==
                 std::pair(gap - radius, gap + radius));
    }
    for (int l = 0; l <= n; ++l)
      for (int r = l; r <= n; ++r) {
        bool palindrome = true;
        for (int i = l, j = r - 1; i < j; ++i, --j)
          if (s[i] != s[j])
            palindrome = false;
        assert(result.is_palindrome(l, r) == palindrome);
      }
  }
}
