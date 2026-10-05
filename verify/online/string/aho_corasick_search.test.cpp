// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/ALDS1_14_D
#include <cp/string/aho_corasick.hpp>
#include <iostream>
#include <string>
int main() {
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    std::string text; int n; std::cin >> text >> n;
    cp::AhoCorasick ac;
    for (int i = 0; i < n; ++i) {std::string pattern; std::cin >> pattern; ac.add_pattern(pattern);}
    ac.build();
    for (auto count : ac.count_matches(text)) std::cout << (count != 0) << '\n';
}
