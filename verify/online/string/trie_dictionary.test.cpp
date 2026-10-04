// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/ALDS1_4_C
#include <cp/string/trie.hpp>
#include <iostream>
#include <string>
int main() {
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n; std::cin >> n;
    cp::Trie trie;
    while (n--) {
        std::string command, word; std::cin >> command >> word;
        if (command == "insert") trie.insert(word);
        else std::cout << (trie.contains(word) ? "yes" : "no") << '\n';
    }
}
