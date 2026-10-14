#include <cp/utility/template.hpp>
#include <cp/data_structure/segment_tree.hpp>
#include <cp/data_structure/coordinate_compression.hpp>
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

using S = array<mint, 3>; // val, sum, n
S op(S a, S b) {
    S c;
    c[2] = a[2] + b[2];
    c[1] = a[1] + b[1];
    c[0] = a[0] + b[0] + (a[2] * b[1]);
    return c;
}
S e() {
    return {0, 0, 0};
}
S add(S a, ll x) {
    return {a[0] + (a[2] + 1) * x, a[1] + x, a[2]+1};
}
S del(S a, ll x) {
    return {a[0] - (a[2]) * x, a[1] - x, a[2]-1};
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N, Q;
    cin >> N >> Q;
    vll A(N);
    for (auto& a : A) cin >> a;
    auto B = A;
    vector<pll> que(Q);
    for (auto& [t, x] : que) {
        cin >> t;
        if (t < 3) {
            cin >> x;
            B.push_back(x);
        } else {
            x = -1;
        }
    }
    if (0) {
        cout << "Q" << endl;
        for (auto& [t, x] : que) {
            cout << t << " " << x << endl;
        }
    }
    {
        vector<pll> q2;
        for (auto& a : A) {
            q2.emplace_back(1, a);
        }
        q2.insert(q2.end(), que.begin(), que.end());
    if (0) {
        cout << "Q" << endl;
        for (auto& [t, x] : q2) {
            cout << t << " " << x << endl;
        }
    }
        que = q2;
    }
    if (0) {
        cout << "Q" << endl;
        for (auto& [t, x] : que) {
            cout << t << " " << x << endl;
        }
    }
    auto C = cp::CoordinateCompression<ll>(B);
    auto D = C.values();
    N = C.size();
    cp::SegmentTree<S, op, e> sg(N);
    for (auto& [t, x] : que) {
        // cout << t << " " << x << endl;
        int i = C.lower_bound(x);
        if (t == 1) {
            sg.set(i, add(sg.get(i), D[i]));
        } else if (t == 2) {
            sg.set(i, del(sg.get(i), D[i]));
        } else {
            cout << sg.all_prod()[0].val() << endl;
        }
        if (0) {
            cout << sg.all_prod()[0].val() << endl;
        }
    }

    return 0;
}
