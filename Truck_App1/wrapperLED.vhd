-- wrapperLED.vhd - a wrapper for the rpm & mph signals to the LED's
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_signed.all;

library XESS;
use XESS.CommonPckg.all;

entity wrapperLED is
-- comment out 1st when testing mph, 2nd when testing rpm, otherwise these are supseded by the caller (Truck_App1.vhd)
--	generic (TIME_DELAY: integer:= 1000; DVSR_M: integer:= 326; CLK_COUNT: integer:= MPH_CLOCK_COUNT;DVND:integer:= MPH_DVND);
	generic (TIME_DELAY: integer:= 1000; DVSR_M: integer:= 326; CLK_COUNT: integer:= RPM_CLOCK_COUNT;DVND:integer:= RPM_DVND);
--	generic (TIME_DELAY: integer:= 1000; DVSR_M: integer:= 326; CLK_COUNT: integer:= 5000;DVND:integer:= 12852);
	port(
		clk, reset: in std_logic;
		start: in std_logic;
		tx: out std_logic;
		sensor: in std_logic;
		cmd: in std_logic_vector(7 downto 0);
		param: in std_logic_vector(7 downto 0);
		factor: in std_logic_vector(5 downto 0);
		display_update_rate: in std_logic_vector(25 downto 0);
--		bcd0_o, bcd1_o, bcd2_o, bcd3_o: out std_logic_vector(3 downto 0);
		bcd_o: out std_logic_vector(15 downto 0);
		result: out std_logic_vector(16 downto 0);
		sensor_done1: out std_logic
		);
end wrapperLED;

architecture arch of wrapperLED is

	type u_state_type is (u_idle, u_start1, u_start2, u_start3, u_start4, u_long_delay, u_done);
	signal wrapper_state_reg, wrapper_state_next: u_state_type;
	signal u_time_reg, u_time_next: unsigned(25 downto 0);

	signal display_done1: std_logic;
	signal bcd0, bcd1, bcd2, bcd3: std_logic_vector(3 downto 0);
	signal update_rate: unsigned(25 downto 0);
	signal real_factor: std_logic_vector(22 downto 0);
--	constant TIME_DELAY_UPDATE: integer:= 8388000;	-- about 168ms
	signal last_factor: std_logic_vector(5 downto 0);
	signal done_sensor: std_logic;
--	signal sub_cmd: std_logic_vector(2 downto 0);
--	signal sub_param: std_logic_vector(7 downto 0);
	signal start_sendchar: std_logic;
	signal sub_cmd: std_logic_vector(7 downto 0);
	signal sub_param: std_logic_vector(7 downto 0);
	signal display_update_proc_done: std_logic;
begin

real_fact_proc: process(clk, reset, factor)
variable temp_fact: integer range -31 to 31;
variable temp_fact2: integer range 0 to 6000000;
begin
	if reset = '0' then
		real_factor <= conv_std_logic_vector(DVND,23);
		last_factor <= (others=>'0');
	elsif clk'event and clk = '1' then
		if last_factor /= factor then
			temp_fact:= conv_integer(factor);
			temp_fact2:= conv_integer(real_factor);
			temp_fact2:= temp_fact + temp_fact2;
			real_factor <= conv_std_logic_vector(temp_fact2,23);
			last_factor <= factor;
		end if;	
	end if;
end process;
		
sensor_unit: entity work.sensor(arch)
	generic map(CLK_COUNT=>CLK_COUNT,DVND_FACTOR=>DVND)
	port map(clk=>clk, reset=>reset,
		sensor1=>sensor,
		result=>result,
		factor=>real_factor,
		LED0m=>bcd0,
		LED1m=>bcd1,
		LED2m=>bcd2,
		LED3m=>bcd3,
		sensor_done=>done_sensor);

sendcharLED_unit: entity work.sendcharLED(arch)
	generic map(TIME_DELAY=>TIME_DELAY,DVSR_M=>DVSR_M)
	port map(clk=>clk, reset=>reset,
		tx=>tx,
		start=>start_sendchar,
		cmd=>sub_cmd,
		param=>sub_param,
		LED0=>bcd0,
		LED1=>bcd1,
		LED2=>bcd2,
		LED3=>bcd3,
		display_done=>display_done1);

display_update_proc: process(clk, reset, wrapper_state_reg, display_update_rate)
begin
	if reset = '0' then
		wrapper_state_reg <= u_idle;	
		wrapper_state_next <= u_idle;
		u_time_reg <= (others=>'0');
		u_time_next <= (others=>'0');
		sub_cmd <= (others=>'0');
		sub_param <= (others=>'0');
--		sub_param <= (others=>'0');
--		sendchar_busy <= '0';		-- flag to tell the other commands to wait
--		sendchar_busy <= '1';		-- flag to tell the other commands to wait
		bcd_o <= (others=>'0');
		update_rate <= (others=>'1');
		sensor_done1 <= '0';
		start_sendchar <= '0';
		display_update_proc_done <= '0';
		
	elsif clk'event and clk= '1' then
--		sub_cmd <= cmd;
		case wrapper_state_reg is
			when u_idle =>
				if start = '1' then
					sensor_done1 <= '0';
					update_rate <= unsigned(display_update_rate);
					wrapper_state_next <= u_start1;
					display_update_proc_done <= '0';
				end if;	
			when u_start1 =>
				sub_cmd <= cmd;
				sub_param <= param;
				start_sendchar <= '1';
				wrapper_state_next <= u_start2;
			when u_start2 =>
				start_sendchar <= '0';
				wrapper_state_next <= u_long_delay;
			when u_start3 =>
				display_update_proc_done <= '0';
				start_sendchar <= '1';
				wrapper_state_next <= u_start4;
			when u_start4 =>
				start_sendchar <= '0';
				wrapper_state_next <= u_long_delay;
			when u_long_delay =>
				if u_time_reg > update_rate - 1000 then
					u_time_next <= (others=>'0');
					wrapper_state_next <= u_done;
					update_rate <= unsigned(display_update_rate);
				else
					u_time_next <= u_time_reg + 1;
					if start = '0' then
						u_time_next <= (others=>'0');
						wrapper_state_next <= u_done;
					end if;
				end if;
			when u_done =>
				if display_done1 = '1' then
					bcd_o <= bcd0 & bcd1 & bcd2 & bcd3;
					sensor_done1 <= '1';
					wrapper_state_next <= u_idle;
					display_update_proc_done <= '1';
				end if;
		end case;
		wrapper_state_reg <= wrapper_state_next;
		u_time_reg <= u_time_next;
	end if;
end process;

end arch;