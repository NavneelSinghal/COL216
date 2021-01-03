#include <bits/stdc++.h>

using namespace std;

/*
 * format for each instruction is as follows
 * <register alias> number
 */

int regtostring(string reg) {
    int num = 0;
    if (reg[0] == 't') {
        if (reg[1] < '8') {
            num += 8 + reg[1] - '0';
        } else
            num += 16 + reg[1] - '0';
    } else {
        num += 16 + reg[1] - '0';
    }
    return num;
}

string inttobin32(int a) {
    string s(32, '0');
    for (int i = 0; i < 32; i++) {
        s[31 - i] += (a & 1);
        a /= 2;
    }
    return s;
}

int main() {
    string regs[32];
    for (int i = 0; i < 32; i++) {
        regs[i].resize(32);
        for (int j = 0; j < 32; j++) {
            regs[i][j] = '0';
        }
    }
    string a;
    while (cin >> a) {
        int reg = regtostring(a);
        int w;
        cin >> w;
        regs[reg] = inttobin32(w);
    }
    for (int i = 0; i < 32; i++) {
        cout << regs[i];
        cout << endl;
    }
}
