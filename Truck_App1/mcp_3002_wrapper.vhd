--  Multi-Byte.vhd
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_signed.all;
--use IEEE.MATH_REAL.ALL;

library XESS;
use XESS.CommonPckg.all;
--use XESS.PulsePckg.all;
--use XESS.DelayPckg.all;
--use XESS.PulsePckg.all;

-- stuff 4 10-bit values (returned by MCP) into 5 8-bit values (mcp_array)
-- ch 0 MCP3002 bits 0->7
-- ch 0 MCP3002 bits 8->9 & ch 1 bits 0 -> 5
-- ch 1 bits 6 -> 9 & ch 2 bits 0 -> 3
-- ch 2 bits 4 -> 9 & ch 3 0 -> 1
-- ch 3 2 -> 9

entity mcp_wrapper is
	port(
		clk, reset: in std_logic;
		MCP_CS1: out std_logic;
		MCP_CLK1: out std_logic;
		MCP_DIN1: in std_logic;
		MCP_DOUT1: out std_logic;
		MCP_CS2: out std_logic;
		MCP_CLK2: out std_logic;
		MCP_DIN2: in std_logic;
		MCP_DOUT2: out std_logic;
		start: in std_logic;
		done: out std_logic;
		compact: in std_logic;
		results: out mcp_array);
end mcp_wrapper;

architecture truck_arch of mcp_wrapper is

	type calc_type1 is (
			idle, start1, start2, start3);
	signal main_reg1, main_next1: calc_type1;

	type calc_type2 is (
			idle2, start1a, start2a, start3a);
	signal main_reg2, main_next2: calc_type2;

	type finish_type is (finish_idle, finish_done);
	signal finish_reg, finish_next: finish_type;

	signal time_delay_reg, time_delay_next: unsigned(25 downto 0);		-- send_uart1
	signal diff: std_logic;
	signal msbf: std_logic;
	signal enable1: std_logic;
	signal enable2: std_logic;
	signal channel1: std_logic;
	signal channel2: std_logic;
	signal done_flag1: std_logic;
	signal done_flag2: std_logic;
	signal raw_register1: std_logic_vector(9 downto 0);
	signal raw_register2: std_logic_vector(9 downto 0);
	signal raw_register1a: std_logic_vector(9 downto 0);
	signal raw_register2a: std_logic_vector(9 downto 0);
	signal raw_register1b: std_logic_vector(9 downto 0);
	signal raw_register2b: std_logic_vector(9 downto 0);
	signal stlv_temp: std_logic_vector(7 downto 0);
	signal proc1_done, proc2_done: std_logic;

begin

MCP_unit1: entity work.mcp3002(truck_arch)
	port map(clk=>clk, reset=>reset,
		MCP_CS=>MCP_CS1,
		MCP_CLK=>MCP_CLK1,
		MCP_DIN=>MCP_DIN1,
		MCP_DOUT=>MCP_DOUT1,
		enable=>enable1,
		diff=>diff,
		channel=>channel1,
		msbf=>msbf,
		done_flag=>done_flag1,
		raw_register=>raw_register1);

MCP_unit2: entity work.mcp3002(truck_arch)
	port map(clk=>clk, reset=>reset,
		MCP_CS=>MCP_CS2,
		MCP_CLK=>MCP_CLK2,
		MCP_DIN=>MCP_DIN2,
		MCP_DOUT=>MCP_DOUT2,
		enable=>enable2,
		diff=>diff,
		channel=>channel2,
		msbf=>msbf,
		done_flag=>done_flag2,
		raw_register=>raw_register2);

-- ********************************************************************************
calc_proc1: process(clk,reset,main_reg1)
variable temp: integer range 0 to 11:= 0;
variable temp2: integer range 32 to 126:= 32;
begin
	if reset = '0' then
		main_reg1 <= idle;
		main_next1 <= idle;
		enable1 <= '0';
		channel1 <= '0';
		msbf <= '1';
		diff <= '1';
		proc1_done <= '0';

	elsif clk'event and clk = '1' then
		case main_reg1 is

			when idle =>
				if start = '1' then
					proc1_done <= '0';
					channel1 <= '0';		-- start ch 0 of 1st chip
					enable1 <= '1';
					main_next1 <= start1;
				end if;

			when start1 =>
				enable1 <= '0';
				if done_flag1 = '1' then
					raw_register1a <= raw_register1;
					main_next1 <= start2;
				end if;

			when start2 =>
				channel1 <= '1';
				enable1 <= '1';
				main_next1 <= start3;
			
			when start3 =>
				enable1 <= '0';
				if done_flag1 = '1' then
					raw_register1b <= raw_register1;
					proc1_done <= '1';
					main_next1 <= idle;
				end if;

		end case;
		main_reg1 <= main_next1;
	end if;
end process;		

-- ********************************************************************************
calc_proc2: process(clk,reset,main_reg2)
begin
	if reset = '0' then
		main_reg2 <= idle2;
		main_next2 <= idle2;
		enable2 <= '0';
		channel2 <= '0';
		proc2_done <= '0';

	elsif clk'event and clk = '1' then
		case main_reg2 is

			when idle2 =>
				if start = '1' then
					proc2_done <= '0';
					channel2 <= '0';		-- start ch 0 of 1st chip
					enable2 <= '1';
					main_next2 <= start1a;
				end if;

			when start1a =>
				enable2 <= '0';
				if done_flag2 = '1' then
					raw_register2a <= raw_register2;
					main_next2 <= start2a;
				end if;

			when start2a =>
				channel2 <= '1';
				enable2 <= '1';
				main_next2 <= start3a;
			
			when start3a =>
				enable2 <= '0';
				if done_flag2 = '1' then
					raw_register2b <= raw_register2;
					proc2_done <= '1';
					main_next2 <= idle2;
				end if;

		end case;
		main_reg2 <= main_next2;
	end if;
end process;		

-- ********************************************************************************
finish_proc2: process(clk,reset,finish_reg)
begin
	if reset = '0' then
		finish_reg <= finish_idle;
		finish_next <= finish_idle;
		results <= (others=>(others=>'0'));
		done <= '0';

	elsif clk'event and clk = '1' then
		case finish_reg is

			when finish_idle =>
				done <= '0';
				if proc2_done = '1' and proc1_done = '1' then
					if raw_register1a(9 downto 2) = X"FE" then
						results(0) <= X"FD";
					else
						results(0) <= raw_register1a(9 downto 2);
					end if;
				
					if compact = '1' then
						if raw_register1b(9 downto 2) = X"FE" then
							results(1) <= X"FD";
						else
							results(1) <= raw_register1b(9 downto 2);
						end if;	
					else
						if raw_register1a(1 downto 0) & raw_register1b(9 downto 4) = X"FE" then
							results(1) <= X"FD";
						else
							results(1) <= raw_register1a(1 downto 0) & raw_register1b(9 downto 4);
						end if;
					end if;

					if compact = '1' then
						if raw_register2a(9 downto 2) = X"FE" then
							results(2) <= X"FD";
						else results(2) <= raw_register2a(9 downto 2);
						end if;
					else
						if raw_register1b(3 downto 0) & raw_register2a(9 downto 6) = X"FE" then
							results(2) <= X"FD";
						else
							results(2) <= raw_register1b(3 downto 0) & raw_register2a(9 downto 6);
						end if;
					end if;

					if compact = '1' then
						if raw_register2b(9 downto 2) = X"FE" then
							results(3) <= X"FD";
						else results(3) <= raw_register2b(9 downto 2);
						end if;
					else	
						if raw_register2a(5 downto 0) & raw_register2b(9 downto 8) = X"FE" then
							results(3) <= X"FD";
						else
							results(3) <= raw_register2a(5 downto 0) & raw_register2b(9 downto 8);
						end if;
					end if;

					if compact = '1' then
						results(4) <= X"00";
					else	
						if raw_register2b(7 downto 0) = X"FE" then
							results(4) <= X"FD";
						else
							results(4) <= raw_register2b(7 downto 0);
						end if;
					end if;
					finish_next <= finish_done;
				end if;

			when finish_done =>
				done <= '1';
				finish_next <= finish_idle;

		end case;
		finish_reg <= finish_next;
	end if;
end process;		

end truck_arch;