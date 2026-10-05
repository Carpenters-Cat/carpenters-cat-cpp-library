// competitive-verifier: STANDALONE
#include <cp/data_structure/rollback_union_find.hpp>
#include <cassert>

int main() {
    cp::RollbackUnionFind empty;
    assert(empty.components() == 0 && empty.snapshot() == 0);
    empty.rollback(0);
    cp::RollbackUnionFind uf(6);
    const auto initial = uf.snapshot();
    assert(!uf.merge(0, 0) && uf.snapshot() == initial); // no history for failed merges
    assert(uf.merge(0, 1));
    const auto outer = uf.snapshot();
    assert(!uf.merge(1, 0) && uf.snapshot() == outer);
    assert(uf.merge(2, 3) && uf.merge(3, 4));
    const auto inner = uf.snapshot();
    assert(uf.merge(0, 4)); // smaller component must attach to larger component
    assert(uf.same(0, 3) && uf.size(0) == 5 && uf.components() == 2);
    assert(!uf.merge(1, 2));
    assert(uf.snapshot() == inner + 1);
    uf.rollback(inner);
    assert(!uf.same(0, 3) && uf.size(0) == 2 && uf.size(3) == 3 && uf.components() == 3);
    uf.rollback(inner); // idempotent
    const auto branch = uf.snapshot();
    assert(uf.merge(4, 5) && uf.size(5) == 4);
    uf.rollback(branch);
    assert(uf.size(5) == 1);
    uf.rollback(outer);
    assert(uf.same(0, 1) && uf.size(2) == 1 && uf.components() == 5);
    uf.rollback(initial);
    for (int v = 0; v < 6; ++v) assert(uf.leader(v) == v && uf.size(v) == 1);
    assert(uf.components() == 6 && uf.snapshot() == 0);
    cp::RollbackUnionFind single(1);
    assert(!single.merge(0, 0));
    single.rollback(0);
    assert(single.size(0) == 1 && single.same(0, 0));
    cp::RollbackUnionFind large(200000);
    for (int v = 1; v < 200000; ++v) assert(large.merge(v, v - 1));
    assert(large.size(199999) == 200000);
    large.rollback(0);
    assert(large.components() == 200000 && large.size(199999) == 1);
}
