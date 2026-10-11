#include <iostream>
#include <string>
using namespace std;
int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int N; string s;
    cin >> N >> s;
    s = s + "S";
    int p = -1;
    int ans = 0;
    for (int i = 0; i <= N; i ++) {
        if (s[i] == 'S') {
            ans = max(ans, i-p-1);
            p = i;
        }
    }
    cout << ans << endl;
}
