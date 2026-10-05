// competitive-verifier: STANDALONE
#include <cp/string/aho_corasick.hpp>
#include <algorithm>
#include <cassert>
#include <random>
#include <string>
#include <tuple>
#include <vector>
using Match = cp::AhoCorasick::Match;
int main() {
    std::mt19937 rng(29471);
    for (int trial = 0; trial < 400; ++trial) {
        cp::AhoCorasick ac;
        std::vector<std::string> patterns;
        for (unsigned p = rng() % 35; p--; ) {
            std::string word;
            if (!patterns.empty() && rng() % 3 == 0) word = patterns[rng() % patterns.size()];
            else for (unsigned n = rng() % 12; n--; ) {
                const unsigned bytes[]{0, 97, 98, 128, 255};
                word += static_cast<char>(bytes[rng() % 5]);
            }
            patterns.push_back(word); ac.add_pattern(word);
            if (rng() % 5 == 0) ac.build(); // Rebuilding after further registration.
        }
        ac.build();
        std::string text;
        for (unsigned n = rng() % 80; n--; ) {
            const unsigned bytes[]{0, 97, 98, 128, 255};
            text += static_cast<char>(bytes[rng() % 5]);
        }
        std::vector<Match> expected;
        std::vector<std::size_t> counts(patterns.size());
        for (std::size_t p = 0; p < patterns.size(); ++p)
            for (std::size_t i = 0; i + patterns[p].size() <= text.size(); ++i)
                if (text.compare(i, patterns[p].size(), patterns[p]) == 0) {
                    expected.push_back({p, i, i + patterns[p].size()}); ++counts[p];
                }
        auto actual = ac.matches(text);
        auto less = [](auto a, auto b) {return std::tie(a.pattern_id,a.begin,a.end) < std::tie(b.pattern_id,b.begin,b.end);};
        std::sort(actual.begin(), actual.end(), less); std::sort(expected.begin(), expected.end(), less);
        assert(actual == expected && ac.count_matches(text) == counts);
        // Failure link is the longest proper suffix present anywhere in the trie.
        std::vector<std::string> paths(ac.node_count());
        for (std::size_t v = 1; v < ac.node_count(); ++v) {
            paths[v] = paths[ac.trie().parent(static_cast<int>(v))] + static_cast<char>(ac.trie().symbol(static_cast<int>(v)));
            int expected_link = 0;
            for (std::size_t i = 1; i < paths[v].size(); ++i) {
                auto suffix = ac.trie().find_node(std::string_view(paths[v]).substr(i));
                if (suffix) { expected_link = *suffix; break; }
            }
            assert(ac.failure_link(static_cast<int>(v)) == expected_link);
        }
    }
    // Output count is enormous: counts must not enumerate all occurrences or copy suffix ID lists.
    cp::AhoCorasick chain;
    std::string pattern;
    for (int i = 1; i <= 4000; ++i) { pattern += 'a'; chain.add_pattern(pattern); }
    chain.add_pattern(""); chain.build();
    std::string text(100000, 'a');
    auto counts = chain.count_matches(text);
    for (int i = 1; i <= 4000; ++i) assert(counts[i - 1] == text.size() - i + 1);
    assert(counts.back() == text.size() + 1);
    cp::AhoCorasick reporting;
    reporting.add_pattern(std::string(10000, 'a')); reporting.build();
    std::size_t seen = 0;
    reporting.for_each_match(text, [&](Match match) {
        assert(match.pattern_id == 0 && match.begin == seen && match.end == seen + 10000); ++seen;
    });
    assert(seen == text.size() - 10000 + 1);
    cp::AhoCorasick fanout;
    for (int c = 0; c < 256; ++c) fanout.add_pattern(std::string(1, static_cast<char>(c)));
    fanout.build();
    std::string all_bytes;
    for (int c = 0; c < 256; ++c) all_bytes += static_cast<char>(c);
    for (auto count : fanout.count_matches(all_bytes)) assert(count == 1);
}
