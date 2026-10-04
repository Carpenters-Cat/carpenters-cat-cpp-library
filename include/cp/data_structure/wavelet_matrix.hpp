#pragma once

#include <cp/data_structure/coordinate_compression.hpp>

#include <bit>
#include <cassert>
#include <cstdint>
#include <limits>
#include <optional>
#include <utility>
#include <vector>

namespace cp {

// Static ordered values, coordinate-compressed without arithmetic on T.
template <class T>
class WaveletMatrix {
    struct Level {
        std::vector<std::uint64_t> bits;
        std::vector<int> ones_prefix;
        int zeros;
        int zeros_before(int p) const {
            const int block = p / 64, offset = p % 64;
            int ones = ones_prefix[block];
            if (offset != 0) ones += std::popcount(bits[block] & ((std::uint64_t{1} << offset) - 1));
            return p - ones;
        }
    };
public:
    explicit WaveletMatrix(const std::vector<T>& values = {}) : coordinates_(values) {
        assert(values.size() <= static_cast<std::size_t>(std::numeric_limits<int>::max()));
        n_ = static_cast<int>(values.size());
        const int height = coordinates_.empty() ? 0 : std::bit_width(static_cast<unsigned>(coordinates_.size() - 1));
        std::vector<int> ranks(n_), next(n_);
        for (int i = 0; i < n_; ++i) ranks[i] = static_cast<int>(*coordinates_.index(values[i]));
        for (int bit = height - 1; bit >= 0; --bit) {
            Level level;
            level.bits.assign((static_cast<std::size_t>(n_) + 63) / 64, 0);
            level.zeros = 0;
            for (int i = 0; i < n_; ++i) {
                if ((ranks[i] >> bit) & 1) level.bits[i / 64] |= std::uint64_t{1} << (i % 64);
                else ++level.zeros;
            }
            level.ones_prefix.assign(level.bits.size() + 1, 0);
            for (std::size_t i = 0; i < level.bits.size(); ++i)
                level.ones_prefix[i + 1] = level.ones_prefix[i] + std::popcount(level.bits[i]);
            int zero = 0, one = level.zeros;
            for (const int rank : ranks) {
                if ((rank >> bit) & 1) next[one++] = rank;
                else next[zero++] = rank;
            }
            ranks.swap(next);
            levels_.push_back(std::move(level));
        }
    }
    int size() const { return n_; }
    std::optional<T> kth_smallest(int l, int r, int k) const {
        check_interval(l, r);
        if (k < 0 || k >= r - l) return std::nullopt;
        int rank = 0;
        for (const auto& level : levels_) {
            const int zl = level.zeros_before(l), zr = level.zeros_before(r);
            rank <<= 1;
            if (k < zr - zl) { l = zl; r = zr; }
            else {
                k -= zr - zl;
                rank |= 1;
                l = level.zeros + l - zl; r = level.zeros + r - zr;
            }
        }
        return coordinates_.values()[rank];
    }
    int frequency(int l, int r, const T& value) const {
        check_interval(l, r);
        const auto found = coordinates_.index(value);
        if (!found) return 0;
        const int rank = static_cast<int>(*found);
        int bit = static_cast<int>(levels_.size());
        for (const auto& level : levels_) {
            --bit;
            const int zl = level.zeros_before(l), zr = level.zeros_before(r);
            if ((rank >> bit) & 1) { l = level.zeros + l - zl; r = level.zeros + r - zr; }
            else { l = zl; r = zr; }
        }
        return r - l;
    }
    int count_less(int l, int r, const T& value) const {
        check_interval(l, r);
        return count_less_rank(l, r, coordinates_.lower_bound(value));
    }
    int range_frequency(int l, int r, const T& low, const T& high) const {
        check_interval(l, r);
        assert(!(high < low));
        return count_less_rank(l, r, coordinates_.lower_bound(high))
             - count_less_rank(l, r, coordinates_.lower_bound(low));
    }
private:
    void check_interval(int l, int r) const { assert(0 <= l && l <= r && r <= n_); }
    int count_less_rank(int l, int r, std::size_t bound) const {
        if (bound == 0) return 0;
        if (bound >= coordinates_.size()) return r - l;
        int result = 0, bit = static_cast<int>(levels_.size());
        for (const auto& level : levels_) {
            --bit;
            const int zl = level.zeros_before(l), zr = level.zeros_before(r);
            if ((bound >> bit) & 1) {
                result += zr - zl;
                l = level.zeros + l - zl; r = level.zeros + r - zr;
            } else { l = zl; r = zr; }
        }
        return result;
    }
    int n_;
    CoordinateCompression<T> coordinates_;
    std::vector<Level> levels_;
};

}  // namespace cp
