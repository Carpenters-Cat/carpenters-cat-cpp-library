// competitive-verifier: STANDALONE
#include <cassert>
#include <cp/math/matrix.hpp>
#include <cp/math/modint.hpp>
#include <type_traits>
// An exact field need not use its additive identity as the default value.
struct NonzeroDefaultField : cp::StaticModint<5> {
    using Base = cp::StaticModint<5>;
    NonzeroDefaultField() : Base(1) {}
    explicit NonzeroDefaultField(int value) : Base(value) {}
    NonzeroDefaultField(Base value) : Base(value) {}
};
static_assert(!std::is_convertible_v<int, NonzeroDefaultField>);
void check_nonzero_default_field() {
    using F = NonzeroDefaultField;
    using A = cp::Matrix<F>;
    assert(F() == F(1));
    A equation(std::vector<std::vector<F>>{{F(0), F(1), F(1)}});
    auto homogeneous = cp::solve_linear_system(equation, std::vector<F>{F(0)});
    assert(homogeneous && homogeneous->particular == std::vector<F>({F(0), F(0), F(0)}));
    assert(homogeneous->basis ==
           std::vector<std::vector<F>>({{F(1), F(0), F(0)}, {F(0), F(4), F(1)}}));
    auto affine = cp::solve_linear_system(equation, std::vector<F>{F(3)});
    assert(affine && affine->particular == std::vector<F>({F(0), F(3), F(0)}));
    assert(affine->basis == homogeneous->basis);
    auto unconstrained = cp::solve_linear_system(A(0, 3), std::vector<F>{});
    assert(unconstrained && unconstrained->particular == std::vector<F>({F(0), F(0), F(0)}));
    assert(unconstrained->basis ==
           std::vector<std::vector<F>>(
               {{F(1), F(0), F(0)}, {F(0), F(1), F(0)}, {F(0), F(0), F(1)}}));
    auto zero = cp::solve_linear_system(A(2, 3), std::vector<F>{F(0), F(0)});
    assert(zero && zero->particular == unconstrained->particular &&
           zero->basis == unconstrained->basis);
    A swap(std::vector<std::vector<F>>{{F(0), F(1)}, {F(1), F(0)}});
    auto unique = cp::solve_linear_system(swap, std::vector<F>{F(2), F(3)});
    assert(unique && unique->particular == std::vector<F>({F(3), F(2)}) &&
           unique->basis.empty());
    assert(!cp::solve_linear_system(A(1, 2), std::vector<F>{F(1)}));
    auto no_variables = cp::solve_linear_system(A(2, 0), std::vector<F>{F(0), F(0)});
    assert(no_variables && no_variables->particular.empty() && no_variables->basis.empty());

    // Expected reductions, determinants and inverses over F_5.
    auto check_reduction = [](const A &a, const A &expected, const std::vector<int> &pivots) {
        auto [reduced, actual_pivots] = cp::gauss_jordan(a);
        assert(reduced == expected && actual_pivots == pivots);
        assert(cp::matrix_rank(a) == static_cast<int>(pivots.size()));
    };
    A empty;
    check_reduction(empty, empty, {});
    assert(cp::determinant(empty) == F(1) && cp::inverse(empty).value() == empty);
    A scalar(std::vector<std::vector<F>>{{F(2)}});
    check_reduction(scalar, A::identity(1), {0});
    assert(cp::determinant(scalar) == F(2));
    assert(cp::inverse(scalar).value() == A(std::vector<std::vector<F>>{{F(3)}}));
    check_reduction(swap, A::identity(2), {0, 1});
    assert(cp::determinant(swap) == F(4) && cp::inverse(swap).value() == swap);
    A singular(std::vector<std::vector<F>>{{F(1), F(2)}, {F(2), F(4)}});
    check_reduction(singular, A(std::vector<std::vector<F>>{{F(1), F(2)}, {F(0), F(0)}}),
                    {0});
    assert(cp::determinant(singular) == F(0) && !cp::inverse(singular));
    A diagonal(std::vector<std::vector<F>>{{F(2), F(0)}, {F(0), F(3)}});
    check_reduction(diagonal, A::identity(2), {0, 1});
    assert(cp::determinant(diagonal) == F(1));
    auto inverted = cp::inverse(diagonal).value();
    assert(inverted == A(std::vector<std::vector<F>>{{F(3), F(0)}, {F(0), F(2)}}));
    assert(diagonal * inverted == A::identity(2) && inverted * diagonal == A::identity(2));
}
int main() {
    check_nonzero_default_field();
    using M = cp::modint998244353;
    using A = cp::Matrix<M>;
    A empty;
    assert(empty.rows() == 0 && empty.columns() == 0 && cp::determinant(empty) == M(1));
    assert(cp::inverse(empty).value() == empty && empty.pow(0) == empty &&
           cp::matrix_rank(empty) == 0);
    A swap(std::vector<std::vector<M>>{{0, 1}, {1, 0}});
    assert(cp::determinant(swap) == M(-1));
    assert(cp::inverse(swap).value() == swap && swap.pow(2) == A::identity(2));
    A singular(std::vector<std::vector<M>>{{1, 2}, {2, 4}});
    assert(!cp::inverse(singular) && cp::matrix_rank(singular) == 1);
    assert(!cp::solve_linear_system(singular, std::vector<M>{1, 3}));
    auto many = cp::solve_linear_system(singular, std::vector<M>{3, 6});
    assert(many && many->basis.size() == 1 && many->free_columns == std::vector<int>{1});
    auto unique = cp::solve_linear_system(swap, std::vector<M>{2, 3});
    assert(unique && unique->particular == std::vector<M>({3, 2}) && unique->basis.empty());
    auto [reduced, pivots] = cp::gauss_jordan(swap);
    assert(reduced == A::identity(2) && pivots == std::vector<int>({0, 1}));
    auto unconstrained = cp::solve_linear_system(A(0, 3), std::vector<M>{});
    assert(unconstrained && unconstrained->basis.size() == 3);
    assert(cp::solve_linear_system(A(2, 0), std::vector<M>{0, 0}));
    assert(!cp::solve_linear_system(A(2, 0), std::vector<M>{0, 1}));
    assert((A(3, 0) * A(0, 4)) == A(3, 4));
    assert((A(0, 3) * A(3, 4)) == A(0, 4));
    cp::Matrix<long long> fibonacci(std::vector<std::vector<long long>>{{1, 1}, {1, 0}});
    assert(fibonacci.pow(10)(0, 1) == 55);
    using B = cp::StaticModint<2>;
    cp::Matrix<B> bit(std::vector<std::vector<B>>{{0, 1}, {1, 0}});
    assert(cp::determinant(bit) == B(1));
}
