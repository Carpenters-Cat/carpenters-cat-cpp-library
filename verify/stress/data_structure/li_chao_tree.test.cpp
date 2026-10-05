// competitive-verifier: STANDALONE
#include <cp/data_structure/li_chao_tree.hpp>
#include <algorithm>
#include <cassert>
#include <optional>
#include <random>
#include <vector>
struct Segment { long long low, high, a, b; };
int main() {
    std::mt19937 random(20261022);
    for (int trial = 0; trial < 200; ++trial) {
        const int n = random() % 70;
        std::vector<long long> xs(n);
        for (auto& x : xs) x = static_cast<int>(random() % 201) - 100;
        cp::LiChaoTree<> minimum(xs);
        cp::LiChaoTree<long long, long long, false> maximum(xs);
        std::vector<Segment> segments;
        for (int step = 0; step < 200; ++step) {
            const long long a = static_cast<int>(random() % 21) - 10, b = static_cast<int>(random() % 201) - 100;
            long long low = static_cast<int>(random() % 241) - 120, high = static_cast<int>(random() % 241) - 120;
            if (low > high) std::swap(low, high);
            if (random() % 2) {
                minimum.add_line(a, b); maximum.add_line(a, b);
                segments.push_back({-1000, 1000, a, b});
            } else {
                minimum.add_segment(low, high, a, b); maximum.add_segment(low, high, a, b);
                segments.push_back({low, high, a, b});
            }
            for (const long long x : minimum.coordinates()) {
                std::optional<long long> lo, hi;
                for (const auto line : segments) if (line.low <= x && x < line.high) {
                    const long long value = line.a * x + line.b;
                    if (!lo || value < *lo) lo = value;
                    if (!hi || value > *hi) hi = value;
                }
                assert(minimum.query(x) == lo && maximum.query(x) == hi);
            }
            assert(!minimum.query(1001) && !maximum.query(1001));
        }
    }
}
