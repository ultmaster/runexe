#include <bits/stdc++.h>
using namespace std;

const int N = 1e3 + 10;
const int MOD = 100;
int n, b, a[N], vis[N];
multiset<int> s;
int dis[N], parent[N];

void get_s() {
    queue<int> q;
    memset(dis, -1, sizeof dis);
    dis[0] = 0;
    q.push(0);
    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (int i = 1; i <= 2 * n; ++i) {
            int v = (u + a[i]) % MOD;
            if (dis[v] == -1) {
                dis[v] = dis[u] + 1;
                parent[v] = u;
                q.push(v);
            }
        }
    }
    int last = b;
    while (last != 0) {
        int v = parent[last];
        int step = (last - v + MOD) % MOD;
        s.insert(step);
        last = v;
    }
}

int main() {
    cin >> n >> b;
    for (int i = 1; i <= 2 * n; ++i)
        cin >> a[i];
    if (b == 100) {
        int select;
        for (int i = 1; i <= n; ++i)
            cin >> select;
        cout << 0 << " " << select << endl;
        b -= a[select];
        if (b == 0)
            return 0;
    }
    get_s();
    // for (int x: s)
    //     cout << x << endl;
    do {
        memset(vis, 0, sizeof vis);
        for (int i = 1; i <= n; ++i) {
            int select;
            cin >> select;
            assert (select >= 1 && select <= 2 * n);
            vis[select] = 1;
        }
        int ok = 0;
        for (int i = 1; i <= 2 * n; ++i) {
            if (vis[i] && (b >= 100 || s.count(a[i]))) {
                if (b >= a[i]) {
                    cout << 0 << " " << i << endl;
                } else {
                    cout << 1 << " " << i << endl;
                    b += 100;
                }
                b -= a[i];
                s.erase(s.find(a[i]));
                ok = 1;
                break;
            }
        }
        if (!ok) {
            cout << 0 << " " << 0 << endl;
        }
    } while (b != 0);
}