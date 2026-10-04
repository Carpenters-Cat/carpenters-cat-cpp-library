// competitive-verifier: STANDALONE
#include <cp/data_structure/wavelet_matrix.hpp>
#include <cassert>
#include <limits>
#include <string>
#include <vector>
struct Key { int x; friend bool operator<(Key a, Key b) { return a.x < b.x; } };
int main() {
    cp::WaveletMatrix<long long> empty;
    assert(empty.size() == 0 && empty.frequency(0, 0, 1) == 0);
    assert(empty.count_less(0, 0, 1) == 0 && empty.range_frequency(0, 0, -3, 3) == 0);
    assert(!empty.kth_smallest(0, 0, 0) && !empty.kth_smallest(0, 0, -1));
    cp::WaveletMatrix<int> same(std::vector<int>(128, -7));
    assert(same.kth_smallest(0, 128, 127) == -7 && same.frequency(64, 128, -7) == 64);
    assert(same.count_less(0, 128, -7) == 0 && same.count_less(0, 128, -6) == 128);
    assert(!same.kth_smallest(0, 128, 128));
    constexpr auto lo = std::numeric_limits<long long>::min(), hi = std::numeric_limits<long long>::max();
    cp::WaveletMatrix<long long> wm(std::vector<long long>{hi, -4, lo, -4, 0, hi, 7});
    assert(wm.size() == 7 && wm.kth_smallest(0, 7, 0) == lo && wm.kth_smallest(0, 7, 6) == hi);
    assert(wm.kth_smallest(1, 5, 2) == -4);
    assert(wm.frequency(0, 7, -4) == 2 && wm.frequency(2, 4, -4) == 1);
    assert(wm.frequency(0, 7, 6) == 0 && wm.frequency(3, 3, -4) == 0);
    assert(wm.range_frequency(0, 7, lo, hi) == 5);
    assert(wm.range_frequency(0, 7, -4, 7) == 3 && wm.range_frequency(0, 7, 0, 0) == 0);
    assert(wm.count_less(0, 7, hi) == 5 && wm.count_less(0, 7, lo) == 0);
    cp::WaveletMatrix<std::string> words(std::vector<std::string>{"z", "aa", "b", "aa"});
    assert(words.kth_smallest(0, 4, 1) == "aa" && words.frequency(0, 4, "aa") == 2);
    assert(words.range_frequency(0, 4, "a", "c") == 3);
    for (const int n : {63, 64, 65, 127, 128, 129}) {
        std::vector<int> distinct(n);
        for (int i = 0; i < n; ++i) distinct[i] = n - i - 1;
        cp::WaveletMatrix<int> blocks(distinct);
        assert(blocks.kth_smallest(0, n, 0) == 0 && blocks.kth_smallest(0, n, n - 1) == n - 1);
        assert(blocks.frequency(0, n, n - 1) == 1 && blocks.count_less(0, n, n / 2) == n / 2);
        assert(blocks.frequency(n, n, 0) == 0);
    }
    cp::WaveletMatrix<bool> flags(std::vector<bool>{true, false, true, false});
    assert(flags.kth_smallest(0, 4, 0) == false && flags.kth_smallest(0, 4, 3) == true);
    assert(flags.frequency(0, 4, true) == 2 && flags.range_frequency(0, 4, false, true) == 2);
    cp::WaveletMatrix<Key> keys(std::vector<Key>{{5}, {-1}, {5}});
    assert(keys.kth_smallest(0, 3, 0)->x == -1 && keys.frequency(0, 3, {5}) == 2);
}
