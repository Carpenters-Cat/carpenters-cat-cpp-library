#pragma once

#include <cstddef>
#include <utility>
#include <vector>

namespace cp {

// FIFO monoid aggregation. Operand order is preserved even for noncommutative op.
template <class S, auto op, auto e>
class SWAG {
public:
    explicit SWAG(const std::vector<S>& values = {}) {
        for (const S& value : values) push(value);
    }
    void push(S value) {
        const S aggregate = back_.empty() ? value : op(back_.back().aggregate, value);
        back_.push_back({std::move(value), aggregate});
    }
    // Empty pop is a no-op and returns false.
    bool pop() {
        if (empty()) return false;
        if (front_.empty()) {
            while (!back_.empty()) {
                S value = std::move(back_.back().value);
                back_.pop_back();
                const S aggregate = front_.empty() ? value : op(value, front_.back().aggregate);
                front_.push_back({std::move(value), aggregate});
            }
        }
        front_.pop_back();
        return true;
    }
    S prod() const {
        if (front_.empty()) return back_.empty() ? e() : back_.back().aggregate;
        if (back_.empty()) return front_.back().aggregate;
        return op(front_.back().aggregate, back_.back().aggregate);
    }
    std::size_t size() const { return front_.size() + back_.size(); }
    bool empty() const { return front_.empty() && back_.empty(); }
private:
    struct Entry { S value, aggregate; };
    std::vector<Entry> front_, back_;
};

template <class S, auto op, auto e> using SlidingWindowAggregation = SWAG<S, op, e>;
}  // namespace cp
