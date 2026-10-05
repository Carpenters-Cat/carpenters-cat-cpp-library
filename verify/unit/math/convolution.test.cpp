// competitive-verifier: STANDALONE
#include <cassert>
#include <climits>
#include <cp/math/convolution.hpp>
#include <vector>
int main() {
  using M = cp::modint998244353;
  assert(cp::convolution(std::vector<M>{}, std::vector<M>{1}).empty());
  assert((cp::convolution(std::vector<int>{1, 2}, std::vector<int>{3, 4}) ==
          std::vector<int>{3, 10, 8}));
  assert(
      (cp::convolution<17>(std::vector<int>{-1, 2}, std::vector<int>{3, 4}) ==
       std::vector<int>{14, 2, 8}));
  assert((cp::convolution(std::vector<M>{1, 2}, std::vector<M>{3, 4}) ==
          std::vector<M>{3, 10, 8}));
  assert(cp::convolution_ll({}, {1}).empty());
  assert((cp::convolution_ll({LLONG_MIN, LLONG_MAX}, {1}) ==
          std::vector<long long>{LLONG_MIN, LLONG_MAX}));
  assert((cp::convolution_ll({-1, 2}, {3, -4}) ==
          std::vector<long long>{-3, 10, -8}));
  std::vector<M> a(4096, 1), b(4096, 1);
  auto c = cp::convolution(a, b);
  for (int i = 0; i < 8191; i++)
    assert(c[i].val() == (i < 4096 ? i + 1 : 8191 - i));
}
