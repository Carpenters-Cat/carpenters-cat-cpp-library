#pragma once

#include <cp/data_structure/coordinate_compression.hpp>

#include <cassert>
#include <cstddef>
#include <limits>
#include <optional>
#include <type_traits>
#include <utility>
#include <vector>

namespace cp {

// Discrete registered integer x-coordinates. All a * Y(x) + b must be exact in Y.
template <class X = long long, class Y = long long, bool Minimize = true>
class LiChaoTree {
    static_assert(std::is_integral_v<X> && !std::is_same_v<X, bool>);
public:
    struct Line {
        Y slope, intercept;
        Y evaluate(X x) const { return slope * static_cast<Y>(x) + intercept; }
    };
    explicit LiChaoTree(std::vector<X> coordinates = {}) : coordinates_(std::move(coordinates)) {
        assert(coordinates_.size() <= (std::numeric_limits<std::size_t>::max() - 1) / 4);
        lines_.resize(4 * coordinates_.size() + 1);
    }
    std::size_t size() const { return coordinates_.size(); }
    const std::vector<X>& coordinates() const { return coordinates_.values(); }
    bool contains(X x) const { return coordinates_.contains(x); }
    void add_line(Line line) {
        if (size() != 0) insert(std::move(line), 1, 0, size());
    }
    void add_line(Y slope, Y intercept) { add_line({slope, intercept}); }
    // Segment endpoints need not be registered. The interval is [low, high).
    void add_segment(X low, X high, Line line) {
        assert(!(high < low));
        const auto l = coordinates_.lower_bound(low), r = coordinates_.lower_bound(high);
        if (l != r) insert_segment(line, 1, 0, size(), l, r);
    }
    void add_segment(X low, X high, Y slope, Y intercept) {
        add_segment(low, high, {slope, intercept});
    }
    // nullopt means no applicable line, or x is outside the registered domain.
    std::optional<Y> query(X x) const {
        const auto rank = coordinates_.index(x);
        if (!rank) return std::nullopt;
        std::optional<Y> result;
        std::size_t node = 1, l = 0, r = size();
        while (true) {
            if (lines_[node]) {
                const Y value = lines_[node]->evaluate(x);
                if (!result || better(value, *result)) result = value;
            }
            if (r - l == 1) break;
            const auto mid = l + (r - l) / 2;
            if (*rank < mid) { node *= 2; r = mid; }
            else { node = 2 * node + 1; l = mid; }
        }
        return result;
    }
private:
    static bool better(const Y& a, const Y& b) {
        if constexpr (Minimize) return a < b;
        else return b < a;
    }
    void insert(Line line, std::size_t node, std::size_t l, std::size_t r) {
        if (!lines_[node]) { lines_[node] = std::move(line); return; }
        Line& current = *lines_[node];
        if (line.slope == current.slope) {
            if (better(line.intercept, current.intercept)) current = std::move(line);
            return;
        }
        const auto mid = l + (r - l) / 2;
        const bool left_better = better(line.evaluate(coordinates_.value(l)), current.evaluate(coordinates_.value(l)));
        const bool mid_better = better(line.evaluate(coordinates_.value(mid)), current.evaluate(coordinates_.value(mid)));
        if (mid_better) std::swap(line, current);
        if (r - l == 1) return;
        if (left_better != mid_better) insert(std::move(line), 2 * node, l, mid);
        else insert(std::move(line), 2 * node + 1, mid, r);
    }
    void insert_segment(const Line& line, std::size_t node, std::size_t l, std::size_t r,
                        std::size_t ql, std::size_t qr) {
        if (qr <= l || r <= ql) return;
        if (ql <= l && r <= qr) { insert(line, node, l, r); return; }
        const auto mid = l + (r - l) / 2;
        insert_segment(line, 2 * node, l, mid, ql, qr);
        insert_segment(line, 2 * node + 1, mid, r, ql, qr);
    }
    CoordinateCompression<X> coordinates_;
    std::vector<std::optional<Line>> lines_;
};

}  // namespace cp
