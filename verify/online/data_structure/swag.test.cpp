// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/queue_operate_all_composite
#include <cp/data_structure/swag.hpp>
#include <iostream>
constexpr long long mod = 998244353;
struct Affine { long long a, b; };
Affine op(Affine left, Affine right) { return {left.a * right.a % mod, (left.b * right.a + right.b) % mod}; }
Affine e() { return {1, 0}; }
int main() {
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int q; std::cin >> q;
    cp::SWAG<Affine, op, e> queue;
    while (q--) {
        int type; std::cin >> type;
        if (type == 0) { Affine f; std::cin >> f.a >> f.b; queue.push(f); }
        else if (type == 1) queue.pop();
        else { long long x; std::cin >> x; const auto f = queue.prod(); std::cout << (f.a * x + f.b) % mod << '\n'; }
    }
}
