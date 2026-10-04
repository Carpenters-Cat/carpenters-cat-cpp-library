#include <cp/math/subset_transform.hpp>

#include <cassert>
#include <random>
#include <vector>

int main() {
    std::mt19937_64 rng(20261005);
    for (int trial = 0; trial < 1000; ++trial) {
        const int n = 1 << (rng() % 8);
        std::vector<long long> original(n), subsets(n), supersets(n);
        for (auto& x : original) x = static_cast<long long>(rng() % 21) - 10;
        for (int mask = 0; mask < n; ++mask) {
            for (int part = mask;; part = (part - 1) & mask) {
                subsets[mask] += original[part];
                supersets[part] += original[mask];
                if (part == 0) break;
            }
        }
        auto a = original;
        cp::subset_zeta(a); assert(a == subsets);
        cp::subset_mobius(a); assert(a == original);
        a = original;
        cp::superset_zeta(a); assert(a == supersets);
        cp::superset_mobius(a); assert(a == original);
        // Also verify the other inverse ordering for arbitrary input.
        cp::subset_mobius(a); cp::subset_zeta(a); assert(a == original);
        cp::superset_mobius(a); cp::superset_zeta(a); assert(a == original);
    }
}
