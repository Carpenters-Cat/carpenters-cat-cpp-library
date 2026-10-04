#pragma once

#include <cassert>
#include <optional>
#include <utility>
#include <vector>

namespace cp {

// Additive potentials: merge(a, b, d) constrains potential[b] - potential[a] = d.
// T must be an additive commutative group with exact equality and zero T{}.
template <class T = long long>
class PotentialUnionFind {
public:
    enum class MergeResult { merged, already_consistent, contradiction };

    explicit PotentialUnionFind(int n = 0) : components_(n) {
        assert(n >= 0);
        parent_or_size_.assign(static_cast<std::size_t>(n), -1);
        difference_to_parent_.resize(n);
    }

    int leader(int vertex) {
        assert(0 <= vertex && vertex < static_cast<int>(parent_or_size_.size()));
        int root = vertex;
        T total{};
        while (parent_or_size_[root] >= 0) {
            total = total + difference_to_parent_[root];
            root = parent_or_size_[root];
        }
        while (vertex != root) {
            const int next = parent_or_size_[vertex];
            const T edge = difference_to_parent_[vertex];
            parent_or_size_[vertex] = root;
            difference_to_parent_[vertex] = total;
            total = total - edge;
            vertex = next;
        }
        return root;
    }

    // Potential relative to the current component's leader; the origin may change.
    T potential(int vertex) {
        leader(vertex);
        return difference_to_parent_[vertex];
    }

    MergeResult merge(int a, int b, T difference) {
        int root_a = leader(a), root_b = leader(b);
        if (root_a == root_b) {
            return difference_to_parent_[b] - difference_to_parent_[a] == difference
                ? MergeResult::already_consistent : MergeResult::contradiction;
        }
        T offset = difference + difference_to_parent_[a] - difference_to_parent_[b];
        if (-parent_or_size_[root_a] < -parent_or_size_[root_b]) {
            std::swap(root_a, root_b);
            offset = -offset;
        }
        parent_or_size_[root_a] += parent_or_size_[root_b];
        parent_or_size_[root_b] = root_a;
        difference_to_parent_[root_b] = offset;
        --components_;
        return MergeResult::merged;
    }

    bool same(int a, int b) { return leader(a) == leader(b); }
    int size(int vertex) { return -parent_or_size_[leader(vertex)]; }
    int components() const { return components_; }

    std::optional<T> difference(int a, int b) {
        if (!same(a, b)) return std::nullopt;
        return difference_to_parent_[b] - difference_to_parent_[a];
    }

private:
    std::vector<int> parent_or_size_;
    std::vector<T> difference_to_parent_;
    int components_;
};

}  // namespace cp
