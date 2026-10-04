// competitive-verifier: STANDALONE
#include <cassert>
#include <cp/string/rolling_hash.hpp>
#include <random>
#include <string>
int main() {
  std::mt19937_64 rng(26);
  for (int trial = 0; trial < 1500; ++trial) {
    std::size_t n = rng() % 81, m = rng() % 81;
    std::string first(n, ' '), second(m, ' ');
    for (char &ch : first)
      ch = char(rng() % (trial % 2 ? 256 : 4));
    for (char &ch : second)
      ch = char(rng() % (trial % 2 ? 256 : 4));
    if (trial % 5 == 0)
      second = first;
    auto parameters = cp::RollingHashParameters::from_seed(rng());
    cp::RollingHash a(first, parameters), b(second, parameters);
    for (int query = 0; query < 180; ++query) {
      std::size_t l = rng() % (first.size() + 1),
                  r = rng() % (first.size() + 1);
      std::size_t x = rng() % (second.size() + 1),
                  y = rng() % (second.size() + 1);
      if (l > r)
        std::swap(l, r);
      if (x > y)
        std::swap(x, y);
      auto aa = std::string_view(first).substr(l, r - l),
           bb = std::string_view(second).substr(x, y - x);
      assert(a.equal(l, r, b, x, y) == (aa == bb));
      cp::RollingHash standalone(aa, parameters);
      assert(a.get(l, r) == standalone.whole());
      cp::RollingHash joined(std::string(aa) + std::string(bb), parameters);
      auto concatenated = cp::RollingHash::concat(a.get(l, r), b.get(x, y));
      assert(concatenated == joined.whole() &&
             concatenated.length() == aa.size() + bb.size());
      std::size_t limit = rng() % 100, expected = 0;
      while (expected < limit && l + expected < first.size() &&
             x + expected < second.size() &&
             first[l + expected] == second[x + expected])
        ++expected;
      assert(a.lcp(b, l, x, limit) == expected);
    }
    // Different grouping must preserve concatenation and cached base powers.
    std::size_t split = first.size() / 2;
    auto grouped = cp::RollingHash::concat(
        cp::RollingHash::concat(a.get(0, split), a.get(split, first.size())),
        b.whole());
    auto alternate = cp::RollingHash::concat(
        a.get(0, split),
        cp::RollingHash::concat(a.get(split, first.size()), b.whole()));
    assert(grouped == alternate);
  }
}
