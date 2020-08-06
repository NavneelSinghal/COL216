#include <bits/stdc++.h>
using namespace std;

#define MAX_INSTRUCTIONS 400
#define MEM_MAX 4095
#define MEM_ZERO 1024
#define MAX_INT 0X7FFFFFFF
#define REP(i, n) for (int i = 0; i < n; i++)

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
uniform_int_distribution<int> dist18(0, 17);
uniform_int_distribution<int> distbig(0, MAX_INT);
uniform_int_distribution<int> distmem(0, MEM_MAX - MEM_ZERO);

int main() {
    srand(time(NULL));
    string instr[6] = {"add", "sub", "sw", "lw", "sll", "srl"};
    string regs[18];
    for (int i = 0; i < 18; i++) {
        if (i < 8) {
            regs[i] = "t" + to_string(i);
        } else if (i < 16) {
            regs[i] = "s" + to_string(i - 8);
        } else {
            regs[i] = "t" + to_string(i - 8);
        }
    }
    int numinstr = MAX_INSTRUCTIONS;
    cout << numinstr << endl;
    REP(i, numinstr) {
        string command =
            instr[rand() %
                  4];  // not doing sll and srl right now else weird issues
        string args[3];
        if (command == "add" || command == "sub") {
            cout << "add"
                 << " ";  // not doing sub
            REP(j, 3) cout << regs[dist18(rng)] << " ";
            cout << endl;
        } else if (command == "sll" || command == "srl") {
            cout << command << " ";
            REP(j, 2) { cout << regs[dist18(rng)] << " "; }
            cout << rand() % 32 << endl;
        } else {
            cout << command << " ";
            cout << regs[dist18(rng)] << " ";
            cout << distmem(rng) << " ";
            cout << regs[dist18(rng)] << endl;
        }
    }
    cout << (4095 - 1024) << endl;
    REP(i, 12) { cout << rand() % 2 << '\n'; }
    REP(i, (4095 - 1024 - 12)) { cout << rand() % 2 << '\n'; }
}
