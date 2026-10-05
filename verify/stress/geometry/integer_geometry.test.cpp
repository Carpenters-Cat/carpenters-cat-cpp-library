// competitive-verifier: STANDALONE
#include <cassert>
#include <cp/geometry/integer_geometry.hpp>
#include <random>
#include <set>
using P = cp::IntPoint;
// Small-coordinate reference uses a six-term shoelace determinant, not widened differences.
long long area(P a, P b, P c) {
    return a.x * b.y + b.x * c.y + c.x * a.y - a.y * b.x - b.y * c.x - c.y * a.x;
}
bool between(P a, P b, P p) {
    return area(a, b, p) == 0 && (p.x - a.x) * (p.x - b.x) + (p.y - a.y) * (p.y - b.y) <= 0;
}
long long det(P a, P b) { return a.x * b.y - a.y * b.x; }
bool intersection(P a, P b, P c, P d) {
    P u{b.x - a.x, b.y - a.y}, v{d.x - c.x, d.y - c.y}, w{c.x - a.x, c.y - a.y};
    long long denominator = det(u, v), t = det(w, v), s = det(w, u);
    if (!denominator)
        return between(a, b, c) || between(a, b, d) || between(c, d, a) || between(c, d, b);
    if (denominator < 0) {
        denominator = -denominator;
        t = -t;
        s = -s;
    }
    return 0 <= t && t <= denominator && 0 <= s && s <= denominator;
}
long long distance_squared(P a, P b) {
    return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
}
std::vector<P> jarvis(std::vector<P> p) {
    std::sort(p.begin(), p.end());
    p.erase(std::unique(p.begin(), p.end()), p.end());
    if (p.size() <= 1)
        return p;
    std::vector<P> hull;
    P current = p.front();
    do {
        hull.push_back(current);
        P next = current == p.front() ? p.back() : p.front();
        for (auto q : p)
            if (q != current && (area(current, next, q) < 0 ||
                                 (area(current, next, q) == 0 &&
                                  distance_squared(current, q) > distance_squared(current, next))))
                next = q;
        current = next;
    } while (current != hull.front());
    return hull;
}
bool in_triangle(P a, P b, P c, P p) {
    if (area(a, b, c) == 0)
        return false;
    auto x = area(a, b, p), y = area(b, c, p), z = area(c, a, p);
    return (x >= 0 && y >= 0 && z >= 0) || (x <= 0 && y <= 0 && z <= 0);
}
// Enumerate pairs and triangles of all other points (Caratheodory in R^2).
std::set<P> brute_vertices(const std::vector<P> &points) {
    std::set<P> vertices;
    for (auto p : points) {
        bool redundant = false;
        for (std::size_t i = 0; i < points.size(); i++)
            if (points[i] != p)
                for (std::size_t j = i + 1; j < points.size(); j++)
                    if (points[j] != p) {
                        if (between(points[i], points[j], p))
                            redundant = true;
                        for (std::size_t k = j + 1; k < points.size(); k++)
                            if (points[k] != p && in_triangle(points[i], points[j], points[k], p))
                                redundant = true;
                    }
        if (!redundant)
            vertices.insert(p);
    }
    return vertices;
}
std::vector<P> with_boundary(const std::vector<P> &vertices, const std::vector<P> &points) {
    if (vertices.size() <= 1)
        return vertices;
    if (vertices.size() == 2)
        return points;
    std::vector<P> result;
    for (std::size_t i = 0; i < vertices.size(); i++) {
        P a = vertices[i], b = vertices[(i + 1) % vertices.size()];
        std::vector<P> edge;
        for (auto p : points)
            if (p != b && between(a, b, p))
                edge.push_back(p);
        std::sort(edge.begin(), edge.end(),
                  [a](P p, P q) { return distance_squared(a, p) < distance_squared(a, q); });
        result.insert(result.end(), edge.begin(), edge.end());
    }
    return result;
}
int main() {
    std::mt19937 rng(735819);
    auto random_point = [&]() {
        return P{static_cast<int>(rng() % 21) - 10, static_cast<int>(rng() % 21) - 10};
    };
    for (int t = 0; t < 20000; t++) {
        P a = random_point(), b = random_point(), c = random_point(), d = random_point();
        auto value = area(a, b, c);
        assert(cp::orientation(a, b, c) == (value > 0) - (value < 0));
        assert(cp::cross(a, b, c) == value);
        assert(cp::on_segment(a, b, c) == between(a, b, c));
        assert(cp::segments_intersect(a, b, c, d) == intersection(a, b, c, d));
        assert(cp::dot(a, b) == a.x * b.x + a.y * b.y && cp::cross(a, b) == det(a, b));
    }
    for (int t = 0; t < 1000; t++) {
        int n = rng() % 10;
        std::vector<P> p(n);
        for (auto &x : p)
            x = random_point();
        auto hull = cp::convex_hull(p);
        assert(hull == jarvis(p));
        std::sort(p.begin(), p.end());
        p.erase(std::unique(p.begin(), p.end()), p.end());
        assert(std::set<P>(hull.begin(), hull.end()) == brute_vertices(p));
        auto boundary = cp::convex_hull(p, true);
        assert(boundary == with_boundary(hull, p));
        assert(std::set<P>(boundary.begin(), boundary.end()).size() == boundary.size());
        for (auto x : p) {
            if (hull.size() == 1)
                assert(x == hull.front());
            if (hull.size() == 2)
                assert(between(hull[0], hull[1], x));
            if (hull.size() >= 3) {
                bool contained = false;
                for (std::size_t i = 1; i + 1 < hull.size(); i++)
                    contained |= in_triangle(hull[0], hull[i], hull[i + 1], x);
                assert(contained);
                for (std::size_t i = 0; i < hull.size(); i++)
                    assert(area(hull[i], hull[(i + 1) % hull.size()], x) >= 0);
            }
        }
    }
}
