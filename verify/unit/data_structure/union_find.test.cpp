// competitive-verifier: STANDALONE
#include <cp/data_structure/union_find.hpp>

#include <algorithm>
#include <cassert>
#include <vector>

int main() {
    cp::UnionFind empty;
    assert(empty.components() == 0);
    assert(empty.groups().empty());
    cp::UnionFind single(1);
    assert(single.leader(0) == 0);
    assert(single.size(0) == 1);
    assert(single.same(0, 0));
    assert(!single.merge(0, 0));
    cp::UnionFind uf(6);
    assert(uf.components() == 6);
    assert(uf.merge(0, 1));
    assert(uf.merge(2, 3));
    assert(uf.merge(3, 4));
    assert(uf.merge(0, 4));
    assert(!uf.merge(1, 2));
    assert(uf.components() == 2);
    for (int vertex = 0; vertex < 5; ++vertex) {
        assert(uf.same(0, vertex));
        assert(uf.size(vertex) == 5);
    }
    assert(!uf.same(0, 5));
    auto groups = uf.groups();
    std::sort(groups.begin(), groups.end());
    assert((groups == std::vector<std::vector<int>>{{0, 1, 2, 3, 4}, {5}}));
    cp::UnionFind large(200000);
    for (int i = 1; i < 200000; ++i) assert(large.merge(i - 1, i));
    assert(large.size(199999) == 200000);
    assert(large.components() == 1);
}
