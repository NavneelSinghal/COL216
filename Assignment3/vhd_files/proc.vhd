library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use STD.textio.all;
use IEEE.NUMERIC_STD.ALL;

entity processor is
  Port (
         clk: in std_logic;
         reset: in std_logic --to start the implementation
        );
end processor;

architecture Behavioral of processor is

type registerfile_type is array(0 to 36) of std_logic_vector(0 to 31);
type memory_type is array(0 to 256) of std_logic_vector(0 to 31);
type state_type is (idle, si);      -- idle - initial state, si - active window

signal i : integer := 0;            -- program counter (ie. pointer)
signal mem : memory_type;           -- mem is the main memory containing 4k memory locations each of 32 bits
signal reg : registerfile_type;     -- reg is the register file containing 32 register
signal state : state_type := idle;  -- state indicates the current state of the processor (it is intialized to idle)
signal memzero : integer := 32;     -- mem(memzero) is the beginning of the data memory unit
signal src1 : integer;
signal src2 : integer;
signal dest : integer;
signal n : integer := 0;

begin
--register numbers for t0 - t7 : 8 - 15
--register numbers for s0 - s7 : 16 - 23
--register numbers for t8 - t9 : 24 - 25

src1 <= to_integer(unsigned(mem(i)(6 to 10)));
src2 <= to_integer(unsigned(mem(i)(11 to 15)));
dest <= 

process(clk)
        file file_pointer: text;
        file file_pointer1: text;
        variable line_content : bit_vector(0 to 31);
        variable line_content2 : bit_vector(0 to 31);
        variable line_num2 : line;
        variable line_num : line;
        variable h : integer := 0;
        variable r : integer := 0;

begin
    --mem(0) <=  "00000001010010110100100000100000";
    if(n = 0) then
       file_open(file_pointer,"/home/btech/cs1180436/Downloads/Assignment3/machinecode",READ_MODE);
       while not endfile(file_pointer) loop
            readline (file_pointer,line_num);
            READ (line_num,line_content);
            mem(h) <=To_stdlogicvector(line_content);
            h := h + 1;
        end loop;
        file_close(file_pointer);
		reg(9) <= "00000000000000000000000000001010";
		reg(10) <= "00000000000000000000001111101000";
		reg(11) <= "00000000000000000000000000000000";
		reg(17) <= "00000000000000000010011100010000";
    end if;
    n <= 1;
    
    if(clk = '1' and clk'event) then
        case state is
            when idle =>
                if(reset = '1') then    -- if start button is pressed
                    state <= si;
                    i <= 0; -- mem(0-1000) contains the instructions to be executed
                end if;
            when others =>

                case mem(i)(0 to 5) is -- checking if it is mips r or mips i by checking the op code
                    when "000000" => -- it is mips r format
                        case mem(i)(26 to 31) is  -- checking the funct to identify the arithmetic instruction
                            when "100000" => reg(to_integer(unsigned(mem(i)(6 to 10)))) <= std_logic_vector(signed(reg(to_integer(unsigned(mem(i)(6 to 10))))) + signed(reg(to_integer(unsigned(mem(i)(11 to 15))))));  -- add
                            when "100010" => reg(to_integer(unsigned(mem(i)(6 to 10)))) <= std_logic_vector(signed(reg(to_integer(unsigned(mem(i)(6 to 10))))) - signed(reg(to_integer(unsigned(mem(i)(11 to 15))))));  -- subtract
                            when "000000" =>
                                 case mem(i) is
                                     when "00000000000000000000000000000000" => -- check if instructions is all zero, then send the state to idle
                                         i <= 0;
                                         state <= idle;
                                     when others =>
                                         reg(to_integer(unsigned(mem(i)(6 to 10)))) <= std_logic_vector(shift_left(unsigned(reg(to_integer(unsigned(mem(i)(6 to 10))))), to_integer(unsigned(reg(to_integer(unsigned(mem(i)(11 to 15))))))));--shift left unsigned
                                 end case;
                            when "000010" =>
                                 reg(to_integer(unsigned(mem(i)(6 to 10)))) <= std_logic_vector(shift_right(unsigned(reg(to_integer(unsigned(mem(i)(6 to 10))))), to_integer(unsigned(reg(to_integer(unsigned(mem(i)(11 to 15)))))))); -- shift right unsigned
                            when others => -- any instruction other than add,sub,lw,sw,sll,srl is ignored
                                 null;
                        end case;

                    when "100011" =>  -- load word (address = offset + memzero + register_value)
                        reg(src2) <= mem(to_integer(signed(mem(i)(16 to 31))) + memzero + to_integer(unsigned(reg(to_integer(unsigned(mem(i)(6 to 10))))))); -- offset can be negative but not the memory location
                    when "101011" =>  -- save word (address = offset + memzero + register_value)
                        mem(to_integer(signed(mem(i)(16 to 31))) + memzero + to_integer(unsigned(reg(src1)))) <= reg(to_integer(unsigned(mem(i)(11 to 15))));
                    when others =>   -- ignore unrecognized instruction
                        null;
                end case;
                i <= i + 1;

        end case;
    end if;
end process;

end Behavioral;
