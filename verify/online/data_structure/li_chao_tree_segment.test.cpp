// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/segment_add_get_min
#include <cp/data_structure/li_chao_tree.hpp>
#include <iostream>
#include <utility>
#include <vector>
struct Segment { long long low, high, a, b; };
struct Operation { int type; Segment segment; long long x; };
int main() {
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n, q; std::cin >> n >> q;
    std::vector<Segment> initial(n);
    for (auto& line : initial) std::cin >> line.low >> line.high >> line.a >> line.b;
    std::vector<Operation> operations(q);
    std::vector<long long> coordinates;
    for (auto& operation : operations) {
        std::cin >> operation.type;
        if (operation.type == 0) {
            auto& line = operation.segment;
            std::cin >> line.low >> line.high >> line.a >> line.b;
        } else { std::cin >> operation.x; coordinates.push_back(operation.x); }
    }
    cp::LiChaoTree<> tree(std::move(coordinates));
    for (const auto line : initial) tree.add_segment(line.low, line.high, line.a, line.b);
    for (const auto operation : operations) {
        if (operation.type == 0) {
            const auto line = operation.segment;
            tree.add_segment(line.low, line.high, line.a, line.b);
        } else {
            const auto result = tree.query(operation.x);
            if (result) std::cout << *result << '\n';
            else std::cout << "INFINITY\n";
        }
    }
}
