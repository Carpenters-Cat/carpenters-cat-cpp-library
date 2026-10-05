// competitive-verifier: STANDALONE
#include <cassert>
#include <cp/math/factorization.hpp>
#include <limits>
int main() {
    cp::PollardRho rho(123);
    assert(rho.factorize(1).empty());
    bool rejected = false;
    try {
        rho.factorize(0);
    } catch (const std::invalid_argument &) {
        rejected = true;
    }
    assert(rejected);
    assert((rho.factor_powers(72) == std::vector<std::pair<std::uint64_t, int>>{{2, 3}, {3, 2}}));
    assert(rho.factorize(18446744073709551557ULL) ==
           std::vector<std::uint64_t>{18446744073709551557ULL});
    auto power = rho.factor_powers(1ULL << 63);
    assert(power.size() == 1 && power[0].first == 2 && power[0].second == 63);
    constexpr std::uint64_t p = 4294967291ULL, q = 4294967279ULL;
    assert((rho.factorize(p * p) == std::vector<std::uint64_t>{p, p}));
    assert((rho.factorize(p * q) == std::vector<std::uint64_t>{q, p}));
    rho.reseed(123);
    assert((rho.factorize(p * q) == std::vector<std::uint64_t>{q, p}));
    assert((cp::factorize_u64(std::numeric_limits<std::uint64_t>::max()) ==
            std::vector<std::uint64_t>{3, 5, 17, 257, 641, 65537, 6700417}));
}
