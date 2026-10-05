#include <cp/data_structure/coordinate_compression.hpp>

#include <cassert>
#include <cstdint>
#include <limits>
#include <string>
#include <type_traits>
#include <vector>

struct OrderedOnly {
    int key;
    bool operator<(const OrderedOnly& other) const { return key < other.key; }
};

int main() {
    cp::CoordinateCompression<long long> empty;
    assert(empty.empty() && empty.size() == 0);
    assert(empty.lower_bound(0) == 0 && empty.upper_bound(0) == 0);
    assert(!empty.index(0) && !empty.contains(0));
    constexpr auto lo = std::numeric_limits<long long>::min();
    constexpr auto hi = std::numeric_limits<long long>::max();
    cp::CoordinateCompression<long long> c({hi, -2, lo, hi, 7, -2});
    assert((c.values() == std::vector<long long>{lo, -2, 7, hi}));
    assert(!c.empty());
    for (std::size_t i = 0; i < c.size(); ++i) {
        assert(c.index(c.value(i)) == i);
        assert(c.lower_bound(c.value(i)) == i);
        assert(c.upper_bound(c.value(i)) == i + 1);
    }
    assert(!c.index(0) && c.lower_bound(0) == 2 && c.upper_bound(0) == 2);
    assert(c.lower_bound(lo) == 0 && c.upper_bound(hi) == c.size());
    cp::CoordinateCompression<int> singleton({42, 42, 42});
    assert(singleton.size() == 1 && singleton.value(0) == 42);
    assert(singleton.lower_bound(41) == 0 && singleton.upper_bound(43) == 1);
    cp::CoordinateCompression<std::string> words({"z", "a", "a", "cat"});
    assert(words.size() == 3 && words.index("cat") == 1 && !words.contains("dog"));
    cp::CoordinateCompression<OrderedOnly> ordered({{2}, {1}, {2}});
    assert(ordered.size() == 2 && ordered.index({2}) == 1);
    cp::CoordinateCompression<bool> bits({true, false, true, false});
    static_assert(std::is_same_v<decltype(bits.value(0)), bool>);
    static_assert(std::is_same_v<decltype(c.value(0)), const long long&>);
    assert(bits.size() == 2 && !bits.value(0) && bits.value(1));
    assert(bits.index(false) == 0 && bits.index(true) == 1);
    assert(bits.lower_bound(false) == 0 && bits.upper_bound(false) == 1);
    assert(bits.lower_bound(true) == 1 && bits.upper_bound(true) == 2);
}
