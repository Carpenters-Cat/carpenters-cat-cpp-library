#include <cp/io/fast_io.hpp>

#include <cassert>
#include <cstdio>
#include <limits>
#include <string>

std::FILE* input(const std::string& text) {
    auto* file = std::tmpfile();
    assert(file);
    assert(std::fwrite(text.data(), 1, text.size(), file) == text.size());
    std::rewind(file);
    return file;
}

template <class T> void roundtrip(T minimum, T maximum) {
    auto* f = std::tmpfile();
    assert(f);
    {
        cp::FastOutput<3> out(f);
        assert(out.write(minimum) && out.write(' ') && out.write(maximum));
        assert(out.flush() && !out.error());
    }
    std::rewind(f);
    cp::FastInput<2> in(f);
    T a{}, b{};
    assert(in.read(a) && a == minimum);
    assert(in.read(b) && b == maximum);
    assert(!in.read(a) && a == minimum && in.eof() && !in.error());
    std::fclose(f);
}

int main() {
    auto* f = input("");
    { cp::FastInput<1> in(f); int x = 7; assert(!in.read(x) && x == 7 && in.eof()); }
    std::fclose(f);
    f = input(" \t\r\n\v\f -0 +12 -34 cat !");
    {
        cp::FastInput<1> in(f);
        int x;
        assert(in.read(x) && x == 0);
        assert(in.read(x) && x == 12);
        assert(in.read(x) && x == -34);
        std::string s;
        assert(in.read(s) && s == "cat");
        char c;
        assert(in.read(c) && c == '!');
        assert(in.eof());
        s = "untouched";
        assert(!in.read(s) && s == "untouched");
    }
    std::fclose(f);
    f = input("+ - 12bad 18446744073709551616 -1 7");
    {
        cp::FastInput<4> in(f);
        unsigned long long x = 99;
        for (int i = 0; i < 5; ++i) assert(!in.read(x) && x == 99);
        assert(in.read(x) && x == 7);
    }
    std::fclose(f);
    roundtrip<signed char>(std::numeric_limits<signed char>::min(), std::numeric_limits<signed char>::max());
    roundtrip<unsigned char>(0, std::numeric_limits<unsigned char>::max());
    roundtrip<short>(std::numeric_limits<short>::min(), std::numeric_limits<short>::max());
    roundtrip<unsigned short>(0, std::numeric_limits<unsigned short>::max());
    roundtrip<int>(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
    roundtrip<unsigned int>(0, std::numeric_limits<unsigned int>::max());
    roundtrip<long>(std::numeric_limits<long>::min(), std::numeric_limits<long>::max());
    roundtrip<unsigned long>(0, std::numeric_limits<unsigned long>::max());
    roundtrip<long long>(std::numeric_limits<long long>::min(), std::numeric_limits<long long>::max());
    roundtrip<unsigned long long>(0, std::numeric_limits<unsigned long long>::max());
    f = std::tmpfile();
    assert(f);
    { cp::FastOutput<1> out(f); assert(out.write("hello world")); } // destructor flush
    std::rewind(f);
    char text[12]{};
    assert(std::fread(text, 1, 11, f) == 11 && std::string(text) == "hello world");
    std::fclose(f);
}
