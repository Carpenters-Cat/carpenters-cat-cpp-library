// competitive-verifier: STANDALONE
#include <cp/string/trie.hpp>
#include <algorithm>
#include <cassert>
#include <random>
#include <string>
#include <vector>
int main() {
    std::mt19937 rng(28471);
    for (int trial = 0; trial < 70; ++trial) {
        cp::Trie trie;
        std::vector<std::string> words;
        for (int step = 0; step < 300; ++step) {
            std::string word;
            if (!words.empty() && rng() % 4 == 0) word = words[rng() % words.size()];
            else for (unsigned n = rng() % 13; n--; ) {
                const unsigned bytes[]{0, 1, 97, 98, 128, 255};
                word += static_cast<char>(bytes[rng() % 6]);
            }
            auto node = trie.insert(word);
            words.push_back(word);
            assert(trie.size() == words.size());
            assert(trie.find_node(word) == node);
            for (int probe = 0; probe < 10; ++probe) {
                auto query = words[rng() % words.size()];
                if (rng() % 2) query.resize(rng() % (query.size() + 1));
                if (rng() % 4 == 0) query += static_cast<char>(rng() % 256);
                std::size_t exact = 0, prefix = 0;
                for (const auto& w : words) { exact += w == query; prefix += w.starts_with(query); }
                assert(trie.count(query) == exact && trie.contains(query) == (exact != 0));
                assert(trie.prefix_count(query) == prefix && trie.has_prefix(query) == (prefix != 0));
            }
            // Every node's parent/label path corresponds to the canonical trie path.
            if (step % 100 == 0) for (std::size_t v = 1; v < trie.node_count(); ++v) {
                auto parent = trie.parent(static_cast<int>(v));
                assert(parent < static_cast<int>(v));
                assert(trie.transition(parent, trie.symbol(static_cast<int>(v))) == v);
                auto edges = trie.children(static_cast<int>(v));
                assert(std::is_sorted(edges.begin(), edges.end(), [](auto a, auto b) {return a.symbol < b.symbol;}));
            }
        }
    }
    cp::Trie chain;
    std::string long_word(200000, 'x');
    chain.insert(long_word); chain.insert(long_word);
    assert(chain.node_count() == long_word.size() + 1 && chain.count(long_word) == 2);
    assert(chain.prefix_count(std::string_view(long_word).substr(0, 150000)) == 2);
}
