#include <iostream>
#include <vector>
#include <utility>
using namespace std;

using ll = long long;
using vi = vector<int>;
using vvi = vector<vi>;
using pii = pair<int, int>;
using vl = vector<ll>;
using vpii = vector<pii>;

void dfs(const vvi& gr, vi& siz, int s = 0, int p = -1) {
    for (int v : gr[s]) {
        if (v == p) continue;
        dfs(gr, siz, v, s);
        siz[s] += siz[v];
    }
}
void ds1(const vector<vpii>& gr, vl& ans, int s, int p, ll d);
void ds2(const vector<vpii>& gr, vl& ans, int s, int p, ll d);
void df2(const vector<vpii>& gr, vl& ans, int s) {
    for (auto& [v, i] : gr[s]) {
        ds1(gr, ans, v, s, i);
    }
    for (auto& [v, i] : gr[s]) {
        ds2(gr, ans, v, s, ans[s] - ans[v] - i);
    }
}

void ds1(const vector<vpii>& gr, vl& ans, int s, int p, ll d) {
    for (auto& [v, i] : gr[s]) {
        if (v == p) continue;
        ds1(gr, ans, v, s, i);
    }
    ans[p] += ans[s] + d;
}

void ds2(const vector<vpii>& gr, vl& ans, int s, int p, ll d) {
    for (auto& [v, i] : gr[s]) {
        if (v == p) continue;
        ds2(gr, ans, v, s, d);
    }
    ans[s] += d;
}

ll dfs3(const vector<vpii>& gr, const vi& siz, vl& ans, int s = 0, int p = -1, ll d = 0) {
    int N = gr.size();
    ans[s] += d;
    ll o = 0;
    for (auto& [v, i] : gr[s]) {
        if (v == p) continue;
        ll d2 = d;
        if (siz[v] > N/2) {
            d2 += i;
        } else {
            o += i;
            d2 -= i;
        }
        o += dfs3(gr, siz, ans, v, s, d2);
    }
    return o;
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int N;
    cin >> N;
    vvi gr(N);
    vector<vpii> g2(N);
    for (int i = 1; i < N; i ++) {
        int a, b;
        cin >> a >> b;
        a--; b--;
        gr[a].push_back(b);
        gr[b].push_back(a);
        g2[a].emplace_back(b, i);
        g2[b].emplace_back(a, i);
    }
    vi siz(N, 1);
    dfs(gr, siz);

    {
        vl ans(N, 0);
        ll x = dfs3(g2, siz, ans);
        for (ll& a : ans) cout << a + x << " ";
        cout << endl;
    }
    return 0;
    int g = 0;
    while (1) {
        int x = -1;
        for (int v : gr[g]) {
            int s = siz[v];
            if (s > siz[g]) continue;
            if (s > N/2) {
                x = v;
                break;
            }
        }
        if (x < 0) break;
        g = x;
    }
    vl ans(N, 0);
    df2(g2, ans, g);
    for (auto&a : ans) cout << a << " ";
    cout << endl;
}
