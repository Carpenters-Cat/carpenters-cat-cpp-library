#include <cp/io/fast_io.hpp>

#include <cassert>
#include <charconv>
#include <cstdio>
#include <limits>
#include <random>
#include <string>
#include <vector>

int main() {
    std::mt19937_64 rng(20261004);
    std::vector<long long> values{0, -1, 1, std::numeric_limits<long long>::min(), std::numeric_limits<long long>::max()};
    for (int i = 0; i < 100000; ++i) {
        const auto magnitude = static_cast<long long>(rng() >> 1);
        values.push_back(rng() % 2 ? magnitude : -magnitude);
    }
    std::string reference;
    for (auto x : values) {
        char buffer[32];
        auto result = std::to_chars(buffer, buffer + sizeof(buffer), x);
        assert(result.ec == std::errc{});
        reference.append(buffer, result.ptr);
        reference.push_back('\n');
    }
    auto* file = std::tmpfile();
    assert(file);
    {
        cp::FastOutput<7> out(file);
        for (auto x : values) assert(out.write(x) && out.write('\n'));
        assert(out.flush());
    }
    std::rewind(file);
    std::string actual(reference.size(), '\0');
    assert(std::fread(actual.data(), 1, actual.size(), file) == actual.size());
    assert(actual == reference);
    std::rewind(file);
    {
        cp::FastInput<11> in(file);
        for (auto expected : values) { long long x; assert(in.read(x) && x == expected); }
        long long x;
        assert(!in.read(x) && in.eof() && !in.error());
    }
    std::fclose(file);
}
