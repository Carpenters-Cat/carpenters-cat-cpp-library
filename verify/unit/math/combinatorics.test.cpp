// competitive-verifier: STANDALONE
#include <cassert>
#include <cp/math/combinatorics.hpp>
int main() {
    using M = cp::modint998244353;
    cp::Combinatorics<M> c;
    assert(c.size() == 0 && c.factorial(0) == M(1));
    assert(c.nCr(0, 0) == M(1) && c.nPr(0, 0) == M(1));
    assert(c.nCr(0, 1) == M(0) && c.nCr(2, -1) == M(0));
    assert(c.nCr(5, 2) == M(10) && c.nPr(5, 2) == M(20));
    assert(c.nCr(5, 0) == M(1) && c.nCr(5, 5) == M(1));
    c.ensure(100);
    assert(c.factorial(5) == M(120));
    assert(c.factorial(100) * c.inverse_factorial(100) == M(1));
    cp::Combinatorics<cp::StaticModint<2>> small(1);
    assert(small.nCr(1, 1).val() == 1);
    using D = cp::DynamicModint<11>;
    D::set_mod(7);
    cp::Combinatorics<D> dynamic(6);
    assert(dynamic.nCr(6, 2).val() == 1);
}
