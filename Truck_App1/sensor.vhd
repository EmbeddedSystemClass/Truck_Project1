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
		sensor1: in std_logic;
		result: out std_logic_vector(15 downto 0);
		factor: in std_logic_vector(22 downto 0);
		sensor_done: out std_logic);
end sensor;

architecture arch of sensor is
	type state_type is (idle, count, frq, calc1, calc2, calc3, calc4, calc5, calc6, b2b);
	signal fstate_reg, fstate_next: state_type;

	type s_state_type is (s_idle, s_delay);
	signal s_state_reg, s_state_next: s_state_type;

	signal s_time_reg, s_time_next: unsigned(24 downto 0);

	signal prd: std_logic_vector(PRD_SIZE-1 downto 0);
	signal dvsr, dvnd, quo: std_logic_vector(DVSR_SIZE-1 downto 0);
	signal prd_start, div_start, b2b_start: std_logic;
	signal prd_done_tick, div_done_tick,b2b_done_tick, bcd_ready: std_logic;
--	signal calc_done: std_logic;
--	signal sensor_done: std_logic;
--	type result_array is array(0 to 9) of std_logic_vector(12 downto 0);
--	signal calc_count: unsigned(4 downto 0);
--	signal calc_count: std_logic_vector(4 downto 0);
--	signal data_array4: my_array3:= (others=>(others=>'0'));
--	signal result2: std_logic_vector(16 downto 0);
--	signal double_result: std_logic_vector(16 downto 0);
--	signal data_array5: my_array3;
-- INIT_PRD is used to preset the data_array4 for testing in simulator
	constant INIT_PRD : std_logic_vector :=  "0" & X"1388";	-- 5000rpm
--	constant INIT_PRD : std_logic_vector :=  "0" & X"13FF";	-- 5119rpm
--	constant INIT_PRD : std_logic_vector :=  "0" & X"09C4";	-- 2500rpm
--	constant INIT_PRD : std_logic_vector :=  "0" & X"000f";	-- 15mph
--	constant INIT_PRD : std_logic_vector :=  "0" & X"004b";	-- 75mph
	signal first_flag: std_logic;
	signal mrmd: std_logic_vector(22 downto 0);
	signal prd_reset: std_logic;
	signal time_up: std_logic;
	
begin
--===============================================
-- component instantiation
--===============================================

   -- instantiate period counter for mph
	prd_count_unit: entity work.period_counter
		generic map(CLK_COUNT=>CLK_COUNT,P_REG_SIZE=>16,T_REG_SIZE=>PRD_SIZE)
		port map(clk=>clk, reset=>reset,
		start=>prd_start, 
		si=>sensor1,
--		ready=>open, 
		done_tick=>prd_done_tick,
		reset_prd=>prd_reset,
		prd=>prd);

	-- instantiate division circuit for mph
	div_unit: entity work.div
		generic map(W=>DVSR_SIZE, CBIT=>5)
		port map(clk=>clk, reset=>reset, 
		start=>div_start,
		dvsr=>dvsr,		-- divisor divides into the dividend
		dvnd=>dvnd,
		quo=>quo,		-- quotient is the result of division
--		rmd=>open,
		rmd=>mrmd,		-- remainder is what's left over
--		ready=>open,
		done_tick=>div_done_tick);

-- instantiate binary-to-BCD convertor
	-- bin2bcd_unit: entity work.bin2bcd
		-- generic map(Q=>13)
		-- port map(clk=>clk, reset=>reset,
		-- start=>b2b_start,
-- --		bin=>quo(12 downto 0),
		-- bin=>result2(12 downto 0),
		-- ready=>bcd_ready,
		-- done_tick=>b2b_done_tick,
		-- bcd3=>LED0m,
		-- bcd2=>LED1m,
		-- bcd1=>LED2m,
		-- bcd0=>LED3m);

--	dvnd <= std_logic_vector(to_unsigned(DVND_FACTOR, DVSR_SIZE));
--  quo + rem = dvnd/dvsr
	dvnd <= factor;
	dvsr <= "000000" & prd;

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
--		b2b_start <='0';
		result <= (others=>'0');
--		result2 <= (others=>'0');
--		double_result <= (others=>'0');
		sensor_done <= '0';
--		data_array5 <= (others=>(others=>'0'));
		-- use these for testing in ISim so that
		-- the average starts out the same
		-- data_array4(0) <= INIT_PRD-7;
		-- data_array4(1) <= INIT_PRD-6;
		-- data_array4(2) <= INIT_PRD-5;
		-- data_array4(3) <= INIT_PRD-4;
		-- data_array4(4) <= INIT_PRD-3;
		-- data_array4(5) <= INIT_PRD-2;
		-- data_array4(6) <= INIT_PRD-1;
		-- data_array4(7) <= INIT_PRD;
		-- data_array4(8) <= INIT_PRD+1;
		-- data_array4(9) <= INIT_PRD+2;
		-- data_array4(10) <= INIT_PRD+3;
		-- data_array4(11) <= INIT_PRD+4;
		-- data_array4(12) <= INIT_PRD+5;
		-- data_array4(13) <= INIT_PRD+6;
		-- data_array4(14) <= INIT_PRD+7;
		-- data_array4(15) <= INIT_PRD+8;
		first_flag <= '0';

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
				if time_up = '1' then
					result <= (others=>'0');
--					result2 <= (others=>'0');
					fstate_next <= calc6;
				end if;
			when frq =>
				div_start <= '0';
				if div_done_tick='1' then
					if first_flag = '1' then
						fstate_next <= calc1;
					else 
						first_flag <= '1';
						fstate_next <= idle;
						result <= quo(15 downto 0);
					end if;

					fstate_next <= calc1;
				end if;
			when calc1 =>
				result <= quo(15 downto 0);
--				double_result <= quo(16 downto 0);
--				data_array5 <= data_array4;
--				fstate_next <= calc2;
				fstate_next <= idle;	-- go back to idle because we are not messing with
										-- averaging algo
			when calc2 =>
--				result2 <= double_result(15 downto 0) & '1';
--				result2 <= double_result;
				fstate_next <= calc3;
			when calc3 =>
--				result <= result2;
--				data_array4 <= shift_avg(data_array5,result2);
				fstate_next <= calc4;
			when calc4 =>
--				double_result <= average(data_array4);
				fstate_next <= calc5;
			when calc5 =>
--				result2 <= double_result;
				fstate_next <= calc6;
			when calc6 =>
--				result <= result2;
--				b2b_start <='1';
--				fstate_next <= b2b;
				fstate_next <= idle;
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

-- this is disabled
check_sensor_proc: process(clk, reset, s_state_reg, prd_done_tick)
begin
	if reset = '0' then
		s_time_reg <= (others=>'0');
		s_time_next <= (others=>'0');
		s_state_reg <= s_idle;
		s_state_next <= s_idle;
		time_up <= '0';
		prd_reset <= '0';

	elsif clk'event and clk = '1' then
		case s_state_reg is
			when s_idle =>
				time_up <= '0';
				prd_reset <= '0';
				s_time_reg <= (others=>'0');
				s_time_next <= (others=>'0');
--				s_state_next <= s_delay;
				s_state_next <= s_idle;
			when s_delay =>
				if s_time_reg > TIME_DELAY_500RPM then
					time_up <= '1';
					prd_reset <= '1';
		 			s_state_next <= s_idle;
				else
					s_time_next <= s_time_reg + 1;
					if prd_done_tick = '1' then
						s_state_next <= s_idle;
					end if;
				end if;
		end case;

		s_time_reg <= s_time_next;
		s_state_reg <= s_state_next;
	end if;
end process;

end arch;