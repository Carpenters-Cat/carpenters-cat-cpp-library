// competitive-verifier: STANDALONE
#include <cp/data_structure/fenwick_tree.hpp>
#include <cassert>
#include <limits>
#include <vector>

struct Residue {
    int x = 0;
    Residue(int value = 0) : x((value % 7 + 7) % 7) {}
    Residue& operator+=(Residue other) { x = (x + other.x) % 7; return *this; }
    friend Residue operator-(Residue a, Residue b) { return Residue(a.x - b.x); }
};
int main() {
    cp::FenwickTree<long long> empty;
    assert(empty.size() == 0 && empty.sum(0, 0) == 0);
    cp::fenwick_tree<long long> tree(std::vector<long long>{3, -4, 8, 0, 2});
    assert(tree.size() == 5 && tree.sum(0, 5) == 9);
    tree.add(0, -3); tree.add(4, 5);
    assert(tree.sum(0, 1) == 0 && tree.sum(1, 5) == 11);
    assert(tree.sum(2, 2) == 0);
    cp::FenwickTree<int> wrap(2);
    wrap.add(0, std::numeric_limits<int>::max());
    wrap.add(1, 1);
    assert(wrap.sum(0, 2) == std::numeric_limits<int>::min());
    wrap.add(0, 1);
    assert(wrap.sum(0, 1) == std::numeric_limits<int>::min());
    cp::FenwickTree<unsigned> unsigned_tree(1);
    unsigned_tree.add(0, std::numeric_limits<unsigned>::max());
    unsigned_tree.add(0, 2);
    assert(unsigned_tree.sum(0, 1) == 1);
    cp::FenwickTree<Residue> modular(std::vector<Residue>{3, 6, 2});
    modular.add(1, Residue(-4));
    assert(modular.sum(0, 3).x == 0 && modular.sum(1, 3).x == 4);
}
