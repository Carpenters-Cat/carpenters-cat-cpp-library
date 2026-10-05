#pragma once

#include <algorithm>
#include <cassert>
#include <climits>
#include <cstddef>
#include <limits>
#include <optional>
#include <string_view>
#include <utility>
#include <vector>

namespace cp {

// Byte strings, including embedded NUL. Nodes are stable, insertion-order IDs.
class Trie {
    static_assert(CHAR_BIT == 8);
public:
    using NodeId = int;
    struct Edge { unsigned char symbol; NodeId child; };
    Trie() { nodes_.push_back(Node{}); }

    NodeId insert(std::string_view word) {
        NodeId node = 0;
        ++nodes_[node].subtree_count;
        for (const char character : word) {
            const auto symbol = static_cast<unsigned char>(character);
            auto child = transition(node, symbol);
            if (!child) {
                assert(nodes_.size() < static_cast<std::size_t>(std::numeric_limits<NodeId>::max()));
                const NodeId created = static_cast<NodeId>(nodes_.size());
                Node next; next.parent = node; next.symbol = symbol;
                nodes_.push_back(std::move(next));
                auto& edges = nodes_[node].children;
                const auto position = std::lower_bound(edges.begin(), edges.end(), symbol,
                    [](const Edge& edge, unsigned char byte) { return edge.symbol < byte; });
                edges.insert(position, {symbol, created});
                child = created;
            }
            node = *child;
            ++nodes_[node].subtree_count;
        }
        ++nodes_[node].terminal_count;
        return node;
    }
    std::optional<NodeId> find_node(std::string_view prefix) const {
        NodeId node = 0;
        for (const char character : prefix) {
            const auto next = transition(node, static_cast<unsigned char>(character));
            if (!next) return std::nullopt;
            node = *next;
        }
        return node;
    }
    std::size_t count(std::string_view word) const {
        const auto node = find_node(word);
        return node ? nodes_[*node].terminal_count : 0;
    }
    bool contains(std::string_view word) const { return count(word) != 0; }
    std::size_t prefix_count(std::string_view prefix) const {
        const auto node = find_node(prefix);
        return node ? nodes_[*node].subtree_count : 0;
    }
    bool has_prefix(std::string_view prefix) const { return prefix_count(prefix) != 0; }
    std::size_t size() const { return nodes_[0].subtree_count; }
    std::size_t node_count() const { return nodes_.size(); }

    std::optional<NodeId> transition(NodeId node, unsigned char symbol) const {
        check_node(node);
        const auto& edges = nodes_[node].children;
        const auto found = std::lower_bound(edges.begin(), edges.end(), symbol,
            [](const Edge& edge, unsigned char byte) { return edge.symbol < byte; });
        if (found == edges.end() || found->symbol != symbol) return std::nullopt;
        return found->child;
    }
    const std::vector<Edge>& children(NodeId node) const { check_node(node); return nodes_[node].children; }
    NodeId parent(NodeId node) const { check_node(node); return nodes_[node].parent; }
    unsigned char symbol(NodeId node) const { check_node(node); return nodes_[node].symbol; }
    std::size_t terminal_count(NodeId node) const { check_node(node); return nodes_[node].terminal_count; }
private:
    struct Node {
        std::vector<Edge> children;
        std::size_t terminal_count = 0, subtree_count = 0;
        NodeId parent = -1;
        unsigned char symbol = 0;
    };
    void check_node(NodeId node) const { assert(0 <= node && static_cast<std::size_t>(node) < nodes_.size()); }
    std::vector<Node> nodes_;
};

}  // namespace cp
