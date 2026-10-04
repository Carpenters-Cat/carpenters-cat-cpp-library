// competitive-verifier: STANDALONE
#include <cassert>
#include <cp/tree/rerooting.hpp>
#include <memory>
struct CountSum {
  long long count, sum;
};
int main() {
  auto combine = [](CountSum a, CountSum b) {
    return CountSum{a.count + b.count, a.sum + b.sum};
  };
  auto finish = [](CountSum a, int) { return CountSum{a.count + 1, a.sum}; };
  auto lift = [](CountSum a, int, int, std::size_t) {
    return CountSum{a.count, a.sum + a.count};
  };
  assert(
      cp::rerooting(cp::Tree{}, CountSum{0, 0}, combine, lift, finish).empty());
  cp::Tree one(1);
  auto single = cp::rerooting(one, CountSum{0, 0}, combine, lift, finish);
  assert(single[0].count == 1 && single[0].sum == 0);
  cp::Tree star(6);
  for (int i = 1; i < 6; ++i)
    star.add_edge(0, i);
  auto values = cp::rerooting(star, CountSum{0, 0}, combine, lift, finish);
  assert(values[0].sum == 5 && values[1].sum == 9);
  // Result and State can differ; Result need not be default-constructible or
  // copyable.
  auto pointers = cp::rerooting(
      star, 0, [](int a, int b) { return a + b; },
      [](const std::unique_ptr<int> &p, int, int, std::size_t) { return *p; },
      [](int a, int) { return std::make_unique<int>(a + 1); });
  for (const auto &p : pointers)
    assert(*p == 6);
  const int n = 200000;
  cp::Tree chain(n);
  for (int v = 1; v < n; ++v)
    chain.add_edge(v - 1, v);
  auto deep = cp::rerooting(chain, CountSum{0, 0}, combine, lift, finish);
  for (long long v = 0; v < n; ++v)
    assert(deep[v].count == n &&
           deep[v].sum == v * (v + 1) / 2 + (n - 1 - v) * (n - v) / 2);
}
