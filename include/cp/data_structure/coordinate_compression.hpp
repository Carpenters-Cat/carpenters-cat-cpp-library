#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <optional>
#include <type_traits>
#include <utility>
#include <vector>

namespace cp {

// Immutable ranks under T's strict weak ordering. Equivalent values share a rank.
template <class T>
class CoordinateCompression {
public:
    explicit CoordinateCompression(std::vector<T> values = {}) : values_(std::move(values)) {
        std::sort(values_.begin(), values_.end());
        values_.erase(std::unique(values_.begin(), values_.end(), equivalent), values_.end());
    }

    [[nodiscard]] std::size_t size() const { return values_.size(); }
    [[nodiscard]] bool empty() const { return values_.empty(); }
    [[nodiscard]] const std::vector<T>& values() const { return values_; }

    [[nodiscard]] std::size_t lower_bound(const T& value) const {
        return static_cast<std::size_t>(std::lower_bound(values_.begin(), values_.end(), value) - values_.begin());
    }
    [[nodiscard]] std::size_t upper_bound(const T& value) const {
        return static_cast<std::size_t>(std::upper_bound(values_.begin(), values_.end(), value) - values_.begin());
    }
    [[nodiscard]] std::optional<std::size_t> index(const T& value) const {
        const auto rank = lower_bound(value);
        if (rank == size() || !equivalent(values_[rank], value)) return std::nullopt;
        return rank;
    }
    [[nodiscard]] bool contains(const T& value) const { return index(value).has_value(); }
    [[nodiscard]] std::conditional_t<std::is_same_v<T, bool>, bool, const T&> value(std::size_t rank) const {
        assert(rank < size());
        return values_[rank];
    }

private:
    static bool equivalent(const T& a, const T& b) { return !(a < b) && !(b < a); }
    std::vector<T> values_;
};

}  // namespace cp
