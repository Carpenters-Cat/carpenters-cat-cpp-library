// competitive-verifier: STANDALONE
#include <cp/data_structure/persistent_segment_tree.hpp>
#include <algorithm>
#include <bit>
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
    std::mt19937 random(20261032);
    for (int n = 0; n <= 100; ++n) {
        std::vector<long long> initial(n);
        for (auto& x : initial) x = static_cast<int>(random() % 101) - 50;
        cp::PersistentSegmentTree<long long, op, e> tree(initial);
        std::vector<std::vector<long long>> versions{initial};
        for (int step = 0; step < 500; ++step) {
            if (n && random() % 2) {
                const auto parent = random() % versions.size();
                const int p = random() % n;
                const long long value = static_cast<int>(random() % 101) - 50;
                const auto previous_nodes = tree.node_count();
                const auto version = tree.set(parent, p, value);
                auto copy = versions[parent]; copy[p] = value; versions.push_back(copy);
                assert(version == versions.size() - 1);
                assert(tree.node_count() - previous_nodes <= static_cast<std::size_t>(std::bit_width(static_cast<unsigned>(n - 1)) + 1));
            }
            const auto version = random() % versions.size();
            int l = random() % (n + 1), r = random() % (n + 1);
            if (l > r) std::swap(l, r);
            assert(tree.prod(version, l, r) == std::accumulate(versions[version].begin() + l, versions[version].begin() + r, 0LL));
            assert(tree.all_prod(version) == std::accumulate(versions[version].begin(), versions[version].end(), 0LL));
            if (n) { const int p = random() % n; assert(tree.get(version, p) == versions[version][p]); }
            assert(tree.versions() == versions.size());
        }
        cp::PersistentSegmentTree<std::string, join, blank> words(std::vector<std::string>(n, "a"));
        std::vector<std::string> texts{std::string(n, 'a')};
        for (int step = 0; step < 200; ++step) {
            if (n) {
                const auto parent = random() % texts.size();
                const int p = random() % n;
                const char letter = 'a' + random() % 26;
                const auto version = words.set(parent, p, std::string(1, letter));
                auto copy = texts[parent]; copy[p] = letter; texts.push_back(copy);
                assert(version == texts.size() - 1);
            }
            const auto version = random() % texts.size();
            int l = random() % (n + 1), r = random() % (n + 1);
            if (l > r) std::swap(l, r);
            assert(words.prod(version, l, r) == texts[version].substr(l, r - l));
        }
    }
}
