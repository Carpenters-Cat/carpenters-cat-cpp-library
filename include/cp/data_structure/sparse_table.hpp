#pragma once

#include <bit>
#include <cassert>
#include <limits>
#include <optional>
#include <utility>
#include <vector>

namespace cp {

// Static associative, idempotent aggregation. Empty intervals return nullopt.
template <class S, auto op>
class SparseTable {
public:
    explicit SparseTable(std::vector<S> values = {}) {
        assert(values.size() <= static_cast<std::size_t>(std::numeric_limits<int>::max()));
        n_ = static_cast<int>(values.size());
        if (n_ == 0) return;
        table_.push_back(std::move(values));
        const int levels = std::bit_width(static_cast<unsigned>(n_));
        for (int level = 1; level < levels; ++level) {
            const int half = 1 << (level - 1);
            const int count = n_ - 2 * half + 1;
            std::vector<S> row;
            row.reserve(count);
            for (int i = 0; i < count; ++i) row.push_back(op(table_.back()[i], table_.back()[i + half]));
            table_.push_back(std::move(row));
        }
    }
    int size() const { return n_; }
    std::optional<S> prod(int l, int r) const {
        assert(0 <= l && l <= r && r <= n_);
        if (l == r) return std::nullopt;
        const int level = std::bit_width(static_cast<unsigned>(r - l)) - 1;
        return op(table_[level][l], table_[level][r - (1 << level)]);
    }
private:
    int n_ = 0;
    std::vector<std::vector<S>> table_;
};

}  // namespace cp
