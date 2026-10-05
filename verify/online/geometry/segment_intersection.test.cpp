// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/CGL_2_B
#include <cp/geometry/integer_geometry.hpp>
#include <iostream>
int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int q;
    std::cin >> q;
    while (q--) {
        cp::IntPoint a, b, c, d;
        std::cin >> a.x >> a.y >> b.x >> b.y >> c.x >> c.y >> d.x >> d.y;
        std::cout << cp::segments_intersect(a, b, c, d) << '\n';
    }
}
