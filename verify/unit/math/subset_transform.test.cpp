#include <cp/math/subset_transform.hpp>

#include <cassert>
#include <vector>

int main() {
    std::vector<long long> a{1, 2, 3, 4};
    cp::subset_zeta(a);
    assert((a == std::vector<long long>{1, 3, 4, 10}));
    cp::subset_mobius(a);
    assert((a == std::vector<long long>{1, 2, 3, 4}));
    cp::superset_zeta(a);
    assert((a == std::vector<long long>{10, 6, 7, 4}));
    cp::superset_mobius(a);
    assert((a == std::vector<long long>{1, 2, 3, 4}));
    std::vector<long long> singleton{-7};
    cp::subset_zeta(singleton); cp::subset_mobius(singleton);
    cp::superset_zeta(singleton); cp::superset_mobius(singleton);
    assert(singleton[0] == -7);
    std::vector<long long> zeros(16);
    cp::subset_zeta(zeros); cp::superset_mobius(zeros);
    assert(zeros == std::vector<long long>(16));
}
