#include <iostream>
#include <cp/data_structure/segment_tree.hpp>
#include <vector>
#include <numeric>
#include <algorithm>

using namespace std;

using ll = long long;
ll op(ll a, ll b) {return a + b;}
ll e() {return 0;}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int N;
    cin >> N;
    vector<ll> S(N), T(N);
    {
        vector<pair<ll, ll>> X(N);
        for (auto& [a, b] : X) cin >> a;
        for (auto& [a, b] : X) cin >> b;
        sort(X.begin(), X.end());
        for (int i = 0; i < N; i ++) {
            tie(S[i], T[i]) = X[i];
        }
    }
    ll ans = 0;
    vector<int> ID(N), IV(N);
    iota(ID.begin(), ID.end(), 0);
    sort(ID.begin(), ID.end(), [&](int i, int j) {return T[i] < T[j];});
    for (int i = 0; i < N; i ++) {
        IV[ID[i]] = i;
    }
    {
        cp::SegmentTree<ll, op, e> sg(N);
        for (int i = 0; i < N; i ++) {
            ans += sg.prod(IV[i], N);
            S[i] -= sg.prod(IV[i], N);
            sg.set(IV[i], 1);
        }
    }
    {
        cp::SegmentTree<ll, op, e> sg(N);
        for (int i = N-1; i >= 0; i --) {
            S[i] += sg.prod(0, IV[i]);
            sg.set(IV[i], 1);
        }
    }
    for (int i = 0; i < N; i ++) {
        ans += abs(S[i] - T[i]);
    }
    cout << ans << endl;
}
