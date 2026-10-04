// competitive-verifier: STANDALONE
#include <cp/data_structure/segment_tree.hpp>
#include <algorithm>
#include <cassert>
#include <numeric>
#include <random>
#include <string>
#include <vector>
long long op(long long a, long long b) { return a + b; }
long long e() { return 0; }
std::string join(std::string a, std::string b) { return a + b; }
std::string blank() { return {}; }
int main() {
    std::mt19937 random(20261005);
    for (int n = 0; n <= 100; ++n) {
        std::vector<long long> values(n);
        cp::SegmentTree<long long, op, e> tree(values);
        for (int step = 0; step < 1000; ++step) {
            if (n && random() % 2) {
                const int p = random() % n;
                values[p] = random() % 20; tree.set(p, values[p]);
                assert(tree.get(p) == values[p]);
            }
            int l = random() % (n + 1), r = random() % (n + 1);
            if (l > r) std::swap(l, r);
            assert(tree.prod(l, r) == std::accumulate(values.begin() + l, values.begin() + r, 0LL));
            assert(tree.all_prod() == std::accumulate(values.begin(), values.end(), 0LL));
            const long long limit = random() % 200;
            auto predicate = [limit](long long x) { return x <= limit; };
            int expected_r = l, expected_l = r;
            long long sum = 0;
            while (expected_r < n && sum + values[expected_r] <= limit) sum += values[expected_r++];
            sum = 0;
            while (expected_l > 0 && sum + values[expected_l - 1] <= limit) sum += values[--expected_l];
            assert(tree.max_right(l, predicate) == expected_r);
            assert(tree.min_left(r, predicate) == expected_l);
        }
        std::vector<std::string> letters(n);
        std::string text;
        for (auto& letter : letters) { letter = char('a' + random() % 26); text += letter; }
        cp::SegmentTree<std::string, join, blank> words(letters);
        for (int step = 0; step < 100; ++step) {
            int l = random() % (n + 1), r = random() % (n + 1);
            if (l > r) std::swap(l, r);
            const auto target = text.substr(l, r - l);
            assert(words.prod(l, r) == target);
            assert(words.max_right(l, [&](const auto& s) { return target.starts_with(s); }) == r);
            assert(words.min_left(r, [&](const auto& s) { return target.ends_with(s); }) == l);
        }
    }
}
