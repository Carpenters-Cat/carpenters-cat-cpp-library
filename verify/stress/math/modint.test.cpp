// competitive-verifier: STANDALONE
#include <cassert>
#include <cp/math/modint.hpp>
#include <numeric>
#include <random>
int main() {
  std::mt19937_64 rng(723049);
  using D = cp::DynamicModint<42>;
  for (int m : {1, 2, 12, 97, 998244353, 2147483647}) {
    D::set_mod(m);
    for (int t = 0; t < 10000; t++) {
      long long a = rng() % m, b = rng() % m;
      assert((D(a) + D(b)).val() == (a + b) % m);
      assert((D(a) - D(b)).val() == (a - b + m) % m);
      assert((D(a) * D(b)).val() == a * b % m);
      int n = rng() % 50;
      long long p = 1 % m;
      for (int i = 0; i < n; i++)
        p = p * a % m;
      assert(D(a).pow(n).val() == p);
      if (std::gcd(b, static_cast<long long>(m)) == 1)
        assert((D(a) / D(b) * D(b)).val() == a);
    }
  }
}
