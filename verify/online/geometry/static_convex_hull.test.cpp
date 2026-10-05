// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/static_convex_hull
#include <cp/geometry/integer_geometry.hpp>
#include <iostream>
int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int t;
    std::cin >> t;
    while (t--) {
        int n;
        std::cin >> n;
        std::vector<cp::IntPoint> p(n);
        for (auto &x : p)
            std::cin >> x.x >> x.y;
        auto h = cp::convex_hull(std::move(p));
        std::cout << h.size() << '\n';
        for (auto x : h)
            std::cout << x.x << ' ' << x.y << '\n';
    }
}
