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
constexpr int L = 5;
using mint = cp::modint998244353;
using S = array<mint, L>;
vector<vector<mint>> comb = {
    {1, 0, 0, 0, 0},
    {1, 1, 0, 0, 0},
    {1, 2, 1, 0, 0},
    {1, 3, 3, 1, 0},
    {1, 4, 6, 4, 1}
};
S op(S a, S b) {
    S c;
    for (int i = 0; i < L; i++) {
        c[i] = a[i] + b[i];
    }
    return c;
}
S e() {
    return {0, 0, 0, 0, 0};
}
S mp(mint f, S x) {
    auto y = x;
    mint pw[] = {1, f, f*f, f*f*f, f*f*f*f};
    for (int i = L-1; i >= 0; i --) {
        y[i] = 0;
        for (int j = 0; j <= i; j ++) {
            y[i] += x[j] * pw[i-j] * comb[i][j];
        }
    }
    return y;
}
mint co(mint f, mint g) {
    return f + g;
}
mint id() {return 0;}
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N, Q;
    cin >> N >> Q;
    vector<S> A(N);
    for (auto& a : A) {
        int x;
        cin >> x;
        a[0] = 1;
        for (int i = 1; i < L; i ++) {
            a[i] = a[i-1] * x;
        }
    }
    cp::LazySegmentTree<S, op, e, mint, mp, co, id> lg(A);
    while (Q--)
    {
        /* code */
        int t;
        cin >> t;
        if (t-1) {
            int l, r, k;
            cin >> l >> r >> k;
            cout << lg.prod(--l, r)[k].val() << endl;
        } else {
            int l, r; ll x;
            cin >> l >> r >> x;
            lg.apply(--l, r, x);
        }
        if (0) {
            for (int i = 0; i < N; i ++) {
                cout << lg.prod(i, i+1)[1].val() << " ";
            }
            cout << endl;
        }
    }


    return 0;
}
