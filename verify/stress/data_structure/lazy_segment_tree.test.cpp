// competitive-verifier: STANDALONE
#include <cp/data_structure/lazy_segment_tree.hpp>
#include <algorithm>
#include <cassert>
#include <numeric>
#include <random>
#include <string>
#include <vector>
struct S { long long sum; int length; };
struct F { long long a, b; };
S op(S x, S y) { return {x.sum + y.sum, x.length + y.length}; }
S e() { return {0, 0}; }
S mapping(F f, S x) { return {f.a * x.sum + f.b * x.length, x.length}; }
F composition(F f, F g) { return {f.a * g.a, f.a * g.b + f.b}; }
F id() { return {1, 0}; }
std::string join(std::string a, std::string b) { return a + b; }
std::string blank() { return {}; }
std::string shift(int f, std::string s) { for (char& c : s) c = 'a' + (c - 'a' + f) % 26; return s; }
int compose_shift(int f, int g) { return (f + g) % 26; }
int no_shift() { return 0; }
int main() {
    std::mt19937 random(20261006);
    for (int n = 0; n <= 100; ++n) {
        std::vector<long long> values(n);
        cp::LazySegmentTree<S, op, e, F, mapping, composition, id> tree(std::vector<S>(n, {0, 1}));
        for (int step = 0; step < 1000; ++step) {
            int l = random() % (n + 1), r = random() % (n + 1);
            if (l > r) std::swap(l, r);
            const F update{random() % 2, random() % 20};
            switch (random() % 4) {
                case 0:
                    tree.apply(l, r, update);
                    for (int i = l; i < r; ++i) values[i] = update.a * values[i] + update.b;
                    break;
                case 1:
                    if (n) { const int p = random() % n; values[p] = random() % 20; tree.set(p, {values[p], 1}); }
                    break;
                case 2:
                    if (n) { const int p = random() % n; tree.apply(p, update); values[p] = update.a * values[p] + update.b; }
                    break;
            }
            if (n) { const int p = random() % n; assert(tree.get(p).sum == values[p]); }
            assert(tree.prod(l, r).sum == std::accumulate(values.begin() + l, values.begin() + r, 0LL));
            assert(tree.prod(l, r).length == r - l);
            assert(tree.all_prod().sum == std::accumulate(values.begin(), values.end(), 0LL));
            const long long limit = random() % 200;
            auto predicate = [limit](S x) { return x.sum <= limit; };
            int expected_r = l, expected_l = r;
            long long sum = 0;
            while (expected_r < n && sum + values[expected_r] <= limit) sum += values[expected_r++];
            sum = 0;
            while (expected_l > 0 && sum + values[expected_l - 1] <= limit) sum += values[--expected_l];
            assert(tree.max_right(l, predicate) == expected_r);
            assert(tree.min_left(r, predicate) == expected_l);
        }
        std::vector<std::string> letters(n, "a");
        std::string text(n, 'a');
        cp::LazySegmentTree<std::string, join, blank, int, shift, compose_shift, no_shift> words(letters);
        for (int step = 0; step < 100; ++step) {
            int l = random() % (n + 1), r = random() % (n + 1);
            if (l > r) std::swap(l, r);
            const int delta = random() % 26;
            words.apply(l, r, delta);
            for (int i = l; i < r; ++i) text[i] = 'a' + (text[i] - 'a' + delta) % 26;
            const auto target = text.substr(l, r - l);
            assert(words.prod(l, r) == target);
            assert(words.max_right(l, [&](const auto& s) { return target.starts_with(s); }) == r);
            assert(words.min_left(r, [&](const auto& s) { return target.ends_with(s); }) == l);
        }
    }
}
