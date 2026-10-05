#pragma once

#include <cp/string/trie.hpp>

#include <cassert>
#include <cstddef>
#include <string_view>
#include <vector>

namespace cp {

// Sparse byte trie, failure links, and terminal output links without suffix-list copies.
class AhoCorasick {
public:
    using NodeId = Trie::NodeId;
    using PatternId = std::size_t;
    struct Match {
        PatternId pattern_id;
        std::size_t begin, end; // Byte offsets, [begin, end).
        bool operator==(const Match&) const = default;
    };
    PatternId add_pattern(std::string_view pattern) {
        pattern_nodes_.push_back(trie_.insert(pattern));
        pattern_lengths_.push_back(pattern.size());
        built_ = false;
        return pattern_nodes_.size() - 1;
    }
    void build() {
        if (built_) return;
        const auto n = trie_.node_count();
        failure_.assign(n, 0);
        output_link_.assign(n, -1);
        outputs_.assign(n, {});
        for (PatternId id = 0; id < pattern_count(); ++id) outputs_[pattern_nodes_[id]].push_back(id);
        order_.clear(); order_.push_back(0);
        for (const auto edge : trie_.children(0)) {
            output_link_[edge.child] = outputs_[0].empty() ? -1 : 0;
            order_.push_back(edge.child);
        }
        for (std::size_t i = 1; i < order_.size(); ++i) {
            const NodeId node = order_[i];
            for (const auto edge : trie_.children(node)) {
                const NodeId target = advance(failure_[node], edge.symbol);
                failure_[edge.child] = target;
                output_link_[edge.child] = outputs_[target].empty() ? output_link_[target] : target;
                order_.push_back(edge.child);
            }
        }
        built_ = true;
    }
    bool is_built() const { return built_; }
    std::size_t pattern_count() const { return pattern_nodes_.size(); }
    std::size_t node_count() const { return trie_.node_count(); }
    const Trie& trie() const { return trie_; }
    NodeId pattern_node(PatternId id) const { check_pattern(id); return pattern_nodes_[id]; }
    std::size_t pattern_length(PatternId id) const { check_pattern(id); return pattern_lengths_[id]; }
    NodeId failure_link(NodeId node) const {
        assert(built_);
        assert(0 <= node && static_cast<std::size_t>(node) < failure_.size());
        return failure_[node];
    }
    template <class Callback> void for_each_match(std::string_view text, Callback callback) const {
        assert(built_);
        NodeId state = 0;
        emit(state, 0, callback); // Empty patterns match before the first byte too.
        std::size_t end = 0;
        for (const char character : text) {
            state = advance(state, static_cast<unsigned char>(character));
            emit(state, ++end, callback);
        }
    }
    std::vector<Match> matches(std::string_view text) const {
        std::vector<Match> result;
        for_each_match(text, [&](const Match& match) { result.push_back(match); });
        return result;
    }
    std::vector<std::size_t> count_matches(std::string_view text) const {
        assert(built_);
        std::vector<std::size_t> visits(node_count());
        visits[0] = 1;
        NodeId state = 0;
        for (const char character : text) {
            state = advance(state, static_cast<unsigned char>(character));
            ++visits[state];
        }
        for (std::size_t i = order_.size(); i > 1; --i) {
            const NodeId node = order_[i - 1];
            visits[failure_[node]] += visits[node];
        }
        std::vector<std::size_t> result(pattern_count());
        for (PatternId id = 0; id < pattern_count(); ++id) result[id] = visits[pattern_nodes_[id]];
        return result;
    }
private:
    NodeId advance(NodeId state, unsigned char symbol) const {
        while (true) {
            const auto next = trie_.transition(state, symbol);
            if (next) return *next;
            if (state == 0) return 0;
            state = failure_[state];
        }
    }
    template <class Callback> void emit(NodeId state, std::size_t end, Callback& callback) const {
        for (NodeId node = state; node != -1; node = output_link_[node])
            for (const PatternId id : outputs_[node]) callback(Match{id, end - pattern_lengths_[id], end});
    }
    void check_pattern(PatternId id) const { assert(id < pattern_count()); }
    Trie trie_;
    std::vector<NodeId> pattern_nodes_, failure_, output_link_, order_;
    std::vector<std::size_t> pattern_lengths_;
    std::vector<std::vector<PatternId>> outputs_;
    bool built_ = false;
};

}  // namespace cp
