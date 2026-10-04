// competitive-verifier: STANDALONE
#include <cassert>
#include <climits>
#include <cp/math/modint.hpp>
#include <type_traits>

template <class M> void check() {
  assert(M().val() == 0);
  assert(M(-1).val() == M::mod() - 1);
  assert(M(LLONG_MIN).val() == (LLONG_MIN % M::mod() + M::mod()) % M::mod());
  assert(M(ULLONG_MAX).val() == ULLONG_MAX % M::mod());
  M x = 2;
  assert((x++).val() == 2);
  assert(x.val() == 3);
  assert((++x).val() == 4);
  assert((x--).val() == 4);
  assert((--x).val() == 2);
  assert((+x).val() == 2);
  assert((-x + x).val() == 0);
  assert((M(3) + M(4)).val() == 7 % M::mod());
  assert((M(3) - M(4)).val() == M::mod() - 1);
  assert((M(3) * M(4)).val() == 12 % M::mod());
  assert(M(0).pow(0).val() == 1 % M::mod());
  assert(M(2).pow(10).val() == 1024 % M::mod());
  assert(M::raw(0) == M(0));
  assert(M(0) != M(1));
}
int main() {
  check<cp::modint998244353>();
  check<cp::modint1000000007>();
  check<cp::StaticModint<12>>();
  using D = cp::DynamicModint<0>;
  D::set_mod(12);
  check<D>();
  assert((D(5) / D(5)).val() == 1);
  assert(D(5).inv().val() == 5);
  assert((cp::modint998244353(17) / 17).val() == 1);
  assert(cp::StaticModint<1>(LLONG_MIN).val() == 0);
  assert(cp::StaticModint<1>(0).pow(0).val() == 0);
  assert(cp::StaticModint<1>(0).inv().val() == 0);
  cp::DynamicModint<1>::set_mod(INT_MAX);
  assert((cp::DynamicModint<1>(INT_MAX - 1) * cp::DynamicModint<1>(INT_MAX - 1))
             .val() == 1);
  assert(D::mod() == 12);
  D::set_mod(1);
  assert(D(0).inv().val() == 0);
}
