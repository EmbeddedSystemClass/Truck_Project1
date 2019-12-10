-- testing the MCP3002 SPI data lines:
-- The  first  clock received with CS low and DIN high will constitute a startbit.
-- The SGL/DIFF bit and the ODD/SIGN bit follow the start bit and are used to select
-- the input channel configuration.
-- 1) pull CS low and DIN high
-- 2) clk low
-- 3) clk high
-- 4) DIN = SGL/DIFF
-- 5) cycle clk
-- 6) DIN = ODD/SIGN
-- 7) cycle clock


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

entity MCP3002 is
	port(
		clk, reset: in std_logic;
		MCP_CS: out std_logic;
		MCP_CLK: out std_logic;
		MCP_DIN: in std_logic;
		MCP_DOUT: out std_logic;
		enable: in std_logic;
		diff: in std_logic;
		channel: in std_logic;
		msbf: in std_logic;
		done_flag: out std_logic;
		raw_register: out std_logic_vector(9 downto 0));
end MCP3002;

architecture truck_arch of MCP3002 is

	constant MCP_TIME_DELAY:  integer:=  1000;

	type state_MCP3002 is (
			M_start,
			M_preshift1,
			M_preshift2,
			M_preshift3,
			M_shift1,
			M_results0,
			M_results1,
			M_results2);

	signal MCP3002_reg, MCP3002_next: state_MCP3002;

	signal time_delay_reg2, time_delay_next2: unsigned(24 downto 0);
	
	signal results: std_logic_vector(9 downto 0);
	signal shift: std_logic_vector(7 downto 0);
	signal pre_shift: std_logic_vector(7 downto 0);
	signal skip: std_logic;
	signal skip2: std_logic;
--	signal test_bit: std_logic;
--	signal test_bits1: std_logic_vector(11 downto 0);
	signal  start_shift: std_logic_vector(3 downto 0):= "1101";	-- 1st 4 bits: 
																-- start bit: 1 (always)
																-- sgl/diff: 1 (for single-ended)
																-- odd/sign: 0 for channel 0, 
																-- 1 for channel 1
																-- MSBF bit: 1 for MSB first

begin

MCP3002_unit: process(clk, reset, MCP3002_reg)
variable temp: integer range 0 to 13:= 0;
begin
	if reset = '0' then
		MCP3002_reg <= M_start;
		MCP3002_next <= M_start;
		time_delay_reg2 <= (others=>'0');
		time_delay_next2 <= (others=>'0');
		results <= (others=>'0');
		shift <= (others=>'0');
		pre_shift <= (others=>'0');
		raw_register <= (others=>'0');
		done_flag <= '0';
		MCP_CLK <= '0';
		MCP_CS <= '1';
		MCP_DOUT <= '0';
		skip <= '0';
		skip2 <= '0';
--		test_bit <= '0';
--		test_bits1 <= "001101110101";

	else if clk'event and clk = '1' then
		case MCP3002_reg is
			when M_start =>
				done_flag <= '0';
				results <= (others=>'0');
				--raw_register <= (others=>'0');
				if enable = '1' then
					start_shift(2) <= diff;
					start_shift(1) <= channel;
					start_shift(0) <= msbf;
					MCP_CS <= '0';
					MCP_CLK <= '0';
					shift <= (others=>'0');
					pre_shift <= (others=>'0');
					raw_register <= (others=>'0');
					MCP3002_next <= M_preshift1;
				end if;

			when M_preshift1 =>
				if time_delay_reg2 > MCP_TIME_DELAY then
					time_delay_next2 <= (others=>'0');
					MCP_CLK <= '0';
					MCP3002_next <= M_preshift2;
				else
					time_delay_next2 <= time_delay_reg2 + 1;
				end if;

			when M_preshift2 =>
				if time_delay_reg2 > MCP_TIME_DELAY then
					time_delay_next2 <= (others=>'0');
					if skip = '0' then
						temp:= conv_integer(pre_shift);
						if temp > 3 then
							temp:= 0;
							pre_shift <= (others=>'0');
							MCP_CLK <= '0';
							MCP3002_next <= M_shift1;
						else 
							MCP_DOUT <= start_shift(3 - temp);
							temp:= temp + 1;
							pre_shift <= conv_std_logic_vector(temp,8);
							MCP3002_next <= M_preshift3;
						end if;
					end if;
					skip <= not skip;
				else
					time_delay_next2 <= time_delay_reg2 + 1;
				end if;

			when M_preshift3 =>
				if time_delay_reg2 > MCP_TIME_DELAY then
					time_delay_next2 <= (others=>'0');
					MCP_CLK <= '1';
					MCP3002_next <= M_preshift1;
				else
					time_delay_next2 <= time_delay_reg2 + 1;
				end if;

			when M_shift1 =>
				if time_delay_reg2 > MCP_TIME_DELAY/32 then
					time_delay_next2 <= (others=>'0');
					MCP3002_next <= M_results0;
					MCP_DOUT <= '0';
					skip <= '0';
				else
					time_delay_next2 <= time_delay_reg2 + 1;
				end if;

			when M_results0 =>
				if time_delay_reg2 > MCP_TIME_DELAY then
					time_delay_next2 <= (others=>'0');
					MCP_CLK <= '0';
					MCP3002_next <= M_results1;
				else
					time_delay_next2 <= time_delay_reg2 + 1;
				end if;

			when M_results1 =>
				if time_delay_reg2 > MCP_TIME_DELAY then
					time_delay_next2 <= (others=>'0');
					if skip2 = '0' then
						temp:= conv_integer(shift);
--						results <= test_bit & results(10 downto 1);	-- MSB first
						if msbf = '1' then
							results <= results(8 downto 0) & MCP_DIN;	-- LSB first
						else
							results <= MCP_DIN & results(9 downto 1);	-- MSB first
						end if;
						if temp > 10 then
							done_flag <= '1';
							raw_register <= results;
							MCP_CS <= '1';
							MCP3002_next <= M_start;
						else 
							MCP3002_next <= M_results2;
						end if;	
						temp:= temp + 1;
						if temp > 11 then
							temp:= 0;
						end if;
						shift <= conv_std_logic_vector(temp,8);
					end if;
					skip2 <= not skip2;
				else
					time_delay_next2 <= time_delay_reg2 + 1;
				end if;
			
			when M_results2 =>
				if time_delay_reg2 > MCP_TIME_DELAY then
					time_delay_next2 <= (others=>'0');
					MCP_CLK <= '1';
					MCP3002_next <= M_results0;
				else
					time_delay_next2 <= time_delay_reg2 + 1;
				end if;

		end case;
		MCP3002_reg <= MCP3002_next;
		time_delay_reg2 <= time_delay_next2;
		end if;
	end if;
end process;

end truck_arch;
