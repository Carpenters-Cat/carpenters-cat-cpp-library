#pragma once

#include <bit>
#include <cassert>
#include <cstddef>
#include <vector>

namespace cp {
namespace subset_transform_detail {
template <bool Superset, bool Inverse, class T>
void transform(std::vector<T>& values) {
    assert(std::has_single_bit(values.size()));
    for (std::size_t bit = 1; bit < values.size(); bit *= 2) {
        for (std::size_t mask = 0; mask < values.size(); ++mask) {
            if (!(mask & bit)) continue;
            const auto destination = Superset ? mask ^ bit : mask;
            const auto source = Superset ? mask : mask ^ bit;
            if constexpr (Inverse) values[destination] -= values[source];
            else values[destination] += values[source];
        }
    }
}
}  // namespace subset_transform_detail

template <class T> void subset_zeta(std::vector<T>& values) {
    subset_transform_detail::transform<false, false>(values);
}
template <class T> void subset_mobius(std::vector<T>& values) {
    subset_transform_detail::transform<false, true>(values);
}
template <class T> void superset_zeta(std::vector<T>& values) {
    subset_transform_detail::transform<true, false>(values);
}
template <class T> void superset_mobius(std::vector<T>& values) {
    subset_transform_detail::transform<true, true>(values);
}
}  // namespace cp
