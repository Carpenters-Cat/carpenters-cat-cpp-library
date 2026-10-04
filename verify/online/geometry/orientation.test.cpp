// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/CGL_1_C
#include <cp/geometry/integer_geometry.hpp>
#include <iostream>
int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    cp::IntPoint a, b;
    std::cin >> a.x >> a.y >> b.x >> b.y;
    int q;
    std::cin >> q;
    while (q--) {
        cp::IntPoint p;
        std::cin >> p.x >> p.y;
        int turn = cp::orientation(a, b, p);
        if (turn > 0)
            std::cout << "COUNTER_CLOCKWISE\n";
        else if (turn < 0)
            std::cout << "CLOCKWISE\n";
        else if (cp::dot(b - a, p - a) < 0)
            std::cout << "ONLINE_BACK\n";
        else if (!cp::on_segment(a, b, p))
            std::cout << "ONLINE_FRONT\n";
        else
            std::cout << "ON_SEGMENT\n";
    }
}
