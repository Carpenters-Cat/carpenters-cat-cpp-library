// competitive-verifier: STANDALONE
#include <cassert>
#include <cp/math/formal_power_series.hpp>
#include <random>
using M = cp::modint998244353;
using F = cp::FPS<M>;
F naive_product(const F &a, const F &b, int n) {
    F r(n);
    for (int i = 0; i < static_cast<int>(a.size()); i++)
        for (int j = 0; j < static_cast<int>(b.size()) && i + j < n; j++)
            r[i + j] += a[i] * b[j];
    return r;
}
int main() {
    std::mt19937 rng(647382);
    for (int t = 0; t < 120; t++) {
        int n = 1 + rng() % 100;
        F f(n), g(n);
        for (auto &x : f)
            x = rng();
        for (auto &x : g)
            x = rng();
        f[0] = 1;
        g[0] = 1;
        assert(naive_product(f, f.inv(n), n) == F{1}.truncated(n));
        assert(f.log(n).exp(n) == f);
        auto exponential = f;
        exponential[0] = 0;
        assert(exponential.exp(n).log(n) == exponential);
        assert(naive_product(f.divide(g, n), g, n) == f);
        assert(naive_product(f, g, 2 * n - 1) == f * g);
        auto [q, r] = (f * g).divmod(g);
        assert(q == f && r.empty());
        int divisor = 1 + rng() % n;
        F h(divisor);
        for (auto &x : h)
            x = rng();
        h.back() = 1;
        auto [dq, dr] = f.divmod(h);
        F reconstruct = dq * h + dr;
        assert(reconstruct.truncated(n) == f && dr.degree() < h.degree());
        assert(f.derivative().integral() == (f - M(1)));
        std::uint64_t e = rng() % 6;
        int zeros = rng() % std::min(n, 4);
        for (int i = 0; i < zeros; i++)
            f[i] = 0;
        F want(n);
        want[0] = 1;
        for (std::uint64_t i = 0; i < e; i++)
            want = naive_product(want, f, n);
        assert(f.pow(e, n) == want);
        auto square = naive_product(f, f, n);
        auto root = square.sqrt(n);
        assert(root && naive_product(*root, *root, n) == square);
        M c = rng();
        F shifted = f.taylor_shift(c), direct(n);
        cp::Combinatorics<M> table(n);
        for (int i = 0; i < n; i++)
            for (int j = 0; j <= i; j++)
                direct[j] += f[i] * table.nCr(i, j) * c.pow(i - j);
        assert(shifted == direct);
        std::vector<M> points(n);
        for (int i = 0; i < n; i++)
            points[i] = M(i * 17 + 3);
        auto values = cp::multipoint_evaluate(f, points);
        for (int i = 0; i < n; i++)
            assert(values[i] == f.evaluate(points[i]));
        assert(cp::interpolate(points, values) == f);
    }
    // Nonquadratic paths: NTT/Newton and product trees with several thousand coefficients.
    int n = 4096;
    F f(n);
    for (auto &x : f)
        x = rng();
    f[0] = 1;
    assert(f.log(n).exp(n) == f);
    std::vector<M> x(n);
    for (int i = 0; i < n; i++)
        x[i] = i;
    auto y = cp::multipoint_evaluate(f, x);
    assert(cp::interpolate(x, y) == f);
}
