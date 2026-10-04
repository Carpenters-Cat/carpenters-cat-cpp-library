#pragma once

#include <cp/math/subset_transform.hpp>

#include <algorithm>
#include <bit>
#include <cassert>
#include <cstddef>
#include <limits>
#include <utility>
#include <vector>

namespace cp {

// Inverse uses division by length. That scalar must be invertible (or exact).
template <class T>
void walsh_hadamard(std::vector<T>& values, bool inverse = false) {
    assert(std::has_single_bit(values.size()));
    for (std::size_t step = 1; step < values.size(); step *= 2) {
        for (std::size_t block = 0; block < values.size(); block += 2 * step) {
            for (std::size_t i = block; i < block + step; ++i) {
                const T a = values[i], b = values[i + step];
                values[i] = a + b;
                values[i + step] = a - b;
            }
        }
    }
    if (inverse) {
        const T divisor = T(values.size());
        for (auto& x : values) x /= divisor;
    }
}

namespace bitwise_convolution_detail {
template <int Operation, class T>
std::vector<T> convolution(std::vector<T> a, std::vector<T> b) {
    const auto longest = std::max(a.size(), b.size());
    if (longest == 0) return {};
    assert(longest <= (std::numeric_limits<std::size_t>::max() / 2 + 1));
    const auto n = std::bit_ceil(longest);
    a.resize(n); b.resize(n);
    if constexpr (Operation == 0) { superset_zeta(a); superset_zeta(b); }
    if constexpr (Operation == 1) { subset_zeta(a); subset_zeta(b); }
    if constexpr (Operation == 2) { walsh_hadamard(a); walsh_hadamard(b); }
    for (std::size_t i = 0; i < n; ++i) a[i] *= b[i];
    if constexpr (Operation == 0) superset_mobius(a);
    if constexpr (Operation == 1) subset_mobius(a);
    if constexpr (Operation == 2) walsh_hadamard(a, true);
    return a;
}
}  // namespace bitwise_convolution_detail

template <class T>
std::vector<T> and_convolution(std::vector<T> a, std::vector<T> b) {
    return bitwise_convolution_detail::convolution<0>(std::move(a), std::move(b));
}
template <class T>
std::vector<T> or_convolution(std::vector<T> a, std::vector<T> b) {
    return bitwise_convolution_detail::convolution<1>(std::move(a), std::move(b));
}
template <class T>
std::vector<T> xor_convolution(std::vector<T> a, std::vector<T> b) {
    return bitwise_convolution_detail::convolution<2>(std::move(a), std::move(b));
}
}  // namespace cp
