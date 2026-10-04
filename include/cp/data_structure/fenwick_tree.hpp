#pragma once

#include <cassert>
#include <type_traits>
#include <vector>

namespace cp {
namespace detail {
template <class T, bool = std::is_integral_v<T>>
struct FenwickStorage { using type = T; };
template <class T>
struct FenwickStorage<T, true> { using type = std::make_unsigned_t<T>; };
}  // namespace detail

// Point addition and half-open range sums over an additive commutative group.
template <class T>
class FenwickTree {
    using Storage = typename detail::FenwickStorage<T>::type;
public:
    explicit FenwickTree(int n = 0) {
        assert(0 <= n && n <= 100000000);
        data_.resize(n);
    }
    explicit FenwickTree(const std::vector<T>& values) : FenwickTree(static_cast<int>(values.size())) {
        for (int i = 0; i < size(); ++i) {
            data_[i] += static_cast<Storage>(values[i]);
            const int parent = i | (i + 1);
            if (parent < size()) data_[parent] += data_[i];
        }
    }
    int size() const { return static_cast<int>(data_.size()); }
    void add(int p, T value) {
        assert(0 <= p && p < size());
        for (int i = p; i < size(); i |= i + 1) data_[i] += static_cast<Storage>(value);
    }
    T sum(int l, int r) const {
        assert(0 <= l && l <= r && r <= size());
        return static_cast<T>(prefix(r) - prefix(l));
    }
private:
    Storage prefix(int r) const {
        Storage result{};
        for (; r > 0; r &= r - 1) result += data_[r - 1];
        return result;
    }
    std::vector<Storage> data_;
};

template <class T> using fenwick_tree = FenwickTree<T>;
}  // namespace cp
