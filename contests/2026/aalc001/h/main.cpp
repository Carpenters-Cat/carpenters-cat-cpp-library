#include <cp/utility/template.hpp>
#include <cp/data_structure/segment_tree.hpp>
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

using mint = cp::modint998244353;

struct  S
{
    mint a;
    ll b;
    bool ex;
};

S op(S a, S b) {
    if (b.ex) {
        return S{a.a + a.b + b.a, b.b, 1};
    } else {
        return S{a.a, a.b + b.b, a.ex};
    }
}

S e() {
    return S{0, 0, 0};
}

S p(ll x) {
    if (x) return S{0, x, 0};
    else return S{0, 0, 1};
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
    while (Q--) {
        int t;
        cin >> t;
        if (t-1) {
            int l, r, x;
            cin >> l >> r >> x;
            auto [a, b, c] = sg.prod(--l, r);
            if (c) {
                x = (a + x).val();
            }
            cout << x + b << endl;
        } else {
            int i, x;
            cin >> i >> x;
            sg.set(--i, p(x));
        }
    }

    return 0;
}
