#include <bits/stdc++.h>
using namespace std;

#define MAX_INSTRUCTIONS 32
#define MEM_MAX 255
#define MEM_ZERO 32
#define MAX_INT 0X7FFFFFFF
#define REP(i, n) for(int i = 0; i < n; i++)

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
uniform_int_distribution<int> dist18(0, 17);
uniform_int_distribution<int> distbig(0, MAX_INT);
uniform_int_distribution<int> distmem(0, MEM_MAX - MEM_ZERO);

int main(){
	srand(time(NULL));
	string instr[6] = {"add", "sub", "sll", "srl", "sw", "lw"};
	string regs[18];
	for(int i = 0; i < 18; i++){
		if(i < 8){
			regs[i] = "t" + to_string(i);
		}
		else if (i < 16){
			regs[i] = "s" + to_string(i - 8);
		}
		else{
			regs[i] = "t" + to_string(i - 8);
		}
	}
	int numinstr = MAX_INSTRUCTIONS;
	
	REP(i, numinstr){
		string command = instr[rand()%4];
		string args[3];
		if(command == "add" || command == "sub"){
			cout << command << " ";
			REP(j, 3) cout << regs[dist18(rng)] << " ";
			cout << endl;
		}
		else if (command == "sll" || command == "srl"){
			cout << command << " ";
			REP(j, 2){
				cout << regs[dist18(rng)] << " ";
			}
			cout << rand()%32 << endl;
		}
		else{
			cout << command << " ";
			cout << regs[dist18(rng)] << " ";
			cout << distmem(rng) << " ";
			cout << regs[dist18(rng)] << endl;
		}
	}
	cout << ".\n";
	REP(i, 32){
		cout << 1 << endl;
		//cout << distbig(rng) << endl;
	}
}
