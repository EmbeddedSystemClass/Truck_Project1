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

entity multi_byte is
	port(
		clk, reset: in std_logic;
		tx_uart: out std_logic;
		rx_uart: in std_logic;
		tx_rpm: out std_logic;
		tx_mph: out std_logic;
		rpm_signal : in std_logic;
		mph_signal : in std_logic;
		pwm_spk1 : out std_logic;
		pwm_spk2 : out std_logic;
		pwm_lcd : out std_logic;
		fp_shutoff : out std_logic;
		rev_limit: out std_logic;
		-- MOSI_o : out std_logic;
		-- MISO_i : in std_logic;
		-- SCLK_o : out std_logic;
		-- SS_o : out std_logic_vector(1 downto 0);
		-- MOSI_o : in std_logic;
		-- MISO_i : out std_logic;
		-- SCLK_o : in std_logic;
		-- SS_o : in std_logic;
		data_ready : in std_logic;
		cmd_param : in std_logic;
		data_sent : out std_logic;
		led1: out std_logic_vector(3 downto 0)
		);
end multi_byte;

architecture truck_arch of multi_byte is

	-- send_uart1
	type state_uart1 is (idle10, idle1, start, start_a, start_b, start_c, start_d, delay);
	signal state_tx1_reg, state_tx1_next: state_uart1;

	-- recv_uart
	type state_uart2 is (pre_idle, idle2, next1, start1, done);
	signal state_uart_reg2, state_uart_next2: state_uart2;

	type state_fp is (idle_fp, start_fp, done_fp);
	signal state_reg_fp, state_next_fp: state_fp;

	type state_rev is (idle_rev, start_rev, done_rev);
	signal state_reg_rev, state_next_rev: state_rev;

	type calc_type2 is (idle2a, spin2);
	signal main_reg2, main_next2: calc_type2;

	type calc_type1 is (idle1a, spin1);
	signal main_reg1, main_next1: calc_type1;

	type state_pwm is (pwm_idle, pwm_start, pwm_next1, pwm_next2, pwm_next3, pwm_done);
	signal drive_pwm_reg, drive_pwm_next: state_pwm;
	
	signal time_delay_reg, time_delay_next: unsigned(24 downto 0);		-- send_uart1
	signal time_delay_reg2, time_delay_next2: unsigned(17 downto 0);	-- calc_proc2

	signal rpm_db, mph_db: std_logic;
	signal cmd_rpm: std_logic_vector(7 downto 0);
	signal cmd_mph: std_logic_vector(7 downto 0);
	signal param_mph: std_logic_vector(7 downto 0);
	signal param_rpm: std_logic_vector(7 downto 0);
	signal rpm_result: std_logic_vector(16 downto 0);
	signal urpm_result: unsigned(16 downto 0);
	signal mph_result: std_logic_vector(16 downto 0);
	signal mph_factor: std_logic_vector(5 downto 0);
	signal rpm_factor: std_logic_vector(5 downto 0);
	signal time_delay_mph: std_logic_vector(25 downto 0);
	signal time_delay_rpm: std_logic_vector(25 downto 0);
	signal stdlv_transmit_update_rate: std_logic_vector(23 downto 0);
	signal bcd_mph, bcd_rpm, bcdc: std_logic_vector(15 downto 0);
	signal start_tx, done_tx, start_rx, done_rx: std_logic;
	signal data_tx, data_rx: std_logic_vector(7 downto 0);

	signal mph_done, rpm_done: std_logic;

	-- signal stlv_temp1 : std_logic_vector(7 downto 0);
	-- signal stlv_temp1a : std_logic_vector(7 downto 0);
	-- signal stlv_temp2 : std_logic_vector(7 downto 0);
	-- signal stlv_temp2a : std_logic_vector(7 downto 0);
	-- signal stlv_temp3 : std_logic_vector(7 downto 0);
	-- signal stlv_temp4 : std_logic_vector(7 downto 0);
	-- signal stlv_temp5 : std_logic_vector(7 downto 0);
	-- signal stlv_temp6 : std_logic_vector(7 downto 0);
	-- signal stlv_temp7 : std_logic_vector(7 downto 0);
	signal start_mph_wrapper: std_logic;
	signal start_rpm_wrapper: std_logic;
	signal mcmd: std_logic_vector(7 downto 0);
	signal mparam: std_logic_vector(7 downto 0);
	signal mph_or_not: std_logic;
	signal low_byte, high_byte: std_logic_vector(7 downto 0);
	signal start_dtmf: std_logic;
	signal dtmf_done1: std_logic;
	signal stlv_duty_cycle: std_logic_vector(4 downto 0);
	signal dtmf_index: std_logic_vector(4 downto 0);

	signal special: std_logic;
	signal done_pwm: std_logic;
	signal fp_override: std_logic;
	signal rev_limit_max: std_logic_vector(7 downto 0);
	signal rev_limit_min: std_logic_vector(7 downto 0);
	signal urev_limit_max: unsigned(16 downto 0);
	signal urev_limit_min: unsigned(16 downto 0);
	signal stdlv_temp1, stdlv_temp2: std_logic_vector(16 downto 0);
	signal reset_rev_limits: std_logic;
	signal test_rpm_rev_limits: std_logic;
	signal start_calc: std_logic;
	signal calc_done: std_logic;
	signal shutdown_rpm, shutdown_mph: std_logic;
	
begin

--rx_temp <= tx_temp;
mph_wrapper_unit: entity work.wrapperLED(arch)
	generic map(TIME_DELAY=>TIME_DELAY7,DVSR_M=>DVSR_MU_9600,CLK_COUNT=>MPH_CLOCK_COUNT,DVND=>MPH_DVND)
	port map(clk=>clk, reset=>reset,
	start=>start_mph_wrapper,
	tx=>tx_mph,
	sensor=>mph_db,
	cmd=>cmd_mph,
	param=>param_mph,
	factor=>mph_factor,
	display_update_rate=>time_delay_mph,
	bcd_o=>bcd_mph,
	result=>mph_result,
	shutdown_fp=>shutdown_mph,
	sensor_done1=>mph_done);

rpm_wrapper_unit: entity work.wrapperLED(arch)
	generic map(TIME_DELAY=>TIME_DELAY7,DVSR_M=>DVSR_MU_9600,CLK_COUNT=>RPM_CLOCK_COUNT,DVND=>RPM_DVND)
	port map(clk=>clk, reset=>reset,
	start=>start_rpm_wrapper,
	tx=>tx_rpm,
	sensor=>rpm_db,
	cmd=>cmd_rpm,
	param=>param_rpm,
	factor=>rpm_factor,
	display_update_rate=>time_delay_rpm,
	bcd_o=>bcd_rpm,
	result=>rpm_result,
	shutdown_fp=>shutdown_rpm,
	sensor_done1=>rpm_done);

tx_uart_wrapper_unit: entity work.uartLED(str_arch)
	generic map(DVSR_M=>DVSR_MU_19200)
	port map(clk=>clk, reset=>reset,
	tx_start=>start_tx,
	w_data=>data_tx,
	done_tick=>done_tx,
	tx=>tx_uart);

rx_uart_wrapper_unit: entity work.uartLED2(str_arch)
	generic map(DVSR_M=>DVSR_MU_19200)
	port map(clk=>clk, reset=>reset,
	rx_start=>start_rx,
	r_data=>data_rx,
	done_tick=>done_rx,
	rx=>rx_uart);

db0_unit: entity work.db_fsm
	port map(clk=>clk,reset=>reset,
		sw=>rpm_signal,
		db=>rpm_db);

db1_unit: entity work.db_fsm
	port map(clk=>clk,reset=>reset,
		sw=>mph_signal,
		db=>mph_db);

pwm_unit: entity work.lcd_pwm
	generic map(W2=>W2_SIZE)
	port map(clk=>clk,reset=>reset,
		pwm_signal=>pwm_lcd,
		duty_cycle=>stlv_duty_cycle,
		done=>done_pwm);
	
dtmf_unit: entity work.dtmf
	port map(clk=>clk,reset=>reset,
		dtmf_index=>dtmf_index,
		special_cmd=>special,
		dtmf_signal_1=>pwm_spk1,
		dtmf_signal_2=>pwm_spk2,
		start=>start_dtmf,
		done=>dtmf_done1);
		
-- ********************************************************************************
mon_fp: process(clk, reset, state_reg_fp)
begin
	if reset = '0' then
		state_reg_fp <= idle_fp;
		state_next_fp <= idle_fp;
		urpm_result <= (others=>'0');
		fp_shutoff <= '1';	-- start out with relay closed (fp can be on if other relay closed)

	else if clk'event and clk = '1' then
		case state_reg_fp is
			when idle_fp =>
				if test_rpm_rev_limits = '1' then
					urpm_result <= unsigned(rpm_result);
				else
					urpm_result <= unsigned(rpm_result);
				end if;
				state_next_fp <= start_fp;
			when start_fp =>
				if fp_override = '1' or urpm_result > 700 then
					fp_shutoff <= '1';		-- close relay to fuel pump
					state_next_fp <= idle_fp;
				else
					state_next_fp <= done_fp;
				end if;	
			when done_fp =>
				-- if urpm_result < FP_RPM_MINIMUM then
					-- fp_shutoff <= '0';
				-- else fp_shutoff <= '1';
				-- end if;
				if shutdown_rpm = '1' then
					fp_shutoff <= '0';
				end if;
				state_next_fp <= idle_fp;
		end case;
		state_reg_fp <= state_next_fp;
		end if;
	end if;
end process;

-- ********************************************************************************
-- 1) check if rpm > MAX -> turn off
-- 2) wait for rpm to be < MIN -> turn on
mon_rev: process(clk, reset, state_reg_rev)
begin
	if reset = '0' then
		state_reg_rev <= idle_rev;
		state_next_rev <= idle_rev;
		rev_limit <= '1';		-- start out with relay closed so ignition can be on if other relay closed

	else if clk'event and clk = '1' then
		case state_reg_rev is
			when idle_rev =>
				stdlv_temp1 <= "00000" & rev_limit_max & "0000";
				stdlv_temp2 <= "00000" & rev_limit_min & "0000";
				urev_limit_max <= unsigned(stdlv_temp1);
				urev_limit_min <= unsigned(stdlv_temp2);
				state_next_rev <= start_rev;
			when start_rev =>
				if reset_rev_limits = '1' then
					state_next_rev <= idle_rev;
				else
					if urpm_result > urev_limit_max then
						rev_limit <= '0';
						state_next_rev <= done_rev;
					end if;
				end if;
			when done_rev =>
				if reset_rev_limits = '1' then
					state_next_rev <= idle_rev;
				elsif urpm_result < urev_limit_min then
					rev_limit <= '1';
					state_next_rev <= start_rev;
				end if;
		end case;
		state_reg_rev <= state_next_rev;
		end if;
	end if;
end process;

-- ********************************************************************************
-- send rpm & mph results rpm first, high_byte first
-- could also send it in bcd form
send_uart1: process(clk, reset, state_tx1_reg)
variable temp_uart: integer range 0 to 255:= 33;
begin
	if reset = '0' then
		state_tx1_reg <= idle10;
		data_tx <= (others=>'0');
		start_tx <= '0';
--		skip <= '0';
		time_delay_reg <= (others=>'0');
		time_delay_next <= (others=>'0');
		low_byte <= X"00";
		high_byte <= X"00";
		bcdc <= X"0201";

	else if clk'event and clk = '1' then
		case state_tx1_reg is
			when idle10 =>
				start_tx <= '0';
				-- never goes faster than this delay
 				if time_delay_reg > TIME_DELAY9 then
					time_delay_next <= (others=>'0');
					state_tx1_next <= idle1;
				else
					time_delay_next <= time_delay_reg + 1;
				end if;	
			when idle1 =>
				low_byte <= rpm_result(7 downto 0);
				high_byte <= rpm_result(15 downto 8);
				data_tx <= X"FF";
				start_tx <= '1';
				state_tx1_next <= start;
			when start =>
				start_tx <= '0';
				if done_tx = '1' then
					if low_byte = X"FF" then
						low_byte <= X"FE";
					end if;
--						data_tx <= bcd_rpm(15 downto 8);
					data_tx <= high_byte;
					start_tx <= '1';
					state_tx1_next <= start_a;
				end if;
			when start_a =>
				start_tx <= '0';
				if done_tx = '1' then
--					data_tx <= bcd_rpm(7 downto 0);
					data_tx <= low_byte;
					start_tx <= '1';
					low_byte <= mph_result(7 downto 0);
					high_byte <= mph_result(15 downto 8);
					state_tx1_next <= start_b;
				end if;
			when start_b =>
				start_tx <= '0';
				if done_tx = '1' then
					data_tx <= high_byte;
					start_tx <= '1';
					state_tx1_next <= start_c;
				end if;
			when start_c =>
				start_tx <= '0';
				if done_tx = '1' then
					if low_byte = X"FF" then
						low_byte <= X"FE";
					end if;
					data_tx <= low_byte;
					start_tx <= '1';
					state_tx1_next <= start_d;
				end if;
			when start_d =>	
				start_tx <= '0';
				if done_tx = '1' then
--					data_tx <= stlv_temp5;
					start_tx <= '1';
					state_tx1_next <= delay;
--					state_tx1_next <= idle10;
				end if;
			when delay =>
				start_tx <= '0';
				if time_delay_reg > unsigned(stdlv_transmit_update_rate) - 1 then
-- 				if time_delay_reg > TIME_DELAY5a then
					time_delay_next <= (others=>'0');
					state_tx1_next <= idle1;
				else
					time_delay_next <= time_delay_reg + 1;
				end if;	
		end case;
		time_delay_reg <= time_delay_next;
		state_tx1_reg <= state_tx1_next;
		end if;
	end if;
end process;

-- ********************************************************************************
recv_uart: process(clk, reset, state_uart_reg2)
begin
	if reset = '0' then
		state_uart_reg2 <= pre_idle;
		state_uart_next2 <= pre_idle;
		start_rx <= '0';
		mparam <= (others=>'0');
		mcmd <= (others=>'0');
--		led1 <= "1111";
		start_calc <= '0';
		data_sent <= '0';

	else if clk'event and clk = '1' then
		case state_uart_reg2 is
			when pre_idle =>
				mcmd <= LCD_PWM;
				mparam <= PWM_30DC_PARAM;
				state_uart_next2 <= idle2;
			
			when idle2 =>
				start_calc <= '0';
				if data_ready = '1' then
					start_rx <= '1';
					state_uart_next2 <= next1;
				end if;

			when next1 =>
				if done_rx = '1' then
					start_rx <= '0';
--					led1 <= data_rx(3 downto 0);
					if cmd_param = '1' then
						mcmd <= data_rx;
						data_sent <= '0';
					else
						mparam <= data_rx;
						data_sent <= '1';
					end if;	
					state_uart_next2 <= done;
				end if;	
						
			when start1 =>
				state_uart_next2 <= done;

			when done =>
				if cmd_param = '0' then
					start_calc <= '1';
				end if;
				state_uart_next2 <= idle2;
		end case;
		state_uart_reg2 <= state_uart_next2;
		end if;
	end if;
end process;

-- ********************************************************************************
calc_proc1: process(clk,reset,main_reg1)
begin
	if reset = '0' then
		main_reg1 <= idle1a;
		main_next1 <= idle1a;
		stlv_duty_cycle <= (others=>'1');
		special <= '0';
		dtmf_index <= (others=>'0');
		calc_done <= '0';
		stdlv_transmit_update_rate <=  X"1FFFFF";
		
		fp_override <= '0';	-- start off with relay open (need to send command to close it before starting)
--		rev_limit_max <= conv_std_logic_vector(RPM_MAXIMUM,7);	-- start out with defaults
--		rev_limit_min <= conv_std_logic_vector(RPM_MINIMUM,7);
-- 4000 = 0xFA0
-- 3888 = 0xF30
		rev_limit_max <= X"FA";
		rev_limit_min <= X"F3";
		reset_rev_limits <= '0';
--		stlv_temp2 <= (others=>'1');
		test_rpm_rev_limits <= '0';

	elsif clk'event and clk = '1' then
		case main_reg1 is
			when idle1a =>
				reset_rev_limits <= '0';
				if start_calc = '1' then
--					stlv_temp6 <= mcmd;
--					stlv_temp7 <= mparam;
					calc_done <= '0';
					main_next1 <= spin1;
				end if;
			when spin1 =>
				case mcmd is
					when SET_UPDATE_RATE =>
						stdlv_transmit_update_rate <= mparam & X"FFFF";
--						stlv_temp3 <= mparam;
					when DTMF_TONE_ON =>
						dtmf_index <= mparam(4 downto 0);
						special <= '0';
						start_dtmf <= '1';
					when DTMF_TONE_OFF =>
						start_dtmf <= '0';
						special <= '0';
					when SPECIAL_TONE_ON =>
						special <= '1';
						start_dtmf <= '1';
					when LCD_PWM =>
						stlv_duty_cycle <= mparam(4 downto 0);
----						led1 <= mparam(3 downto 0);
					when FP_SHUTOFF_OVERRIDE =>
						fp_override <= mparam(0);
--						stlv_temp2a <= mparam;
					when SET_MAX_REV_LIMITER =>
						rev_limit_max <= mparam;
						reset_rev_limits <= '1';
--						stlv_temp2a <= mparam;
					when SET_MIN_REV_LIMITER =>
						rev_limit_max <= mparam;
						reset_rev_limits <= '1';
--						stlv_temp2a <= mparam;
					when TEST_RPM_LIMIT =>
						test_rpm_rev_limits <= mparam(0);
--						stlv_temp2a <= mparam;
					when others =>	
				end case;
				calc_done <= '1';
				main_next1 <= idle1a;
		end case;
		main_reg1 <= main_next1;
	end if;
end process;		

-- ********************************************************************************
calc_proc2: process(clk,reset,main_reg2)
begin
	if reset = '0' then
		main_reg2 <= idle2a;
		main_next2 <= idle2a;
		time_delay_reg2 <= (others=>'0');
		time_delay_next2 <= (others=>'0');
		cmd_mph <= MPH_SEND_CHAR_CMD;
		cmd_rpm <= RPM_SEND_CHAR_CMD;
		param_mph <= (others=>'1');
		param_rpm <= (others=>'1');
		time_delay_mph <= "00" & X"FFFFFF";
		time_delay_rpm <= "00" & X"7FFFFF";
--		time_delay_rpm <= "00" & X"3FFFFF";		-- 184ms
--		time_delay_rpm <= "00" & X"1FFFFF";		-- 92ms
--		time_delay_rpm <= "00" & X"0FFFFF";		-- 46ms
--		time_delay_rpm <= "00" & X"07FFFF";		-- 23ms
--		time_delay_rpm <= "00" & X"03FFFF";		-- 11ms
		start_rpm_wrapper <= '1';
		start_mph_wrapper <= '1';
		mph_or_not <= '0';
		led1 <= "1111";

	elsif clk'event and clk = '1' then
		case main_reg2 is
			when idle2a =>
				if start_calc = '1' then
--					main_next2 <= spin2;
					main_next2 <= idle2a;
				end if;
			when spin2 =>
				case mcmd is
					when RPM_OFF_CMD =>
						cmd_rpm <= mcmd;
					when RPM_SEND_CHAR_CMD =>
						led1 <= "1110";
						cmd_rpm <= mcmd;
					when RPM_SET_BRIGHTNESS_CMD =>
						cmd_rpm <= mcmd;
						param_rpm <= mparam;
						led1 <= "1101";
					when RPM_SET_CDECIMAL_CMD =>
						cmd_rpm <= mcmd;
						param_rpm <= mparam;
					when RPM_SET_UPDATE_RATE_CMD =>
						cmd_rpm <= mcmd;
						led1 <= "1100";
						time_delay_rpm <= mparam & "11" & X"FFFF";
					when RPM_SET_FACTOR_CMD =>
						cmd_rpm <= mcmd;
						rpm_factor <= mparam(5 downto 0);
						led1 <= "1011";
						when MPH_OFF_CMD =>
						cmd_mph <= mcmd;
						led1 <= "1010";
					when MPH_SEND_CHAR_CMD =>
						cmd_mph <= mcmd;
						led1 <= "1001";
					when MPH_SET_BRIGHTNESS_CMD =>
						cmd_mph <= mcmd;
						param_mph <= mparam;
						led1 <= "1000";
					when MPH_SET_CDECIMAL_CMD =>
						cmd_mph <= mcmd;
						param_mph <= mparam;
						led1 <= "0111";
					when MPH_SET_UPDATE_RATE_CMD =>
						cmd_mph <= mcmd;
						time_delay_mph <= mparam & "11" & X"FFFF";
						led1 <= "0110";
					when MPH_SET_FACTOR_CMD =>
						cmd_mph <= mcmd;
						mph_factor <= mparam(5 downto 0);
						led1 <= "0101";
					when others =>
				end case;
				cmd_mph <= MPH_SEND_CHAR_CMD;
				cmd_rpm <= RPM_SEND_CHAR_CMD;
				main_next2 <= idle2a;
		end case;
		main_reg2 <= main_next2;
		time_delay_reg2 <= time_delay_next2;
	end if;
end process;		

end truck_arch;