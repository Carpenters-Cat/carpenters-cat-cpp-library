#ifndef CP_MATH_SIEVE_HPP
#define CP_MATH_SIEVE_HPP
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>
namespace cp {
class PrimeSieve {
    int limit_;
    std::vector<int> smallest_, primes_;
    std::vector<bool> primality_;

  public:
    explicit PrimeSieve(int limit) : limit_(limit) {
        assert(limit >= 0);
        smallest_.resize(static_cast<std::size_t>(limit) + 1);
        primality_.resize(static_cast<std::size_t>(limit) + 1);
        if (limit >= 1)
            smallest_[1] = 1;
        for (std::uint64_t x = 2; x <= static_cast<std::uint64_t>(limit); ++x) {
            if (!smallest_[x]) {
                smallest_[x] = static_cast<int>(x);
                primes_.push_back(static_cast<int>(x));
                primality_[x] = true;
            }
            for (int p : primes_) {
                if (p > smallest_[x] ||
                    x * static_cast<std::uint64_t>(p) > static_cast<std::uint64_t>(limit))
                    break;
                smallest_[x * p] = p;
            }
        }
    }
    int limit() const { return limit_; }
    const std::vector<int> &primes() const { return primes_; }
    const std::vector<int> &smallest_factors() const { return smallest_; }
    const std::vector<bool> &prime_table() const { return primality_; }
    bool is_prime(int n) const {
        assert(0 <= n && n <= limit_);
        return primality_[n];
    }
    int smallest_prime_factor(int n) const {
        assert(0 <= n && n <= limit_);
        return smallest_[n];
    }
    std::vector<std::pair<int, int>> factorize(int n) const {
        assert(0 <= n && n <= limit_);
        if (!n)
            throw std::invalid_argument("zero has no prime factorization");
        std::vector<std::pair<int, int>> factors;
        while (n > 1) {
            int p = smallest_[n], e = 0;
            do {
                n /= p;
                ++e;
            } while (n > 1 && smallest_[n] == p);
            factors.emplace_back(p, e);
        }
        return factors;
    }
};
// Segmented odd-only enumeration avoids the SPF table's linear memory cost.
inline std::vector<int> enumerate_primes(int limit) {
    assert(limit >= 0);
    if (limit < 2)
        return {};
    int root = 0;
    while (static_cast<std::int64_t>(root + 1) * (root + 1) <= limit)
        ++root;
    PrimeSieve base(root);
    std::vector<int> primes{2};
    constexpr int block_size = 32768;
    std::vector<unsigned char> composite(block_size);
    for (std::int64_t low = 3; low <= limit; low += 2LL * block_size) {
        std::int64_t high = std::min<std::int64_t>(limit, low + 2LL * (block_size - 1));
        int count = static_cast<int>((high - low) / 2 + 1);
        std::fill(composite.begin(), composite.begin() + count, 0);
        for (int p : base.primes()) {
            if (p == 2)
                continue;
            if (1LL * p * p > high)
                break;
            std::int64_t start = std::max<std::int64_t>(1LL * p * p, (low + p - 1) / p * p);
            if (!(start & 1))
                start += p;
            for (std::int64_t x = start; x <= high; x += 2LL * p)
                composite[(x - low) / 2] = 1;
        }
        for (int i = 0; i < count; ++i)
            if (!composite[i])
                primes.push_back(static_cast<int>(low + 2LL * i));
    }
    return primes;
}
} // namespace cp
#endif
