#ifndef CP_MATH_FACTORIZATION_HPP
#define CP_MATH_FACTORIZATION_HPP
#include <algorithm>
#include <cp/math/primality.hpp>
#include <cstdint>
#include <numeric>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>
namespace cp {
class PollardRho {
    std::mt19937_64 random_;
    static constexpr std::uint64_t attempt_budget = 1ULL << 20;
    std::uint64_t divisor(std::uint64_t n) {
        if (n % 2 == 0)
            return 2;
        if (n % 3 == 0)
            return 3;
        for (;;) {
            std::uint64_t y = random_() % (n - 2) + 2, c = random_() % (n - 1) + 1;
            auto advance = [n, c](std::uint64_t x) {
                x = mul_mod_u64(x, x, n);
                return x >= n - c ? x - (n - c) : x + c;
            };
            std::uint64_t g = 1, r = 1, x = 0, saved = y, iterations = 0;
            while (g == 1 && iterations < attempt_budget) {
                x = y;
                for (std::uint64_t i = 0; i < r && iterations < attempt_budget; ++i) {
                    y = advance(y);
                    ++iterations;
                }
                for (std::uint64_t k = 0; k < r && g == 1 && iterations < attempt_budget;
                     k += 128) {
                    saved = y;
                    std::uint64_t product = 1;
                    for (std::uint64_t i = 0;
                         i < std::min<std::uint64_t>(128, r - k) && iterations < attempt_budget;
                         ++i) {
                        y = advance(y);
                        ++iterations;
                        product = mul_mod_u64(product, x > y ? x - y : y - x, n);
                    }
                    g = std::gcd(product, n);
                }
                r *= 2;
            }
            if (g == n) {
                g = 1;
                while (g == 1 && iterations < attempt_budget) {
                    saved = advance(saved);
                    ++iterations;
                    g = std::gcd(x > saved ? x - saved : saved - x, n);
                }
            }
            if (g > 1 && g < n)
                return g;
            // Degenerate cycles and bounded attempts restart with the next seeded parameters.
        }
    }
    void split(std::uint64_t n, std::vector<std::uint64_t> &factors) {
        if (n == 1)
            return;
        if (is_prime_u64(n)) {
            factors.push_back(n);
            return;
        }
        auto p = divisor(n);
        split(p, factors);
        split(n / p, factors);
    }

  public:
    static constexpr std::uint64_t default_seed = 0x9e3779b97f4a7c15ULL;
    explicit PollardRho(std::uint64_t seed = default_seed) : random_(seed) {}
    void reseed(std::uint64_t seed) { random_.seed(seed); }
    std::vector<std::uint64_t> factorize(std::uint64_t n) {
        if (!n)
            throw std::invalid_argument("zero has no prime factorization");
        std::vector<std::uint64_t> factors;
        split(n, factors);
        std::sort(factors.begin(), factors.end());
        return factors;
    }
    std::vector<std::pair<std::uint64_t, int>> factor_powers(std::uint64_t n) {
        auto flat = factorize(n);
        std::vector<std::pair<std::uint64_t, int>> result;
        for (auto p : flat) {
            if (result.empty() || result.back().first != p)
                result.emplace_back(p, 1);
            else
                ++result.back().second;
        }
        return result;
    }
};
inline std::vector<std::uint64_t> factorize_u64(std::uint64_t n,
                                                std::uint64_t seed = PollardRho::default_seed) {
    return PollardRho(seed).factorize(n);
}
} // namespace cp
#endif
