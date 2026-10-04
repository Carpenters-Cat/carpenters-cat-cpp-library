// competitive-verifier: STANDALONE
#include <cassert>
#include <cp/math/linear_recurrence.hpp>
#include <limits>
int main() {
    using M = cp::modint998244353;
    using F = cp::FPS<M>;
    std::vector<M> f{0, 1, 1, 2, 3, 5, 8, 13};
    assert(cp::berlekamp_massey(f) == std::vector<M>({1, 1}));
    assert(cp::berlekamp_massey(std::vector<M>{}).empty());
    assert(cp::berlekamp_massey(std::vector<M>(10)).empty());
    assert(cp::bmbm(f, 10) == M(55));
    assert(cp::bmbm(std::vector<M>{}, 100) == M(0));
    assert(cp::linear_recurrence_nth(std::vector<M>{}, std::vector<M>{}, 10) == M(0));
    assert(cp::bostan_mori(F{1}, F{1, -1}, std::numeric_limits<std::uint64_t>::max()) == M(1));
    assert(cp::bostan_mori(F{}, F{1}, 10) == M(0));
    assert(cp::bostan_mori(F{1, 2, 3}, F{1}, 2) == M(3));
    assert(cp::bostan_mori(F{1, 2, 3}, F{1}, 3) == M(0));
    assert(cp::bmbm(std::vector<M>{7, 0, 0, 0}, 100) == M(0));
}
