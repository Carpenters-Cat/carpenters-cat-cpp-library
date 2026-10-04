// competitive-verifier: STANDALONE
#include <cassert>
#include <cp/string/manacher.hpp>
#include <string>
int main() {
  cp::Manacher empty("");
  assert(empty.odd_radii().empty() &&
         empty.even_radii() == std::vector<int>{0});
  assert(empty.is_palindrome(0, 0) &&
         empty.even_interval(0) == std::pair(0, 0));
  cp::Manacher one("x");
  assert(one.odd_radius(0) == 1 && one.odd_interval(0) == std::pair(0, 1));
  assert(one.even_radius(0) == 0 && one.even_radius(1) == 0 &&
         one.is_palindrome(1, 1));
  cp::Manacher odd("abacaba");
  assert(odd.odd_interval(3) == std::pair(0, 7) && odd.is_palindrome(2, 5));
  assert(!odd.is_palindrome(0, 6));
  cp::Manacher even("abba");
  assert(even.even_radius(2) == 2 && even.even_interval(2) == std::pair(0, 4));
  assert(even.is_palindrome(1, 3) && !even.is_palindrome(0, 3));
  std::string bytes;
  bytes += char(255);
  bytes += char(0);
  bytes += char(255);
  cp::Manacher binary(bytes);
  assert(binary.odd_radius(1) == 2 && binary.is_palindrome(0, 3));
  const int n = 300000;
  cp::Manacher repeated(std::string(n, 'a'));
  for (int i = 0; i < n; ++i)
    assert(repeated.odd_radius(i) == std::min(i + 1, n - i));
  for (int i = 0; i <= n; ++i)
    assert(repeated.even_radius(i) == std::min(i, n - i));
  assert(repeated.is_palindrome(0, n));
}
