#ifndef CP_MATH_LINEAR_RECURRENCE_HPP
#define CP_MATH_LINEAR_RECURRENCE_HPP
#include <cassert>
#include <cp/math/formal_power_series.hpp>
#include <cstdint>
#include <vector>

namespace cp {
// Returns c with s[i] = sum(c[j] * s[i-1-j]) for i >= c.size().
// This reconstructs a shortest recurrence consistent with the finite prefix.
template <class Mint> std::vector<Mint> berlekamp_massey(const std::vector<Mint> &sequence) {
    std::vector<Mint> current{Mint(1)}, previous{Mint(1)};
    int length = 0, shift = 1;
    Mint last_discrepancy = 1;
    for (int n = 0; n < static_cast<int>(sequence.size()); ++n) {
        Mint discrepancy = sequence[n];
        for (int i = 1; i <= length; ++i)
            discrepancy += current[i] * sequence[n - i];
        if (discrepancy == Mint(0)) {
            ++shift;
            continue;
        }
        auto old = current;
        Mint factor = discrepancy / last_discrepancy;
        current.resize(std::max(current.size(), previous.size() + shift));
        for (int i = 0; i < static_cast<int>(previous.size()); ++i)
            current[i + shift] -= factor * previous[i];
        if (2 * length <= n) {
            length = n + 1 - length;
            previous = std::move(old);
            last_discrepancy = discrepancy;
            shift = 1;
        } else
            ++shift;
    }
    current.resize(length + 1);
    std::vector<Mint> coefficients(length);
    for (int i = 0; i < length; ++i)
        coefficients[i] = -current[i + 1];
    return coefficients;
}
// Coefficient [x^k] of numerator / denominator; denominator[0] must be nonzero.
template <class Mint>
Mint bostan_mori(FormalPowerSeries<Mint> numerator, FormalPowerSeries<Mint> denominator,
                 std::uint64_t k) {
    assert(denominator.coefficient(0) != Mint(0));
    numerator.trim();
    denominator.trim();
    while (k && !numerator.empty()) {
        auto negative = denominator;
        for (std::size_t i = 1; i < negative.size(); i += 2)
            negative[i] = -negative[i];
        auto p = numerator * negative, q = denominator * negative;
        numerator.clear();
        denominator.clear();
        for (std::size_t i = k & 1; i < p.size(); i += 2)
            numerator.push_back(p[i]);
        for (std::size_t i = 0; i < q.size(); i += 2)
            denominator.push_back(q[i]);
        numerator.trim();
        denominator.trim();
        k >>= 1;
    }
    return numerator.coefficient(0) / denominator[0];
}
template <class Mint>
Mint linear_recurrence_nth(const std::vector<Mint> &initial, const std::vector<Mint> &coefficients,
                           std::uint64_t k) {
    assert(initial.size() == coefficients.size());
    if (k < initial.size())
        return initial[static_cast<std::size_t>(k)];
    if (initial.empty())
        return Mint(0);
    FormalPowerSeries<Mint> denominator(coefficients.size() + 1);
    denominator[0] = 1;
    for (std::size_t i = 0; i < coefficients.size(); ++i)
        denominator[i + 1] = -coefficients[i];
    auto numerator = (FormalPowerSeries<Mint>(initial) * denominator)
                         .truncated(static_cast<int>(initial.size()));
    return bostan_mori(std::move(numerator), std::move(denominator), k);
}
// Future values are valid when the infinite sequence has order <= prefix.size()/2.
template <class Mint> Mint bmbm(const std::vector<Mint> &prefix, std::uint64_t k) {
    if (k < prefix.size())
        return prefix[static_cast<std::size_t>(k)];
    auto coefficients = berlekamp_massey(prefix);
    std::vector<Mint> initial(prefix.begin(), prefix.begin() + coefficients.size());
    return linear_recurrence_nth(initial, coefficients, k);
}
} // namespace cp
#endif
