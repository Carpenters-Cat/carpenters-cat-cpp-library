// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/CGL_4_A
#include <cp/geometry/integer_geometry.hpp>
#include <iostream>
int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n;
    std::cin >> n;
    std::vector<cp::IntPoint> p(n);
    for (auto &x : p)
        std::cin >> x.x >> x.y;
    auto h = cp::convex_hull(std::move(p), true);
    auto first = std::min_element(
        h.begin(), h.end(), [](auto a, auto b) { return a.y < b.y || (a.y == b.y && a.x < b.x); });
    std::rotate(h.begin(), first, h.end());
    std::cout << h.size() << '\n';
    for (auto x : h)
        std::cout << x.x << ' ' << x.y << '\n';
}
