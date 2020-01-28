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

-- stuff 8 10-bit values (returned by MCP) into 10 8-bit values (mcp_array)
-- mpc_array(0) ch 0 MCP3002 bits 0-> 7
-- mpc_array(1) ch 0 MCP3002 bits 8-> 9 & ch 1 MCP3002 bits 0 -> 5
-- mpc_array(2) ch 1 MCP3002 bits 6 -> 9 & ch 2 MCP3002 bits 0 -> 3
-- mpc_array(3) ch 2 MCP3002 bits 4 -> 9 & ch 3 MCP3002 0 -> 1
-- mpc_array(4) ch 3 MCP3002 bits 2 -> 9
-- mpc_array(5) ch 4 MCP3002 bits 0 -> 7
-- mpc_array(6) ch 4 MCP3002 bits 8 -> 9 & ch 5 MCP3002 bits 0 -> 5
-- mpc_array(7) ch 5 MCP3002 bits 6 -> 9 & ch 6 MCP3002 bits 0 -> 3
-- mpc_array(8) ch 6 MCP3002 bits 4 -> 9 & ch 7 MCP3002 bits 0 -> 1
-- mpc_array(9) ch 7 MCP3002 bits 2 -> 9

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
		MCP_CS3: out std_logic;
		MCP_CLK3: out std_logic;
		MCP_DIN3: in std_logic;
		MCP_DOUT3: out std_logic;
		MCP_CS4: out std_logic;
		MCP_CLK4: out std_logic;
		MCP_DIN4: in std_logic;
		MCP_DOUT4: out std_logic;
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

	type calc_type3 is (
			idle3, start1b, start2b, start3b);
	signal main_reg3, main_next3: calc_type3;

	type calc_type4 is (
			idle4, start1c, start2c, start3c);
	signal main_reg4, main_next4: calc_type4;

	type finish_type is (finish_idle, finish_done);
	signal finish_reg, finish_next: finish_type;

--	signal time_delay_reg, time_delay_next: unsigned(25 downto 0);		-- send_uart1
	signal diff: std_logic;
	signal msbf: std_logic;
	signal enable1: std_logic;
	signal enable2: std_logic;
	signal enable3: std_logic;
	signal enable4: std_logic;
	signal channel1: std_logic;
	signal channel2: std_logic;
	signal channel3: std_logic;
	signal channel4: std_logic;
	signal done_flag1: std_logic;
	signal done_flag2: std_logic;
	signal done_flag3: std_logic;
	signal done_flag4: std_logic;
	signal raw_register1: std_logic_vector(9 downto 0);
	signal raw_register2: std_logic_vector(9 downto 0);
	signal raw_register3: std_logic_vector(9 downto 0);
	signal raw_register4: std_logic_vector(9 downto 0);
	signal raw_register1a: std_logic_vector(9 downto 0);
	signal raw_register1b: std_logic_vector(9 downto 0);
	signal raw_register2a: std_logic_vector(9 downto 0);
	signal raw_register2b: std_logic_vector(9 downto 0);
	signal raw_register3a: std_logic_vector(9 downto 0);
	signal raw_register3b: std_logic_vector(9 downto 0);
	signal raw_register4a: std_logic_vector(9 downto 0);
	signal raw_register4b: std_logic_vector(9 downto 0);
	signal stlv_temp: std_logic_vector(7 downto 0);
	signal proc1_done,  proc2_done, proc3_done, proc4_done: std_logic;

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

MCP_unit3: entity work.mcp3002(truck_arch)
	port map(clk=>clk, reset=>reset,
		MCP_CS=>MCP_CS3,
		MCP_CLK=>MCP_CLK3,
		MCP_DIN=>MCP_DIN3,
		MCP_DOUT=>MCP_DOUT3,
		enable=>enable3,
		diff=>diff,
		channel=>channel3,
		msbf=>msbf,
		done_flag=>done_flag3,
		raw_register=>raw_register3);

MCP_unit4: entity work.mcp3002(truck_arch)
	port map(clk=>clk, reset=>reset,
		MCP_CS=>MCP_CS4,
		MCP_CLK=>MCP_CLK4,
		MCP_DIN=>MCP_DIN4,
		MCP_DOUT=>MCP_DOUT4,
		enable=>enable4,
		diff=>diff,
		channel=>channel4,
		msbf=>msbf,
		done_flag=>done_flag4,
		raw_register=>raw_register4);

-- ********************************************************************************
calc_proc1: process(clk,reset,main_reg1)
begin
	if reset = '0' then
		main_reg1 <= idle;
		main_next1 <= idle;
		enable1 <= '0';
		channel1 <= '0';
		msbf <= '1';
		diff <= '1';
		proc1_done <= '0';
		raw_register1a <= (others=>'0');
		raw_register1b <= (others=>'0');

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
		raw_register2a <= (others=>'0');
		raw_register2b <= (others=>'0');

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
calc_proc3: process(clk,reset,main_reg3)
begin
	if reset = '0' then
		main_reg3 <= idle3;
		main_next3 <= idle3;
		enable3 <= '0';
		channel3 <= '0';
		proc3_done <= '0';
		raw_register3a <= (others=>'0');
		raw_register3b <= (others=>'0');

	elsif clk'event and clk = '1' then
		case main_reg3 is

			when idle3 =>
				if start = '1' then
					proc3_done <= '0';
					channel3 <= '0';		-- start ch 0 of 1st chip
					enable3 <= '1';
					main_next3 <= start1b;
				end if;

			when start1b =>
				enable3 <= '0';
				if done_flag3 = '1' then
					raw_register3a <= raw_register3;
					main_next3 <= start2b;
				end if;

			when start2b =>
				channel3 <= '1';
				enable3 <= '1';
				main_next3 <= start3b;

			when start3b =>
				enable3 <= '0';
				if done_flag3 = '1' then
					raw_register3b <= raw_register3;
					proc3_done <= '1';
					main_next3 <= idle3;
				end if;

		end case;
		main_reg3 <= main_next3;
	end if;
end process;

-- ********************************************************************************
calc_proc4: process(clk,reset,main_reg4)
begin
	if reset = '0' then
		main_reg4 <= idle4;
		main_next4 <= idle4;
		enable4 <= '0';
		channel4 <= '0';
		proc4_done <= '0';
		raw_register4a <= (others=>'0');
		raw_register4b <= (others=>'0');

	elsif clk'event and clk = '1' then
		case main_reg4 is

			when idle4 =>
				if start = '1' then
					proc4_done <= '0';
					channel4 <= '0';		-- start ch 0 of 1st chip
					enable4 <= '1';
					main_next4 <= start1c;
				end if;

			when start1c =>
				enable4 <= '0';
				if done_flag4 = '1' then
					raw_register4a <= raw_register4;
					main_next4 <= start2c;
				end if;

			when start2c =>
				channel4 <= '1';
				enable4 <= '1';
				main_next4 <= start3c;

			when start3c =>
				enable4 <= '0';
				if done_flag4 = '1' then
					raw_register4b <= raw_register4;
					proc4_done <= '1';
					main_next4 <= idle4;
				end if;

		end case;
		main_reg4 <= main_next4;
	end if;
end process;

-- ********************************************************************************
-- just assume compact = 1 and don't worry about getting 10-bit accuracy for now
-- (see ..\unfinished_finish_proc.vhd)
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
				if proc1_done = '1' and proc2_done = '1' and proc3_done = '1' and proc4_done = '1' then
					-- 1st one is the same whether compacted or not
					if raw_register1a(9 downto 2) = X"FE" then
						results(0) <= X"FD";
					else results(0) <= raw_register1a(9 downto 2);
					end if;
				
					if raw_register1b(9 downto 2) = X"FE" then
						results(1) <= X"FD";
					else results(1) <= raw_register1b(9 downto 2);
					end if;	

					if raw_register2a(9 downto 2) = X"FE" then
						results(2) <= X"FD";
					else results(2) <= raw_register2a(9 downto 2);
					end if;

					if raw_register2b(9 downto 2) = X"FE" then
						results(3) <= X"FD";
					else results(3) <= raw_register2b(9 downto 2);
					end if;

					if raw_register3a(9 downto 2) = X"FE" then
						results(4) <= X"FD";
					else results(4) <= raw_register3a(9 downto 2);
					end if;

					if raw_register3b(9 downto 2) = X"FE" then
						results(5) <= X"FD";
					else results(5) <= raw_register3b(9 downto 2);
					end if;

					if raw_register4a(9 downto 2) = X"FE" then
						results(6) <= X"FD";
					else results(6) <= raw_register4a(9 downto 2);
					end if;

					if raw_register4b(9 downto 2) = X"FE" then
						results(7) <= X"FD";
					else results(7) <= raw_register4b(9 downto 2);
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
