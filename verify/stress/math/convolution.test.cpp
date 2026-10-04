// competitive-verifier: STANDALONE
#include <cassert>
#include <cp/math/convolution.hpp>
#include <random>
int main() {
  using M = cp::modint998244353;
  std::mt19937 rng(938475);
  for (int t = 0; t < 150; t++) {
    int n = 1 + rng() % 160, m = 1 + rng() % 160;
    std::vector<long long> a(n), b(m), want(n + m - 1);
    std::vector<M> am(n), bm(m), wm(n + m - 1);
    for (int i = 0; i < n; i++) {
      a[i] = static_cast<int>(rng() % 20001) - 10000;
      am[i] = rng();
    }
    for (int i = 0; i < m; i++) {
      b[i] = static_cast<int>(rng() % 20001) - 10000;
      bm[i] = rng();
    }
    for (int i = 0; i < n; i++)
      for (int j = 0; j < m; j++) {
        want[i + j] += a[i] * b[j];
        wm[i + j] += am[i] * bm[j];
      }
    assert(cp::convolution_ll(a, b) == want);
    assert(cp::convolution(am, bm) == wm);
    assert(cp::convolution(std::vector<M>(am), std::vector<M>(bm)) == wm);
  }
}
