#include "testlib.h"
#include <bits/stdc++.h>

using namespace std;

const int N = 5e3 + 10;
int n, b, repo[N], sel[N];
const int MAXQ = 510;

void interact() {
    shuffle(sel, sel + 2 * n);
    set<int> available;
    for (int i = 0; i < n; ++i) {
        cout << sel[i];
        available.insert(sel[i]);
        if (i + 1 < n) cout << " ";
    }
    cout << endl;

    int cash_in = ouf.readInt(0, 100);
    b += cash_in * 100;
    int choice = ouf.readInt(0, 2 * n);
    if (choice == 0) {
        tout << cash_in << " " << choice << " " << b << endl;
        return;
    }
    b -= repo[choice];
    if (available.find(choice) == available.end())
        quitf(_wa, "you are seling %d, which is not presented", choice);

    tout << cash_in << " " << choice << " " << b << endl;

    if (b < 0)
        quitf(_wa, "balance is less than 0");
    if (b == 0)
        quitf(_ok, "correct");
}

int main(int argc, char* argv[]) {
    setName("Interactor Canteen");
    registerInteraction(argc, argv);

    n = inf.readInt(1, 500);
    b = inf.readInt(1, 100);
    rnd.setSeed(n * 123 + b);

    for (int i = 1; i <= 2 * n; ++i)
        repo[i] = inf.readInt();
    iota(sel, sel + 2 * n, 1);

    cout << n << " " << b << endl;
    for (int i = 1; i <= 2 * n; ++i) {
        cout << repo[i];
        if (i + 1 <= 2 * n) cout << " ";
    }
    cout << endl;


    for (int i = 1; i <= MAXQ; ++i)
        interact();

    quitf(_ok, "correct");
}