// Manual reproducible benchmark, not a verification gate. See fast_io.md.
#include <cp/io/fast_io.hpp>

#include <cassert>
#include <chrono>
#include <cstdio>
#include <string>

int main() {
    constexpr int n = 1000000;
    constexpr int repetitions = 3;
    std::string text;
    long long expected = 0;
    for (int i = 0; i < n; ++i) {
        const long long x = 48271LL * i - 25000000000LL;
        expected += x;
        text += std::to_string(x) + '\n';
    }
    auto* source = std::tmpfile();
    auto* sink = std::tmpfile();
    assert(source && sink);
    assert(std::fwrite(text.data(), 1, text.size(), source) == text.size());
    std::fflush(source);
    auto measured = [](auto action) {
        auto start = std::chrono::steady_clock::now();
        action();
        return std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
    };
    double std_input = 0, fast_input = 0, std_output = 0, fast_output = 0;
    for (int trial = 0; trial < repetitions; ++trial) {
        std::rewind(source);
        std_input += measured([&] {
            long long sum = 0, x;
            for (int i = 0; i < n; ++i) { assert(std::fscanf(source, "%lld", &x) == 1); sum += x; }
            assert(sum == expected);
        });
        std::rewind(source);
        fast_input += measured([&] {
            cp::FastInput<> in(source);
            long long sum = 0, x;
            for (int i = 0; i < n; ++i) { assert(in.read(x)); sum += x; }
            assert(sum == expected);
        });
        std::rewind(sink);
        std_output += measured([&] {
            for (int i = 0; i < n; ++i) assert(std::fprintf(sink, "%lld\n", 48271LL * i - 25000000000LL) > 0);
            assert(std::fflush(sink) == 0);
        });
        std::rewind(sink);
        fast_output += measured([&] {
            cp::FastOutput<> out(sink);
            for (int i = 0; i < n; ++i) { assert(out.write(48271LL * i - 25000000000LL)); assert(out.write('\n')); }
            assert(out.flush());
        });
        std::rewind(sink);
        std::string actual(text.size(), '\0');
        assert(std::fread(actual.data(), 1, actual.size(), sink) == actual.size() && actual == text);
    }
    std::printf("n=%d bytes=%zu repetitions=%d buffer=65536\n", n, text.size(), repetitions);
    std::printf("mean seconds: fscanf=%.6f FastInput=%.6f fprintf=%.6f FastOutput=%.6f\n",
                std_input / repetitions, fast_input / repetitions, std_output / repetitions, fast_output / repetitions);
    std::fclose(source);
    std::fclose(sink);
}
