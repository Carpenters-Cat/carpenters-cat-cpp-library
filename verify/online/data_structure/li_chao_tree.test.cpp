// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/line_add_get_min
#include <cp/data_structure/li_chao_tree.hpp>
#include <iostream>
#include <utility>
#include <vector>
struct Operation { int type; long long a, b, x; };
int main() {
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n, q; std::cin >> n >> q;
    using Tree = cp::LiChaoTree<>;
    std::vector<Tree::Line> initial(n);
    for (auto& line : initial) std::cin >> line.slope >> line.intercept;
    std::vector<Operation> operations(q);
    std::vector<long long> coordinates;
    for (auto& operation : operations) {
        std::cin >> operation.type;
        if (operation.type == 0) std::cin >> operation.a >> operation.b;
        else { std::cin >> operation.x; coordinates.push_back(operation.x); }
    }
    Tree tree(std::move(coordinates));
    for (const auto line : initial) tree.add_line(line);
    for (const auto operation : operations) {
        if (operation.type == 0) tree.add_line(operation.a, operation.b);
        else {
            const auto result = tree.query(operation.x);
            if (result) std::cout << *result << '\n';
            else std::cout << "INFINITY\n";
        }
    }
}
