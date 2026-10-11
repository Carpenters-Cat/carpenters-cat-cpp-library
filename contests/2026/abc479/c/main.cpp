#include <iostream>
using namespace std;
#include <vector>
int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    using ll = long long;
    int n; ll W;
    cin >> n >> W;
    using P = pair<ll, ll>;
    vector<P> A(n);
    for (auto& [a, b] : A) cin >> a >> b;
    ll ans = 0;
    for (int s = 0; s < (1 << n); s ++) {
        ll w = 0, v = 0;
        for (int i = 0; i < n; i ++) {
            auto& [a, b] = A[i];
            if ((s >> i) & 1) {
                v += a;
                w += b;
            }
        }
        if (w <= W) ans = max(ans, v);
    }
    cout << ans << endl;
}
