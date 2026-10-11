#include <cp/utility/template.hpp>

int main() {
    std::vector<long long> values{3, 1, 2};
    std::ranges::sort(values);
    assert(std::accumulate(values.begin(), values.end(), 0LL) == 6);
    std::array<int, 3> counts{1, 2, 3};
    assert(std::span(counts).size() == 3);
    std::priority_queue<std::pair<int, int>> queue;
    queue.emplace(2, 3);
    assert(queue.top().second == 3);
    std::unordered_map<std::string, std::optional<int>> lookup;
    lookup["answer"] = 42;
    assert(lookup.at("answer").value() == 42);
    std::stringstream stream;
    stream << std::get<0>(std::tuple{42, 0});
    assert(stream.str() == "42");
    assert(std::popcount(7U) == 3);
    assert(std::bitset<8>(7).count() == 3);
    assert(std::abs(std::complex<double>{3, 4}) == 5);
    std::mt19937 engine(0);
    std::mt19937 same_seed(0);
    assert(engine() == same_seed());
    static_assert(std::is_same_v<std::int64_t, std::int_least64_t>);
}
