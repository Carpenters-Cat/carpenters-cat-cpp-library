// competitive-verifier: STANDALONE
#include <cp/data_structure/fenwick_tree.hpp>
#include <algorithm>
#include <cassert>
#include <numeric>
#include <random>
#include <vector>
int main() {
    std::mt19937 random(20261004);
    for (int n = 0; n <= 100; ++n) {
        std::vector<long long> values(n);
        for (auto& x : values) x = static_cast<int>(random() % 201) - 100;
        cp::FenwickTree<long long> tree(values);
        for (int step = 0; step < 1000; ++step) {
            if (n && random() % 2) {
                const int p = random() % n, delta = static_cast<int>(random() % 201) - 100;
                values[p] += delta; tree.add(p, delta);
            }
            int l = random() % (n + 1), r = random() % (n + 1);
            if (l > r) std::swap(l, r);
            assert(tree.sum(l, r) == std::accumulate(values.begin() + l, values.begin() + r, 0LL));
        }
    }
}
