// competitive-verifier: STANDALONE
#include <cp/data_structure/sparse_table.hpp>
#include <algorithm>
#include <cassert>
#include <numeric>
#include <random>
#include <vector>
int minimum(int a, int b) { return std::min(a, b); }
int maximum(int a, int b) { return std::max(a, b); }
int gcd(int a, int b) { return std::gcd(a, b); }
int first(int a, int) { return a; }
int main() {
    std::mt19937 random(20261020);
    for (int n = 0; n <= 130; ++n) {
        std::vector<int> values(n);
        for (int& x : values) x = random() % 1000;
        cp::SparseTable<int, minimum> low(values);
        cp::SparseTable<int, maximum> high(values);
        cp::SparseTable<int, gcd> common(values);
        cp::SparseTable<int, first> ordered(values);
        for (int l = 0; l <= n; ++l) {
            assert(!low.prod(l, l) && !high.prod(l, l) && !common.prod(l, l) && !ordered.prod(l, l));
            if (l == n) continue;
            int lo = values[l], hi = values[l], g = 0;
            for (int r = l + 1; r <= n; ++r) {
                lo = std::min(lo, values[r - 1]); hi = std::max(hi, values[r - 1]);
                g = std::gcd(g, values[r - 1]);
                assert(low.prod(l, r) == lo && high.prod(l, r) == hi && common.prod(l, r) == g);
                assert(ordered.prod(l, r) == values[l]);
            }
        }
    }
}
