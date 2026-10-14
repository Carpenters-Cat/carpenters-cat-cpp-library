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

ll op(ll a, ll b) {
    return min(a, b);
}

ll e() {
    return (ll)1e16;
}

ll mp(ll f, ll x) {
    return f + x;
}

ll co(ll f, ll g) {
    return f + g;
}

ll id() {
    return 0;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N, Q;
    cin >> N >> Q;
    vll A(N);
    for (auto& a : A) cin >> a;
    cp::LazySegmentTree<ll, op, e, ll, mp, co, id> lg(A);
    while (Q--)
    {
        int t;
        cin >> t;
        if (t-1) {
            int l, r;
            cin >> l >> r;
            cout << lg.prod(--l, r) << endl;
        } else {
            int l, r; ll x;
            cin >> l >> r >> x;
            lg.apply(--l, r, x);
        }
    }


}
