// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/system_of_linear_equations
#include <cp/math/matrix.hpp>
#include <cp/math/modint.hpp>
#include <iostream>
using M = cp::modint998244353;
using A = cp::Matrix<M>;
A read(int n, int m) {
    A a(n, m);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++) {
            int x;
            std::cin >> x;
            a(i, j) = x;
        }
    return a;
}
void print(const A &a) {
    for (int i = 0; i < a.rows(); i++) {
        for (int j = 0; j < a.columns(); j++)
            std::cout << a(i, j).val() << ' ';
        std::cout << '\n';
    }
}
int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, m;
    std::cin >> n >> m;
    auto a = read(n, m);
    std::vector<M> b(n);
    for (auto &v : b) {
        int x;
        std::cin >> x;
        v = x;
    }
    auto solution = cp::solve_linear_system(a, b);
    if (!solution) {
        std::cout << -1 << "\n";
        return 0;
    }
    std::cout << solution->basis.size() << "\n";
    for (auto v : solution->particular)
        std::cout << v.val() << " ";
    std::cout << "\n";
    for (auto &row : solution->basis) {
        for (auto v : row)
            std::cout << v.val() << " ";
        std::cout << "\n";
    }
}
