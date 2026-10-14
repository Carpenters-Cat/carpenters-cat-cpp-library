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

ll op(ll a, ll b) {return a + b;}
ll e() {return 0;}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N, Q;
    cin >> N >> Q;
    vll A(N);
    for (auto& a : A) cin >> a;
    cp::SegmentTree<ll, op, e> sg(A);
    while (Q--) {
        int t;
        cin >> t;
        if (t == 1) {
            int i; ll x;
            cin >> i >> x;
            sg.set(--i, x);
        } else if (t == 2) {
            int l; ll x;
            cin >> l >> x;
            l--;
            cout << sg.max_right(l, [&](ll y){return y < x;}) + 1 << endl;
        } else {
            int r; ll x;
            cin >> r >> x;
            cout << sg.min_left(r, [&](ll y){return y < x;}) << endl;
        }
    }

    return 0;
}
