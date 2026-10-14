#include <cp/utility/template.hpp>
#include <cp/data_structure/segment_tree.hpp>

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
    array<ll, 3> mval;
    ll sum;
    ll bmax;
    ll emax;
    ll bm2;
    ll em2;
    ll be;
};

S op(S a, S b) {
    S c;
    c.sum = a.sum + b.sum;
    for (int i = 0; i < 3; i ++) {
        c.mval[i] = max(a.mval[i], b.mval[i]);
    }
    for (int i = 0; i < 3; i ++) {
        c.mval[2] = max(c.mval[2], a.mval[2-i] + b.mval[i]);
    }
    c.mval[2] = max({c.mval[2], a.emax + b.bm2, a.em2 + b.bmax});
    c.be = max({a.bmax + b.emax, a.sum + b.be, a.be + b.sum});
    {
        c.bmax = max(a.bmax, a.sum + b.bmax);
        c.bm2 = max({c.be, a.bm2, a.be + b.bmax, a.sum + b.bm2});
    }
    {
       c.emax = max(b.emax, b.sum + a.emax);
       c.em2 = max({c.be, b.em2, b.be + a.emax, b.sum + a.em2});
    }
    return c;
}

S e() {
    return S{{0, 0, 0}, 0, 0, 0, 0, 0, 0};
}
S p(ll x) {
    return S{{0, x, x}, x, x, x, x, x, x};
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N, Q;
    cin >> N >> Q;
    vector<S> A(N);
    for (auto& a : A) {
        ll x;
        cin >> x;
        a = p(x);
    }
    cp::SegmentTree<S, op, e> sg(A);

    while (Q--)
    {
        /* code */
        int t;
        cin >> t;
        if (t-1) {
            int l, r; cin >> l >> r;
            cout << sg.prod(--l, r).mval[2] << endl;
        } else {
            int i, x;
            cin >> i >> x;
            sg.set(--i, p(x));
        }
    }


    return 0;
}
