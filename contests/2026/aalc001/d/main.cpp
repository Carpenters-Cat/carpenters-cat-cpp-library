#include <cp/utility/template.hpp>
#include <cp/data_structure/segment_tree.hpp>
#include <cp/data_structure/lazy_segment_tree.hpp>

using namespace std;
using ll = long long;
using ull = unsigned long long;
using ld = long double;
using pii = pair<int, int>;
using pll = pair<ll, ll>;
using vi = vector<int>;
using vll = vector<ll>;
using vvi = vector<vi>;
using vvll = vector<vll>;

struct S {
    array<ll, 3> cnt;
    ll te;
};

S op(S a, S b) {
    S c;
    for (int i = 0; i < 3; i ++) {
        c.cnt[i] = a.cnt[i] + b.cnt[i];
    }
    c.te = a.te + b.te;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < i; j ++) {
            c.te += a.cnt[i] * b.cnt[j];
        }
    }
    return c;
}

S e() {
    return S{{0, 0, 0}, 0};
}

S ey(int i) {
    S r = e();
    r.cnt[i] ++;
    return r;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N, Q;
    cin >> N >> Q;
    vector<S> A(N);
    for (auto& a : A) {
        int c;
        cin >> c;
        a = ey(c);
    }
    cp::SegmentTree<S, op, e> sg(A);
    while (Q--) {
        int t;
        cin >> t;
        if (t-1) {
            int l, r;
            cin >> l >> r;
            cout << sg.prod(--l, r).te << endl;
        } else {
            int i, x;
            cin >> i >> x;
            sg.set(--i, ey(x));
        }
    }
    return 0;
}
