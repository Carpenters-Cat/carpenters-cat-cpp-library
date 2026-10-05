#ifndef CP_GEOMETRY_INTEGER_GEOMETRY_HPP
#define CP_GEOMETRY_INTEGER_GEOMETRY_HPP
#include <algorithm>
#include <cassert>
#include <compare>
#include <cstdint>
#include <limits>
#include <vector>
namespace cp {
using GeometryWide = __int128_t;
inline constexpr std::int64_t integer_geometry_coordinate_bound = (std::int64_t(1) << 62) - 1;
struct IntPoint {
    std::int64_t x = 0, y = 0;
    auto operator<=>(const IntPoint &) const = default;

  private:
    static std::int64_t narrow(GeometryWide value) {
        assert(value >= std::numeric_limits<std::int64_t>::min() &&
               value <= std::numeric_limits<std::int64_t>::max());
        return static_cast<std::int64_t>(value);
    }

  public:
    IntPoint operator+(IntPoint b) const {
        return {narrow(GeometryWide(x) + b.x), narrow(GeometryWide(y) + b.y)};
    }
    IntPoint operator-(IntPoint b) const {
        return {narrow(GeometryWide(x) - b.x), narrow(GeometryWide(y) - b.y)};
    }
    IntPoint operator-() const { return {narrow(-GeometryWide(x)), narrow(-GeometryWide(y))}; }
    IntPoint operator*(std::int64_t scalar) const {
        return {narrow(GeometryWide(x) * scalar), narrow(GeometryWide(y) * scalar)};
    }
    IntPoint &operator+=(IntPoint b) { return *this = *this + b; }
    IntPoint &operator-=(IntPoint b) { return *this = *this - b; }
    IntPoint &operator*=(std::int64_t scalar) { return *this = *this * scalar; }
    friend IntPoint operator*(std::int64_t scalar, IntPoint p) { return p * scalar; }
};
using IntVector = IntPoint;
namespace integer_geometry_internal {
inline void check_point(IntPoint p) {
    assert(-integer_geometry_coordinate_bound <= p.x && p.x <= integer_geometry_coordinate_bound);
    assert(-integer_geometry_coordinate_bound <= p.y && p.y <= integer_geometry_coordinate_bound);
    (void)p;
}
inline void check_vector(IntVector v) {
    assert(v.x != std::numeric_limits<std::int64_t>::min() &&
           v.y != std::numeric_limits<std::int64_t>::min());
    (void)v;
}
} // namespace integer_geometry_internal
inline GeometryWide dot(IntVector a, IntVector b) {
    integer_geometry_internal::check_vector(a);
    integer_geometry_internal::check_vector(b);
    return GeometryWide(a.x) * b.x + GeometryWide(a.y) * b.y;
}
inline GeometryWide cross(IntVector a, IntVector b) {
    integer_geometry_internal::check_vector(a);
    integer_geometry_internal::check_vector(b);
    return GeometryWide(a.x) * b.y - GeometryWide(a.y) * b.x;
}
// Signed twice-area of triangle abc. Differences are widened before subtraction.
inline GeometryWide cross(IntPoint a, IntPoint b, IntPoint c) {
    integer_geometry_internal::check_point(a);
    integer_geometry_internal::check_point(b);
    integer_geometry_internal::check_point(c);
    GeometryWide bx = GeometryWide(b.x) - a.x, by = GeometryWide(b.y) - a.y;
    GeometryWide cx = GeometryWide(c.x) - a.x, cy = GeometryWide(c.y) - a.y;
    return bx * cy - by * cx;
}
inline int orientation(IntPoint a, IntPoint b, IntPoint c) {
    auto value = cross(a, b, c);
    return (value > 0) - (value < 0);
}
inline bool on_segment(IntPoint a, IntPoint b, IntPoint p) {
    return orientation(a, b, p) == 0 && std::min(a.x, b.x) <= p.x && p.x <= std::max(a.x, b.x) &&
           std::min(a.y, b.y) <= p.y && p.y <= std::max(a.y, b.y);
}
// Closed segments intersect: endpoints, overlaps and zero-length segments count.
inline bool segments_intersect(IntPoint a, IntPoint b, IntPoint c, IntPoint d) {
    integer_geometry_internal::check_point(a);
    integer_geometry_internal::check_point(b);
    integer_geometry_internal::check_point(c);
    integer_geometry_internal::check_point(d);
    if (std::max(std::min(a.x, b.x), std::min(c.x, d.x)) >
            std::min(std::max(a.x, b.x), std::max(c.x, d.x)) ||
        std::max(std::min(a.y, b.y), std::min(c.y, d.y)) >
            std::min(std::max(a.y, b.y), std::max(c.y, d.y)))
        return false;
    return orientation(a, b, c) * orientation(a, b, d) <= 0 &&
           orientation(c, d, a) * orientation(c, d, b) <= 0;
}
// CCW, starts at lexicographically smallest point, no repeated closing vertex.
// All-collinear input: endpoints by default; all sorted points when keeping collinear.
inline std::vector<IntPoint> convex_hull(std::vector<IntPoint> points,
                                         bool keep_collinear = false) {
    for (auto p : points)
        integer_geometry_internal::check_point(p);
    std::sort(points.begin(), points.end());
    points.erase(std::unique(points.begin(), points.end()), points.end());
    if (points.size() <= 2)
        return points;
    bool collinear = true;
    for (auto p : points)
        if (orientation(points.front(), points.back(), p) != 0) {
            collinear = false;
            break;
        }
    if (collinear)
        return keep_collinear ? points : std::vector<IntPoint>{points.front(), points.back()};
    std::vector<IntPoint> lower, upper;
    auto append = [keep_collinear](std::vector<IntPoint> &chain, IntPoint p) {
        while (chain.size() >= 2) {
            int turn = orientation(chain[chain.size() - 2], chain.back(), p);
            if (turn > 0 || (keep_collinear && turn == 0))
                break;
            chain.pop_back();
        }
        chain.push_back(p);
    };
    for (auto p : points)
        append(lower, p);
    for (auto it = points.rbegin(); it != points.rend(); ++it)
        append(upper, *it);
    lower.pop_back();
    upper.pop_back();
    lower.insert(lower.end(), upper.begin(), upper.end());
    return lower;
}
} // namespace cp
#endif
