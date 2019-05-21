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
		rx_ds1620 : in std_logic;
		ref_signal : out std_logic;
		pwm_signal_1 : out std_logic;
		pwm_signal_2 : out std_logic;
		led1: out std_logic_vector(3 downto 0)
		);
end multi_byte;

architecture truck_arch of multi_byte is

	type state_uart1 is (xmit_idle, xmit_start, xmit_check_FF,
	xmit_check_FF2, xmit_presend, xmit_send, xmit_wait, xmit_inc, xmit_delay);
	signal state_tx1_reg, state_tx1_next: state_uart1;

	-- recv_uart
	type state_uart2 is (pre_idle, idle2, next1, start1, done, rx_delay);
	signal state_uart_reg2, state_uart_next2: state_uart2;

	type state_fp is (idle_fp, start_fp, done_fp);
	signal state_reg_fp, state_next_fp: state_fp;

	type state_rev is (idle_rev, start_rev, done_rev);
	signal state_reg_rev, state_next_rev: state_rev;

	type calc_type2 is (idle2a, wait_for_rpm_done, wait_for_mph_done, delay_rpm, delay_mph, do_rpm_cmd, 
				do_mph_cmd, special_rpm_done, special_mph_done);
	signal main_reg2, main_next2: calc_type2;

	type calc_type1 is (idle1a, do_mcmd, other_cmd);
	signal main_reg1, main_next1: calc_type1;

	type state_ref is (idle_ref, start_ref, done_ref);
	signal state_ref_reg, state_ref_next: state_ref;

	type state_pwm2 is (pwm_idle2, pwm_start2, pwm_next12, pwm_done2);
	signal drive_pwm_reg2, drive_pwm_next2: state_pwm2;

	signal time_delay_reg, time_delay_next: unsigned(24 downto 0);		-- send_uart1
	signal time_delay_reg2, time_delay_next2: unsigned(17 downto 0);	-- calc_proc2
	signal time_delay_reg3, time_delay_next3: unsigned(25 downto 0);	-- ref signal
	signal time_delay_reg4, time_delay_next4: unsigned(19 downto 0);	-- recv uart
	signal time_delay_reg8, time_delay_next8: unsigned(24 downto 0);

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
	signal shutdown_rpm, shutdown_mph: std_logic;
	signal temp_data1, temp_data2, temp_data3, temp_data4: std_logic_vector(15 downto 0);
		constant NO_ELEMS:  integer:=  16;
	type xmit_array_type is array(0 to NO_ELEMS-1) of std_logic_vector(7 downto 0);
	signal xmit_array: xmit_array_type:=(others=>(others=>'0'));
	signal xmit_uindex: unsigned(3 downto 0);
	signal skip: std_logic;
	signal skip2: std_logic;
	signal skip4: std_logic;
	signal xmit_stdlv: std_logic_vector(7 downto 0);

	signal start_pwm: std_logic;
	signal start_pwm2: std_logic;
	signal pwm_done1: std_logic;
	signal key_index: unsigned(7 downto 0);
	signal key_len: signed(7 downto 0);
	signal stlv_duty_cycle2: std_logic_vector(5 downto 0);
	signal tune_ptr: signed(2 downto 0);
	signal inc_params: signed(7 downto 0);	-- used in recv
	signal tinc_params: signed(7 downto 0); -- used in xmit
	signal download: dl_array;
	signal tune1: tune_array;
	signal note_len: std_logic_vector(7 downto 0);
	signal stlv_flag: std_logic_vector(7 downto 0);
	
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

pwm2_unit: entity work.pwm2
	port map(clk=>clk, reset=>reset,
		pwm_signal_1=>pwm_signal_1,
		pwm_signal_2=>pwm_signal_2,
		start=>start_pwm2,
		done=>pwm_done1,
		len=>key_len,
		duty_cycle=>stlv_duty_cycle2,
		tune1=>tune1,
		note_len=>note_len,
		tune_ptr=>tune_ptr);

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

wrapper_DS1620_unit: entity work.poll_DS1620
	port map(clk=>clk, reset=>reset,
		rx_ds1620=>rx_ds1620,
		temp_data1=>temp_data1,
		temp_data2=>temp_data2,
		temp_data3=>temp_data3,
		temp_data4=>temp_data4);
		
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
begin
	if reset = '0' then
		state_tx1_reg <= xmit_idle;
		state_tx1_next <= xmit_idle;
		data_tx <= (others=>'0');
		time_delay_reg <= (others=>'0');
		time_delay_next <= (others=>'0');
		xmit_array <= (others=>(others=>'0'));
		xmit_array(0) <= X"FF";
		xmit_uindex <= "0000";
--		pstart_tx <= '0';
		start_tx <= '0';
--		led1 <= "1111";
		skip <= '1';
		skip2 <= '1';
		xmit_stdlv <= (others=>'0');

	else if clk'event and clk = '1' then
		case state_tx1_reg is
			when xmit_idle =>
				start_tx <= '0';
				-- never goes faster than this delay
 				if time_delay_reg > TIME_DELAY5 then
					time_delay_next <= (others=>'0');
					state_tx1_next <= xmit_start;
				else
					time_delay_next <= time_delay_reg + 1;
				end if;	

			when xmit_start =>
				xmit_uindex <= "0001";
--				led1 <= "1110";
				xmit_array(0) <= X"FF";
				-- send high_byte first because this is byte reversed in DS1620.vhd
				xmit_array(1) <= temp_data1(7 downto 0);
				xmit_array(2) <= temp_data1(15 downto 8);
				xmit_array(3) <= temp_data2(7 downto 0);
				xmit_array(4) <= temp_data2(15 downto 8);
				xmit_array(5) <= temp_data3(7 downto 0);
				xmit_array(6) <= temp_data3(15 downto 8);
				xmit_array(7) <= temp_data4(7 downto 0);
				xmit_array(8) <= temp_data4(15 downto 8);
				xmit_array(9) <= rpm_result(7 downto 0);
				xmit_array(10) <= rpm_result(15 downto 8);
				xmit_array(11) <= mph_result(7 downto 0);
				xmit_array(12) <= mph_result(15 downto 8);
				xmit_array(13) <= X"05";
				xmit_array(14) <= X"06";
				xmit_array(15) <= X"07";
				state_tx1_next <= xmit_check_FF;
--				state_tx1_next <= xmit_presend;

			when xmit_check_FF =>
--				led1 <= "1101";
				xmit_stdlv <= xmit_array(conv_integer(xmit_uindex));
				state_tx1_next <= xmit_check_FF2;
				
			when xmit_check_FF2 =>
--				led1 <= "1011";
				skip <= not skip;
				if skip = '1' then
					if xmit_stdlv = X"FF" then
						xmit_array(conv_integer(xmit_uindex)) <= X"FE";
					end if;
					if xmit_uindex < NO_ELEMS - 1 then
						xmit_uindex <= xmit_uindex + 1;
						state_tx1_next <= xmit_check_FF;
					else
--						xmit_uindex <= "00001";
						state_tx1_next <= xmit_presend;
					end if;
				end if;

			when xmit_presend =>
--				led1 <= "1110";
				xmit_uindex <= "0000";
				state_tx1_next <= xmit_send;
				
			when xmit_send =>
--				led1 <= "1101";
				data_tx <= xmit_array(conv_integer(xmit_uindex));
				start_tx <= '1';
				state_tx1_next <= xmit_wait;
					
			when xmit_wait =>
--				led1 <= "1011";
				if done_tx = '1' then
					start_tx <= '0';
					state_tx1_next <= xmit_inc;
				end if;

			when xmit_inc =>	
				skip2 <= not skip2;
				if skip2 = '1' then
					if xmit_uindex < NO_ELEMS - 1 then
						xmit_uindex <= xmit_uindex + 1;
						state_tx1_next <= xmit_send;
					else
						state_tx1_next <= xmit_delay;
					end if;
				end if;
				
			when xmit_delay =>
--				led1 <= "0111";
--				if time_delay_reg > TIME_DELAY9/1000 then
				if time_delay_reg > TIME_DELAY5 then
					time_delay_next <= (others=>'0');
					state_tx1_next <= xmit_start;
--					state_tx1_next <= xmit_presend;
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
variable no_params: integer range 0 to 255:= 0;
variable no_params2: integer range 0 to 255:= 0;
begin
	if reset = '0' then
		state_uart_reg2 <= pre_idle;
		state_uart_next2 <= pre_idle;
		time_delay_reg4 <= (others=>'0');
		time_delay_next4 <= (others=>'0');
		start_rx <= '0';
		mparam <= (others=>'0');
		mcmd <= (others=>'0');
--		led1 <= "1111";
		start_calc <= '0';
		data_sent <= '0';
		inc_params <= (others=>'0');
--		data_tx <= (others=>'0');
--		start_tx <= '0';
		stlv_flag <= (others=>'0');
	
	else if clk'event and clk = '1' then
		case state_uart_reg2 is
			when pre_idle =>
				mcmd <= LCD_PWM;
				mparam <= PWM_30DC_PARAM;
				state_uart_next2 <= idle2;
			
			when idle2 =>
				data_sent <= '0';
--				start_tx <= '0';
				start_calc <= '0';
				if data_ready = '1' then
					start_rx <= '1';
					stlv_flag <= (others=>'0');
					state_uart_next2 <= next1;
				end if;

			when next1 =>
				if done_rx = '1' then
					start_rx <= '0';
					if cmd_param = '1' then
						mcmd <= data_rx;
						inc_params <= (others=>'0');
					else
						no_params := conv_integer(inc_params);
						download(no_params) <= data_rx;
						inc_params <= inc_params + 1;
						stlv_flag <= data_rx;
					end if;
					state_uart_next2 <= start1;
				end if;	
						
			when start1 =>
--				data_tx <= data_rx;
--				start_tx <= '1';
				if stlv_flag = X"FF" then
					start_calc <= '1';
				end if;
				state_uart_next2 <= done;

			when done =>
--				start_tx <= '0';
				start_calc <= '0';
				if start_calc = '1' then
					state_uart_next2 <= rx_delay;
				else state_uart_next2 <= idle2;
				end if;

			-- 40ms delay to allow sender finish sending manditory 8 bytes
			-- and then pull data_ready line back low
			when rx_delay =>
				if time_delay_reg4 > TIME_DELAY7 then
					time_delay_next4 <= (others=>'0');
					state_uart_next2 <= idle2;
				else
					time_delay_next4 <= time_delay_reg4 + 1;
				end if;

		end case;
		state_uart_reg2 <= state_uart_next2;
		time_delay_reg4 <= time_delay_next4;
		end if;
	end if;
end process;

-- ********************************************************************************
calc_proc1: process(clk,reset,main_reg1)
variable temp: integer range 0 to 11:= 0;
begin
	if reset = '0' then
		main_reg1 <= idle1a;
		main_next1 <= idle1a;
		stlv_duty_cycle <= (others=>'1');
		special <= '0';
		dtmf_index <= (others=>'0');
		stdlv_transmit_update_rate <=  X"1FFFFF";
		stlv_duty_cycle <= (others=>'1');
		start_pwm2 <= '0';
		key_len <= X"14";
		stlv_duty_cycle2 <= "111111";
		
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
					case mcmd is
						when SET_UPDATE_RATE =>
							stdlv_transmit_update_rate <= mparam & X"FFFF";
							main_next1 <= do_mcmd;
						when DTMF_TONE_ON =>
							led1 <= "1011";
							dtmf_index <= mparam(4 downto 0);
							special <= '0';
							start_dtmf <= '1';
							main_next1 <= do_mcmd;
						when DTMF_TONE_OFF =>
							led1 <= "1101";
							start_dtmf <= '0';
							special <= '0';
							main_next1 <= do_mcmd;
						when SPECIAL_TONE_ON =>
							special <= '1';
							start_dtmf <= '1';
							main_next1 <= do_mcmd;
						when LCD_PWM =>
							stlv_duty_cycle <= mparam(4 downto 0);
							main_next1 <= do_mcmd;
						when FP_SHUTOFF_OVERRIDE =>
							fp_override <= mparam(0);
							main_next1 <= do_mcmd;
						when SET_MAX_REV_LIMITER =>
							rev_limit_max <= mparam;
							reset_rev_limits <= '1';
							main_next1 <= do_mcmd;
						when SET_MIN_REV_LIMITER =>
							rev_limit_min <= mparam;
							reset_rev_limits <= '1';
							main_next1 <= do_mcmd;
						when TEST_RPM_LIMIT =>
							test_rpm_rev_limits <= mparam(0);
							main_next1 <= do_mcmd;
						when TUNE_ON =>
							key_len <= conv_signed(conv_integer(download(0)),8);
							note_len <= download(1);
							stlv_duty_cycle2 <= download(2)(5 downto 0);
							start_pwm2 <= '1';
							main_next1 <= do_mcmd;
						when TUNE_OFF =>
--							led1 <= "1111";
							start_pwm2 <= '0';
							main_next1 <= do_mcmd;
						when LOAD_TUNE =>
--							tune1 <= convert_dl_to_tune_array(download);
							tune1 <= load_tune_array(tune1);
							main_next1 <= do_mcmd;
						when others =>
--							main_next1 <= idle1a;
							main_next1 <= other_cmd;
					end case;
				end if;
			when do_mcmd =>
				main_next1 <= idle1a;
			when other_cmd =>	
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
		time_delay_mph <= "00" & X"3FF000";
		time_delay_rpm <= "00" & X"3FFFFF";		-- 184ms
--		time_delay_rpm <= "00" & X"1FFFFF";		-- 92ms
--		time_delay_rpm <= "00" & X"0FFFFF";		-- 46ms
--		time_delay_rpm <= "00" & X"07FFFF";		-- 23ms
--		time_delay_rpm <= "00" & X"03FFFF";		-- 11ms
		-- these start the rpm/mph wrappers in the normal
		-- send char mode
		start_rpm_wrapper <= '1';
		start_mph_wrapper <= '1';
		mph_or_not <= '0';
--		led1 <= "1111";

-- if mcmd is one of the special commands other than
-- the normal SEND_CHAR_CMD then we must stop the wrapper
-- and wait for the sendcharLED to finish
-- the sendcharLED sets the display_done1 in wrapperLED
-- and wrapperLED sets either the rpm_done or the mph_done 
-- flags

	elsif clk'event and clk = '1' then
		case main_reg2 is
			when idle2a =>
				if start_calc = '1' then

					case mcmd is
						when RPM_OFF_CMD =>
							start_rpm_wrapper <= '0';
							cmd_rpm <= mcmd;
							param_rpm <= mparam;
							main_next2 <= wait_for_rpm_done;
						when RPM_SET_BRIGHTNESS_CMD =>
							start_rpm_wrapper <= '0';
							cmd_rpm <= mcmd;
							param_rpm <= mparam;
							main_next2 <= wait_for_rpm_done;
						when RPM_SET_CDECIMAL_CMD =>
							start_rpm_wrapper <= '0';
							cmd_rpm <= mcmd;
							param_rpm <= mparam;
							main_next2 <= wait_for_rpm_done;
						when RPM_SET_UPDATE_RATE_CMD =>
							start_rpm_wrapper <= '0';
							cmd_rpm <= mcmd;
							param_rpm <= mparam;
							time_delay_rpm <= mparam & "11" & X"FFFF";
							main_next2 <= wait_for_rpm_done;
						when RPM_SET_FACTOR_CMD =>
							start_rpm_wrapper <= '0';
							cmd_rpm <= mcmd;
							param_rpm <= mparam;
							mph_factor <= mparam(5 downto 0);
							main_next2 <= wait_for_rpm_done;
						when RPM_SET_FACTORY_RESET =>
							start_rpm_wrapper <= '0';
							cmd_rpm <= mcmd;
							param_rpm <= mparam;
							main_next2 <= wait_for_rpm_done;
						when MPH_OFF_CMD =>
							start_mph_wrapper <= '0';
							cmd_mph <= mcmd;
							param_mph <= mparam;
							main_next2 <= wait_for_mph_done;
						when MPH_SET_BRIGHTNESS_CMD =>
							start_mph_wrapper <= '0';
							cmd_mph <= mcmd;
							param_mph <= mparam;
							main_next2 <= wait_for_mph_done;
						when MPH_SET_CDECIMAL_CMD =>
							start_mph_wrapper <= '0';
							cmd_mph <= mcmd;
							param_mph <= mparam;
							main_next2 <= wait_for_mph_done;
						when MPH_SET_UPDATE_RATE_CMD =>
							start_mph_wrapper <= '0';
							cmd_mph <= mcmd;
							param_mph <= mparam;
							time_delay_mph <= mparam & "11" & X"FFFF";
							main_next2 <= wait_for_mph_done;
						when MPH_SET_FACTOR_CMD =>
							start_mph_wrapper <= '0';
							cmd_mph <= mcmd;
							param_mph <= mparam;
							mph_factor <= mparam(5 downto 0);
							main_next2 <= wait_for_mph_done;
						when MPH_SET_FACTORY_RESET =>
							start_mph_wrapper <= '0';
							cmd_mph <= mcmd;
							param_mph <= mparam;
							main_next2 <= wait_for_mph_done;
						when others =>
							main_next2 <= idle2a;
					end case;	
				end if;	

			when wait_for_rpm_done =>
				if rpm_done = '1' then
					start_rpm_wrapper <= '1';
					main_next2 <= delay_rpm;
				end if;

			when wait_for_mph_done =>
				if mph_done = '1' then
					start_mph_wrapper <= '1';
					main_next2 <= delay_mph;
				end if;

			when delay_rpm =>
				if time_delay_reg2 > TIME_DELAY9/500 then
					time_delay_next2 <= (others=>'0');
					main_next2 <= do_rpm_cmd;
				else
					time_delay_next2 <= time_delay_reg2 + 1;
				end if;
			when delay_mph =>
				if time_delay_reg2 > TIME_DELAY9/500 then
					time_delay_next2 <= (others=>'0');
					main_next2 <= do_mph_cmd;
				else
					time_delay_next2 <= time_delay_reg2 + 1;
				end if;
				
			when do_rpm_cmd =>
					start_rpm_wrapper <= '0';
					cmd_rpm <= RPM_SEND_CHAR_CMD;
					param_rpm <= (others=>'0');
					main_next2 <= special_rpm_done;

			when do_mph_cmd =>
					start_mph_wrapper <= '0';
					cmd_mph <= MPH_SEND_CHAR_CMD;
					param_mph <= (others=>'0');
					main_next2 <= special_mph_done;

			when special_rpm_done =>
--				if rpm_done = '1' then
					start_rpm_wrapper <= '1';
					main_next2 <= idle2a;
--				end if;

			when special_mph_done =>
--				if mph_done = '1' then
					start_mph_wrapper <= '1';
					main_next2 <= idle2a;
--				end if;
		end case;
		main_reg2 <= main_next2;
		time_delay_reg2 <= time_delay_next2;
	end if;
end process;		

-- ********************************************************************************
-- slow on/off pulse on pin 11 to use as a reference using voltmeter
ref_delay: process(clk, reset, state_ref_reg)
begin
	if reset = '0' then
		time_delay_reg3 <= (others=>'0');
		time_delay_next3 <= (others=>'0');
		state_ref_reg <= idle_ref;
		state_ref_next <= idle_ref;
		ref_signal <= '0';

	else if clk'event and clk = '1' then
		case state_ref_reg is
			when idle_ref =>
 				if time_delay_reg3 > TIME_DELAY3 then
					time_delay_next3 <= (others=>'0');
					state_ref_next <= start_ref;
					ref_signal <= '1';
				else
					time_delay_next3 <= time_delay_reg3 + 1;
				end if;	

			when start_ref =>
 				if time_delay_reg3 > TIME_DELAY9 then
					time_delay_next3 <= (others=>'0');
					state_ref_next <= done_ref;
					ref_signal <= '0';
				else
					time_delay_next3 <= time_delay_reg3 + 1;
				end if;	

			when done_ref =>
				state_ref_next <= idle_ref;

		end case;
		time_delay_reg3 <= time_delay_next3;
		state_ref_reg <= state_ref_next;
		end if;
	end if;
end process;

end truck_arch;
