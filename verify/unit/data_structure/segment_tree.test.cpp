// competitive-verifier: STANDALONE
#include <cp/data_structure/segment_tree.hpp>
#include <cassert>
#include <string>
#include <vector>

long long add(long long a, long long b) { return a + b; }
long long zero() { return 0; }
bool small(long long x) { return x <= 6; }
std::string join(std::string a, std::string b) { return a + b; }
std::string blank() { return {}; }
int main() {
    cp::SegmentTree<long long, add, zero> empty;
    assert(empty.size() == 0 && empty.all_prod() == 0 && empty.prod(0, 0) == 0);
    assert(empty.max_right<small>(0) == 0 && empty.min_left<small>(0) == 0);
    cp::segtree<long long, add, zero> tree(std::vector<long long>{1, 2, 3, 4, 5});
    assert(tree.size() == 5 && tree.all_prod() == 15);
    assert(tree.max_right<small>(0) == 3 && tree.min_left<small>(3) == 0);
    assert(tree.max_right(5, small) == 5 && tree.min_left(0, small) == 0);
    assert(tree.max_right(1, [](long long x) { return x == 0; }) == 1);
    assert(tree.min_left(4, [](long long x) { return x == 0; }) == 4);
    tree.set(4, -5);
    assert(tree.get(4) == -5 && tree.prod(3, 5) == -1);
    assert(tree.prod(2, 2) == 0);
    cp::SegmentTree<long long, add, zero> single(1);
    single.set(0, 7);
    assert(single.max_right<small>(0) == 0 && single.min_left<small>(1) == 1);
    cp::SegmentTree<std::string, join, blank> words(std::vector<std::string>{"a", "b", "c", "d", "e"});
    assert(words.prod(1, 4) == "bcd");
    assert(words.max_right(1, [](const auto& s) { return std::string("bcd").starts_with(s); }) == 4);
    assert(words.min_left(4, [](const auto& s) { return std::string("bcd").ends_with(s); }) == 1);
    words.set(2, "XYZ");
    assert(words.all_prod() == "abXYZde");
}
