#include <bits/stdc++.h>
using namespace std;

#define REP(i, n) for (int i = 0; i < n; ++i)
#define INSTRUCTION_NOT_FOUND 0

// TODO:
// 1. implement j, jr, jal
// 2. implement hazard handling by stalling
// 3. implement instruction to 32 bits - DONE

// HELPER FUNCTIONS FOR STRING TO INT
int string_to_int(string s) {
    int ans = 0;
    for (int i = (int)s.size() - 1; i >= 0; i--) {
        ans <<= 1;
        ans += s[i] - '0';
    }
    return ans;
}

/*
your string to int is wrong

modified correct one: check and decomment - @akash

int string_to_int(string s){
    int ans = 0;
    int m = s.size()-1;
    for(int i = (int)s.size() - 1; i >= 0; i--){
        if(s[i]=='1')
            ans += (1)<<(m-i);
    }
    return ans;
}


*/

//---------------------------------------- STRUCTURE DEFINITIONS
//---------------------------------------------

struct EX {
    bool alu_src;
    int alu_op;
    bool reg_dst;
};

struct M {
    int mem_read;
    int mem_write;
    int branch;
};

struct WB {
    int reg_write;
    bool mem_to_reg;
    bool isj;
    bool isjr;
    bool isjal;
    int cur_pc;
    string instr_last_26;
};

struct IF_ID {
    int pc;

    string instruction;  // this is not the tokenised instruction, it is a 32
                         // bit instruction

} if_id;

struct ID_EX {
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

struct EX_MEM {
    int write_register;  // mux
    int read_data_2;
    int alu_result;
    int zero;
    int add_result;

    WB wb;
    M m;

} ex_mem;

struct MEM_WB {
    int write_register;
    int alu_result;
    int write_data;
    bool reg_write;
    int read_data;

    WB wb;

} mem_wb;

bool pc_src;
bool do_fetch;
bool not_stall;
int pc;
int hazard_type;
int cur_pc;

//--------------------------------------------- VARIABLES
//----------------------------------------------------

const int MAX_INSTR = 1e5;
const int MAX_MEM = 1e7;

string instr[MAX_INSTR];
string cur_instr;
string empty_instruction = "00000000000000000000000000000000";

int mem[MAX_MEM] = {0};
int regs[32] = {0};

//-------------------------------- FUNCTIONS FOR RUNNING STAGES OF PIPELINE
//----------------------------------

// firstly assuming no hazards

// helper functions for control
bool rformat(string s) { return (s.substr(0, 6).compare("000000") == 0); }

bool lw(string s) { return (s.substr(0, 6).compare("100011") == 0); }

bool sw(string s) { return (s.substr(0, 6).compare("101011") == 0); }

void control() {
    if (!not_stall) {  // empty instruction

        id_ex.ex.reg_dst = false;
        id_ex.ex.alu_op = 0;
        id_ex.ex.alu_src = false;

        id_ex.m.mem_read = 0;
        id_ex.m.mem_write = 0;
        id_ex.m.branch = 0;

        id_ex.wb.reg_write = 0;
        id_ex.wb.mem_to_reg = false;
    } else if (rformat(if_id.instruction)) {
        id_ex.ex.reg_dst = true;
        if (if_id.instruction.substr(26, 6).compare("100000") == 0)
            id_ex.ex.alu_op = 5;
        else if (if_id.instruction.substr(26, 6).compare("100010") == 0)
            id_ex.ex.alu_op = 6;
        else if (if_id.instruction.substr(26, 6).compare("000000") == 0)
            id_ex.ex.alu_op = 7;
        else if (if_id.instruction.substr(26, 6).compare("000010") == 0)
            id_ex.ex.alu_op = 8;
        else if (if_id.instruction.substr(26, 6).compare("001000") == 0)
            id_ex.ex.alu_op = 9;  // jr
        else
            id_ex.ex.alu_op = 0;

        id_ex.ex.alu_src = false;

        id_ex.m.mem_read = 0;
        id_ex.m.mem_write = 0;
        id_ex.m.branch = (id_ex.ex.alu_op == 9);  // only if jr

        id_ex.wb.reg_write = 1;
        id_ex.wb.mem_to_reg = false;
    }

    else if (lw(if_id.instruction)) {
        id_ex.ex.reg_dst = false;
        id_ex.ex.alu_op = 0;
        id_ex.ex.alu_src = true;

        id_ex.m.mem_read = 1;
        id_ex.m.mem_write = 0;
        id_ex.m.branch = 0;

        id_ex.wb.reg_write = 1;
        id_ex.wb.mem_to_reg = true;
    }

    else if (sw(if_id.instruction)) {
        id_ex.ex.alu_op = 0;
        id_ex.ex.alu_src = true;

        id_ex.m.mem_read = 0;
        id_ex.m.mem_write = 1;
        id_ex.m.branch = 0;

        id_ex.wb.reg_write = 0;
    }

    else {  // branch instruction

        if (if_id.instruction.substr(0, 6).compare("000101") == 0)  // bne
            id_ex.ex.alu_op = 1;
        else if (if_id.instruction.substr(0, 6).compare("000110") == 0)  // blez
            id_ex.ex.alu_op = 3;
        else if (if_id.instruction.substr(0, 6).compare("000111") == 0)  // bgtz
            id_ex.ex.alu_op = 4;
        else if (if_id.instruction.substr(0, 6).compare("000100") == 0)  // beq
            id_ex.ex.alu_op = 2;
        else if (if_id.instruction.substr(0, 6).compare("000010") == 0)
            id_ex.ex.alu_op = 10;  // j
        else if (if_id.instruction.substr(0, 6).compare("000011") == 0)
            id_ex.ex.alu_op = 11;  // jal
        else
            id_ex.ex.alu_op = 0;

        id_ex.ex.alu_src = false;

        id_ex.m.mem_read = 0;
        id_ex.m.mem_write = 0;
        id_ex.m.branch = 1;

        id_ex.wb.reg_write =
            (id_ex.ex.alu_op == 11);  // we only write in the case of jal
    }

    id_ex.wb.isj = (id_ex.ex.alu_op == 9);
    id_ex.wb.isjr = (id_ex.ex.alu_op == 10);
    id_ex.wb.isjal = (id_ex.ex.alu_op == 11);

    id_ex.wb.instr_last_26 = if_id.instruction.substr(6, 26);

    id_ex.wb.cur_pc = if_id.pc;  // TODO: check if this is pc or pc + 1
}

void IF() {
    if (!do_fetch)
        return;  // we don't want to fetch this instruction because right now we
                 // go through a stall

    // while dealing with stalls, note that we do all this update only if the
    // hazard is not present (look for more places where we do stuff like this)
    // our plan is to keep a track of the current instruction - this will help
    // with dealing with stalls as well as in the end when we run out of
    // instructions
    if_id.pc = pc + 1;
    if_id.instruction = cur_instr;

    if (pc_src) {  // hazard here - @author - akash
        // else
        //{
        // ex_mem.zero = 0;
        pc = ex_mem.add_result;
    } else {
        pc = pc + 1;
    }
    if (string_to_int(if_id.instruction) ==
        0) {  // we signal to the control of this intruction that this is an
              // empty instruction
        not_stall = false;
    } else {
        not_stall = true;
    }
}

void ID() {
    id_ex.pc = if_id.pc;

    id_ex.read_data_1 = regs[string_to_int(if_id.instruction.substr(6, 5))];
    id_ex.read_data_2 = regs[string_to_int(if_id.instruction.substr(11, 5))];

    // this needs to be done in WB, done.
    // if(mem_wb.reg_write) regs[mem_wb.write_register] = mem_wb.write_data;

    id_ex.sign_extend = if_id.instruction.substr(16, 16);
    string extend = "";
    for (int i = 0; i < 16; i++) extend += if_id.instruction[16];
    id_ex.sign_extend = extend + id_ex.sign_extend;

    id_ex.instr_20_16 = if_id.instruction.substr(11, 5);
    id_ex.instr_15_11 = if_id.instruction.substr(16, 5);

    // now initialise the wb, m, ex of id_ex - work of control unit - done
    // ex - alu_src, alu_op, reg_dst
    // m - mem_read, mem_write, branch
    // wb - reg_write, mem_to_reg

    control();
}

void EX() {
    if (id_ex.wb.isj || id_ex.wb.isjal) {
        ex_mem.add_result = string_to_int(
            id_ex.wb.instr_last_26);  // last 26 bits of the instruction
    } else if (id_ex.wb.isjr) {
        ex_mem.add_result = id_ex.read_data_1;  // rs
    } else
        ex_mem.add_result =
            id_ex.pc +
            string_to_int(id_ex.sign_extend);  // check if this int can be
                                               // negative or not - done @akash

    int alu_operand_1 = id_ex.read_data_1;
    int alu_operand_2 = (id_ex.ex.alu_src) ? string_to_int(id_ex.sign_extend)
                                           : id_ex.read_data_2;
    int shift_amt = (string_to_int(id_ex.sign_extend.substr(5, 5)));
    ex_mem.read_data_2 = id_ex.read_data_2;
    ex_mem.write_register = (id_ex.ex.reg_dst)
                                ? string_to_int(id_ex.instr_15_11)
                                : string_to_int(id_ex.instr_20_16);

    ex_mem.wb = id_ex.wb;
    ex_mem.m = id_ex.m;

    ex_mem.zero = 0;

    // TODO: find what alu should do, using the aluop and last 6 bits of
    // id_ex.sign_extend or something - work of the alu control unit - DONE
    if (id_ex.ex.alu_op == 0)  // sw or lw
    {
        ex_mem.alu_result = alu_operand_1 + alu_operand_2;
    } else if (id_ex.ex.alu_op == 1)  // bne
    {
        if (id_ex.read_data_1 != id_ex.read_data_2) {
            ex_mem.zero = 1;
        } else {
            ex_mem.zero = 0;
        }

    } else if (id_ex.ex.alu_op == 2)  // beq
    {
        if (alu_operand_2 == alu_operand_1) {
            ex_mem.zero = 1;
        } else {
            ex_mem.zero = 0;
        }
    } else if (id_ex.ex.alu_op == 3)  // blez
    {
        if (alu_operand_1 <= 0) {
            ex_mem.zero = 1;
        } else {
            ex_mem.zero = 0;
        }
    } else if (id_ex.ex.alu_op == 4)  // bgtz
    {
        if (alu_operand_1 > 0) {
            ex_mem.zero = 1;
        } else {
            ex_mem.zero = 0;
        }
    } else if (id_ex.ex.alu_op == 5)  // add
    {
        ex_mem.alu_result = alu_operand_1 + alu_operand_2;

    } else if (id_ex.ex.alu_op == 6)  // sub
    {
        ex_mem.alu_result = alu_operand_1 - alu_operand_2;
    } else if (id_ex.ex.alu_op == 7)  // sll
    {
        ex_mem.alu_result = alu_operand_2 << shift_amt;
    } else if (id_ex.ex.alu_op == 8)  // srl
    {
        ex_mem.alu_result = alu_operand_2 >> shift_amt;
    } else if (id_ex.ex.alu_op == 9) {  // jr
        ex_mem.zero = 1;
    } else if (id_ex.ex.alu_op == 10) {  // j
        ex_mem.zero = 1;
    } else if (id_ex.ex.alu_op == 11) {  // jal
        ex_mem.zero = 1;
    }
}

void MEM() {
    // need to check if this needs to be done in the instruction fetch or here
    // we plan to do everything (all the stages of the pipeline) in the reverse
    // order to avoid overwriting, so if we do this here, then we probably dont
    // have an issue
    pc_src = ex_mem.zero && ex_mem.m.branch;

    if (ex_mem.m.mem_read) {
        mem_wb.read_data = mem[ex_mem.alu_result];
    } else {
        mem_wb.read_data = 0;
    }

    if (ex_mem.m.mem_write) {
        mem[ex_mem.alu_result] = ex_mem.read_data_2;
    }

    mem_wb.alu_result =
        (!ex_mem.wb.isjal) ? ex_mem.alu_result : ex_mem.wb.cur_pc;
    mem_wb.write_register = (!ex_mem.wb.isjal) ? ex_mem.write_register : 31;

    // casual TODO: can we do this^ in previous stages too by doing this change
    // in the previous stage itself? (only for minimising the number of stalls,
    //not relevant now, remember that premature optimization is the root cause
    //of all evil)

    mem_wb.wb = ex_mem.wb;
}

void WB() {
    mem_wb.write_data =
        (mem_wb.wb.mem_to_reg) ? mem_wb.read_data : mem_wb.alu_result;

    // changed because write back happens in write back and not in the
    // instruction decode stage
    if (mem_wb.reg_write)
        regs[mem_wb.write_register] =
            mem_wb.write_data;  // possible hazard - @author - akash
}

//------------------------------------- HELPER FUNCTIONS FOR PARSING
//-----------------------------------------

void replace(string& s, char a, char b) {
    REP(i, (int)s.size()) {
        if (s[i] == a) s[i] = b;
    }
}

int regint(string reg) {
    if (reg[0] == '$') {
        reg = reg.substr(1);
    }
    if (reg == "zero")
        return 0;
    else if (reg == "at")
        return 1;
    else if (reg[0] == 'v')
        return 2 + reg[1] - '0';
    else if (reg[0] == 'a')
        return 4 + reg[1] - '0';
    else if (reg[0] == 't' && reg[1] < '8')
        return 8 + reg[1] - '0';
    else if (reg[0] == 't')
        return 24 + reg[1] - '8';
    else if (reg == "sp")
        return 29;
    else if (reg[0] == 's')
        return 16 + reg[1] - '0';
    else if (reg[0] == 'k')
        return 26 + reg[1] - '0';
    else if (reg == "gp")
        return 28;
    else if (reg == "fp")
        return 30;
    else if (reg == "ra")
        return 31;
    else
        return stoi(reg);
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

string instruction_to_32(string s) {
    string shift, offset;
    string instr, reg[3];
    stringstream in(s);
    getline(in, s, ' ');
    string out;
    if (instr == "add") {
        out += "000000";
        getline(in, reg[2], ' ');
        getline(in, reg[0], ' ');
        getline(in, reg[1], ' ');
        out += regtostring(reg[0]);
        out += regtostring(reg[1]);
        out += regtostring(reg[2]);
        out += "00000";
        out += "100000";
    } else if (instr == "sub") {
        out += "000000";
        getline(in, reg[2], ' ');
        getline(in, reg[0], ' ');
        getline(in, reg[1], ' ');
        out += regtostring(reg[0]);
        out += regtostring(reg[1]);
        out += regtostring(reg[2]);
        out += "00000";
        out += "100010";
    } else if (instr == "lw") {
        out += "100011";
        getline(in, reg[1], ' ');
        getline(in, offset, ' ');
        getline(in, reg[0], ' ');
        out += regtostring(reg[0]);
        out += regtostring(reg[1]);
        out += inttobin16(stoi(offset));
    } else if (instr == "sw") {
        out += "101011";
        getline(in, reg[1], ' ');
        getline(in, offset, ' ');
        getline(in, reg[0], ' ');
        out += regtostring(reg[0]);
        out += regtostring(reg[1]);
        out += inttobin16(stoi(offset));
    } else if (instr == "sll") {
        out += "000000";
        out += "00000";
        getline(in, reg[1], ' ');
        getline(in, reg[0], ' ');
        getline(in, shift, ' ');
        out += (regtostring(reg[0]) + regtostring(reg[1]));
        out += inttobin(stoi(shift));
        out += "000000";
    } else if (instr == "srl") {
        out += "000000";
        out += "00000";
        getline(in, reg[1], ' ');
        getline(in, reg[0], ' ');
        getline(in, shift, ' ');
        out += (regtostring(reg[0]) + regtostring(reg[1]));
        out += inttobin(stoi(shift));
        out += "000010";
    } else if (instr == "j") {
        out += "000010";
        out += "0000000000";
        getline(in, offset, ' ');
        out += inttobin16(stoi(offset));
    } else if (instr == "jr") {
        out += "000000";
        string r;
        getline(in, r, ' ');
        out += regtostring(r);
        out += "000000000000000";
        out += "001000";
    } else if (instr == "jal") {
        out += "000010";
        out += "0000000000";
        getline(in, offset, ' ');
        out += inttobin16(stoi(offset));
    } else if (instr == "bne") {
        out += "000101";
        string r;
        getline(in, r, ' ');
        out += regtostring(r);
        getline(in, r, ' ');
        out += regtostring(r);
        getline(in, offset, ' ');
        out += inttobin16(stoi(offset));
    } else if (instr == "beq") {
        out += "000100";
        string r;
        getline(in, r, ' ');
        out += regtostring(r);
        getline(in, r, ' ');
        out += regtostring(r);
        getline(in, offset, ' ');
        out += inttobin16(stoi(offset));
    } else if (instr == "blez") {
        out += "000110";
        string r;
        getline(in, r, ' ');
        out += regtostring(r);
        out += "00000";
        getline(in, offset, ' ');
        out += inttobin16(stoi(offset));
    } else if (instr == "bgtz") {
        out += "000111";
        string r;
        getline(in, r, ' ');
        out += regtostring(r);
        out += "00000";
        getline(in, offset, ' ');
        out += inttobin16(stoi(offset));
    }
    return out;
}

//-------------------------------------------- MAIN FUNCTION
//-------------------------------------------------

int main(int argc, char* argv[]) {
    int pc = 0;
    int cycles = 0;
    int execed = -1;

    // register initialisation
    regs[regint("sp")] = MAX_MEM - 1;

    // parsing of input
    int i_ = 0;
    while (true) {
        getline(cin, instr[i_]);
        replace(instr[i_], '(', ' ');
        replace(instr[i_], ')', ' ');
        replace(instr[i_], ',', ' ');
        if (instr[i_] == "END_INSTRUCTIONS") {
            instr[i_] = "";
            for (int j = 0; j < 32; j++) instr[i_] += "0";
            break;
        }
        instr[i_] = instruction_to_32(instr[i_]);
        ++i_;
    }

    // for this, we model the state of the program as {IFID, IDEX, EXME, MEWB}
    // registers
    //
    // now we need to think what all we need to store in all 4 register sets
    // for IFID, we should store the whole instruction and the program counter
    // for IDEX, we should store
    // for EXMEM, we should store
    // for MEMWB, we should store
    //
    // Then we need to make functions for each stage of pipeline that run this
    // stage and return set of registers for the next stage of the pipeline for
    // this instruction We also need to check for hazards

    for (int i = 0; i < i_ + 3; i++) {
        cur_pc = i;
        // write back
        WB();
        // memory access
        MEM();
        // execute
        EX();
        // instruction decode
        ID();
        if (i < i_ - 1)
            cur_instr = instr[i];
        else
            cur_instr = empty_instruction;
        IF();
        // now that we are done with this stage, we see the kind of hazard that
        // has been encountered the hazard type is stored in hazard_type
    }
}
