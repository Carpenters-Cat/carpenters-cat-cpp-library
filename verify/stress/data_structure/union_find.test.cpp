// competitive-verifier: STANDALONE
#include <cp/data_structure/union_find.hpp>

#include <algorithm>
#include <cassert>
#include <numeric>
#include <random>
#include <vector>

int main() {
    std::mt19937 random(20261004);
    for (int trial = 0; trial < 500; ++trial) {
        const int n = 1 + random() % 64;
        cp::UnionFind uf(n);
        std::vector<int> label(n);
        std::iota(label.begin(), label.end(), 0);
        for (int step = 0; step < 500; ++step) {
            const int a = random() % n;
            const int b = random() % n;
            if (random() % 3 == 0) {
                const bool changed = label[a] != label[b];
                assert(uf.merge(a, b) == changed);
                const int old_label = label[b];
                const int new_label = label[a];
                for (int& value : label) if (value == old_label) value = new_label;
            } else {
                assert(uf.same(a, b) == (label[a] == label[b]));
                assert(uf.size(a) == std::count(label.begin(), label.end(), label[a]));
            }
            auto unique = label;
            std::sort(unique.begin(), unique.end());
            unique.erase(std::unique(unique.begin(), unique.end()), unique.end());
            assert(uf.components() == static_cast<int>(unique.size()));
        }
        const auto groups = uf.groups();
        assert(static_cast<int>(groups.size()) == uf.components());
        std::vector<int> seen(n);
        for (const auto& group : groups) {
            assert(!group.empty());
            assert(static_cast<int>(group.size()) == uf.size(group.front()));
            for (const int vertex : group) {
                assert(uf.same(group.front(), vertex));
                assert(++seen[vertex] == 1);
            }
        }
        assert(std::all_of(seen.begin(), seen.end(), [](int count) { return count == 1; }));
    }
}
