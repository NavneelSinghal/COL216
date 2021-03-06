#include <cassert>
#include <iostream>
#include <string>
#define MAX_INSTRUCTIONS 1024
#define MEM_MAX 4095
#define MEM_ZERO 1024

using namespace std;

/*
 * INPUT FORMAT: Firstly number of instructions, then all instructions in
 * different lines Then the number of memory elements that need to be
 * initialised, and each memory element on a new line
 */

int regint(string reg) {
    assert(reg.size() == 2);
    int num = 0;
    if (reg[0] == 't') {
        if (reg[1] < '8') {
            num = 8 + reg[1] - '0';
        } else
            num = 16 + reg[1] - '0';
    } else if (reg[0] == 's') {
        num = 16 + reg[1] - '0';
    } else if (reg[0] == 'a') {
        num = 4 + reg[1] - '0';
    } else if (reg[0] == 'v') {
        num = 2 + reg[1] - '0';
    } else if (reg == "ra") {
        num = 31;
    } else if (reg == "sp") {
        num = 29;
    } else if (reg == "ze") {
        num = 0;
    }
    return num;
}

string regtostring(string reg) {
    int num = regint(reg);
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

int main() {
    cout << "memory_initialization_radix=2;\n";
    cout << "memory_initialization_vector=\n";
    string instr;
    int shift;
    int offset;
    string reg[3];
    int instrcount = 0;
    int cnt;
    cin >> cnt;
    while (cnt--) {
        cin >> instr;
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
        } else if (instr == "j") {
            out += "000010";
            out += "0000000000";
            cin >> offset;
            assert(offset >= 0);
            out += inttobin16(offset);
        } else if (instr == "jr") {
            out += "000000";
            string r;
            cin >> r;
            out += regtostring(r);
            out += "000000000000000";
            out += "001000";
        } else if (instr == "jal") {
            out += "000010";
            out += "0000000000";
            cin >> offset;
            assert(offset >= 0);
            out += inttobin16(offset);
        } else if (instr == "bne") {
            out += "000101";
            string r;
            cin >> r;
            out += regtostring(r);
            cin >> r;
            out += regtostring(r);
            cin >> offset;
            out += inttobin16(offset);
        } else if (instr == "beq") {
            out += "000100";
            string r;
            cin >> r;
            out += regtostring(r);
            cin >> r;
            out += regtostring(r);
            cin >> offset;
            out += inttobin16(offset);
        } else if (instr == "blez") {
            out += "000110";
            string r;
            cin >> r;
            out += regtostring(r);
            out += "00000";
            cin >> offset;
            out += inttobin16(offset);
        } else if (instr == "bgtz") {
            out += "000111";
            string r;
            cin >> r;
            out += regtostring(r);
            out += "00000";
            cin >> offset;
            out += inttobin16(offset);
        } else if (instr == ".")
            break;
        cout << out << ",\n";
    }
    for (int i = 0; i < 32; i++) cout << 0;
    cout << ",\n";
    for (int i = instrcount; i < MAX_INSTRUCTIONS - 1; i++) {
        for (int j = 0; j < 32; j++) {
            cout << 0;
        }
        cout << ",\n";
    }
    int mems;
    cin >> mems;
    int memcnt = 0;
    while (mems--) {
        int data;
        cin >> data;
        string s = inttobin32(data);
        cout << s << ",\n";
        memcnt++;
    }
    for (int i = memcnt; i <= MEM_MAX - MEM_ZERO; i++) {
        for (int j = 0; j < 32; j++) cout << 0;
        if (i == MEM_MAX - MEM_ZERO)
            cout << ";\n";
        else
            cout << ",\n";
    }
}
