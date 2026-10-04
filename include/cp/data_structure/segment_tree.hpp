#pragma once

#include <bit>
#include <cassert>
#include <vector>

namespace cp {

// op must be associative and e() its two-sided identity. Operand order is preserved.
template <class S, auto op, auto e>
class SegmentTree {
public:
    explicit SegmentTree(int n = 0) : SegmentTree(initial_values(n)) {}
    explicit SegmentTree(const std::vector<S>& values) : n_(static_cast<int>(values.size())) {
        assert(values.size() <= 100000000);
        base_ = static_cast<int>(std::bit_ceil(static_cast<unsigned>(n_ > 0 ? n_ : 1)));
        data_.assign(2 * base_, e());
        for (int i = 0; i < n_; ++i) data_[base_ + i] = values[i];
        for (int node = base_ - 1; node > 0; --node) pull(node);
    }
    int size() const { return n_; }
    void set(int p, S value) {
        assert(0 <= p && p < n_);
        int node = base_ + p;
        data_[node] = value;
        while ((node >>= 1) > 0) pull(node);
    }
    S get(int p) const {
        assert(0 <= p && p < n_);
        return data_[base_ + p];
    }
    S prod(int l, int r) const {
        assert(0 <= l && l <= r && r <= n_);
        S left = e(), right = e();
        for (l += base_, r += base_; l < r; l >>= 1, r >>= 1) {
            if (l & 1) left = op(left, data_[l++]);
            if (r & 1) right = op(data_[--r], right);
        }
        return op(left, right);
    }
    S all_prod() const { return data_[1]; }
    template <auto predicate> int max_right(int l) const { return max_right(l, predicate); }
    template <class Predicate> int max_right(int l, Predicate predicate) const {
        assert(0 <= l && l <= n_);
        assert(predicate(e()));
        if (l == n_) return n_;
        S aggregate = e();
        int node = l + base_;
        do {
            while ((node & 1) == 0) node >>= 1;
            const S candidate = op(aggregate, data_[node]);
            if (!predicate(candidate)) {
                while (node < base_) {
                    node *= 2;
                    const S next = op(aggregate, data_[node]);
                    if (predicate(next)) { aggregate = next; ++node; }
                }
                return node - base_;
            }
            aggregate = candidate;
            ++node;
        } while ((node & -node) != node);
        return n_;
    }
    template <auto predicate> int min_left(int r) const { return min_left(r, predicate); }
    template <class Predicate> int min_left(int r, Predicate predicate) const {
        assert(0 <= r && r <= n_);
        assert(predicate(e()));
        if (r == 0) return 0;
        S aggregate = e();
        int node = r + base_;
        do {
            --node;
            while (node > 1 && (node & 1)) node >>= 1;
            const S candidate = op(data_[node], aggregate);
            if (!predicate(candidate)) {
                while (node < base_) {
                    node = 2 * node + 1;
                    const S next = op(data_[node], aggregate);
                    if (predicate(next)) { aggregate = next; --node; }
                }
                return node + 1 - base_;
            }
            aggregate = candidate;
        } while ((node & -node) != node);
        return 0;
    }
private:
    static std::vector<S> initial_values(int n) {
        assert(0 <= n && n <= 100000000);
        return std::vector<S>(n, e());
    }
    void pull(int node) { data_[node] = op(data_[2 * node], data_[2 * node + 1]); }
    int n_, base_;
    std::vector<S> data_;
};

template <class S, auto op, auto e> using segtree = SegmentTree<S, op, e>;
}  // namespace cp
