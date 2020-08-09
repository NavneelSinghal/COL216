#include <bits/stdc++.h>
using namespace std;

#define REP(i, n) for (int i = 0; i < n; ++i)
#define INSTRUCTION_NOT_FOUND 0

int string_to_int(string s) {
    int ans = 0;
    int m = s.size() - 1;
    for (int i = (int)s.size() - 1; i >= 0; i--) {
        if (s[i] == '1') ans += (1) << (m - i);
    }
    return ans;
}

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
    string instr;

} if_id;

struct ID_EX {
    int pc;
    int read_data_1;
    int read_data_2;

    pair<int, int> hazard;

    string instruction;

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

    string instruction;

    WB wb;
    M m;

} ex_mem;

struct MEM_WB {
    int write_register;
    int alu_result;
    int write_data;
    bool reg_write;
    int read_data;

    string instruction;

    WB wb;

} mem_wb;

bool pc_src;
bool do_fetch;
bool not_stall;
int pc;
int hazard_type;
int cur_pc;
int jump_to;
int hazardBuffer[2];
//--------------------------------------------- VARIABLES
//----------------------------------------------------

const int MAX_INSTR = 1e5;
const int MAX_MEM = 1e7;

string instr[MAX_INSTR];
string ins[MAX_INSTR];
string cur_instr;
string cur_ins;
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

int control() {  // control returns the destination register number of the
                 // innstruction

    if (!not_stall) {  // empty instruction

        id_ex.ex.reg_dst = false;
        id_ex.ex.alu_op = 0;
        id_ex.ex.alu_src = false;

        id_ex.m.mem_read = 0;
        id_ex.m.mem_write = 0;
        id_ex.m.branch = 0;

        id_ex.wb.reg_write = 0;
        id_ex.wb.mem_to_reg = false;

        id_ex.wb.isj = (id_ex.ex.alu_op == 9);
        id_ex.wb.isjr = (id_ex.ex.alu_op == 10);
        id_ex.wb.isjal = (id_ex.ex.alu_op == 11);

        id_ex.wb.instr_last_26 = if_id.instruction.substr(6, 26);
        return -1;
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
        id_ex.wb.reg_write = 1;
        id_ex.m.branch = 0;
        if (id_ex.ex.alu_op == 9)  // for jr
        {
            id_ex.wb.reg_write = 0;
            id_ex.m.branch = 1;
        }
        id_ex.wb.mem_to_reg = false;

        id_ex.wb.isj = (id_ex.ex.alu_op == 10);
        id_ex.wb.isjr = (id_ex.ex.alu_op == 9);
        id_ex.wb.isjal = (id_ex.ex.alu_op == 11);

        id_ex.wb.instr_last_26 = if_id.instruction.substr(6, 26);
        if (id_ex.wb.isjr)  // jr
            return -4;
        else
            return string_to_int(if_id.instruction.substr(16, 5));
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

        id_ex.wb.isj = (id_ex.ex.alu_op == 10);
        id_ex.wb.isjr = (id_ex.ex.alu_op == 9);
        id_ex.wb.isjal = (id_ex.ex.alu_op == 11);

        id_ex.wb.instr_last_26 = if_id.instruction.substr(6, 26);
        return -2;
    }

    else if (sw(if_id.instruction)) {
        id_ex.ex.alu_op = 0;
        id_ex.ex.alu_src = true;

        id_ex.m.mem_read = 0;
        id_ex.m.mem_write = 1;
        id_ex.m.branch = 0;

        id_ex.wb.reg_write = 0;

        id_ex.wb.isj = (id_ex.ex.alu_op == 10);
        id_ex.wb.isjr = (id_ex.ex.alu_op == 9);
        id_ex.wb.isjal = (id_ex.ex.alu_op == 11);

        id_ex.wb.instr_last_26 = if_id.instruction.substr(6, 26);

        return -1;
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

        id_ex.wb.isj = (id_ex.ex.alu_op == 10);
        id_ex.wb.isjr = (id_ex.ex.alu_op == 9);
        id_ex.wb.isjal = (id_ex.ex.alu_op == 11);

        id_ex.wb.instr_last_26 = if_id.instruction.substr(6, 26);
        if (id_ex.ex.alu_op == 1 || id_ex.ex.alu_op == 3 ||
            id_ex.ex.alu_op == 4 || id_ex.ex.alu_op == 2)
            return -100;
        else if (id_ex.wb.isj) {
            return -3;
        } else if (id_ex.wb.isjal) {
            return -5;
        } else
            return 0;
    }
}

void IF(int i) {
    if (!do_fetch) {
        if_id.pc = pc;
        if_id.instruction = empty_instruction;
        if_id.instr = "stall";
        cout << "IF stage instruction : stalling right now\n\n";
        return;  // we don't want to fetch this instruction because right now we
                 // go through a stall
    }
    // while dealing with stalls, note that we do all this update only if the
    // hazard is not present (look for more places where we do stuff like this)
    // our plan is to keep a track of the current instruction - this will help
    // with dealing with stalls as well as in the end when we run out of
    // instructions

    cout << "IF stage instruction : " << cur_ins << endl;
    cout << endl;

    if_id.pc = i + 1;
    if_id.instruction = cur_instr;
    if_id.instr = cur_ins;

    if (pc_src) {  // hazard here - @author - akash
        // else
        //{
        // ex_mem.zero = 0;
        pc = ex_mem.add_result;
    } else {
        pc = pc + 1;
    }
}


pair<int, int> check_hazard(
    int rs, int rt, int rd)  // takes the rs,rt and rd of an arithmetic
                             // instruction and determines no. of stalls if
                             // hazards is there or no hazard is there
{
    if (rs == 0 && rt == 0 &&
        rd == 0)  // empty instruction has no hazard whatsoever
        return make_pair(0, 0);

    bool hazard = false;
    int i = 0;
    int ret_val1 = 0;
    int ret_val2 = 0;

    // encoding used retval1 - 0,10,20 for hazards and 0 for no hazards
    // encoding used retval2 - 0,10,20

    // note i goes from 1 to 0, because if its true for i=0 and 1 then we need
    // i=0 to be selected
    for (i = 1; i >= 0; i--) {
        if (hazardBuffer[i] == -1 || hazardBuffer[i] == 0) continue;
        if (hazardBuffer[i] == rs) {
            hazard = true;
            ret_val1 = 10 * (i + 1);
        }
        if (hazardBuffer[i] == rt) {
            hazard = true;
            ret_val2 = 10 * (i + 1);
        }
    }

    pair<int, int> p = make_pair(ret_val1, ret_val2);

    // no more stalling
    hazardBuffer[1] = hazardBuffer[0];
    hazardBuffer[0] = rd;

    return p;
}

int ID() {
    do_fetch = 1;
    not_stall = true;
    int rs, rt, rd;
    bool jr = false;
    bool jal = false;
    rs = string_to_int(if_id.instruction.substr(6, 5));
    rt = string_to_int(if_id.instruction.substr(11, 5));

    rd = control();  // if rd is -100 its is  branch instruction, for r format
                     // instruction the destination register number is returned

    if (rd == -2 ||
        rd == -4)  // this is lw or jr, where there is only one dependency
    {
        rt = -1;  // as rt becomes dest and only rs is a dependency

        if (rd == -4) jr = true;

        rd = string_to_int(if_id.instruction.substr(11, 5));
    }
    if (rd == -3)  // this is for j
    {
        rs = -1;
        rt = -1;  // j have no dependencies
    }
    if (rd == -5)  // this for jal as writes into register 31;
    {
        rs = -1;
        rt = -1;
        rd = 31;
        jal = true;
    }
    id_ex.instruction = if_id.instr;
    pair<int, int> hz = check_hazard(rs, rt, rd);

    id_ex.hazard = hz;

    if (hz.first == 10 && ex_mem.m.mem_read)  // stall for lw
    {
        id_ex.instr_15_11 = "00000";
        id_ex.instr_20_16 = "00000";
        id_ex.sign_extend = "00000000000000000000000000000000";
        id_ex.read_data_1 = 0;
        id_ex.read_data_2 = 0;
        id_ex.instruction = "stall";

        not_stall = false;
        control();

        return 1;
    }

    if (rd == -100) {
        do_fetch = 0;
    }

    id_ex.pc = if_id.pc;

    if (rs != -1) id_ex.read_data_1 = regs[rs];
    if (rt != -1) id_ex.read_data_2 = regs[rt];

    // this needs to be done in WB, done.
    // if(mem_wb.reg_write) regs[mem_wb.write_register] = mem_wb.write_data;

    id_ex.sign_extend = if_id.instruction.substr(16, 16);
    string extend = "";
    for (int i = 0; i < 16; i++) extend += if_id.instruction[16];
    id_ex.sign_extend = extend + id_ex.sign_extend;

    id_ex.instr_20_16 = if_id.instruction.substr(11, 5);
    id_ex.instr_15_11 = if_id.instruction.substr(16, 5);

    cout << "ID stage instruction : " << if_id.instr << endl;

    // now initialise the wb, m, ex of id_ex - work of control unit - done
    // ex - alu_src, alu_op, reg_dst
    // m - mem_read, mem_write, branch
    // wb - reg_write, mem_to_reg

    if (rd == -100)
        return 3;              // 3 - implies branch
    else if (rd == -3 || jal)  // j or jal
    {
        jump_to = string_to_int(id_ex.wb.instr_last_26);
        cout << "Jump to instruction no. : " << jump_to << endl;
        return 4;   // 4 - implies jump
    } else if (jr)  // jr
    {
        jump_to = regs[rs];
        cout << "Jump to instruction no. : " << jump_to << endl;
        return 4;
    } else {
        return 0;  // 0 - implies no hazard
    }
}

int EX() {  // i is the program counter, its needed for jal

    // forwarding
    if (id_ex.hazard.first == 10) {
        cout << "Forwarding happended" << endl;
        id_ex.read_data_1 = ex_mem.alu_result;
    }
    if (id_ex.hazard.second == 10) {
        cout << "Forwarding happended" << endl;
        id_ex.read_data_2 = ex_mem.alu_result;
    }

    // Since we are executing in reverse order, value in ex_mem.alu_result
    // is pushed into mem_wb.alu_result, so they contain same value

    if (id_ex.hazard.first == 20) {
        cout << "Forwarding happended" << endl;
        // id_ex.read_data_1 = mem_wb.alu_result;
        id_ex.read_data_1 = mem_wb.write_data;
    }
    if (id_ex.hazard.second == 20) {
        cout << "Forwarding happended" << endl;
        // id_ex.read_data_2 = mem_wb.alu_result;
        id_ex.read_data_2 = mem_wb.write_data;
    }

    int offset = string_to_int(id_ex.sign_extend);

    cout << "EX stage instruction : " << id_ex.instruction << endl;

    if (id_ex.wb.isj || id_ex.wb.isjal) {
        ex_mem.add_result = string_to_int(
            id_ex.wb.instr_last_26);  // last 26 bits of the instruction
    } else if (id_ex.wb.isjr) {
        ex_mem.add_result = id_ex.read_data_1;  // rs
    } else {
        ex_mem.add_result =
            id_ex.pc + offset +
            1;  // check if this int can be negative or not - done @akash
    }

    int alu_operand_1 = id_ex.read_data_1;
    int alu_operand_2 = (id_ex.ex.alu_src) ? string_to_int(id_ex.sign_extend)
                                           : id_ex.read_data_2;
    int shift_amt = (string_to_int(id_ex.sign_extend.substr(21, 5)));

    ex_mem.read_data_2 = id_ex.read_data_2;
    ex_mem.write_register = (id_ex.ex.reg_dst)
                                ? string_to_int(id_ex.instr_15_11)
                                : string_to_int(id_ex.instr_20_16);

    ex_mem.instruction = id_ex.instruction;

    ex_mem.wb = id_ex.wb;
    ex_mem.m = id_ex.m;

    ex_mem.zero = 0;

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
        ex_mem.wb.cur_pc = id_ex.pc;
        ex_mem.zero = 1;
    }
    if ((id_ex.ex.alu_op == 1 || id_ex.ex.alu_op == 2 || id_ex.ex.alu_op == 3 ||
         id_ex.ex.alu_op == 4) &&
        (ex_mem.zero && ex_mem.m.branch)) {
        cout << "Branch condition is true  so pc is now " << (cur_pc + offset)
             << endl;
        return offset;
    } else {
        return -1;
    }
}

void MEM() {
    // need to check if this needs to be done in the instruction fetch or here
    // we plan to do everything (all the stages of the pipeline) in the reverse
    // order to avoid overwriting, so if we do this here, then we probably dont
    // have an issue
    cout << "MEM stage instruction : " << ex_mem.instruction << endl;

    pc_src = ex_mem.zero && ex_mem.m.branch;

    mem_wb.instruction = ex_mem.instruction;

    if (ex_mem.m.mem_read) {
        mem_wb.read_data = mem[ex_mem.alu_result];
    } else {
        mem_wb.read_data = 0;
    }

    if (ex_mem.m.mem_write) {
        cout << "mem[" << ex_mem.alu_result << "] becomes "
             << ex_mem.read_data_2 << endl;
        mem[ex_mem.alu_result] = ex_mem.read_data_2;
    }

    mem_wb.alu_result =
        (!ex_mem.wb.isjal) ? ex_mem.alu_result : ex_mem.wb.cur_pc;
    mem_wb.write_register = (!ex_mem.wb.isjal) ? ex_mem.write_register : 31;

    mem_wb.wb = ex_mem.wb;
}

void WB() {
    cout << "WB stage instruction : " << mem_wb.instruction << endl;

    mem_wb.write_data =
        (mem_wb.wb.mem_to_reg) ? mem_wb.read_data : mem_wb.alu_result;

    // changed because write back happens in write back and not in the
    // instruction decode stage
    if (mem_wb.wb.reg_write) {
        regs[mem_wb.write_register] =
            mem_wb.write_data;  // possible hazard - @author - akash
        cout << "reg[" << mem_wb.write_register << "] becomes "
             << mem_wb.write_data << endl;
    }
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

string instruction_to_32(string instr) {
    string shift, offset;
    string reg[3];
    stringstream in(instr);
    getline(in, instr, ' ');
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
        out += "000011";
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
    } else if (instr == "END") {
        for (int i = 0; i < 32; i++) out += "0";
    }
    return out;
}

void init() {
    if_id.pc = 0;
    if_id.instruction = empty_instruction;
    if_id.instr = "stall";

    id_ex.pc = 0;
    id_ex.read_data_1 = 0;
    id_ex.read_data_2 = 0;
    id_ex.sign_extend = empty_instruction;
    id_ex.instr_20_16 = "00000";
    id_ex.instr_15_11 = "00000";
    id_ex.wb = {0, false};
    id_ex.m = {0, 0, 0};
    id_ex.instruction = "stall";
    id_ex.ex = {false, 0, false};

    ex_mem.write_register = 0;
    ex_mem.read_data_2 = 0;
    ex_mem.alu_result = 0;
    ex_mem.zero = 0;
    ex_mem.add_result = 0;
    ex_mem.wb = {0, false};
    ex_mem.m = {0, 0, 0};
    ex_mem.instruction = "stall";

    mem_wb.write_register = 0;
    mem_wb.alu_result = 0;
    mem_wb.write_data = 0;
    mem_wb.reg_write = 0;
    mem_wb.read_data = 0;
    mem_wb.instruction = "stall";
    mem_wb.wb = {0, false};

    pc_src = 0;
    do_fetch = true;
    not_stall = true;
    pc = 0;

    hazardBuffer[0] = -1;
    hazardBuffer[1] = -1;
}

//-------------------------------------------- MAIN FUNCTION
//-------------------------------------------------

int main(int argc, char* argv[]) {
    int pc = 0;
    int execed = -1;
    int stall;

    // register initialisation
    // regs[regint("s1")] = 8;
    // regs[regint("s2")] = -8;
    // regs[regint("s0")] = -1;
    // regs[regint("sp")] = MAX_MEM - 1;

    regs[regint("s1")] = 10;

    // parsing of input
    int i_ = 0;
    while (true) {
        getline(cin, ins[i_]);
        replace(ins[i_], '(', ' ');
        replace(ins[i_], ')', ' ');
        replace(ins[i_], ',', ' ');
        if (ins[i_] == "END_INSTRUCTIONS") {
            instr[i_] = "";
            for (int j = 0; j < 32; j++) instr[i_] += "0";
            break;
        }
        instr[i_] = instruction_to_32(ins[i_]);
        ++i_;
    }
    for (int i = 0; i < i_; i++) {
        if (instr[i] == empty_instruction) {
            instr[i] = instruction_to_32("j " + to_string(i_));
        }
    }
    instr[i_ + 1] = instr[i_];
    ins[i_ + 1] = "END";
    instr[i_ + 2] = instr[i_];
    ins[i_ + 2] = "END";
    instr[i_ + 3] = instr[i_];
    ins[i_ + 3] = "END";

    init();

    int addr;

    int debug = 0;

    for (int i = 0; i <= i_ + 3; i++) {
        cur_pc = i;
        WB();
        MEM();
        addr = EX();
        stall = ID();
        if (stall == 1) {
            i = i - 1;
        } else if (stall == 2)  // double stall
        {
            i = i - 2;
        } else if (stall == 3)  // control stall
        {
            i = i - 1;
        } else if (stall == 4)  // jump instruction !!
        {
            i = jump_to;
            cur_instr = instr[i];
            cur_ins = ins[i];
        } else {
            cur_instr = instr[i];
            cur_ins = ins[i];
        }
        if (addr != -1) {
            i = i + addr;
            cur_instr = instr[i];
            cur_ins = ins[i];
        }
        cout << "Previously fetched instruction was: " << cur_ins << " " << i
             << endl;
        IF(i);
    }

    int x = 0;
    cerr << "Register values:" << endl;
    for (x = 0; x <= 31; x++) {
        cerr << regs[x] << " ";
    }
    cerr << endl;
    cerr << "Memory values:" << endl;
    for (x = 9999999; x >= 9999900; x--) cerr << mem[x] << " ";
    cerr << endl;
}
