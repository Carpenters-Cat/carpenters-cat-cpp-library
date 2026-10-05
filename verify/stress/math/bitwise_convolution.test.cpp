#include <cp/math/bitwise_convolution.hpp>
#include <cp/math/modint.hpp>

#include <cassert>
#include <random>
#include <vector>

template <class T>
void check(std::vector<T> a, std::vector<T> b) {
    const auto longest = std::max(a.size(), b.size());
    const auto n = longest == 0 ? 0 : std::bit_ceil(longest);
    std::vector<T> both(n), either(n), different(n);
    for (std::size_t i = 0; i < a.size(); ++i) {
        for (std::size_t j = 0; j < b.size(); ++j) {
            both[i & j] += a[i] * b[j];
            either[i | j] += a[i] * b[j];
            different[i ^ j] += a[i] * b[j];
        }
    }
    assert(cp::and_convolution(a, b) == both);
    assert(cp::or_convolution(a, b) == either);
    assert(cp::xor_convolution(a, b) == different);
    if (!a.empty()) {
        a.resize(std::bit_ceil(a.size()));
        const auto original = a;
        cp::walsh_hadamard(a); cp::walsh_hadamard(a, true);
        assert(a == original);
    }
}

int main() {
    std::mt19937_64 rng(0xB17B17);
    for (int trial = 0; trial < 1000; ++trial) {
        std::vector<long long> a(rng() % 33), b(rng() % 33);
        for (auto& x : a) x = static_cast<long long>(rng() % 21) - 10;
        for (auto& x : b) x = static_cast<long long>(rng() % 21) - 10;
        check(a, b);
        using Mint = cp::modint998244353;
        std::vector<Mint> ma(a.size()), mb(b.size());
        for (auto& x : ma) x = rng() % Mint::mod();
        for (auto& x : mb) x = rng() % Mint::mod();
        check(ma, mb);
    }
}
