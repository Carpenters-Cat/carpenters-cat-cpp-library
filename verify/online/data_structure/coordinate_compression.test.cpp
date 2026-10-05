// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/static_range_frequency
#include <cp/data_structure/coordinate_compression.hpp>

#include <algorithm>
#include <iostream>
#include <vector>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n, q;
    std::cin >> n >> q;
    std::vector<long long> a(n);
    for (auto& x : a) std::cin >> x;
    cp::CoordinateCompression<long long> c(a);
    std::vector<std::vector<int>> positions(c.size());
    for (int i = 0; i < n; ++i) positions[*c.index(a[i])].push_back(i);
    while (q--) {
        int l, r;
        long long x;
        std::cin >> l >> r >> x;
        const auto rank = c.index(x);
        if (!rank) {
            std::cout << 0 << '\n';
            continue;
        }
        const auto& p = positions[*rank];
        std::cout << std::lower_bound(p.begin(), p.end(), r) - std::lower_bound(p.begin(), p.end(), l) << '\n';
    }
}
