#pragma once

#include <cassert>
#include <cstddef>
#include <limits>
#include <vector>

namespace cp {

// Path-copying monoid segment tree. Versions remain immutable and may branch.
template <class S, auto op, auto e>
class PersistentSegmentTree {
public:
    using Version = std::size_t;
    explicit PersistentSegmentTree(int n = 0) : n_(n) {
        assert(n >= 0);
        nodes_.push_back({e(), 0, 0}); // Shared identity subtree, at any depth.
        roots_.push_back(0);
    }
    explicit PersistentSegmentTree(const std::vector<S>& values)
        : PersistentSegmentTree(checked_size(values.size())) {
        if (n_ > 0) roots_[0] = build(values, 0, n_);
    }
    int size() const { return n_; }
    Version initial_version() const { return 0; }
    std::size_t versions() const { return roots_.size(); }
    std::size_t node_count() const { return nodes_.size(); }
    Version set(Version version, int p, S value) {
        check_version(version);
        assert(0 <= p && p < n_);
        const auto root = update(roots_[version], 0, n_, p, value);
        roots_.push_back(root);
        return roots_.size() - 1;
    }
    S get(Version version, int p) const {
        assert(0 <= p && p < n_);
        return prod(version, p, p + 1);
    }
    S prod(Version version, int l, int r) const {
        check_version(version);
        assert(0 <= l && l <= r && r <= n_);
        if (l == r) return e();
        return query(roots_[version], 0, n_, l, r);
    }
    S all_prod(Version version) const {
        check_version(version);
        return nodes_[roots_[version]].value;
    }
private:
    struct Node { S value; std::size_t left, right; };
    static int checked_size(std::size_t n) {
        assert(n <= static_cast<std::size_t>(std::numeric_limits<int>::max()));
        return static_cast<int>(n);
    }
    void check_version(Version version) const { assert(version < roots_.size()); }
    std::size_t append(S value, std::size_t left = 0, std::size_t right = 0) {
        nodes_.push_back({value, left, right});
        return nodes_.size() - 1;
    }
    std::size_t build(const std::vector<S>& values, int l, int r) {
        if (r - l == 1) return append(values[l]);
        const int mid = l + (r - l) / 2;
        const auto left = build(values, l, mid), right = build(values, mid, r);
        return append(op(nodes_[left].value, nodes_[right].value), left, right);
    }
    std::size_t update(std::size_t root, int l, int r, int p, const S& value) {
        if (r - l == 1) return append(value);
        const int mid = l + (r - l) / 2;
        auto left = nodes_[root].left, right = nodes_[root].right;
        if (p < mid) left = update(left, l, mid, p, value);
        else right = update(right, mid, r, p, value);
        return append(op(nodes_[left].value, nodes_[right].value), left, right);
    }
    S query(std::size_t root, int l, int r, int ql, int qr) const {
        if (root == 0 || qr <= l || r <= ql) return e();
        if (ql <= l && r <= qr) return nodes_[root].value;
        const int mid = l + (r - l) / 2;
        return op(query(nodes_[root].left, l, mid, ql, qr),
                  query(nodes_[root].right, mid, r, ql, qr));
    }
    int n_;
    std::vector<Node> nodes_;
    std::vector<std::size_t> roots_;
};

}  // namespace cp
