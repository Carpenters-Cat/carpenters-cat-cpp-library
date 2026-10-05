// competitive-verifier: STANDALONE
#include <cp/string/trie.hpp>
#include <cassert>
#include <string>
int main() {
    cp::Trie trie;
    assert(trie.size() == 0 && trie.node_count() == 1);
    assert(trie.find_node("") == 0 && !trie.has_prefix(""));
    assert(!trie.contains("") && !trie.find_node("a"));
    assert(trie.parent(0) == -1 && trie.terminal_count(0) == 0);
    auto first = trie.insert("apple");
    assert(trie.insert("apple") == first);
    auto short_word = trie.insert("app");
    assert(trie.node_count() == 6 && short_word == 3);
    assert(trie.size() == 3 && trie.count("apple") == 2);
    assert(trie.count("app") == 1 && trie.count("ap") == 0);
    assert(trie.has_prefix("ap") && trie.prefix_count("app") == 3);
    assert(!trie.contains("ap") && !trie.has_prefix("apples"));
    assert(trie.insert("") == 0 && trie.insert("") == 0);
    assert(trie.count("") == 2 && trie.prefix_count("") == 5);
    const std::string bytes{'\0', static_cast<char>(255), static_cast<char>(128)};
    auto binary = trie.insert(bytes);
    assert(trie.count(bytes) == 1 && trie.prefix_count(std::string(1, '\0')) == 1);
    assert(trie.symbol(binary) == 128);
    assert(trie.transition(trie.parent(binary), 128) == binary);
    // All byte values, irrespective of whether plain char is signed.
    for (int c = 255; c >= 0; --c) trie.insert(std::string(1, static_cast<char>(c)));
    auto edges = trie.children(0);
    assert(edges.size() == 256);
    for (int c = 0; c < 256; ++c) {
        assert(edges[c].symbol == c);
        assert(trie.contains(std::string(1, static_cast<char>(c))));
    }
    assert(trie.count("apple") == 2 && trie.terminal_count(first) == 2);
    auto copy = trie;
    copy.insert("independent");
    assert(!trie.contains("independent") && copy.contains("independent"));
}
