// competitive-verifier: STANDALONE
#include <cassert>
#include <cp/geometry/integer_geometry.hpp>
int main() {
    using P = cp::IntPoint;
    using W = cp::GeometryWide;
    assert((P{1, 2} + P{3, -4} == P{4, -2}));
    assert((P{1, 2} - P{3, -4} == P{-2, 6}));
    assert((-P{1, -2} == P{-1, 2}));
    assert((3 * P{1, -2} == P{3, -6}));
    P v{1, 2};
    v += P{2, 3};
    v -= P{1, 1};
    v *= 2;
    assert((v == P{4, 8}));
    assert(cp::dot({1, 2}, {3, 4}) == 11 && cp::cross({1, 2}, {3, 4}) == -2);
    assert(cp::orientation({0, 0}, {1, 0}, {0, 1}) == 1 &&
           cp::orientation({0, 0}, {1, 0}, {0, -1}) == -1);
    assert(cp::orientation({1, 1}, {1, 1}, {3, 4}) == 0);
    assert(cp::on_segment({0, 0}, {2, 2}, {1, 1}) && !cp::on_segment({0, 0}, {2, 2}, {3, 3}));
    assert(cp::on_segment({0, 0}, {0, 0}, {0, 0}) && !cp::on_segment({0, 0}, {0, 0}, {0, 1}));
    assert(cp::segments_intersect({0, 0}, {2, 2}, {0, 2}, {2, 0}));
    assert(cp::segments_intersect({0, 0}, {2, 0}, {2, 0}, {3, 0}));
    assert(cp::segments_intersect({0, 0}, {3, 0}, {1, 0}, {2, 0}));
    assert(!cp::segments_intersect({0, 0}, {1, 0}, {2, 0}, {3, 0}));
    assert(cp::segments_intersect({1, 0}, {1, 0}, {0, 0}, {2, 0}));
    assert(!cp::segments_intersect({1, 1}, {1, 1}, {0, 0}, {2, 0}));
    assert(cp::segments_intersect({1, 1}, {1, 1}, {1, 1}, {1, 1}));
    assert(cp::convex_hull({}).empty());
    assert(cp::convex_hull({{1, 2}, {1, 2}}) == std::vector<P>({{1, 2}}));
    assert(cp::convex_hull({{2, 2}, {0, 0}, {1, 1}, {0, 0}}) == std::vector<P>({{0, 0}, {2, 2}}));
    assert(cp::convex_hull({{2, 2}, {0, 0}, {1, 1}}, true) ==
           std::vector<P>({{0, 0}, {1, 1}, {2, 2}}));
    std::vector<P> square{{0, 0}, {2, 0}, {2, 2}, {0, 2}, {1, 0}, {1, 1}, {0, 0}};
    assert(cp::convex_hull(square) == std::vector<P>({{0, 0}, {2, 0}, {2, 2}, {0, 2}}));
    assert(cp::convex_hull(square, true) ==
           std::vector<P>({{0, 0}, {1, 0}, {2, 0}, {2, 2}, {0, 2}}));
    const auto b = cp::integer_geometry_coordinate_bound;
    std::vector<P> huge{{-b, -b}, {b, -b}, {b, b}, {-b, b}, {0, 0}, {-b, -b}};
    assert(cp::convex_hull(huge) == std::vector<P>({{-b, -b}, {b, -b}, {b, b}, {-b, b}}));
    assert(cp::cross(P{-b, -b}, P{b, -b}, P{b, b}) == (W(2) * b) * (W(2) * b));
    assert(cp::segments_intersect({-b, -b}, {b, b}, {-b, b}, {b, -b}));
    auto maximum = std::numeric_limits<std::int64_t>::max();
    P max_vector{maximum, maximum};
    assert(cp::dot(max_vector, max_vector) == (W(maximum) * maximum) * 2);
    assert(cp::cross(max_vector, P{-maximum, maximum}) == (W(maximum) * maximum) * 2);
    assert((P{b, b} - P{-b, -b} == P{2 * b, 2 * b}));
}
