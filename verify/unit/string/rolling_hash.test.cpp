// competitive-verifier: STANDALONE
#include <cassert>
#include <cp/string/rolling_hash.hpp>
#include <string>
constexpr auto default_parameters = cp::RollingHashParameters::from_seed();
static_assert(default_parameters.base_first == 505946798 &&
              default_parameters.base_second == 306193453);
int main() {
  auto parameters = cp::RollingHashParameters::from_seed(42);
  cp::RollingHash empty("", parameters);
  assert(empty.size() == 0 && empty.whole().length() == 0 &&
         empty.lcp(empty) == 0);
  assert(empty.whole() == cp::RollingHashValue(parameters));
  std::string bytes;
  bytes += char(0);
  bytes += char(255);
  bytes += char(128);
  bytes += char(0);
  cp::RollingHash hash(bytes, parameters), same(bytes, 42);
  assert(hash.whole() == same.whole());
  assert(hash.equal(0, 1, hash, 3, 4));
  cp::RollingHash short_bytes(std::string_view(bytes).substr(1, 2), parameters);
  assert(hash.get(1, 3) == short_bytes.whole());
  assert(cp::RollingHash::concat(hash.get(0, 2), hash.get(2, 4)) ==
         hash.whole());
  assert(cp::RollingHash::concat(cp::RollingHashValue(parameters),
                                 hash.whole()) == hash.whole());
  assert(cp::RollingHash::concat(
             hash.whole(), cp::RollingHashValue(parameters)) == hash.whole());
  cp::RollingHash a("abacaba", parameters), b("abacus", parameters);
  assert(a.lcp(b) == 4 && a.lcp(b, 0, 0, 2) == 2 && a.lcp(a, 0, 4) == 3);
  assert(a.lcp(b, a.size(), b.size()) == 0);
  cp::RollingHash different("abacaba", 43);
  assert(!a.whole().compatible(different.whole()) &&
         a.whole() != different.whole());
  for (int operation = 0; operation < 3; ++operation) {
    bool threw = false;
    try {
      if (operation == 0)
        (void)a.equal(0, 1, different, 0, 1);
      if (operation == 1)
        (void)a.lcp(different);
      if (operation == 2)
        (void)cp::RollingHash::concat(a.whole(), different.whole());
    } catch (const std::invalid_argument &) {
      threw = true;
    }
    assert(threw);
  }
  for (auto bad : {cp::RollingHashParameters{1, 257},
                   cp::RollingHashParameters{257, 1000000008}}) {
    bool threw = false;
    try {
      cp::RollingHash value("abc", bad);
    } catch (const std::invalid_argument &) {
      threw = true;
    }
    assert(threw);
  }
  cp::RollingHash single("x", parameters);
  auto huge = single.whole();
  for (int bit = 1; bit < std::numeric_limits<std::size_t>::digits; ++bit)
    huge = cp::RollingHash::concat(huge, huge);
  bool overflow = false;
  try {
    (void)cp::RollingHash::concat(huge, huge);
  } catch (const std::length_error &) {
    overflow = true;
  }
  assert(overflow);
  const std::size_t n = 300000;
  std::string repeated(n, char(0));
  cp::RollingHash large(repeated, parameters);
  assert(large.lcp(large, 0, 1) == n - 1);
  auto piece = cp::RollingHash(std::string(n / 2, char(0)), parameters).whole();
  assert(cp::RollingHash::concat(piece, piece) == large.whole());
}
