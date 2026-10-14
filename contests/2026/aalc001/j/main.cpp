#include <cp/utility/template.hpp>
#include <cp/data_structure/segment_tree.hpp>
#include <cp/data_structure/lazy_segment_tree.hpp>
#include <cp/data_structure/coordinate_compression.hpp>

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

using S = array<ll, 2>;

S op(S a, S b) {
    return {a[0] + b[0], a[1] + b[1]};
}
S e() {return {0, 0};}

S mp(int f, S x) {
    S c = x;
    if (f) swap(c[0], c[1]);
    return c;
}
int co(int f, int g) {
    return f ^ g;
}
int id() {return 0;}


int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int Q;
    cin >> Q;
    using T = tuple<int, ll, ll>;
    vector<T> que(Q);
    vll X;
    for (auto& [t, a, b] : que) {
        cin >> t >> a >> b;
        a--; t--;
        X.push_back(a);
        X.push_back(b);
    }
    cp::CoordinateCompression<ll> C(X);
    auto Y = C.values();
    int N = Y.size() -1;
    vector<S> A(N);
    for (int i = 0; i < N; i ++) {
        A[i][0] = Y[i+1] - Y[i];
    }
    cp::LazySegmentTree<S, op, e, int, mp, co, id> lg(A);
    for (auto& [t, a, b] : que) {
        a = C.lower_bound(a);
        b = C.lower_bound(b);
        if (t) {
            cout << lg.prod(a, b)[1] << endl;
        } else {
            lg.apply(a, b, 1);
        }
    }

    return 0;
}
