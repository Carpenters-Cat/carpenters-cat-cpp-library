#include <cp/utility/template.hpp>
#include <cp/data_structure/segment_tree.hpp>
#include <cp/data_structure/lazy_segment_tree.hpp>
#include <cp/math/modint.hpp>

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
    int n;
    ll bg;
    ll ed;
    int nb;
    int ne;
    int scr;
};
S op(S a, S b) {
    S c;
    c.n = a.n + b.n;
    c.bg = a.bg; c.ed = b.ed;
    c.nb = a.nb; c.ne = b.ne;
    c.scr = max(a.scr, b.scr);
    if (a.ed == b.bg) {
        c.scr = max(c.scr, a.ne + b.nb);
        if (b.scr == b.n) {
            c.ne = a.ne + b.n;
        }
        if (a.scr == a.n) {
            c.nb = a.n + b.nb;
        }
    }
    return c;
}

S e() {
    return S{0, 0, 0, 0, 0, 0};
}

S pk(ll x) {
    return S{1, x, x, 1, 1, 1};
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
        a = pk(x);
    }

    cp::SegmentTree<S, op, e> sg(A);
    while (Q--)
    {
        int t;
        cin >> t;
        if (t==1) {
            int i; ll x;
            cin >> i >> x;
            sg.set(--i, pk(x));
        } else {
            int l, r;
            cin >> l >> r;
            cout << sg.prod(--l, r).scr << endl;
        }
    }


    return 0;
}
