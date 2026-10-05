// competitive-verifier: STANDALONE
#include <cp/data_structure/li_chao_tree.hpp>
#include <cassert>
#include <limits>
#include <vector>
int main() {
    cp::LiChaoTree<> empty;
    assert(empty.size() == 0 && !empty.query(0));
    empty.add_line(1, 2); empty.add_segment(-3, 3, 0, 7);
    assert(!empty.query(0));
    cp::LiChaoTree<> tree({-5, -2, 0, 2, 5, -2});
    assert(tree.size() == 5 && tree.contains(-2) && !tree.contains(1));
    assert(!tree.query(0));
    tree.add_line(1, 0); tree.add_line(-1, 0);
    assert(tree.query(-5) == -5 && tree.query(0) == 0 && tree.query(5) == -5);
    tree.add_line(1, 4); tree.add_line(1, -1); // equal slopes, inferior and superior
    assert(tree.query(-5) == -6 && tree.query(5) == -5);
    tree.add_segment(-1, 3, {0, -10});
    assert(tree.query(-2) == -3 && tree.query(0) == -10 && tree.query(2) == -10);
    assert(tree.query(5) == -5 && !tree.query(1));
    tree.add_segment(0, 0, 0, -100); // empty segment
    tree.add_segment(6, 8, 0, -100); // no registered coordinates
    assert(tree.query(5) == -5);
    cp::LiChaoTree<> endpoints({-2, -1, 0, 1, 2});
    endpoints.add_segment(-1, 1, 0, 7);
    assert(!endpoints.query(-2) && endpoints.query(-1) == 7 && endpoints.query(0) == 7 && !endpoints.query(1));
    cp::LiChaoTree<long long, long long, false> maximum({-5, 0, 5});
    maximum.add_line(1, 0); maximum.add_line(-1, 0);
    maximum.add_segment(-1, 1, 0, 10);
    assert(maximum.query(-5) == 5 && maximum.query(0) == 10 && maximum.query(5) == 5);
    constexpr auto lo = std::numeric_limits<long long>::min(), hi = std::numeric_limits<long long>::max();
    cp::LiChaoTree<> extremes({lo, 0, hi});
    extremes.add_line(1, 0); extremes.add_line(0, 0);
    assert(extremes.query(lo) == lo && extremes.query(hi) == 0);
    extremes.add_segment(lo, hi, 0, -1);
    assert(extremes.query(0) == -1 && extremes.query(hi) == 0);
    cp::LiChaoTree<long long, long long, false> max_extremes({lo, 0, hi});
    max_extremes.add_line(1, 0); max_extremes.add_line(0, 0);
    assert(max_extremes.query(lo) == 0 && max_extremes.query(hi) == hi);
    cp::LiChaoTree<int, long long, false> widened({std::numeric_limits<int>::min(), std::numeric_limits<int>::max()});
    widened.add_line(100000000LL, 1000000000000000000LL);
    widened.add_line(0LL, 1000000000000000000LL);
    assert(widened.query(std::numeric_limits<int>::min()) == 1000000000000000000LL);
    assert(widened.query(std::numeric_limits<int>::max()) == 1000000000000000000LL + 100000000LL * std::numeric_limits<int>::max());
    cp::LiChaoTree<> single({3});
    single.add_line(2, 1); single.add_line(-1, 20);
    assert(single.query(3) == 7);
}
