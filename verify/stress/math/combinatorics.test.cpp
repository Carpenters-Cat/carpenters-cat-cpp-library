// competitive-verifier: STANDALONE
#include <cassert>
#include <cp/math/combinatorics.hpp>
#include <random>
int main() {
    using M = cp::modint998244353;
    cp::Combinatorics<M> c;
    std::vector<std::vector<M>> pascal(301);
    for (int n = 0; n <= 300; n++) {
        pascal[n].resize(n + 1);
        pascal[n][0] = pascal[n][n] = 1;
        for (int r = 1; r < n; r++)
            pascal[n][r] = pascal[n - 1][r - 1] + pascal[n - 1][r];
    }
    std::mt19937 rng(152637);
    for (int t = 0; t < 5000; t++) {
        int n = rng() % 301, r = rng() % (n + 1);
        M p = 1;
        for (int i = 0; i < r; i++)
            p *= n - i;
        assert(c.nCr(n, r) == pascal[n][r]);
        assert(c.nPr(n, r) == p);
        assert(c.factorial(n) * c.inverse_factorial(n) == M(1));
    }
}
