// competitive-verifier: STANDALONE
#include <cp/data_structure/swag.hpp>
#include <cassert>
#include <deque>
#include <random>
#include <string>
#include <vector>
constexpr long long mod = 998244353;
struct Affine { long long a, b; bool operator==(const Affine&) const = default; };
Affine op(Affine left, Affine right) { return {left.a * right.a % mod, (left.b * right.a + right.b) % mod}; }
Affine e() { return {1, 0}; }
std::string join(std::string a, std::string b) { return a + b; }
std::string blank() { return {}; }
int main() {
    std::mt19937 random(20261021);
    for (int trial = 0; trial < 200; ++trial) {
        cp::SWAG<Affine, op, e> queue;
        std::deque<Affine> naive;
        for (int step = 0; step < 1000; ++step) {
            if (random() % 2) {
                const Affine value{static_cast<long long>(1 + random() % 100), static_cast<long long>(random() % 100)};
                queue.push(value); naive.push_back(value);
            } else {
                assert(queue.pop() == !naive.empty());
                if (!naive.empty()) naive.pop_front();
            }
            Affine result = e();
            for (const auto value : naive) result = op(result, value);
            assert(queue.prod() == result && queue.size() == naive.size() && queue.empty() == naive.empty());
        }
    }
    cp::SWAG<std::string, join, blank> words;
    std::deque<std::string> naive;
    for (int step = 0; step < 10000; ++step) {
        if (random() % 2) {
            const std::string letter(1, char('a' + random() % 26));
            words.push(letter); naive.push_back(letter);
        } else {
            assert(words.pop() == !naive.empty());
            if (!naive.empty()) naive.pop_front();
        }
        std::string result;
        for (const auto& word : naive) result += word;
        assert(words.prod() == result);
    }
}
