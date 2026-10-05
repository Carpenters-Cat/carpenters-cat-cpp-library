// competitive-verifier: STANDALONE
#include <cp/data_structure/rollback_union_find.hpp>
#include <algorithm>
#include <cassert>
#include <numeric>
#include <random>
#include <vector>
int main() {
    std::mt19937 random(20261030);
    for (int trial = 0; trial < 300; ++trial) {
        const int n = 1 + random() % 50;
        cp::RollbackUnionFind uf(n);
        std::vector<int> label(n);
        std::iota(label.begin(), label.end(), 0);
        // Copies are indexed by successful merge count, exactly like snapshots.
        std::vector<std::vector<int>> states{label};
        std::vector<cp::RollbackUnionFind::Snapshot> snapshots{uf.snapshot()};
        for (int step = 0; step < 1000; ++step) {
            const int action = random() % 5;
            if (action < 3) {
                const int a = random() % n, b = random() % n;
                const bool changed = label[a] != label[b];
                const auto previous = uf.snapshot();
                assert(uf.merge(a, b) == changed);
                if (changed) {
                    const int old = label[b], replacement = label[a];
                    for (int& value : label) if (value == old) value = replacement;
                    states.push_back(label);
                    assert(uf.snapshot() == previous + 1);
                } else assert(uf.snapshot() == previous);
            } else if (action == 3) {
                snapshots.push_back(uf.snapshot());
            } else {
                const auto target = snapshots[random() % snapshots.size()];
                uf.rollback(target);
                label = states[target];
                states.resize(target + 1);
                snapshots.erase(std::remove_if(snapshots.begin(), snapshots.end(),
                    [target](auto s) { return s > target; }), snapshots.end());
                assert(uf.snapshot() == target);
            }
            auto unique = label;
            std::sort(unique.begin(), unique.end());
            unique.erase(std::unique(unique.begin(), unique.end()), unique.end());
            assert(uf.components() == static_cast<int>(unique.size()));
            assert(uf.snapshot() == states.size() - 1);
            for (int v = 0; v < n; ++v) {
                assert(uf.size(v) == std::count(label.begin(), label.end(), label[v]));
                const int other = random() % n;
                assert(uf.same(v, other) == (label[v] == label[other]));
            }
        }
        uf.rollback(0);
        assert(uf.components() == n);
    }
}
