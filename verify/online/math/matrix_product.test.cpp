// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/matrix_product
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
    int n, m, k;
    std::cin >> n >> m >> k;
    auto a = read(n, m), b = read(m, k);
    print(a * b);
}
