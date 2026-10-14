#include <cp/utility/template.hpp>
#include <cp/data_structure/segment_tree.hpp>
#include <cp/data_structure/lazy_segment_tree.hpp>
#include <cp/math/modint.hpp>

using namespace std;
using ll = cp::modint998244353;
using ull = unsigned long long;
using ld = long double;
using pii = pair<int, int>;
using pll = pair<ll, ll>;
using vi = vector<int>;
using vll = vector<ll>;
using vvi = vector<vi>;
using vvll = vector<vll>;

using S = array<ll, 6>; // r, g, b, rg, gb, rgb

S op(S a, S b) {
    S c;
    for (int i = 0; i < 6; i ++) {
        c[i] = a[i] + b[i];
    }
    c[3] += a[0] * b[1]; // rg
    c[4] += a[1] * b[2]; // gb
    c[5] += a[3] * b[2] + a[0] * b[4];
    return c;
}

S e() {
    return {0, 0, 0, 0, 0, 0};
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int N, Q;
    cin >> N >> Q;
    auto c = [](char a) -> int {
        switch (a)
        {
        case 'R':
            return 0;
        case 'G':
            return 1;
        case 'B':
            return 2;
        }
    };

    vector<S> A(N);
    vll x(N);
    for (auto& a : x) {
        int p;
        cin >> p;
        a = p;
    }
    string s;
    cin >> s;
    for (int i = 0; i < N; i ++) {
        A[i][c(s[i])] += x[i];
    }
    cp::SegmentTree<S, op, e> sg(A);
    while (Q--) {
        int t;
        cin >> t;
        if (t-1) {
            int l, r;
            cin >> l >> r;
            l--;
            cout << sg.prod(l, r)[5].val() << endl;
        } else {
            int i, x;
            cin >> i >> x;
            auto p = sg.get(--i);
            p[c(s[i])] = x;
            sg.set(i, p);
        }
    }

    return 0;
}
