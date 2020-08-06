#include <bits/stdc++.h>
using namespace std;

#define REP(i, n) for(int i = 0; i < n; ++i)
#define INSTRUCTION_NOT_FOUND 0

//HELPER FUNCTIONS FOR STRING TO INT
int string_to_int(string s){
    int ans = 0;
    for(int i = (int)s.size() - 1; i >= 0; i--){
        ans <<= 1;
        ans += s[i] - '0';
    }
    return ans;
}

//---------------------------------------- STRUCTURE DEFINITIONS ---------------------------------------------

struct EX{

    bool alu_src;
    int alu_op;
    bool reg_dst;

};

struct M{

    int mem_read;
    int mem_write;
    int branch;

};

struct WB{

    int reg_write;
    bool mem_to_reg;

};

struct IF_ID{

    int pc;
    
    string instruction; //this is not the tokenised instruction, it is a 32 bit instruction

} if_id;

struct ID_EX{

    int pc;
    int read_data_1;
    int read_data_2;
    
    string sign_extend;
    string instr_20_16;
    string instr_15_11;
    
    WB wb;
    M m;
    EX ex;

} id_ex;

struct EX_MEM{

    int write_register; //mux
    int read_data_2;
    int alu_result;
    int zero;
    int add_result;
    
    WB wb;
    M m;

} ex_mem;

struct MEM_WB{
    
    int write_register;
    int alu_result;
    int write_data;
    bool reg_write;
    int read_data;
    
    WB wb;

} mem_wb;

bool pc_src;
bool do_fetch;
int pc;
int hazard_type;

//--------------------------------------------- VARIABLES ----------------------------------------------------

const int MAX_INSTR = 1e5;
const int MAX_MEM = 1e7;

string instr[MAX_INSTR];
string cur_instr;
string empty_instruction = "00000000000000000000000000000000";

int mem[MAX_MEM] = {0};
int regs[32] = {0}; 


//-------------------------------- FUNCTIONS FOR RUNNING STAGES OF PIPELINE ----------------------------------

//firstly assuming no hazards

//helper functions for control
bool rformat(string s){
    return (s.substr(0,6).compare("000000") == 0);
}

bool lw(string s){
    return (s.substr(0,6).compare("100011") == 0);
}

bool sw(string s){
    return (s.substr(0,6).compare("101011") == 0);
}

void control(){    
    
    if(!do_fetch){ //empty instruction

        id_ex.ex.reg_dst = false;
        id_ex.ex.alu_op = 0;
        id_ex.ex.alu_src = false;

        id_ex.m.mem_read = 0;
        id_ex.m.mem_write = 0;
        id_ex.m.branch = 0;

        id_ex.wb.reg_write = 0;
        id_ex.wb.mem_to_reg = false;
    }
    else if(rformat(if_id.instruction)){

        id_ex.ex.reg_dst = true;
        id_ex.ex.alu_op = 2;
        id_ex.ex.alu_src = false;

        id_ex.m.mem_read = 0;
        id_ex.m.mem_write = 0;
        id_ex.m.branch = 0;

        id_ex.wb.reg_write = 1;
        id_ex.wb.mem_to_reg = false;
    }
    
    else if(lw(if_id.instruction)){
        
        id_ex.ex.reg_dst = false;
        id_ex.ex.alu_op = 0;
        id_ex.ex.alu_src = true;

        id_ex.m.mem_read = 1;
        id_ex.m.mem_write = 0;
        id_ex.m.branch = 0;

        id_ex.wb.reg_write = 1;
        id_ex.wb.mem_to_reg = true;
    }
    
    else if(sw(if_id.instruction)){

        id_ex.ex.alu_op = 0;
        id_ex.ex.alu_src = true;

        id_ex.m.mem_read = 0;
        id_ex.m.mem_write = 1;
        id_ex.m.branch = 0;

        id_ex.wb.reg_write = 0;
    }
    
    else{ //branch instruction

        id_ex.ex.alu_op = 1;
        id_ex.ex.alu_src = false;

        id_ex.m.mem_read = 0;
        id_ex.m.mem_write = 0;
        id_ex.m.branch = 1;

        id_ex.wb.reg_write = 0;
    }
}

void IF(){

    if(!do_fetch) return; 
    
    //while dealing with stalls, note that we do all this update only if the hazard is not present (look for more places where we do stuff like this)
    //our plan is to keep a track of the current instruction - this will help with dealing with stalls as well as in the end when we run out of instructions
    if_id.pc = pc + 1;
    if_id.instruction = cur_instr;

    if(pc_src){ //hazard here - @author - akash
        pc = ex_mem.add_result;
    }
    else{
        pc = pc + 1;
    }
    if(string_to_int(if_id.instruction) == 0){
        do_fetch = false;
    }
    else{
        do_fetch = true;
    }

}

void ID(){
    
    id_ex.pc = if_id.pc;

    id_ex.read_data_1 = regs[string_to_int(if_id.instruction.substr(6, 5))];
    id_ex.read_data_2 = regs[string_to_int(if_id.instruction.substr(11, 5))];

    //this needs to be done in WB, done.
    //if(mem_wb.reg_write) regs[mem_wb.write_register] = mem_wb.write_data;

    id_ex.sign_extend = if_id.instruction.substr(16, 16);
    string extend = "";
    for(int i = 0; i < 16; i++) extend += if_id.instruction[16];
    id_ex.sign_extend = extend + id_ex.sign_extend;
    
    id_ex.instr_20_16 = if_id.instruction.substr(11, 5);
    id_ex.instr_15_11 = if_id.instruction.substr(16, 5);

    //now initialise the wb, m, ex of id_ex - work of control unit - done
    //ex - alu_src, alu_op, reg_dst
    //m - mem_read, mem_write, branch
    //wb - reg_write, mem_to_reg
    
    control();

}

void EX(){

    ex_mem.add_result = id_ex.pc + string_to_int(id_ex.sign_extend); //check if this int can be negative or not

    //TODO: find what alu should do, using the aluop and last 6 bits of id_ex.sign_extend or something - work of the alu control unit

    //TODO: now compute the result in alu and assign ex_mem's corresponding registers to the result (also see what zero is) - work of the alu
    
    int alu_operand_1 = id_ex.read_data_1;
    int alu_operand_2 = (id_ex.ex.alu_src) ? string_to_int(id_ex.sign_extend) : id_ex.read_data_2;

    ex_mem.read_data_2 = id_ex.read_data_2;
    ex_mem.write_register = (id_ex.ex.reg_dst) ? string_to_int(id_ex.instr_15_11) : string_to_int(id_ex.instr_20_16);

    ex_mem.wb = id_ex.wb;
    ex_mem.m = id_ex.m;
}

void MEM(){

    //need to check if this needs to be done in the instruction fetch or here
    //we plan to do everything (all the stages of the pipeline) in the reverse order to avoid overwriting, so if we do this here, then we probably dont have an issue
    pc_src = ex_mem.zero && ex_mem.m.branch;
    
    if(ex_mem.m.mem_read){
        mem_wb.read_data = mem[ex_mem.alu_result];
    }
    else{
        mem_wb.read_data = 0;
    }

    if(ex_mem.m.mem_write){
        mem[ex_mem.alu_result] = ex_mem.read_data_2;
    }

    mem_wb.alu_result = ex_mem.alu_result;
    mem_wb.write_register = ex_mem.write_register;

    mem_wb.wb = ex_mem.wb;
}

void WB(){
   
    mem_wb.write_data = (mem_wb.wb.mem_to_reg) ? mem_wb.read_data : mem_wb.alu_result;

    //changed because write back happens in write back and not in the instruction decode stage
    if(mem_wb.reg_write) regs[mem_wb.write_register] = mem_wb.write_data; //possible hazard - @author - akash

}


//------------------------------------- HELPER FUNCTIONS FOR PARSING -----------------------------------------

void replace(string &s, char a, char b){
    REP(i, (int) s.size()){
        if(s[i] == a) s[i] = b;
    }
}

int regint(string reg){
    if(reg[0] == '$'){
        reg = reg.substr(1);
    }
    if(reg == "zero") return 0;
    else if(reg == "at") return 1;
    else if(reg[0] == 'v') return 2 + reg[1] - '0';
    else if(reg[0] == 'a') return 4 + reg[1] - '0';
    else if(reg[0] == 't' && reg[1] < '8') return 8 + reg[1] - '0';
    else if(reg[0] == 't') return 24 + reg[1] - '8';
    else if(reg == "sp") return 29;
    else if(reg[0] == 's') return 16 + reg[1] - '0';
    else  if(reg[0] == 'k') return 26 + reg[1] - '0';
    else if(reg == "gp") return 28;
    else if(reg == "fp") return 30;
    else if(reg == "ra") return 31;
    else return stoi(reg);
}

string instruction_to_32(string s){
    return "";
}

//-------------------------------------------- MAIN FUNCTION -------------------------------------------------

int main(int argc, char* argv[]){

    int pc = 0;
    int cycles = 0; 
    int execed = -1;

    //register initialisation
    regs[regint("sp")] = MAX_MEM - 1;
    regs[regint("$s0")] = -1;
    regs[regint("$s1")] = 5;
    regs[regint("$s2")] = -8;
    //parsing of input
    int i_ = 0;
    while(true){
        getline(cin, instr[i_]);
        replace(instr[i_], '(', ' ');
        replace(instr[i_], ')', ' ');
        replace(instr[i_], ',', ' ');
        if(instr[i_] == "END_INSTRUCTIONS"){
            instr[i_] = "";
            for(int j = 0; j < 32; j++) instr[i_] += "0";
            break;
        }
        instr[i_] = instruction_to_32(instr[i_]);
        ++i_;
    }

    //for this, we model the state of the program as {IFID, IDEX, EXME, MEWB} registers
    //
    //now we need to think what all we need to store in all 4 register sets
    //for IFID, we should store the whole instruction and the program counter
    //for IDEX, we should store
    //for EXMEM, we should store
    //for MEMWB, we should store
    //
    //Then we need to make functions for each stage of pipeline that run this stage and return set of registers for the next stage of the pipeline for this instruction
    //We also need to check for hazards
    
    for(int i = 0; i < i_ + 3; i++){
        //write back
        WB();
        //memory access
        MEM();
        //execute
        EX();
        //instruction decode
        ID();
        if(i < i_ - 1) cur_instr = instr[i];
        else cur_instr = empty_instruction;
        IF();
        //now that we are done with this stage, we see the kind of hazard that has been encountered
        //the hazard type is stored in hazard_type
        //hazard handling starts

    }


}
