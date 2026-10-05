#include <cp/data_structure/coordinate_compression.hpp>

#include <cassert>
#include <cstdint>
#include <limits>
#include <random>
#include <set>
#include <vector>

int main() {
    std::mt19937_64 rng(0xC001D1A7EULL);
    for (int trial = 0; trial < 2000; ++trial) {
        std::vector<long long> input;
        for (int i = 0, n = rng() % 80; i < n; ++i) input.push_back(static_cast<long long>(rng() % 101) - 50);
        if (trial % 3 == 0) input.push_back(std::numeric_limits<long long>::min());
        if (trial % 5 == 0) input.push_back(std::numeric_limits<long long>::max());
        std::set<long long> reference(input.begin(), input.end());
        cp::CoordinateCompression<long long> c(input);
        assert(c.size() == reference.size());
        std::size_t rank = 0;
        for (auto x : reference) {
            assert(c.value(rank) == x && c.index(x) == rank);
            ++rank;
        }
        for (long long x = -55; x <= 55; ++x) {
            std::size_t lower = 0, upper = 0;
            for (auto y : reference) {
                lower += y < x;
                upper += y <= x;
            }
            assert(c.lower_bound(x) == lower && c.upper_bound(x) == upper);
            assert(c.contains(x) == reference.contains(x));
        }
        for (auto x : input) assert(c.value(*c.index(x)) == x);
    }
}
