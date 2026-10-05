// competitive-verifier: STANDALONE
#include <cp/data_structure/potential_union_find.hpp>
#include <cassert>
#include <limits>

struct Mod7 {
    int value;
    Mod7(int x = 0) : value((x % 7 + 7) % 7) {}
    friend Mod7 operator+(Mod7 a, Mod7 b) { return Mod7(a.value + b.value); }
    friend Mod7 operator-(Mod7 a, Mod7 b) { return Mod7(a.value - b.value); }
    Mod7 operator-() const { return Mod7(-value); }
    bool operator==(const Mod7&) const = default;
};
int main() {
    using UF = cp::PotentialUnionFind<long long>;
    using Result = UF::MergeResult;
    UF empty;
    assert(empty.components() == 0);
    UF single(1);
    assert(single.leader(0) == 0 && single.potential(0) == 0 && single.size(0) == 1);
    assert(single.difference(0, 0) == 0);
    assert(single.merge(0, 0, 0) == Result::already_consistent);
    assert(single.merge(0, 0, 1) == Result::contradiction);
    UF uf(7);
    assert(!uf.same(0, 1) && !uf.difference(0, 1));
    assert(uf.merge(0, 1, 5) == Result::merged);
    assert(uf.merge(2, 1, 9) == Result::merged); // reverse union-by-size orientation
    assert(uf.difference(0, 2) == -4 && uf.difference(2, 0) == 4);
    assert(uf.merge(3, 4, 11) == Result::merged);
    assert(uf.merge(4, 5, -8) == Result::merged);
    assert(uf.merge(1, 5, 7) == Result::merged);
    assert(uf.difference(0, 5) == 12 && uf.difference(0, 3) == 9);
    assert(uf.difference(3, 2) == -13);
    assert(uf.merge(2, 3, 13) == Result::already_consistent);
    assert(uf.merge(2, 3, 12) == Result::contradiction);
    assert(uf.difference(2, 3) == 13 && uf.components() == 2 && uf.size(5) == 6);
    assert(!uf.difference(5, 6));
    for (int v = 0; v < 6; ++v) {
        const int root = uf.leader(v);
        assert(uf.potential(v) == *uf.difference(root, v));
        assert(uf.leader(v) == root); // repeated path compression
    }
    UF wide(2);
    constexpr long long large = std::numeric_limits<long long>::max() / 2;
    assert(wide.merge(0, 1, large) == Result::merged);
    assert(wide.difference(1, 0) == -large);
    cp::PotentialUnionFind<Mod7> modular(3);
    using ModResult = cp::PotentialUnionFind<Mod7>::MergeResult;
    assert(modular.merge(0, 1, 5) == ModResult::merged);
    assert(modular.merge(1, 2, 4) == ModResult::merged);
    assert(modular.difference(0, 2)->value == 2);
    assert(modular.merge(0, 2, 9) == ModResult::already_consistent);
    assert(modular.merge(0, 2, 3) == ModResult::contradiction);
}
