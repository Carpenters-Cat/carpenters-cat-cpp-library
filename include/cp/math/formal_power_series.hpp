#ifndef CP_MATH_FORMAL_POWER_SERIES_HPP
#define CP_MATH_FORMAL_POWER_SERIES_HPP

#include <algorithm>
#include <cassert>
#include <cp/math/combinatorics.hpp>
#include <cp/math/convolution.hpp>
#include <cstdint>
#include <optional>
#include <utility>
#include <vector>

namespace cp {
// Coefficients are in increasing degree order; empty represents zero.
// A static odd prime modulus with enough NTT roots is required.
template <class Mint> class FormalPowerSeries : public std::vector<Mint> {
    using Vector = std::vector<Mint>;
    using F = FormalPowerSeries;
    static_assert(acl_math_internal::is_static_modint<Mint>::value);
    static_assert(Mint::mod() > 2 && Mint::mod() <= 2000001000 &&
                  acl_math_internal::is_prime<Mint::mod()>);
    static Mint unsigned_power(Mint a, std::uint64_t n) {
        Mint r = 1;
        while (n) {
            if (n & 1)
                r *= a;
            a *= a;
            n >>= 1;
        }
        return r;
    }
    static std::optional<Mint> scalar_sqrt(Mint a) {
        if (a == Mint(0))
            return Mint(0);
        int p = Mint::mod();
        if (a.pow((p - 1) / 2) != Mint(1))
            return std::nullopt;
        if (p % 4 == 3)
            return a.pow((p + 1LL) / 4);
        int s = 0;
        long long q = p - 1;
        while (!(q & 1)) {
            q >>= 1;
            ++s;
        }
        Mint z = 2;
        while (z.pow((p - 1) / 2) == Mint(1))
            ++z;
        Mint c = z.pow(q), x = a.pow((q + 1) / 2), t = a.pow(q);
        while (t != Mint(1)) {
            int i = 1;
            Mint v = t * t;
            while (v != Mint(1)) {
                v *= v;
                ++i;
            }
            Mint b = c.pow(1LL << (s - i - 1));
            x *= b;
            c = b * b;
            t *= c;
            s = i;
        }
        return x.val() <= (-x).val() ? x : -x;
    }

  public:
    using Vector::Vector;
    FormalPowerSeries() = default;
    FormalPowerSeries(const Vector &v) : Vector(v) {}
    FormalPowerSeries(Vector &&v) : Vector(std::move(v)) {}
    Mint coefficient(int i) const {
        assert(i >= 0);
        return i < static_cast<int>(this->size()) ? (*this)[i] : Mint(0);
    }
    F truncated(int n) const {
        assert(n >= 0);
        F r(this->begin(), this->begin() + std::min<int>(n, this->size()));
        r.resize(n);
        return r;
    }
    void trim() {
        while (!this->empty() && this->back() == Mint(0))
            this->pop_back();
    }
    int degree() const {
        int n = static_cast<int>(this->size());
        while (n && (*this)[n - 1] == Mint(0))
            --n;
        return n - 1;
    }
    F &operator+=(const F &b) {
        this->resize(std::max(this->size(), b.size()));
        for (std::size_t i = 0; i < b.size(); ++i)
            (*this)[i] += b[i];
        return *this;
    }
    F &operator-=(const F &b) {
        this->resize(std::max(this->size(), b.size()));
        for (std::size_t i = 0; i < b.size(); ++i)
            (*this)[i] -= b[i];
        return *this;
    }
    F &operator+=(Mint b) {
        if (this->empty())
            this->resize(1);
        (*this)[0] += b;
        return *this;
    }
    F &operator-=(Mint b) { return *this += -b; }
    F &operator*=(Mint b) {
        for (auto &x : *this)
            x *= b;
        return *this;
    }
    F &operator/=(Mint b) { return *this *= b.inv(); }
    F &operator*=(const F &b) {
        *this =
            F(cp::convolution(static_cast<const Vector &>(*this), static_cast<const Vector &>(b)));
        return *this;
    }
    F &operator/=(const F &b) {
        *this = divide(b, static_cast<int>(this->size()));
        return *this;
    }
    F operator-() const {
        F r = *this;
        for (auto &x : r)
            x = -x;
        return r;
    }
    friend F operator+(F a, const F &b) { return a += b; }
    friend F operator-(F a, const F &b) { return a -= b; }
    friend F operator*(F a, const F &b) { return a *= b; }
    friend F operator/(F a, const F &b) { return a /= b; }
    friend F operator+(F a, Mint b) { return a += b; }
    friend F operator-(F a, Mint b) { return a -= b; }
    friend F operator*(F a, Mint b) { return a *= b; }
    friend F operator/(F a, Mint b) { return a /= b; }
    friend F operator+(Mint a, F b) { return b += a; }
    friend F operator-(Mint a, F b) { return -b += a; }
    friend F operator*(Mint a, F b) { return b *= a; }
    F derivative() const {
        if (this->empty())
            return {};
        F r(this->size() - 1);
        for (std::size_t i = 1; i < this->size(); ++i)
            r[i - 1] = (*this)[i] * Mint(i);
        return r;
    }
    F integral() const {
        assert(this->size() < static_cast<std::size_t>(Mint::mod()));
        F r(this->size() + 1);
        if (this->empty())
            return r;
        std::vector<Mint> inverse(this->size() + 1);
        inverse[1] = 1;
        for (std::size_t i = 2; i <= this->size(); ++i)
            inverse[i] = -Mint(Mint::mod() / i) * inverse[Mint::mod() % i];
        for (std::size_t i = 0; i < this->size(); ++i)
            r[i + 1] = (*this)[i] * inverse[i + 1];
        return r;
    }
    F inv(int n) const {
        assert(n >= 0);
        if (!n)
            return {};
        assert(coefficient(0) != Mint(0));
        F g{coefficient(0).inv()};
        for (int k = 1; k < n; k *= 2) {
            int d = std::min(2 * k, n);
            F correction = -(truncated(d) * g).truncated(d);
            correction[0] += Mint(2);
            g = (g * correction).truncated(d);
        }
        return g;
    }
    F divide(const F &denominator, int n) const {
        assert(n >= 0);
        if (!n)
            return {};
        return (truncated(n) * denominator.inv(n)).truncated(n);
    }
    F log(int n) const {
        assert(n >= 0 && n < Mint::mod());
        if (!n)
            return {};
        assert(coefficient(0) == Mint(1));
        return (truncated(n).derivative() * inv(n)).truncated(n - 1).integral();
    }
    F exp(int n) const {
        assert(n >= 0 && n < Mint::mod());
        if (!n)
            return {};
        assert(coefficient(0) == Mint(0));
        F g{Mint(1)};
        for (int k = 1; k < n; k *= 2) {
            int d = std::min(2 * k, n);
            F correction = truncated(d) - g.log(d);
            correction[0] += Mint(1);
            g = (g * correction).truncated(d);
        }
        return g;
    }
    F pow(std::uint64_t exponent, int n) const {
        assert(n >= 0 && n < Mint::mod());
        if (!n)
            return {};
        if (!exponent) {
            F r(n);
            r[0] = 1;
            return r;
        }
        int lead = 0;
        while (lead < std::min<int>(n, this->size()) && (*this)[lead] == Mint(0))
            ++lead;
        if (lead == std::min<int>(n, this->size()) ||
            (lead && exponent > static_cast<std::uint64_t>((n - 1) / lead)))
            return F(n);
        int shift = static_cast<int>(lead * exponent), d = n - shift;
        Mint c = (*this)[lead];
        F normalized(this->begin() + lead, this->end());
        normalized /= c;
        F g = (normalized.log(d) * Mint(exponent)).exp(d) * unsigned_power(c, exponent);
        F r(n);
        for (int i = 0; i < d; ++i)
            r[i + shift] = g[i];
        return r;
    }
    std::optional<F> sqrt(int n) const {
        assert(n >= 0 && n < Mint::mod());
        if (!n)
            return F{};
        int lead = 0;
        while (lead < n && coefficient(lead) == Mint(0))
            ++lead;
        if (lead == n)
            return F(n);
        if (lead & 1)
            return std::nullopt;
        auto c = scalar_sqrt(coefficient(lead));
        if (!c)
            return std::nullopt;
        int shift = lead / 2, d = n - shift;
        F f(this->begin() + lead, this->end()), g{*c};
        Mint half = Mint(2).inv();
        for (int k = 1; k < d; k *= 2) {
            int next = std::min(2 * k, d);
            g = (g.truncated(next) + f.divide(g, next)) * half;
        }
        F r(n);
        for (int i = 0; i < d; ++i)
            r[i + shift] = g[i];
        return r;
    }
    std::pair<F, F> divmod(F denominator) const {
        F numerator = *this;
        numerator.trim();
        denominator.trim();
        assert(!denominator.empty());
        if (numerator.size() < denominator.size())
            return {{}, numerator};
        int k = static_cast<int>(numerator.size() - denominator.size() + 1);
        F reversed_a(numerator.rbegin(), numerator.rend()),
            reversed_b(denominator.rbegin(), denominator.rend());
        F quotient = reversed_a.divide(reversed_b, k);
        std::reverse(quotient.begin(), quotient.end());
        F remainder = (numerator - quotient * denominator)
                          .truncated(static_cast<int>(denominator.size()) - 1);
        quotient.trim();
        remainder.trim();
        return {quotient, remainder};
    }
    Mint evaluate(Mint x) const {
        Mint r = 0;
        for (auto i = this->rbegin(); i != this->rend(); ++i)
            r = r * x + *i;
        return r;
    }
    F taylor_shift(Mint c) const {
        int n = static_cast<int>(this->size());
        if (!n)
            return {};
        assert(n <= Mint::mod());
        Combinatorics<Mint> table(n - 1);
        F a(n), b(n);
        Mint power = 1;
        for (int i = 0; i < n; ++i) {
            a[n - 1 - i] = (*this)[i] * table.factorial(i);
            b[i] = power * table.inverse_factorial(i);
            power *= c;
        }
        F product = a * b, result(n);
        for (int i = 0; i < n; ++i)
            result[i] = product[n - 1 - i] * table.inverse_factorial(i);
        return result;
    }
};

namespace fps_internal {
template <class Mint> struct ProductTree {
    using F = FormalPowerSeries<Mint>;
    int n, base = 1;
    std::vector<F> product;
    explicit ProductTree(const std::vector<Mint> &points) : n(static_cast<int>(points.size())) {
        while (base < n)
            base *= 2;
        product.resize(2 * base);
        for (int i = 0; i < base; ++i)
            product[base + i] = i < n ? F{-points[i], Mint(1)} : F{Mint(1)};
        for (int i = base - 1; i; --i)
            product[i] = product[2 * i] * product[2 * i + 1];
    }
    std::vector<Mint> evaluate(const F &f) const {
        if (!n)
            return {};
        std::vector<F> remainders(2 * base);
        remainders[1] = f.divmod(product[1]).second;
        for (int i = 1; i < base; ++i) {
            remainders[2 * i] = remainders[i].divmod(product[2 * i]).second;
            remainders[2 * i + 1] = remainders[i].divmod(product[2 * i + 1]).second;
            F{}.swap(remainders[i]);
        }
        std::vector<Mint> values(n);
        for (int i = 0; i < n; ++i)
            values[i] = remainders[base + i].coefficient(0);
        return values;
    }
};
} // namespace fps_internal

template <class Mint>
std::vector<Mint> multipoint_evaluate(const FormalPowerSeries<Mint> &f,
                                      const std::vector<Mint> &points) {
    return fps_internal::ProductTree<Mint>(points).evaluate(f);
}
template <class Mint>
FormalPowerSeries<Mint> interpolate(const std::vector<Mint> &points,
                                    const std::vector<Mint> &values) {
    assert(points.size() == values.size());
    if (points.empty())
        return {};
    using F = FormalPowerSeries<Mint>;
    fps_internal::ProductTree<Mint> tree(points);
    auto denominators = tree.evaluate(tree.product[1].derivative());
    std::vector<F> result(2 * tree.base);
    for (int i = 0; i < tree.base; ++i) {
        if (i < tree.n) {
            assert(denominators[i] != Mint(0));
            result[tree.base + i] = F{values[i] / denominators[i]};
        }
    }
    for (int i = tree.base - 1; i; --i)
        result[i] =
            result[2 * i] * tree.product[2 * i + 1] + result[2 * i + 1] * tree.product[2 * i];
    return result[1].truncated(tree.n);
}
template <class Mint> using FPS = FormalPowerSeries<Mint>;
} // namespace cp
#endif
