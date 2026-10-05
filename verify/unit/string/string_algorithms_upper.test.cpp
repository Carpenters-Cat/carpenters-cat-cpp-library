// competitive-verifier: STANDALONE
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cp/string/string_algorithms.hpp>
#include <cstdlib>
#include <limits>
#include <new>
#include <numeric>
#include <vector>

namespace {
bool reject_large_allocation = false;
std::size_t rejected_bytes = 0;
}

// Verify the enormous alphabet's requested size without committing gigabytes.
void *operator new(std::size_t bytes) {
  if (reject_large_allocation && bytes > 1024 * 1024) {
    rejected_bytes = bytes;
    throw std::bad_alloc();
  }
  if (void *p = std::malloc(bytes == 0 ? 1 : bytes))
    return p;
  throw std::bad_alloc();
}
void operator delete(void *p) noexcept { std::free(p); }
void operator delete(void *p, std::size_t) noexcept { std::free(p); }

int main() {
  std::vector<int> ascending(40), descending(40);
  std::iota(ascending.begin(), ascending.end(), 0);
  std::iota(descending.rbegin(), descending.rend(), 0);
  assert(cp::suffix_array(std::vector<int>(40, 0), 0) == descending);
  assert(cp::suffix_array(ascending, 39) == ascending);
  assert(cp::suffix_array(descending, 39) == descending);
  assert(cp::suffix_array(ascending, 100) == ascending);
  std::vector<int> alternating(40), expected;
  for (int i = 0; i < 40; ++i)
    alternating[i] = i % 2;
  for (int i = 38; i >= 0; i -= 2)
    expected.push_back(i);
  for (int i = 39; i >= 1; i -= 2)
    expected.push_back(i);
  assert(cp::suffix_array(alternating, 1) == expected);

  const int max = std::numeric_limits<int>::max();
  for (int upper : {max - 1, max}) {
    for (int symbol : {0, upper}) {
      const std::vector<int> input(40, symbol);
      rejected_bytes = 0;
      reject_large_allocation = true;
      bool failed_allocation = false;
      try {
        cp::suffix_array(input, upper);
      } catch (const std::bad_alloc &) {
        failed_allocation = true;
      }
      reject_large_allocation = false;
      assert(failed_allocation);
      assert(rejected_bytes ==
             (static_cast<std::size_t>(upper) + 1) * sizeof(int));
    }
  }
}
