// competitive-verifier: STANDALONE
#include <cassert>
#include <cp/math/matrix.hpp>
#include <cp/math/modint.hpp>
#include <numeric>
#include <random>
#include <set>
#include <type_traits>
struct NonzeroDefaultField : cp::StaticModint<5> {
    using Base = cp::StaticModint<5>;
    NonzeroDefaultField() : Base(1) {}
    explicit NonzeroDefaultField(int value) : Base(value) {}
    NonzeroDefaultField(Base value) : Base(value) {}
};
static_assert(!std::is_convertible_v<int, NonzeroDefaultField>);
using M = cp::modint998244353;
using A = cp::Matrix<M>;
M naive_det(const A &a) {
    std::vector<int> p(a.rows());
    std::iota(p.begin(), p.end(), 0);
    M sum = 0;
    do {
        M product = 1;
        int inversions = 0;
        for (int i = 0; i < a.rows(); i++) {
            product *= a(i, p[i]);
            for (int j = 0; j < i; j++)
                inversions += p[j] > p[i];
        }
        sum += (inversions % 2 ? -product : product);
    } while (std::next_permutation(p.begin(), p.end()));
    return sum;
}
int main() {
    std::mt19937 rng(739145);
    for (int t = 0; t < 100; t++) {
        int n = rng() % 7;
        A a(n, n), b(n, n), want(n, n);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++) {
                a(i, j) = rng();
                b(i, j) = rng();
            }
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                for (int k = 0; k < n; k++)
                    want(i, j) += a(i, k) * b(k, j);
        assert(a * b == want);
        int e = rng() % 8;
        A power = A::identity(n);
        for (int i = 0; i < e; i++)
            power *= a;
        assert(a.pow(e) == power);
        auto det = naive_det(a);
        assert(cp::determinant(a) == det);
        auto inv = cp::inverse(a);
        assert(bool(inv) == (det != M(0)));
        if (inv) {
            assert(a * (*inv) == A::identity(n));
            assert((*inv) * a == A::identity(n));
        }
    }
    using F = NonzeroDefaultField;
    using B = cp::Matrix<F>;
    for (int t = 0; t < 200; t++) {
        int rows = rng() % 5, cols = rng() % 5;
        B a(rows, cols);
        std::vector<F> b(rows);
        for (int i = 0; i < rows; i++) {
            b[i] = F(rng() % 5);
            for (int j = 0; j < cols; j++)
                a(i, j) = F(rng() % 5);
        }
        int assignments = 1;
        for (int i = 0; i < cols; i++)
            assignments *= 5;
        int solutions = 0;
        for (int mask = 0; mask < assignments; mask++) {
            int value = mask;
            std::vector<F> x(cols);
            for (auto &v : x) {
                v = F(value % 5);
                value /= 5;
            }
            bool good = true;
            for (int i = 0; i < rows; i++) {
                F sum = F(0);
                for (int j = 0; j < cols; j++)
                    sum += a(i, j) * x[j];
                good &= sum == b[i];
            }
            solutions += good;
        }
        auto result = cp::solve_linear_system(a, b);
        assert(bool(result) == (solutions > 0));
        if (result) {
            int count = 1;
            for (std::size_t k = 0; k < result->basis.size(); k++)
                count *= 5;
            assert(count == solutions);
            for (int i = 0; i < rows; i++) {
                F sum = F(0);
                for (int j = 0; j < cols; j++)
                    sum += a(i, j) * result->particular[j];
                assert(sum == b[i]);
                for (const auto &v : result->basis) {
                    F zero = F(0);
                    for (int j = 0; j < cols; j++)
                        zero += a(i, j) * v[j];
                    assert(zero == F(0));
                }
            }
        }
        std::set<std::vector<int>> span;
        int combinations = 1;
        for (int i = 0; i < rows; i++)
            combinations *= 5;
        for (int mask = 0; mask < combinations; mask++) {
            int value = mask;
            std::vector<F> v(cols, F(0));
            for (int i = 0; i < rows; i++) {
                F c = F(value % 5);
                value /= 5;
                for (int j = 0; j < cols; j++)
                    v[j] += c * a(i, j);
            }
            std::vector<int> key;
            for (auto x : v)
                key.push_back(x.val());
            span.insert(key);
        }
        int cardinality = 1;
        for (int i = 0; i < cp::matrix_rank(a); i++)
            cardinality *= 5;
        assert(span.size() == static_cast<std::size_t>(cardinality));
    }
}
