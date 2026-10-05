// competitive-verifier: STANDALONE
#include <cassert>
#include <climits>
#include <cp/math/math.hpp>
#include <utility>
int main() {
  assert(cp::pow_mod(-2, 3, 7) == 6);
  assert(cp::pow_mod(0, 0, 1) == 0);
  assert(cp::pow_mod(LLONG_MIN, 0, INT_MAX) == 1);
  assert(cp::inv_mod(-3, 11) == 7);
  assert(cp::inv_mod(0, 1) == 0);
  assert((cp::crt({}, {}) == std::pair<long long, long long>{0, 1}));
  assert((cp::crt({-1, 3}, {4, 6}) == std::pair<long long, long long>{3, 12}));
  assert((cp::crt({0, 1}, {2, 2}) == std::pair<long long, long long>{0, 0}));
  assert((cp::crt({LLONG_MIN}, {LLONG_MAX}) ==
          std::pair<long long, long long>{LLONG_MAX - 1, LLONG_MAX}));
  assert(cp::floor_sum(0, 1, LLONG_MIN, LLONG_MIN) == 0);
  assert(cp::floor_sum(4, 3, -2, -1) == -7);
  assert(cp::floor_sum(3, 1, LLONG_MIN, LLONG_MAX) == -3);
  assert(cp::floor_sum((1LL << 32) - 1, (1LL << 32) - 1, 1, 0) == 0);
}
