library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
--use IEEE.ARITH.ALL;
-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity processor is
  Port (
         clk: in std_logic;
         reset: in std_logic --to start the implementation
        );
end processor;

architecture Behavioral of processor is

type registerfile_type is array(0 to 31) of std_logic_vector(0 to 31);
type memory_type is array(0 to 3999) of std_logic_vector(0 to 31);      
type state_type is (idle, si);                                          -- idle - initial state, si - active window

signal i : integer := 0;            -- program counter (ie. pointer)
signal mem : memory_type;           -- mem is the main memory containing 4k memory locations each of 32 bits
signal reg : registerfile_type;     -- reg is the register file containing 32 register
signal state : state_type := idle;  -- state indicates the current state of the processor (it is intialized to idle)
signal clock : std_logic := '0';    
signal clock_counter : integer := 0;-- create a 50kHz clock (increasing clock period to be able to do multiple instructions in a single clock cycle)
signal memzero : integer := 1000;   -- mem(memzero) is the beginning of the data memory unit
signal src1 : integer;              -- 
signal src2 : integer;
signal dest : integer;

begin

--register numbers for t0 - t7 : 8 - 15
--register numbers for s0 - s7 : 16 - 23
--register numbers for t8 - t9 : 24 - 25

--replace dest and src1 and src2 by the correct values based on the location of i
--and convert it into an integer and back from integer to std_logic_vector
 
 -- generate 50kHz clock
process(clk)
begin
	if(clk = '1' and clk'event) then
		clock_counter <= clock_counter + 1;
		if(clock_counter = 999) then
			clock_counter <= 0;
			clock <= not clock;
		end if;
	end if;
end process;

mem(0) <=  "00000001010010110100100000100000";
reg(10) <= "00000000000000000000000000000001";
reg(11) <= "00000000000000000000000000000010";

-- processor
process(clock)
begin
--need to iterate over every code location
--add, sub, lw, sw, sll, srl
	--memzero <= 4095; --check the exact value of the memzero which demarcates the starting of the memory values
	if(clock = '1' and clock'event) then
		case state is
			when idle =>
				if(reset = '1') then    -- if start button is pressed
					state <= si;
					i <= 0; -- mem(0-1000) contains the instructions to be executed
				end if;
			when others =>
			    src1 <= to_integer(unsigned(mem(i)(6 to 10))); -- src1 is the integer value of rs (source register)
			    src2 <= to_integer(unsigned(mem(i)(11 to 15)));-- src2 is the integer value of rt (source register 2)
			    dest <= to_integer(unsigned(mem(i)(16 to 20)));-- dest is the integer value of rd/intermediate (mips -r/ mips -i) (destination regist)
				case mem(i)(0 to 5) is -- checking if it is mips r or mips i by checking the op code
					when "000000" => -- it is mips r format
						case mem(i)(26 to 31) is  -- checking the funct to identify the arithmetic instruction
							when "100000" => reg(dest) <= std_logic_vector(signed(reg(src1)) + signed(reg(src2)));  -- add 
							when "100010" => reg(dest) <= std_logic_vector(signed(reg(src1)) - signed(reg(src2)));  -- subtract
							when "000000" => 
							     case mem(i) is
							         when "00000000000000000000000000000000" => -- check if instructions is all zero, then send the state to idle
							             i <= 0;
							             state <= idle;
							         when others => 
							             reg(dest) <= std_logic_vector(shift_left(unsigned(reg(dest)), to_integer(unsigned(reg(src2)))));--shift left unsigned
							     end case; 
							when "000010" =>
							     reg(dest) <= std_logic_vector(shift_right(unsigned(reg(dest)), to_integer(unsigned(reg(src2))))); -- shift right unsigned
							when others => -- any instruction other than add,sub,lw,sw,sll,srl is ignored
							     null;
						end case;
						
						
					when "100011" =>  -- load word (address = offset + memzero + register_value)
						reg(src2) <= mem(to_integer(signed(mem(i)(16 to 31))) + memzero + to_integer(unsigned(reg(src1)))); -- offset can be negative but not the memory location
					when "101011" =>  -- save word (address = offset + memzero + register_value)
						mem(to_integer(signed(mem(i)(16 to 31))) + memzero + to_integer(unsigned(reg(src1)))) <= reg(src2);
					when others =>   -- ignore unrecognized instruction
						null;
				end case;
				i <= i + 1;
		end case;
	end if;
end process;

end Behavioral;
