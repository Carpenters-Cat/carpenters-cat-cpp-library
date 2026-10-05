// competitive-verifier: STANDALONE
#include <cp/data_structure/persistent_segment_tree.hpp>
#include <bit>
#include <cassert>
#include <string>
#include <vector>
long long op(long long a, long long b) { return a + b; }
long long e() { return 0; }
std::string join(std::string a, std::string b) { return a + b; }
std::string blank() { return {}; }
int main() {
    using Tree = cp::PersistentSegmentTree<long long, op, e>;
    Tree empty;
    assert(empty.size() == 0 && empty.versions() == 1 && empty.initial_version() == 0);
    assert(empty.all_prod(0) == 0 && empty.prod(0, 0, 0) == 0);
    Tree tree(std::vector<long long>{1, 2, 3, 4, 5});
    const auto initial = tree.initial_version(), nodes = tree.node_count();
    const auto first = tree.set(initial, 2, 30);
    assert(tree.node_count() - nodes <= std::bit_width(5U) + 1);
    const auto branch = tree.set(initial, 0, 9);
    const auto child = tree.set(first, 4, -5);
    assert(tree.versions() == 4 && tree.size() == 5);
    assert(tree.all_prod(initial) == 15 && tree.all_prod(first) == 42 && tree.all_prod(branch) == 23);
    assert(tree.all_prod(child) == 32 && tree.get(initial, 2) == 3 && tree.get(first, 2) == 30);
    assert(tree.prod(first, 1, 4) == 36 && tree.prod(first, 3, 3) == 0);
    Tree implicit(1000000000);
    assert(implicit.node_count() == 1 && implicit.all_prod(0) == 0);
    const auto large = implicit.set(0, 999999999, 7);
    assert(implicit.node_count() <= 32 && implicit.prod(large, 0, 1000000000) == 7);
    assert(implicit.get(0, 999999999) == 0);
    Tree single(1);
    const auto one = single.set(0, 0, 5), two = single.set(one, 0, 5);
    assert(single.get(0, 0) == 0 && single.get(two, 0) == 5);
    cp::PersistentSegmentTree<std::string, join, blank> words(std::vector<std::string>{"a", "b", "c", "d", "e"});
    const auto changed = words.set(0, 1, "XYZ");
    const auto other = words.set(0, 4, "!");
    assert(words.all_prod(0) == "abcde" && words.prod(changed, 0, 4) == "aXYZcd");
    assert(words.all_prod(other) == "abcd!");
}
