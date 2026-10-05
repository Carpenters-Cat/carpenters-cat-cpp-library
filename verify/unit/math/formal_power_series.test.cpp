// competitive-verifier: STANDALONE
#include <cassert>
#include <cp/math/formal_power_series.hpp>
#include <cstdint>
#include <limits>
int main() {
    using M = cp::modint998244353;
    using F = cp::FPS<M>;
    F empty;
    assert(empty.degree() == -1 && empty.derivative().empty());
    assert(empty.integral() == F{0});
    assert(empty.truncated(3) == F(3));
    assert(empty.inv(0).empty() && empty.log(0).empty() && empty.exp(0).empty());
    assert(empty.divide(empty, 0).empty());
    assert(empty.pow(0, 0).empty());
    assert(empty.pow(0, 3) == F({1, 0, 0}) && empty.pow(1, 3) == F(3));
    assert(empty.exp(3) == F({1, 0, 0}));
    assert(empty.sqrt(3).value() == F(3));
    assert(!F({0, 1}).sqrt(3));
    M nonresidue = 2;
    while (nonresidue.pow((M::mod() - 1) / 2) == M(1))
        ++nonresidue;
    assert(!F{nonresidue}.sqrt(3));
    assert(F({0, 0, 4}).sqrt(5).value() == F({0, 2, 0, 0, 0}));
    assert((F({1, 2}) + F({3, 4, 5})) == F({4, 6, 5}));
    assert((F({1, 2}) - F({1, 2})) == F({0, 0}));
    assert((M(3) - F({1, 2})) == F({2, -2}));
    assert((F({1, 2}) * M(2) / M(2)) == F({1, 2}));
    assert((F({1, 2}) / F({1, 1})) == F({1, 1}));
    assert(F({3, 2, 0}).derivative() == F({2, 0}));
    assert(F({2, 4}).integral() == F({0, 2, 2}));
    auto [q, r] = F({1, 3, 2, 0}).divmod(F({1, 1, 0}));
    assert(q == F({1, 2}) && r.empty());
    auto [cq, cr] = F({1, 2}).divmod(F{2});
    assert(cq == F({M(1) / 2, 1}) && cr.empty());
    auto [zq, zr] = empty.divmod(F{1});
    assert(zq.empty() && zr.empty());
    assert(F({0, 1}).pow(std::numeric_limits<std::uint64_t>::max(), 5) == F(5));
    assert(F{M(-1)}.pow(std::numeric_limits<std::uint64_t>::max(), 1) == F{M(-1)});
    assert(F({1, 2, 3}).taylor_shift(2) == F({17, 14, 3}));
    assert(cp::multipoint_evaluate(F({1, 2}), std::vector<M>{0, 0, 1}) ==
           std::vector<M>({1, 1, 3}));
    assert(cp::multipoint_evaluate(empty, std::vector<M>{}).empty());
    assert(cp::interpolate(std::vector<M>{}, std::vector<M>{}).empty());
    assert(cp::interpolate(std::vector<M>{5}, std::vector<M>{7}) == F{7});
    assert(cp::interpolate(std::vector<M>{0, 1, 2}, std::vector<M>{1, 6, 17}) == F({1, 2, 3}));
}
