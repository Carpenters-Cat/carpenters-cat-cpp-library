// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/many_aplusb
#include <cp/io/fast_io.hpp>

int main() {
    cp::FastInput<> in;
    cp::FastOutput<> out;
    int t;
    if (!in.read(t)) return 1;
    while (t--) {
        long long a, b;
        if (!in.read(a) || !in.read(b)) return 1;
        if (!out.write(a + b) || !out.write('\n')) return 1;
    }
    return out.flush() ? 0 : 1;
}
