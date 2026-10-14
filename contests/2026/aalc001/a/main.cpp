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

ll op(ll a, ll b) {return min(a, b);}

ll e() {return (ll)1e10;}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int N, Q;
    cin >> N >> Q;
    vector<ll> A(N);
    for (auto& a : A) cin >> a;
    cp::SegmentTree<ll, op, e> sg(A);
    while (Q--)
    {
        int t;
        cin >> t;
        if (t-1) {
            int l, r;
            cin >> l >> r;
            cout << sg.prod(--l, r) << endl;
        } else {
            int i; ll x;
            cin >> i >> x;
            sg.set(--i, x);
        }
    }

    return 0;
}
