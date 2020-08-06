#include <bits/stdc++.h>
using namespace std;
char digtohex(int a) {
    if (a < 10)
        return ('0' + a);
    else
        return ('a' + a - 10);
}
int main() {
    int n;
    cin >> n;
    while (n--) {
        string s;
        cin >> s;
        string ans;
        assert(s.size() % 4 == 0);
        for (int i = 0; i < (int)s.size(); i += 4) {
            int dig = 0;
            for (int j = 0; j < 4; j++) {
                dig *= 2;
                dig += s[i + j] - '0';
            }
            ans += digtohex(dig);
        }
        cout << "0x0" + ans << endl;
    }
}
