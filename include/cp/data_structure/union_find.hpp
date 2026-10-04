#pragma once

#include <cassert>
#include <utility>
#include <vector>

namespace cp {

// Path compression and union by size. Vertices must be in [0, n).
class UnionFind {
public:
    explicit UnionFind(int n = 0) : components_(n) {
        assert(n >= 0);
        parent_or_size_.assign(static_cast<std::size_t>(n), -1);
    }

    int leader(int vertex) {
        assert(0 <= vertex && vertex < static_cast<int>(parent_or_size_.size()));
        int root = vertex;
        while (parent_or_size_[root] >= 0) root = parent_or_size_[root];
        while (vertex != root) {
            const int next = parent_or_size_[vertex];
            parent_or_size_[vertex] = root;
            vertex = next;
        }
        return root;
    }

    bool merge(int a, int b) {
        a = leader(a);
        b = leader(b);
        if (a == b) return false;
        if (-parent_or_size_[a] < -parent_or_size_[b]) std::swap(a, b);
        parent_or_size_[a] += parent_or_size_[b];
        parent_or_size_[b] = a;
        --components_;
        return true;
    }

    bool same(int a, int b) { return leader(a) == leader(b); }
    int size(int vertex) { return -parent_or_size_[leader(vertex)]; }
    int components() const { return components_; }

    std::vector<std::vector<int>> groups() {
        std::vector<int> roots(parent_or_size_.size());
        std::vector<int> counts(parent_or_size_.size());
        for (int i = 0; i < static_cast<int>(parent_or_size_.size()); ++i) {
            roots[i] = leader(i);
            ++counts[roots[i]];
        }
        std::vector<std::vector<int>> buckets(parent_or_size_.size());
        for (int i = 0; i < static_cast<int>(buckets.size()); ++i) buckets[i].reserve(counts[i]);
        for (int i = 0; i < static_cast<int>(roots.size()); ++i) buckets[roots[i]].push_back(i);
        std::vector<std::vector<int>> result;
        result.reserve(components_);
        for (auto& group : buckets) {
            if (!group.empty()) result.push_back(std::move(group));
        }
        return result;
    }

private:
    std::vector<int> parent_or_size_;
    int components_;
};

}  // namespace cp
