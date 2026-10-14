#include <cp/utility/template.hpp>
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
    ll sum;
    ll mx[2];
    ll smax;
};

S op(S a, S b) {
    S c;
    c.sum = a.sum + b.sum;
    c.mx[0] = max(a.mx[0], a.sum + b.mx[0]);
    c.mx[1] = max(b.mx[1], b.sum + a.mx[1]);
    c.smax = max({a.smax, b.smax, a.mx[1] + b.mx[0]});
    return c;
}
S e0() {
    return S{0, {0, 0}, 0};
}
S e1(ll a) {
    return S{a, {a, a}, a};
}

using S3 = array<S, 3>;
S3 op3(S3 a, S3 b) {
    S3 c;
    for (int i = 0; i < 3; i ++) c[i] = op(a[i], b[i]);
    return c;
}
S3 e() {
    return {e0(), e0(), e0()};
}
S3 eix(int i, ll x) {
    auto r = e();
    r[i] = e1(x);
    return r;
}

using F = array<int, 3>;

S3 mp(F f, S3 x) {
    auto y = x;
    for (int i = 0; i < 3; i ++) {
        y[i] = x[f[i]];
    }
    return y;
}
F co(F f, F g) {
    F r;
    for (int i = 0; i < 3; i ++) {
        r[i] = g[f[i]];
    }
    return r;
}
F id() {
    return {0, 1, 2};
}
F sp(int i) {
    F r = id();
    swap(r[i%3], r[i-1]);
    return r;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N, Q;
    cin >> N >> Q;
    vector<S3> A(N);
    for (int i = 0; i < 3; i ++) {
        for (auto& a : A) {
            ll x;
            cin >> x;
            a[i] = e1(x);
        }
    }
    cp::LazySegmentTree<S3, op3, e, F, mp, co, id> lg(A);
    while (Q--)
    {
        /* code */
        int t;
        cin >> t;
        if (t < 4) {
            int l, r;
            cin >> l >> r;
            lg.apply(--l, r, sp(t));
        } else {
            int i;
            cin >> i;
            i--;
            cout << lg.max_right(i, [](S3 x) {return x[0].mx[0] <= 0;}) + 1 << endl;
        }
    }


    return 0;
}
