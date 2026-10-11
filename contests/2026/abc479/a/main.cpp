#include <iostream>
using namespace std;
int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n;
    cin >> n;
    int ans = 0;
    for (int i = 0; i < n; i ++) {
        int a; cin >> a;
        if (!(i&1)) ans += a;
    }
    // cout << "OK" << endl;
    cout << ans << endl;
}
