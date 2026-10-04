// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/range_kth_smallest
#include <cp/data_structure/wavelet_matrix.hpp>
#include <iostream>
#include <vector>
int main() {
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n, q; std::cin >> n >> q;
    std::vector<long long> values(n);
    for (auto& x : values) std::cin >> x;
    cp::WaveletMatrix<long long> matrix(values);
    while (q--) { int l, r, k; std::cin >> l >> r >> k; std::cout << *matrix.kth_smallest(l, r, k) << '\n'; }
}
