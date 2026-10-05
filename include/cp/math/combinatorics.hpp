#ifndef CP_MATH_COMBINATORICS_HPP
#define CP_MATH_COMBINATORICS_HPP

#include <algorithm>
#include <cassert>
#include <cp/math/modint.hpp>
#include <vector>

namespace cp {
// Factorial tables over a prime field. A dynamic modulus must remain unchanged.
template <class Mint> class Combinatorics {
    int modulus_ = Mint::mod();
    std::vector<Mint> factorial_{Mint(1)}, inverse_factorial_{Mint(1)};
    void check_modulus() const { assert(Mint::mod() == modulus_); }

  public:
    explicit Combinatorics(int n = 0) {
        assert(acl_math_internal::is_prime_constexpr(modulus_));
        ensure(n);
    }
    int size() const {
        check_modulus();
        return static_cast<int>(factorial_.size()) - 1;
    }
    void ensure(int n) {
        check_modulus();
        assert(0 <= n && n < modulus_);
        int old = size();
        if (n <= old)
            return;
        int next = static_cast<int>(
            std::min<long long>(modulus_ - 1, std::max<long long>(n, std::max(1LL, 2LL * old))));
        factorial_.resize(next + 1);
        inverse_factorial_.resize(next + 1);
        for (int i = old + 1; i <= next; ++i)
            factorial_[i] = factorial_[i - 1] * i;
        inverse_factorial_[next] = factorial_[next].inv();
        for (int i = next; i > old; --i)
            inverse_factorial_[i - 1] = inverse_factorial_[i] * i;
    }
    Mint factorial(int n) {
        ensure(n);
        return factorial_[n];
    }
    Mint inverse_factorial(int n) {
        ensure(n);
        return inverse_factorial_[n];
    }
    Mint nCr(int n, int r) {
        check_modulus();
        assert(n >= 0);
        if (r < 0 || r > n)
            return Mint(0);
        ensure(n);
        return factorial_[n] * inverse_factorial_[r] * inverse_factorial_[n - r];
    }
    Mint nPr(int n, int r) {
        check_modulus();
        assert(n >= 0);
        if (r < 0 || r > n)
            return Mint(0);
        ensure(n);
        return factorial_[n] * inverse_factorial_[n - r];
    }
};
} // namespace cp
#endif
