// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/aho_corasick
#include <cp/string/aho_corasick.hpp>
#include <iostream>
#include <string>
int main() {
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n; std::cin >> n;
    cp::AhoCorasick ac;
    for (int i = 0; i < n; ++i) {std::string pattern; std::cin >> pattern; ac.add_pattern(pattern);}
    ac.build();
    std::cout << ac.node_count() << '\n';
    for (std::size_t v = 1; v < ac.node_count(); ++v)
        std::cout << ac.trie().parent(static_cast<int>(v)) << ' ' << ac.failure_link(static_cast<int>(v)) << '\n';
    for (int i = 0; i < n; ++i) std::cout << ac.pattern_node(i) << (i + 1 == n ? '\n' : ' ');
}
