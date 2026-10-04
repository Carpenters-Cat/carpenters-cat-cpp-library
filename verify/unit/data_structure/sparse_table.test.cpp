// competitive-verifier: STANDALONE
#include <cp/data_structure/sparse_table.hpp>
#include <algorithm>
#include <cassert>
#include <limits>
#include <numeric>
#include <vector>
int minimum(int a, int b) { return std::min(a, b); }
int maximum(int a, int b) { return std::max(a, b); }
int gcd(int a, int b) { return std::gcd(a, b); }
int first(int a, int) { return a; }
struct Item { int x; explicit Item(int value) : x(value) {} };
Item item_min(Item a, Item b) { return a.x < b.x ? a : b; }
int main() {
    cp::SparseTable<int, minimum> empty;
    assert(empty.size() == 0 && !empty.prod(0, 0));
    cp::SparseTable<int, minimum> single({7});
    assert(single.prod(0, 1) == 7 && !single.prod(1, 1));
    const std::vector<int> values{12, 6, 18, 3, 15, 9, 21, 0, 30};
    cp::SparseTable<int, minimum> low(values);
    cp::SparseTable<int, maximum> high(values);
    cp::SparseTable<int, gcd> common(values);
    assert(low.size() == 9 && low.prod(0, 9) == 0);
    assert(high.prod(1, 5) == 18 && common.prod(0, 3) == 6);
    assert(!low.prod(4, 4));
    cp::SparseTable<int, first> noncommutative(values);
    assert(noncommutative.prod(3, 8) == 3);
    cp::SparseTable<Item, item_min> no_default(std::vector<Item>{Item(9), Item(-4), Item(2)});
    assert(no_default.prod(0, 3)->x == -4);
    cp::SparseTable<int, minimum> extremes({std::numeric_limits<int>::min(), std::numeric_limits<int>::max()});
    assert(extremes.prod(0, 2) == std::numeric_limits<int>::min());
}
