// competitive-verifier: STANDALONE
#include <cp/string/aho_corasick.hpp>
#include <algorithm>
#include <cassert>
#include <string>
#include <tuple>
#include <vector>
using Match = cp::AhoCorasick::Match;
void check(cp::AhoCorasick& ac, const std::vector<std::string>& patterns, const std::string& text) {
    ac.build(); ac.build();
    std::vector<Match> expected;
    std::vector<std::size_t> counts(patterns.size());
    for (std::size_t id = 0; id < patterns.size(); ++id)
        for (std::size_t begin = 0; begin + patterns[id].size() <= text.size(); ++begin)
            if (text.compare(begin, patterns[id].size(), patterns[id]) == 0) {
                expected.push_back({id, begin, begin + patterns[id].size()}); ++counts[id];
            }
    auto actual = ac.matches(text);
    std::vector<Match> callback;
    ac.for_each_match(text, [&](Match match) { callback.push_back(match); });
    assert(actual == callback);
    assert(std::is_sorted(actual.begin(), actual.end(), [](auto a, auto b) { return a.end < b.end; }));
    auto less = [](auto a, auto b) {return std::tie(a.pattern_id,a.begin,a.end) < std::tie(b.pattern_id,b.begin,b.end);};
    std::sort(expected.begin(), expected.end(), less); std::sort(actual.begin(), actual.end(), less);
    assert(actual == expected && ac.count_matches(text) == counts);
}
int main() {
    cp::AhoCorasick empty;
    assert(!empty.is_built() && empty.pattern_count() == 0 && empty.node_count() == 1);
    check(empty, {}, ""); check(empty, {}, "abc");
    assert(empty.failure_link(0) == 0);
    cp::AhoCorasick ac;
    std::vector<std::string> patterns{"", "he", "she", "hers", "his", "he", "", "s", "a", "aa", "aaa"};
    for (std::size_t i = 0; i < patterns.size(); ++i) {
        assert(ac.add_pattern(patterns[i]) == i);
        assert(ac.pattern_length(i) == patterns[i].size());
    }
    check(ac, patterns, "ushershisheaaaa"); check(ac, patterns, "");
    assert(ac.pattern_node(1) == ac.pattern_node(5));
    assert(ac.pattern_node(0) == 0 && ac.pattern_node(6) == 0);
    assert(ac.failure_link(ac.pattern_node(2)) == ac.pattern_node(1));
    patterns.push_back("hershis"); ac.add_pattern(patterns.back());
    assert(!ac.is_built()); check(ac, patterns, "ushershishe");
    patterns.push_back(std::string{'\0', static_cast<char>(255)}); ac.add_pattern(patterns.back());
    patterns.push_back(std::string(1, static_cast<char>(255))); ac.add_pattern(patterns.back());
    check(ac, patterns, std::string{'\0', static_cast<char>(255), '\0', static_cast<char>(255)});
    cp::AhoCorasick order;
    order.add_pattern("a"); order.add_pattern("aa"); order.add_pattern("a"); order.add_pattern(""); order.build();
    assert((order.matches("aa") == std::vector<Match>{
        {3,0,0}, {0,0,1}, {2,0,1}, {3,1,1},
        {1,0,2}, {0,1,2}, {2,1,2}, {3,2,2}
    }));
    cp::AhoCorasick nonterminal;
    nonterminal.add_pattern("abcd"); nonterminal.add_pattern("bce"); nonterminal.build();
    assert(nonterminal.failure_link(*nonterminal.trie().find_node("abc")) == *nonterminal.trie().find_node("bc"));
    auto copied = ac; copied.add_pattern("new"); copied.build();
    assert(ac.is_built() && copied.pattern_count() == ac.pattern_count() + 1);
}
