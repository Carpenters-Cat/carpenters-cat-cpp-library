#pragma once

#include <bit>
#include <cassert>
#include <vector>

namespace cp {

// composition(f, g) applies g first, then f. mapping must distribute over op.
template <class S, auto op, auto e, class F, auto mapping, auto composition, auto id>
class LazySegmentTree {
public:
    explicit LazySegmentTree(int n = 0) : LazySegmentTree(initial_values(n)) {}
    explicit LazySegmentTree(const std::vector<S>& values) : n_(static_cast<int>(values.size())) {
        assert(values.size() <= 100000000);
        base_ = static_cast<int>(std::bit_ceil(static_cast<unsigned>(n_ > 0 ? n_ : 1)));
        height_ = std::countr_zero(static_cast<unsigned>(base_));
        data_.assign(2 * base_, e());
        lazy_.assign(base_, id());
        for (int i = 0; i < n_; ++i) data_[base_ + i] = values[i];
        for (int node = base_ - 1; node > 0; --node) pull(node);
    }
    int size() const { return n_; }
    void set(int p, S value) {
        assert(0 <= p && p < n_);
        const int node = p + base_;
        expose(node);
        data_[node] = value;
        rebuild(node);
    }
    S get(int p) {
        assert(0 <= p && p < n_);
        const int node = p + base_;
        expose(node);
        return data_[node];
    }
    S prod(int l, int r) {
        assert(0 <= l && l <= r && r <= n_);
        if (l == r) return e();
        l += base_; r += base_;
        expose_boundaries(l, r);
        S left = e(), right = e();
        while (l < r) {
            if (l & 1) left = op(left, data_[l++]);
            if (r & 1) right = op(data_[--r], right);
            l >>= 1; r >>= 1;
        }
        return op(left, right);
    }
    S all_prod() const { return data_[1]; }
    void apply(int p, F update) {
        assert(0 <= p && p < n_);
        const int node = p + base_;
        expose(node);
        apply_node(node, update);
        rebuild(node);
    }
    void apply(int l, int r, F update) {
        assert(0 <= l && l <= r && r <= n_);
        if (l == r) return;
        l += base_; r += base_;
        expose_boundaries(l, r);
        const int original_l = l, original_r = r;
        while (l < r) {
            if (l & 1) apply_node(l++, update);
            if (r & 1) apply_node(--r, update);
            l >>= 1; r >>= 1;
        }
        for (int level = 1; level <= height_; ++level) {
            if ((original_l & ((1 << level) - 1)) != 0) pull(original_l >> level);
            if ((original_r & ((1 << level) - 1)) != 0) pull((original_r - 1) >> level);
        }
    }
    template <auto predicate> int max_right(int l) { return max_right(l, predicate); }
    template <class Predicate> int max_right(int l, Predicate predicate) {
        assert(0 <= l && l <= n_);
        assert(predicate(e()));
        if (l == n_) return n_;
        int node = l + base_;
        expose(node);
        S aggregate = e();
        do {
            while ((node & 1) == 0) node >>= 1;
            const S candidate = op(aggregate, data_[node]);
            if (!predicate(candidate)) {
                while (node < base_) {
                    push(node);
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
    template <auto predicate> int min_left(int r) { return min_left(r, predicate); }
    template <class Predicate> int min_left(int r, Predicate predicate) {
        assert(0 <= r && r <= n_);
        assert(predicate(e()));
        if (r == 0) return 0;
        int node = r + base_;
        expose(node - 1);
        S aggregate = e();
        do {
            --node;
            while (node > 1 && (node & 1)) node >>= 1;
            const S candidate = op(data_[node], aggregate);
            if (!predicate(candidate)) {
                while (node < base_) {
                    push(node);
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
    void apply_node(int node, F update) {
        data_[node] = mapping(update, data_[node]);
        if (node < base_) lazy_[node] = composition(update, lazy_[node]);
    }
    void push(int node) {
        apply_node(2 * node, lazy_[node]);
        apply_node(2 * node + 1, lazy_[node]);
        lazy_[node] = id();
    }
    void expose(int node) { for (int level = height_; level > 0; --level) push(node >> level); }
    void rebuild(int node) { while ((node >>= 1) > 0) pull(node); }
    void expose_boundaries(int l, int r) {
        for (int level = height_; level > 0; --level) {
            if ((l & ((1 << level) - 1)) != 0) push(l >> level);
            if ((r & ((1 << level) - 1)) != 0) push((r - 1) >> level);
        }
    }
    int n_, base_, height_;
    std::vector<S> data_;
    std::vector<F> lazy_;
};

template <class S, auto op, auto e, class F, auto mapping, auto composition, auto id>
using lazy_segtree = LazySegmentTree<S, op, e, F, mapping, composition, id>;
}  // namespace cp
