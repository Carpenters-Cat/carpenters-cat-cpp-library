// competitive-verifier: STANDALONE
#include <cassert>
#include <cp/math/math.hpp>
#include <numeric>
#include <random>
int main() {
  std::mt19937 rng(617283);
  for (int t = 0; t < 5000; t++) {
    long long n = rng() % 40, m = 1 + rng() % 40,
              a = static_cast<int>(rng() % 201) - 100,
              b = static_cast<int>(rng() % 201) - 100;
    long long sum = 0;
    for (int i = 0; i < n; i++) {
      long long x = a * i + b;
      sum += x / m - (x % m < 0);
    }
    assert(cp::floor_sum(n, m, a, b) == sum);
    long long p = 1 % m, v = (a % m + m) % m;
    for (int i = 0; i < n; i++)
      p = p * v % m;
    assert(cp::pow_mod(a, n, m) == p);
    if (std::gcd(v, m) == 1)
      assert(v * cp::inv_mod(a, m) % m == 1 % m);
    std::vector<long long> r{a, b, static_cast<int>(rng() % 201) - 100},
        mods{1 + rng() % 10, 1 + rng() % 10, 1 + rng() % 10};
    long long l = std::lcm(std::lcm(mods[0], mods[1]), mods[2]), want = -1;
    for (long long x = 0; x < l; x++) {
      bool ok = true;
      for (int i = 0; i < 3; i++)
        ok &= (x - r[i]) % mods[i] == 0;
      if (ok) {
        want = x;
        break;
      }
    }
    auto [x, z] = cp::crt(r, mods);
    assert(want < 0 ? (x == 0 && z == 0) : (x == want && z == l));
  }
}
