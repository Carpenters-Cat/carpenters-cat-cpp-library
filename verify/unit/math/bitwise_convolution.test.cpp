#include <cp/math/bitwise_convolution.hpp>
#include <cp/math/modint.hpp>

#include <cassert>
#include <vector>

int main() {
    std::vector<long long> a{1, 2}, b{3, 4};
    assert((cp::and_convolution(a, b) == std::vector<long long>{13, 8}));
    assert((cp::or_convolution(a, b) == std::vector<long long>{3, 18}));
    assert((cp::xor_convolution(a, b) == std::vector<long long>{11, 10}));
    for (auto f : {cp::and_convolution<long long>, cp::or_convolution<long long>, cp::xor_convolution<long long>}) {
        assert(f({}, {}).empty());
        assert(f({}, {1, 2, 3}) == std::vector<long long>(4));
        assert(f({1, 2, 3}, {}) == std::vector<long long>(4));
        assert((f({-2}, {3}) == std::vector<long long>{-6}));
    }
    assert((cp::and_convolution(std::vector<long long>{1}, {4, 5, 6}) == std::vector<long long>{15, 0, 0, 0}));
    assert((cp::or_convolution(std::vector<long long>{1}, {4, 5, 6}) == std::vector<long long>{4, 5, 6, 0}));
    assert((cp::xor_convolution(std::vector<long long>{1}, {4, 5, 6}) == std::vector<long long>{4, 5, 6, 0}));
    std::vector<long long> w{1, 2, 3, 4};
    cp::walsh_hadamard(w);
    assert((w == std::vector<long long>{10, -2, -4, 0}));
    cp::walsh_hadamard(w, true);
    assert((w == std::vector<long long>{1, 2, 3, 4}));
    using Mint = cp::modint998244353;
    auto c = cp::xor_convolution(std::vector<Mint>{-1, 2}, std::vector<Mint>{3, -4});
    assert(c[0] == -11 && c[1] == 10);
    using Composite = cp::static_modint<15>;
    auto d = cp::xor_convolution(std::vector<Composite>{1, 2}, std::vector<Composite>{3, 4});
    assert(d[0] == 11 && d[1] == 10);
}
