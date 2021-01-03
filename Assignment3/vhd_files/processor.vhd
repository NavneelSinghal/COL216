library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

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

begin

type registerfile_type is array(0 to 31) of std_logic_vector(0 to 31); -- register file having 32 registers each of 32 bits 
type memory_type is array(0 to 3999) of std_logic_vector(0 to 31); 
type state_type is (idle, si);

signal i : integer := 0;
signal mem : memory_type;
signal reg : registerfile_type;
signal state : state_type := idle;
signal clock : std_logic := '0';
signal clock_counter : integer := 0;

--register numbers for t0 - t7 : 8 - 15
--register numbers for s0 - s7 : 16 - 23
--register numbers for t8 - t9 : 24 - 25

--replace dest and src1 and src2 by the correct values based on the location of i
--and convert it into an integer and back from integer to std_logic_vector

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


process(clock)
begin
--need to iterate over every code location
--add, sub, lw, sw, sll, srl
	memzero <= 4095; --check the exact value of the memzero which demarcates the starting of the memory values
	if(clock = '1' and clock'event) then
		case state is
			when idle =>
				if(reset = 1) then
					state <= si;
					i <= 0;
				end if;
			when others =>
				case mem(i)(0 to 5) is 
					when "000000" =>
						case mem(i)(26 to 31) is 
							when "100000" => reg(dest) <= reg(src1) + reg(src2);
							when "100010" => reg(dest) <= reg(src1) - reg(src2);
							when "000000" => reg(dest) <= (shift reg(dest) by reg(src2)); --using &
							when others => reg(dest) <= (shift reg(dest) by reg(src2)); --using &
						end case;
					when "100011" =>
						reg(src2) <= mem(mem(i)(16 to 31) + memzero + reg(src1));
					when "101011" =>
						mem(mem(i)(16 to 31) + memzero + reg(src1)) <= reg(src2);
					when others =>
						i <= 0;
						state <= idle;
				end case;
				i <= i + 1;
		end case;
	end if;
end process;

end Behavioral;
