#pragma once

#include <cassert>
#include <cstddef>
#include <utility>
#include <vector>

namespace cp {

// Union by size without path compression. Only successful merges enter history.
class RollbackUnionFind {
public:
    using Snapshot = std::size_t;

    explicit RollbackUnionFind(int n = 0) : components_(n) {
        assert(n >= 0);
        parent_or_size_.assign(static_cast<std::size_t>(n), -1);
        history_.reserve(n > 0 ? static_cast<std::size_t>(n - 1) : 0);
    }

    int leader(int vertex) const {
        assert(0 <= vertex && vertex < static_cast<int>(parent_or_size_.size()));
        while (parent_or_size_[vertex] >= 0) vertex = parent_or_size_[vertex];
        return vertex;
    }

    bool merge(int a, int b) {
        a = leader(a); b = leader(b);
        if (a == b) return false;
        if (-parent_or_size_[a] < -parent_or_size_[b]) std::swap(a, b);
        history_.push_back({a, b, parent_or_size_[a], parent_or_size_[b]});
        parent_or_size_[a] += parent_or_size_[b];
        parent_or_size_[b] = a;
        --components_;
        return true;
    }

    bool same(int a, int b) const { return leader(a) == leader(b); }
    int size(int vertex) const { return -parent_or_size_[leader(vertex)]; }
    int components() const { return components_; }
    Snapshot snapshot() const { return history_.size(); }

    // Snapshot must still refer to a prefix of the current history.
    void rollback(Snapshot target) {
        assert(target <= history_.size());
        while (history_.size() > target) {
            const Change change = history_.back();
            history_.pop_back();
            parent_or_size_[change.parent] = change.parent_size;
            parent_or_size_[change.child] = change.child_size;
            ++components_;
        }
    }

private:
    struct Change { int parent, child, parent_size, child_size; };
    std::vector<int> parent_or_size_;
    std::vector<Change> history_;
    int components_;
};

}  // namespace cp
