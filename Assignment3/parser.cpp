#include <bits/stdc++.h>

#define MAX_INSTRUCTIONS 32
#define MEM_MAX 255
#define MEM_ZERO 32

using namespace std;

string regtostring(string reg) {
    int num = 0;
    if (reg[0] == 't') {
        if (reg[1] < '8') {
            num += 8 + reg[1] - '0';
        } else
            num += 16 + reg[1] - '0';
    } else {
        num += 16 + reg[1] - '0';
    }
    string w(5, '0');
    for (int i = 4; i >= 0; i--) {
        w[i] += (num & 1);
        num >>= 1;
    }
    return w;
}

string inttobin(int num) {
    string w(5, '0');
    for (int i = 4; i >= 0; i--) {
        w[i] += (num & 1);
        num >>= 1;
    }
    return w;
}

string inttobin16(int num) {
    string w(16, '0');
    for (int i = 15; i >= 0; i--) {
        w[i] += (num & 1);
        num >>= 1;
    }
    return w;
}

string inttobin32(int num) {
    string w(32, '0');
    for (int i = 31; i >= 0; i--) {
        w[i] += (num & 1);
        num >>= 1;
    }
    return w;
}

// syntax for command - remove all $ and commas, and then replace brackets by
// spaces

// first get all commands, which end using the character '.' an

int main() {
    string instr;
    int shift;
    int offset;
    string reg[3];
    int instrcount = 0;
    while (cin >> instr) {
        instrcount++;
        string out;
        if (instr == "add") {
            out += "000000";
            cin >> reg[2];
            cin >> reg[0];
            cin >> reg[1];
            out += regtostring(reg[0]);
            out += regtostring(reg[1]);
            out += regtostring(reg[2]);
            out += "00000";
            out += "100000";
        } else if (instr == "sub") {
            out += "000000";
            cin >> reg[2];
            cin >> reg[0];
            cin >> reg[1];
            out += regtostring(reg[0]);
            out += regtostring(reg[1]);
            out += regtostring(reg[2]);
            out += "00000";
            out += "100010";
        } else if (instr == "lw") {
            out += "100011";
            cin >> reg[1] >> offset >> reg[0];
            out += regtostring(reg[0]);
            out += regtostring(reg[1]);
            out += inttobin16(offset);
        } else if (instr == "sw") {
            out += "101011";
            cin >> reg[1] >> offset >> reg[0];
            out += regtostring(reg[0]);
            out += regtostring(reg[1]);
            out += inttobin16(offset);
        } else if (instr == "sll") {
            out += "000000";
            out += "00000";
            cin >> reg[1] >> reg[0];
            out += (regtostring(reg[0]) + regtostring(reg[1]));
            cin >> shift;
            out += inttobin(shift);
            out += "000000";
        } else if (instr == "srl") {
            out += "000000";
            out += "00000";
            cin >> reg[1] >> reg[0];
            out += (regtostring(reg[0]) + regtostring(reg[1]));
            cin >> shift;
            out += inttobin(shift);
            out += "000010";
        } else if (instr == ".")
            break;
        cout << out << '\n';
    }
    for (int i = 0; i < 32; i++) cout << 0;
    cout << endl;
    for (int i = instrcount; i < MEM_MAX + 1; i++) {
        for (int j = 0; j < 32; j++) {
            cout << 0;
        }
        cout << endl;
    }
    for (int i = 0; i < 32; i++) {
        int n;
        cin >> n;
        cout << inttobin32(n) << endl;
    }
}
