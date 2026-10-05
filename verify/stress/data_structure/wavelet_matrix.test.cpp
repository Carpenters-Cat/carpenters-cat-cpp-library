// competitive-verifier: STANDALONE
#include <cp/data_structure/wavelet_matrix.hpp>
#include <algorithm>
#include <cassert>
#include <random>
#include <vector>
int main() {
    std::mt19937 random(20261031);
    for (int n = 0; n <= 260; ++n) {
        std::vector<int> values(n);
        for (int& x : values) x = static_cast<int>(random() % 301) - 150;
        cp::WaveletMatrix<int> wm(values);
        for (int step = 0; step < 500; ++step) {
            int l = random() % (n + 1), r = random() % (n + 1);
            if (l > r) std::swap(l, r);
            std::vector<int> sorted(values.begin() + l, values.begin() + r);
            std::sort(sorted.begin(), sorted.end());
            assert(!wm.kth_smallest(l, r, -1) && !wm.kth_smallest(l, r, r - l));
            if (l != r) {
                const int k = random() % (r - l);
                assert(wm.kth_smallest(l, r, k) == sorted[k]);
                assert(wm.kth_smallest(l, r, 0) == sorted.front());
                assert(wm.kth_smallest(l, r, r - l - 1) == sorted.back());
            }
            int lo = static_cast<int>(random() % 401) - 200, hi = static_cast<int>(random() % 401) - 200;
            if (lo > hi) std::swap(lo, hi);
            assert(wm.frequency(l, r, lo) == std::count(sorted.begin(), sorted.end(), lo));
            assert(wm.count_less(l, r, lo) == std::lower_bound(sorted.begin(), sorted.end(), lo) - sorted.begin());
            assert(wm.range_frequency(l, r, lo, hi) == std::lower_bound(sorted.begin(), sorted.end(), hi) - std::lower_bound(sorted.begin(), sorted.end(), lo));
        }
    }
}
