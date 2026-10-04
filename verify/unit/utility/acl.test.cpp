#include <cp/utility/acl.hpp>

#include <cassert>
#include <vector>

using Mint = cp::modint998244353;
Mint sum(Mint a, Mint b) { return a + b; }
Mint zero() { return 0; }
struct Aggregate { Mint sum; int length; };
struct Affine { Mint a, b; };
Aggregate op(Aggregate a, Aggregate b) { return {a.sum + b.sum, a.length + b.length}; }
Aggregate e() { return {0, 0}; }
Aggregate mapping(Affine f, Aggregate s) { return {f.a * s.sum + f.b * s.length, s.length}; }
Affine composition(Affine f, Affine g) { return {f.a * g.a, f.a * g.b + f.b}; }
Affine identity() { return {1, 0}; }

int main() {
    std::vector<Mint> a{998244352, 2, 3, 4};
    cp::FenwickTree<Mint> bit(a);
    cp::SegmentTree<Mint, sum, zero> segment(a);
    assert(bit.sum(0, 4) == segment.all_prod());
    bit.add(1, 998244352);
    segment.set(1, segment.get(1) - 1);
    assert(bit.sum(0, 3) == segment.prod(0, 3));
    cp::LazySegmentTree<Aggregate, op, e, Affine, mapping, composition, identity> lazy(
        std::vector<Aggregate>{{1, 1}, {2, 1}, {3, 1}});
    lazy.apply(0, 3, {2, 1});
    lazy.apply(1, 3, {3, 4});
    assert(lazy.all_prod().sum == Mint(47));
    auto c = cp::convolution(a, std::vector<Mint>{1, 1});
    assert(c[1] == 1 && c[2] == 5);
    using Dynamic = cp::DynamicModint<42>;
    Dynamic::set_mod(12);
    cp::FenwickTree<Dynamic> dynamic(std::vector<Dynamic>{11, 2});
    assert(dynamic.sum(0, 2).val() == 1);
    assert((cp::crt({2, 3}, {3, 5}) == std::pair<long long, long long>(8, 15)));
    cp::SccGraph graph(3);
    graph.add_edge(0, 1); graph.add_edge(1, 0); graph.add_edge(1, 2);
    const auto [count, ids] = graph.scc_ids();
    assert(count == 2 && ids[0] == ids[1] && ids[0] < ids[2]);
    cp::TwoSat sat(1);
    sat.add_clause(0, true, 0, true);
    assert(sat.satisfiable() && sat.answer()[0]);
    cp::UnionFind dsu(3);
    assert(dsu.merge(0, 1) && dsu.same(0, 1));
    cp::MaxFlow<long long> flow(2);
    flow.add_edge(0, 1, 3);
    assert(flow.flow(0, 1) == 3);
    cp::MinCostFlow<long long, long long> cost(2);
    cost.add_edge(0, 1, 3, 2);
    assert((cost.flow(0, 1) == std::pair<long long, long long>(3, 6)));
    assert((cp::suffix_array(std::string("aba")) == std::vector<int>{2, 0, 1}));
}
