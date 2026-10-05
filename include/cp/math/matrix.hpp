#ifndef CP_MATH_MATRIX_HPP
#define CP_MATH_MATRIX_HPP
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <limits>
#include <optional>
#include <utility>
#include <vector>
namespace cp {
// Multiplication/power need a unital semiring; elimination needs an exact field.
template <class T> class Matrix {
    int rows_ = 0, columns_ = 0;
    std::vector<T> data_;

  public:
    Matrix() = default;
    Matrix(int rows, int columns, T value = T(0)) : rows_(rows), columns_(columns) {
        assert(rows >= 0 && columns >= 0);
        data_.assign(static_cast<std::size_t>(rows) * columns, value);
    }
    explicit Matrix(const std::vector<std::vector<T>> &rows)
        : Matrix(static_cast<int>(rows.size()),
                 rows.empty() ? 0 : static_cast<int>(rows[0].size())) {
        for (int i = 0; i < rows_; ++i) {
            assert(rows[i].size() == static_cast<std::size_t>(columns_));
            for (int j = 0; j < columns_; ++j)
                (*this)(i, j) = rows[i][j];
        }
    }
    int rows() const { return rows_; }
    int columns() const { return columns_; }
    T &operator()(int r, int c) {
        assert(0 <= r && r < rows_ && 0 <= c && c < columns_);
        return data_[static_cast<std::size_t>(r) * columns_ + c];
    }
    const T &operator()(int r, int c) const {
        assert(0 <= r && r < rows_ && 0 <= c && c < columns_);
        return data_[static_cast<std::size_t>(r) * columns_ + c];
    }
    const std::vector<T> &data() const { return data_; }
    static Matrix identity(int n) {
        Matrix result(n, n);
        for (int i = 0; i < n; ++i)
            result(i, i) = T(1);
        return result;
    }
    Matrix operator*(const Matrix &b) const {
        assert(columns_ == b.rows_);
        Matrix result(rows_, b.columns_);
        for (int i = 0; i < rows_; ++i)
            for (int k = 0; k < columns_; ++k) {
                T left = data_[static_cast<std::size_t>(i) * columns_ + k];
                if (left == T(0))
                    continue;
                for (int j = 0; j < b.columns_; ++j)
                    result.data_[static_cast<std::size_t>(i) * b.columns_ + j] +=
                        left * b.data_[static_cast<std::size_t>(k) * b.columns_ + j];
            }
        return result;
    }
    Matrix &operator*=(const Matrix &b) {
        *this = *this * b;
        return *this;
    }
    Matrix pow(std::uint64_t exponent) const {
        assert(rows_ == columns_);
        Matrix result = identity(rows_), base = *this;
        while (exponent) {
            if (exponent & 1)
                result *= base;
            exponent >>= 1;
            if (exponent)
                base *= base;
        }
        return result;
    }
    bool operator==(const Matrix &) const = default;
};
// Normalize pivots and eliminate above and below them. Pivot search is limited
// to coefficient_columns; all remaining columns still undergo row operations.
template <class T>
std::pair<Matrix<T>, std::vector<int>> gauss_jordan(Matrix<T> a, int coefficient_columns = -1) {
    assert(coefficient_columns >= -1);
    if (coefficient_columns < 0)
        coefficient_columns = a.columns();
    assert(coefficient_columns <= a.columns());
    int rank = 0;
    std::vector<int> pivots;
    for (int c = 0; c < coefficient_columns && rank < a.rows(); ++c) {
        int pivot = rank;
        while (pivot < a.rows() && a(pivot, c) == T(0))
            ++pivot;
        if (pivot == a.rows())
            continue;
        if (pivot != rank)
            for (int j = 0; j < a.columns(); ++j)
                std::swap(a(pivot, j), a(rank, j));
        T inverse = T(1) / a(rank, c);
        for (int j = c; j < a.columns(); ++j)
            a(rank, j) *= inverse;
        for (int i = 0; i < a.rows(); ++i)
            if (i != rank) {
                T factor = a(i, c);
                if (factor == T(0))
                    continue;
                for (int j = c; j < a.columns(); ++j)
                    a(i, j) -= factor * a(rank, j);
            }
        pivots.push_back(c);
        ++rank;
    }
    return {std::move(a), std::move(pivots)};
}
template <class T> int matrix_rank(const Matrix<T> &a) {
    return static_cast<int>(gauss_jordan(a).second.size());
}
template <class T> T determinant(Matrix<T> a) {
    assert(a.rows() == a.columns());
    T result = 1;
    for (int c = 0; c < a.rows(); ++c) {
        int pivot = c;
        while (pivot < a.rows() && a(pivot, c) == T(0))
            ++pivot;
        if (pivot == a.rows())
            return T(0);
        if (pivot != c) {
            for (int j = 0; j < a.columns(); ++j)
                std::swap(a(pivot, j), a(c, j));
            result = -result;
        }
        T value = a(c, c);
        result *= value;
        T inv = T(1) / value;
        for (int i = c + 1; i < a.rows(); ++i) {
            T factor = a(i, c) * inv;
            for (int j = c + 1; j < a.columns(); ++j)
                a(i, j) -= factor * a(c, j);
            a(i, c) = 0;
        }
    }
    return result;
}
template <class T> std::optional<Matrix<T>> inverse(const Matrix<T> &a) {
    assert(a.rows() == a.columns());
    int n = a.rows();
    assert(n <= std::numeric_limits<int>::max() / 2);
    Matrix<T> augmented(n, 2 * n);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j)
            augmented(i, j) = a(i, j);
        augmented(i, n + i) = 1;
    }
    auto [reduced, pivots] = gauss_jordan(std::move(augmented), n);
    if (static_cast<int>(pivots.size()) != n)
        return std::nullopt;
    Matrix<T> result(n, n);
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            result(i, j) = reduced(i, n + j);
    return result;
}
template <class T> struct LinearSystemSolution {
    std::vector<T> particular;
    std::vector<std::vector<T>> basis;
    std::vector<int> pivot_columns, free_columns;
};
// All solutions are particular + sum(lambda[i]*basis[i]).
template <class T>
std::optional<LinearSystemSolution<T>> solve_linear_system(const Matrix<T> &a,
                                                           const std::vector<T> &b) {
    assert(b.size() == static_cast<std::size_t>(a.rows()));
    int n = a.columns();
    assert(n < std::numeric_limits<int>::max());
    Matrix<T> augmented(a.rows(), n + 1);
    for (int i = 0; i < a.rows(); ++i) {
        for (int j = 0; j < n; ++j)
            augmented(i, j) = a(i, j);
        augmented(i, n) = b[i];
    }
    auto [reduced, pivots] = gauss_jordan(std::move(augmented), n);
    int rank = static_cast<int>(pivots.size());
    for (int i = rank; i < a.rows(); ++i)
        if (reduced(i, n) != T(0))
            return std::nullopt;
    LinearSystemSolution<T> result;
    result.particular.resize(n, T(0));
    result.pivot_columns = pivots;
    std::vector<bool> is_pivot(n);
    for (int i = 0; i < rank; ++i) {
        result.particular[pivots[i]] = reduced(i, n);
        is_pivot[pivots[i]] = true;
    }
    for (int c = 0; c < n; ++c)
        if (!is_pivot[c]) {
            result.free_columns.push_back(c);
            std::vector<T> v(n, T(0));
            v[c] = 1;
            for (int i = 0; i < rank; ++i)
                v[pivots[i]] = -reduced(i, c);
            result.basis.push_back(std::move(v));
        }
    return result;
}
} // namespace cp
#endif
