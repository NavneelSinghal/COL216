#include <bits/stdc++.h>
using namespace std;

int regint(string reg){
    assert(reg.size() == 2);
    int num = 0;
    if(reg[0] == 't'){
        if(reg[1] < '8'){
            num = 8 + reg[1] - '0';
        }
        else num = 16 + reg[1] - '0';
    }
    else if(reg[0] == 's'){
        num = 16 + reg[1] - '0';
    }
    else if(reg[0] == 'a'){
		num = 4 + reg[1] - '0';
	}
	else if(reg[0] == 'v'){
		num = 2 + reg[1] - '0';
	}
    else if(reg == "ra"){
		num = 31;
	}
	else if(reg == "sp"){
		num = 29;
	}
	else if(reg == "ze"){
		num = 0;
	}
	else if(reg == "gp"){
		num = 28;
	}
    return num;
}

/*
 * INPUT FORMAT: first number of instructions, then pseudocode for each instruction in a new line.
 *               then the number of memory elements needed to specify, then the memory contents in the following lines
 */

int main(){
    int n;
    cin >> n; 
    vector<string> v;
    vector<int> reg(32);
    for(int i = 0; i <= n; i++){ 
        string s;
        getline(cin, s);
        v.push_back(s);
    }
    
    int memcnt;
    cin >> memcnt;
    vector<int> mem(3072);
    for(int i = 0; i < memcnt; i++){
        cin >> mem[i];
    }
    int i = 0;
    for(i = 1; i < (int)v.size(); i++){
        cout << "Instruction number: " << i << endl;
        string s = v[i];
        cout << s << endl;
        vector<string> toks;
        stringstream str(s);
        string inter;
        while(getline(str, inter, ' ')){
            toks.push_back(inter);
        }
        if(toks[0] == "add"){
            int reg1 = regint(toks[1]);
            int reg2 = regint(toks[2]);
            int reg3 = regint(toks[3]);
            int old2 = reg[reg2];
            int old3 = reg[reg3];
            reg[reg1] = reg[reg2] + reg[reg3];
            cout << "reg[" << reg1 << "] becomes reg[" << reg2 << "] + reg[" << reg3 << "] which is " << old2 << " + " << old3 << " = " << reg[reg1] << endl;
        }
        if(toks[0] == "sub"){
            int reg1 = regint(toks[1]);
            int reg2 = regint(toks[2]);
            int reg3 = regint(toks[3]);
            int old2 = reg[reg2];
            int old3 = reg[reg3];
            reg[reg1] = reg[reg2] - reg[reg3];
            cout << "reg[" << reg1 << "] becomes reg[" << reg2 << "] - reg[" << reg3 << "] which is " << old2 << " - " << old3 << " = " << reg[reg1] << endl;
        }
        if(toks[0] == "sll"){
            int reg1 = regint(toks[1]);
            int shift = stoi(toks[3]);
            int reg2 = regint(toks[2]);
            int old2 = reg[reg2];
            reg[reg1] = reg[reg2] << shift;
            cout << "reg[" << reg1 << "] becomes reg[" << reg2 << "] << " << shift << " which is " << old2 << " << " << shift << " = " << reg[reg1] << endl;
        }
        if(toks[0] == "srl"){
            int reg1 = regint(toks[1]);
            int shift = stoi(toks[3]);
            int reg2 = regint(toks[2]);
            int old2 = reg[reg2];
            reg[reg1] = reg[reg2] >> shift;
            cout << "reg[" << reg1 << "] becomes reg[" << reg2 << "] >> " << shift << " which is " << old2 << " >> " << shift << " = " << reg[reg1] << endl;
        }
        if(toks[0] == "sw"){
            int reg1 = regint(toks[1]);
            int reg2 = regint(toks[3]);
            int offset = stoi(toks[2]);
            mem[offset + reg[reg2]] = reg[reg1];
            cout << "sw operation\n";
        }
        if(toks[0] == "lw"){
            int reg1 = regint(toks[1]);
            int reg2 = regint(toks[3]);
            int offset = stoi(toks[2]);
            reg[reg1] = mem[offset + reg[reg2]];
            cout << "lw operation\n";
        }
        cout << endl;
    }
    assert(i == v.size());
    cout << "Register values are\n";
    for(int i = 0; i < 32; i++){
        cout << "reg[" << i << "] = " << reg[i] << '\n';
    }
    cout << "Memory values are\n";
    for(int i = 0; i < 20; i++){
        cout << "mem[" << i << "] = " << mem[i] << '\n';
    }
    return 0;
}
