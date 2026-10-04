// competitive-verifier: STANDALONE
#include <cp/data_structure/swag.hpp>
#include <cassert>
#include <string>
#include <vector>
std::string join(std::string a, std::string b) { return a + b; }
std::string blank() { return {}; }
long long add(long long a, long long b) { return a + b; }
long long zero() { return 0; }
int main() {
    cp::SWAG<std::string, join, blank> queue;
    assert(queue.empty() && queue.size() == 0 && queue.prod().empty());
    assert(!queue.pop() && queue.empty());
    queue.push("a"); queue.push("b"); queue.push("c");
    assert(queue.prod() == "abc" && queue.size() == 3);
    assert(queue.pop() && queue.prod() == "bc"); // transfer to front stack
    queue.push("d"); queue.push("e");
    assert(queue.prod() == "bcde"); // both stacks are nonempty
    assert(queue.pop() && queue.prod() == "cde");
    assert(queue.pop() && queue.prod() == "de");
    assert(queue.pop() && queue.prod() == "e"); // transfer again
    assert(queue.pop() && queue.prod().empty() && !queue.pop());
    queue.push("z");
    assert(queue.prod() == "z" && queue.pop());
    cp::SlidingWindowAggregation<long long, add, zero> numbers(std::vector<long long>{1, -3, 7});
    assert(numbers.prod() == 5 && numbers.pop() && numbers.prod() == 4);
    cp::SWAG<long long, add, zero> large;
    for (int i = 0; i < 200000; ++i) large.push(1);
    assert(large.prod() == 200000);
    for (int i = 0; i < 200000; ++i) assert(large.pop());
    assert(large.prod() == 0 && large.empty());
}
