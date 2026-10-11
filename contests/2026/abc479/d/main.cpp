#include <iostream>
#include <vector>
using namespace std;
int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    using ll = long long;
    int N, Q;
    cin >> N >> Q;
    vector<ll> L(N-1), R(N-1);
    for (auto& a : L) cin >> a;
    for (auto& a : R) cin >> a;
    vector<ll>mR(N, (ll)1e14), mL(N, (ll)1e14);
    for (int i = 1; i < N; i ++) {
        mL[i] = min(mL[i-1], R[i-1]);
        int j = N-i;
        mR[j-1] = min(mR[j], L[j-1]);
    }
    // for (auto& a : mL) cout << a << " ";
    // cout << endl;
    // for (auto& a : mR) cout << a << " ";
    // cout << endl;
    while (Q--) {
        int s, t;
        cin >> s >> t;
        s--; t--;
        if (s < t) {
            cout << min(R[s], L[s-1] + mL[s]) << endl;
        } else {
            cout << min(L[s-1], R[s] + mR[s]) << endl;
        }
    }
}
