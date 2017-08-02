-- sensor.vhd
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_signed.all;

library XESS;
use XESS.CommonPckg.all;

entity sensor is
	generic(CLK_COUNT: integer:=50000; DVND_FACTOR: integer:= 5000);
	port(
		clk, reset: in std_logic;
		sensor: in std_logic;
		result: out std_logic_vector(12 downto 0);
		factor: in std_logic_vector(22 downto 0);
		LED0m: out std_logic_vector(3 downto 0);
		LED1m: out std_logic_vector(3 downto 0);
		LED2m: out std_logic_vector(3 downto 0);
		LED3m: out std_logic_vector(3 downto 0);
		sensor_done: out std_logic);
end sensor;

architecture arch of sensor is
	type state_type is (idle, count, frq, calc1, calc2, b2b);
	signal fstate_reg, fstate_next: state_type;

--	type calc_type is (idle1, calc2, done);
--	signal cstate_reg, cstate_next: calc_type;

	constant RESULT_SIZE: integer:= 18;
	constant PRD_SIZE: integer:= 14;
	constant DVSR_SIZE: integer:= 23;
	constant MPH_P_REG_SIZE: integer:= 16;
	signal prd: std_logic_vector(PRD_SIZE-1 downto 0);
	signal dvsr, dvnd, quo: std_logic_vector(DVSR_SIZE-1 downto 0);
	signal prd_start, div_start, b2b_start: std_logic;
	signal prd_done_tick, div_done_tick,b2b_done_tick, bcd_ready: std_logic;
--	signal calc_done: std_logic;
--	signal sensor_done: std_logic;
--	type result_array is array(0 to 9) of std_logic_vector(12 downto 0);
--	signal calc_count: unsigned(4 downto 0);
--	signal calc_count: std_logic_vector(4 downto 0);
	signal data_array4: my_array3:= (others=>(others=>'0'));
	signal result2: std_logic_vector(16 downto 0);
begin
--===============================================
-- component instantiation
--===============================================

   -- instantiate period counter for mph
	prd_count_unit: entity work.period_counter
		generic map(CLK_COUNT=>CLK_COUNT,P_REG_SIZE=>16,T_REG_SIZE=>PRD_SIZE)
		port map(clk=>clk, reset=>reset,
		start=>prd_start, 
		si=>sensor,
--		ready=>open, 
		done_tick=>prd_done_tick, 
		prd=>prd);

	-- instantiate division circuit for mph
	div_unit: entity work.div
		generic map(W=>DVSR_SIZE, CBIT=>5)
		port map(clk=>clk, reset=>reset, 
		start=>div_start,
		dvsr=>dvsr,
		dvnd=>dvnd,
		quo=>quo,
		rmd=>open,
--		ready=>open,
		done_tick=>div_done_tick);

-- instantiate binary-to-BCD convertor
	bin2bcd_unit: entity work.bin2bcd
		generic map(Q=>13)
		port map(clk=>clk, reset=>reset,
		start=>b2b_start,
--		bin=>quo(12 downto 0),
		bin=>result2(12 downto 0),
		ready=>bcd_ready,
		done_tick=>b2b_done_tick,
		bcd3=>LED0m,
		bcd2=>LED1m,
		bcd1=>LED2m,
		bcd0=>LED3m);

--	dvnd <= std_logic_vector(to_unsigned(DVND_FACTOR, DVSR_SIZE));
	dvnd <= factor;
	dvsr <= "000000000" & prd;

   --===============================================
   -- Master FSM for mph
   --===============================================

sensor_proc: process(clk,reset,fstate_reg)
--variable count: integer range 0 to 5:= 0;
begin
	if reset = '0' then
		fstate_next <= idle;
		fstate_reg <= idle;
		prd_start <='0';
		div_start <='0';
		b2b_start <='0';
		result <= (others=>'0');
		sensor_done <= '0';
--		data_array3 <= (others=>(others=>'0'));
	elsif clk'event and clk = '1' then	
		case fstate_reg is
			when idle =>
				sensor_done <= '0';
				fstate_next <= count;
				prd_start <='1';
			when count =>
				prd_start <= '0';
				if (prd_done_tick='1') then
					div_start <='1';
					fstate_next <= frq;
				end if;
			when frq =>
				div_start <= '0';
				if (div_done_tick='1') then
					fstate_next <= calc1;
				end if;
			when calc1 =>
				b2b_start <='1';
				result2 <= quo(16 downto 0);
				data_array4 <= shift_avg(data_array4,result2);
				fstate_next <= calc2;
			when calc2 =>
				result2 <= average(data_array4,result2);
--				quo <= "000000" & result2;
				result <= result2(12 downto 0);
				fstate_next <= b2b;
			when b2b =>
				b2b_start <= '0';
				if (b2b_done_tick='1') then
					sensor_done <= '1';
					fstate_next <= idle;
				end if;
		end case;
		fstate_reg <= fstate_next;
	end if;	
end process;

end arch;

--calc_proc: process(clk,reset)
--variable count: integer range 0 to 15:= 0;
--begin
--	if reset = '0' then
--		cstate_reg <= idle1;
--		calc_done <= '0';
--		calc_count <= (others=>'0');
--		data_array3 <= (others=>(others=>'0'));
--	elsif clk'event and clk = '1' then
--		case cstate_reg is
--			when idle1 =>
--				count := TO_INTEGER(unsigned(calc_count));
--				if count > 15 then
--					count := 0;
--					cstate_next <= calc2;
--				else
----					res_array(count) <= result;
--					count := count + 1;
--					calc_count <= conv_std_logic_vector(count,4);
--					cstate_next <= idle1;
--				end if;
--				calc_done <= '0';
--			when calc2 =>
--				cstate_next <= done;			
--			when done =>
--				calc_done <= '1';
--				cstate_next <= idle1;			
--		end case;
--		cstate_reg <= cstate_next;
--	end if;
--end process;		

