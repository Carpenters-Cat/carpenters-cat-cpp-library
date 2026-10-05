// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/static_range_frequency
#include <cp/data_structure/wavelet_matrix.hpp>
#include <iostream>
#include <vector>
int main() {
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n, q; std::cin >> n >> q;
    std::vector<long long> values(n);
    for (auto& x : values) std::cin >> x;
    cp::WaveletMatrix<long long> matrix(values);
    while (q--) {
        int l, r; long long x; std::cin >> l >> r >> x;
        std::cout << matrix.frequency(l, r, x) << '\n';
    }
}
