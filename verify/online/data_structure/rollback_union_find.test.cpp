// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/persistent_unionfind
#include <cp/data_structure/rollback_union_find.hpp>
#include <iostream>
#include <vector>

struct Query { int type, a, b; };
struct Frame { int node; cp::RollbackUnionFind::Snapshot before; bool leaving; };
int main() {
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n, q; std::cin >> n >> q;
    std::vector<Query> queries(q + 1);
    std::vector<std::vector<int>> children(q + 1);
    std::vector<int> answers(q + 1, -1);
    for (int i = 1; i <= q; ++i) {
        int parent; std::cin >> queries[i].type >> parent >> queries[i].a >> queries[i].b;
        children[parent + 1].push_back(i); // -1 is the empty initial version
    }
    cp::RollbackUnionFind uf(n);
    std::vector<Frame> stack{{0, 0, false}};
    while (!stack.empty()) {
        const Frame frame = stack.back(); stack.pop_back();
        if (frame.leaving) { uf.rollback(frame.before); continue; }
        const auto before = uf.snapshot();
        if (frame.node != 0) {
            const auto query = queries[frame.node];
            if (query.type == 0) uf.merge(query.a, query.b);
            else answers[frame.node] = uf.same(query.a, query.b);
        }
        stack.push_back({frame.node, before, true});
        for (const int child : children[frame.node]) stack.push_back({child, 0, false});
    }
    for (int i = 1; i <= q; ++i) if (answers[i] != -1) std::cout << answers[i] << '\n';
}
