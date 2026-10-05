// competitive-verifier: STANDALONE
#include <cp/data_structure/lazy_segment_tree.hpp>
#include <cassert>
#include <string>
#include <vector>

struct S { long long sum; int length; };
struct F { long long a, b; };
S op(S x, S y) { return {x.sum + y.sum, x.length + y.length}; }
S e() { return {0, 0}; }
S mapping(F f, S x) { return {f.a * x.sum + f.b * x.length, x.length}; }
F composition(F f, F g) { return {f.a * g.a, f.a * g.b + f.b}; }
F id() { return {1, 0}; }
bool small(S s) { return s.sum <= 10; }
std::string join(std::string a, std::string b) { return a + b; }
std::string blank() { return {}; }
std::string shift(int f, std::string s) { for (char& c : s) c = 'a' + (c - 'a' + f) % 26; return s; }
int compose_shift(int f, int g) { return (f + g) % 26; }
int no_shift() { return 0; }
int main() {
    using Tree = cp::LazySegmentTree<S, op, e, F, mapping, composition, id>;
    Tree empty;
    assert(empty.size() == 0 && empty.prod(0, 0).sum == 0 && empty.all_prod().length == 0);
    empty.apply(0, 0, {2, 3});
    assert(empty.max_right<small>(0) == 0 && empty.min_left<small>(0) == 0);
    cp::lazy_segtree<S, op, e, F, mapping, composition, id> tree(std::vector<S>{{1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}});
    tree.apply(0, 5, {2, 3}); // 5, 7, 9, 11, 13
    tree.apply(1, 4, {0, 2}); // 5, 2, 2, 2, 13
    tree.apply(0, {3, 1}); // 16, 2, 2, 2, 13
    assert(tree.all_prod().sum == 35 && tree.get(0).sum == 16);
    assert(tree.prod(1, 4).sum == 6 && tree.prod(3, 3).sum == 0);
    tree.set(4, {1, 1});
    assert(tree.max_right<small>(1) == 5 && tree.min_left<small>(5) == 1);
    assert(tree.max_right(0, small) == 0 && tree.min_left(1, small) == 1);
    assert(tree.max_right(5, small) == 5 && tree.min_left(0, small) == 0);
    Tree composition_test(std::vector<S>(8, {1, 1}));
    composition_test.apply(0, 8, {2, 3});
    composition_test.apply(0, 8, {3, 4});
    assert(composition_test.get(3).sum == 19);
    composition_test.set(3, {7, 1});
    assert(composition_test.all_prod().sum == 140);
    Tree single(std::vector<S>{{3, 1}});
    single.apply(0, 1, {2, 1});
    assert(single.get(0).sum == 7);
    cp::LazySegmentTree<std::string, join, blank, int, shift, compose_shift, no_shift> words(std::vector<std::string>{"a", "b", "c", "d", "e"});
    words.apply(0, 5, 1); // bcdef
    assert(words.prod(1, 4) == "cde");
    assert(words.max_right(1, [](const auto& s) { return std::string("cde").starts_with(s); }) == 4);
    assert(words.min_left(4, [](const auto& s) { return std::string("cde").ends_with(s); }) == 1);
}
