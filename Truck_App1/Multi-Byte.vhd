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
		PP_CS : out std_logic;
		PP_ACK : in std_logic;
		PP_DATA0 : out std_logic;
		PP_DATA1 : out std_logic;
		PP_DATA2 : out std_logic;
		PP_DATA3 : out std_logic;
		PP_DATA4 : out std_logic;
		PP_DATA5 : out std_logic;
		PP_DATA6 : out std_logic;
		PP_DATA7 : out std_logic;
		rx_uart: in std_logic;
		data_ready : in std_logic;
		cmd_param : in std_logic;
		data_sent : out std_logic;

		pwm_signal_1 : out std_logic;
		pwm_signal_2 : out std_logic;
		pwm_spk1 : out std_logic;
		pwm_spk2 : out std_logic;
		pwm_lcd : out std_logic;
--		fp_shutoff : out std_logic;
--		rev_limit : out std_logic;
		rpm1_signal : in std_logic;
--		rpm2_signal : in std_logic;
		mph1_signal : in std_logic;
--		mph2_signal : in std_logic;
		tx_rpm_mph : out std_logic;
		led1: out std_logic_vector(3 downto 0)
		);
end multi_byte;

architecture truck_arch of multi_byte is

	type pport_type is (pp_idle, pp_start1, pp_start2, pp_done, pp_done2);
	signal pport_reg, pport_next: pport_type;

	-- recv_uart
	type state_uart2 is (pre_idle, idle2, next1, start1, done, rx_delay);
	signal state_uart_reg2, state_uart_next2: state_uart2;

	type calc_type1 is (idle1a, do_mcmd, other_cmd);
	signal main_reg1, main_next1: calc_type1;

	type state_fp is (idle_fp, start_fp, done_fp);
	signal state_reg_fp, state_next_fp: state_fp;

	type state_rev is (idle_rev, start_rev, done_rev);
	signal state_reg_rev, state_next_rev: state_rev;

	type state_led is (led_idle, led_start, led_delay, led_start_xmit, led_done);
	signal state_led_reg, state_led_next: state_led;

	signal time_delay_reg, time_delay_next: unsigned(24 downto 0);		-- send_uart1
	signal time_delay_reg3, time_delay_next3: unsigned(27 downto 0);
	signal time_delay_reg2, time_delay_next2: unsigned(17 downto 0);
	signal time_delay_reg1, time_delay_next1: unsigned(24 downto 0);

	signal rpm_db, mph_db: std_logic;
	signal rpm_result: std_logic_vector(15 downto 0);
	signal mph_result: std_logic_vector(15 downto 0);
	signal mph_factor: std_logic_vector(22 downto 0);
	signal rpm_factor: std_logic_vector(22 downto 0);
	signal start_tx, done_tx, start_rx, done_rx: std_logic;
	signal data_tx, data_rx: std_logic_vector(7 downto 0);
	signal urpm_result: unsigned(16 downto 0);

	signal stlv_duty_cycle: std_logic_vector(4 downto 0);

	signal stdlv_temp1, stdlv_temp2: std_logic_vector(16 downto 0);
	signal skip, skip2: std_logic;
	signal xmit_stdlv: std_logic_vector(7 downto 0);
	signal done_rpm, done_mph: std_logic;
	signal stlv_duty_cycle2: std_logic_vector(5 downto 0);
	signal sPort, sPort2: unsigned(7 downto 0);
	signal download: dl_array;
	signal upload: dl_array;
	signal LED_cmd: std_logic_vector(7 downto 0);
--	signal uLED_cmd: unsigned(7 downto 0);
	signal mcmd: std_logic_vector(7 downto 0);
	signal mparam: std_logic_vector(7 downto 0);
	signal stlv_flag: std_logic_vector(7 downto 0);
	signal inc_params: signed(7 downto 0);	-- used in recv
	signal start_calc: std_logic;
	signal start_pwm2: std_logic;
	signal pwm_done1: std_logic;
	signal key_len: signed(7 downto 0);
	signal tune_ptr: signed(2 downto 0);
	signal tune1: tune_array;
	signal note_len: std_logic_vector(7 downto 0);
	signal dtmf_index: std_logic_vector(4 downto 0);
	signal special: std_logic;
	signal start_dtmf: std_logic;
	signal dtmf_done1: std_logic;
	signal done_pwm: std_logic;
	signal shutdown_rpm: std_logic;
	signal fp_override: std_logic;
	signal rev_limit_max: std_logic_vector(7 downto 0);
	signal rev_limit_min: std_logic_vector(7 downto 0);
	signal urev_limit_max: unsigned(16 downto 0);
	signal urev_limit_min: unsigned(16 downto 0);
	signal reset_rev_limits: std_logic;
	signal led_dl_array: led_array;

begin

pwm_unit: entity work.lcd_pwm
	generic map(W2=>W2_SIZE)
	port map(clk=>clk,reset=>reset,
		pwm_signal=>pwm_lcd,
		duty_cycle=>stlv_duty_cycle,
		done=>done_pwm);

dtmf_unit: entity work.dtmf
	port map(clk=>clk, reset=>reset,
		dtmf_index=>dtmf_index,
		special_cmd=>special,
		dtmf_signal_1=>pwm_spk1,
		dtmf_signal_2=>pwm_spk2,
		start=>start_dtmf,
		done=>dtmf_done1);

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

rpm_sensor_unit: entity work.sensor(arch)
	generic map(CLK_COUNT=>RPM_CLOCK_COUNT,DVND_FACTOR=>RPM_DVND)
	port map(clk=>clk, reset=>reset,
		sensor1=>rpm_db,
		result=>rpm_result,
		factor=>rpm_factor,
		sensor_done=>done_rpm);

mph_sensor_unit: entity work.sensor(arch)
	generic map(CLK_COUNT=>MPH_CLOCK_COUNT,DVND_FACTOR=>MPH_DVND)
	port map(clk=>clk, reset=>reset,
		sensor1=>mph_db,
		result=>mph_result,
		factor=>mph_factor,
		sensor_done=>done_mph);

tx_uart_wrapper_unit: entity work.uartLED(str_arch)
	generic map(DVSR_M=>DVSR_MU_19200)
	port map(clk=>clk, reset=>reset,
	tx_start=>start_tx,
	w_data=>data_tx,
	done_tick=>done_tx,
	tx=>tx_rpm_mph);

rx_uart_wrapper_unit: entity work.uartLED2(str_arch)
	generic map(DVSR_M=>DVSR_MU_19200)
	port map(clk=>clk, reset=>reset,
	rx_start=>start_rx,
	r_data=>data_rx,
	done_tick=>done_rx,
	rx=>rx_uart);

-- tx_uart_wrapper_unit2: entity work.uartLED(str_arch)
	-- generic map(DVSR_M=>DVSR_MU_19200)
	-- port map(clk=>clk, reset=>reset,
	-- tx_start=>start_tx,
	-- w_data=>data_tx,
	-- done_tick=>done_tx,
	-- tx=>tx_LEDcmd);

db0_unit: entity work.db_fsm
	port map(clk=>clk,reset=>reset,
		sw=>rpm1_signal,
		db=>rpm_db);

db1_unit: entity work.db_fsm
	port map(clk=>clk,reset=>reset,
		sw=>mph1_signal,
		db=>mph_db);

send_mph_rpm_unit: process(clk, reset, state_led_reg)
begin
	if reset = '0' then
		state_led_reg <= led_idle;
		state_led_next <= led_idle;
		time_delay_reg1 <= (others=>'0');
		time_delay_next1 <= (others=>'0');
		sPort2 <= (others=>'0');
		start_tx <= '0';
		data_tx <= (others=>'0');
		LED_cmd <= X"FD";
--		uLED_cmd <= X"FD";
		led_dl_array <= (others=>(others=>'0'));
--		led1 <= "1111";
		led_dl_array(0) <= X"FF";

	else if clk'event and clk = '1' then
		case state_led_reg is
			when led_idle =>
				start_tx <= '0';
				skip2 <= not skip2;
				if skip2 = '1' then
					sPort2 <= sPort2 + 1;
					if sPort2 > 4 then
						sPort2 <= (others=>'0');
					end if;
				end if;
				state_led_next <= led_start;

			when led_start =>
--				led1 <= "0111";
				led_dl_array(1) <= rpm_result(7 downto 0);
				led_dl_array(2) <= rpm_result(15 downto 8);
				led_dl_array(3) <= mph_result(7 downto 0);
				led_dl_array(4) <= mph_result(15 downto 8);
				led_dl_array(5) <= LED_cmd;
				state_led_next <= led_delay;

			when led_delay =>
				if time_delay_reg1 > TIME_DELAY7 then
					time_delay_next1 <= (others=>'0');
					state_led_next <= led_start_xmit;
--					led1 <= "1011";
				else
					time_delay_next1 <= time_delay_reg1 + 1;
				end if;

			when led_start_xmit =>
				data_tx <= led_dl_array(conv_integer(sPort2));
				start_tx <= '1';
--				uLED_cmd <= uLED_cmd - 1;
				state_led_next <= led_done;

			when led_done =>
				start_tx <= '0';
				state_led_next <= led_idle;
--				led1 <= "1110";
		end case;		
		state_led_reg <= state_led_next;
		time_delay_reg1 <= time_delay_next1;
		end if;
	end if;
end process;

-- ********************************************************************************
-- mon_fp: process(clk, reset, state_reg_fp)
-- begin
	-- if reset = '0' then
		-- state_reg_fp <= idle_fp;
		-- state_next_fp <= idle_fp;
		-- urpm_result <= (others=>'0');
		-- fp_shutoff <= '1';	-- start out with relay closed (fp can be on if other relay closed)

	-- else if clk'event and clk = '1' then
		-- case state_reg_fp is
			-- when idle_fp =>
				-- -- if test_rpm_rev_limits = '1' then
					-- -- urpm_result <= unsigned(rpm_result);
				-- -- else
					-- -- urpm_result <= unsigned(rpm_result);
				-- -- end if;
				-- state_next_fp <= start_fp;
			-- when start_fp =>
				-- if fp_override = '1' or urpm_result > 700 then
					-- fp_shutoff <= '1';		-- close relay to fuel pump
					-- state_next_fp <= idle_fp;
				-- else
					-- state_next_fp <= done_fp;
				-- end if;	
			-- when done_fp =>
				-- if urpm_result < FP_RPM_MINIMUM then
					-- fp_shutoff <= '0';
				-- else fp_shutoff <= '1';
				-- end if;
				-- -- if shutdown_rpm = '1' then
					-- -- fp_shutoff <= '0';
				-- -- end if;
				-- state_next_fp <= idle_fp;
		-- end case;
		-- state_reg_fp <= state_next_fp;
		-- end if;
	-- end if;
-- end process;

-- -- ********************************************************************************
-- -- 1) check if rpm > MAX -> turn off
-- -- 2) wait for rpm to be < MIN -> turn on
-- mon_rev: process(clk, reset, state_reg_rev)
-- begin
	-- if reset = '0' then
		-- state_reg_rev <= idle_rev;
		-- state_next_rev <= idle_rev;
		-- rev_limit <= '1';		-- start out with relay closed so ignition can be on if other relay closed

	-- else if clk'event and clk = '1' then
		-- case state_reg_rev is
			-- when idle_rev =>
				-- stdlv_temp1 <= "00000" & rev_limit_max & "0000";
				-- stdlv_temp2 <= "00000" & rev_limit_min & "0000";
				-- urev_limit_max <= unsigned(stdlv_temp1);
				-- urev_limit_min <= unsigned(stdlv_temp2);
				-- state_next_rev <= start_rev;
			-- when start_rev =>
				-- if reset_rev_limits = '1' then
					-- state_next_rev <= idle_rev;
				-- else
					-- if urpm_result > urev_limit_max then
						-- rev_limit <= '0';
						-- state_next_rev <= done_rev;
					-- end if;
				-- end if;
			-- when done_rev =>
				-- if reset_rev_limits = '1' then
					-- state_next_rev <= idle_rev;
				-- elsif urpm_result < urev_limit_min then
					-- rev_limit <= '1';
					-- state_next_rev <= start_rev;
				-- end if;
		-- end case;
		-- state_reg_rev <= state_next_rev;
		-- end if;
	-- end if;
-- end process;

-- ********************************************************************************
pport_unit: process(clk, reset, pport_reg)
variable temp: integer range 0 to 255:= 0;
begin
	if reset = '0' then
		pport_reg <= pp_idle;
		pport_next <= pp_idle;
		time_delay_reg <= (others=>'0');
		time_delay_next <= (others=>'0');
		sPort <= (others=>'0');
--		pwm_lcd <= '0';
--sPort <= "10101010";

		PP_CS <= '0';
		PP_DATA0 <= '0';
		PP_DATA1 <= '0';
		PP_DATA2 <= '0';
		PP_DATA3 <= '0';
		PP_DATA4 <= '0';
		PP_DATA5 <= '0';
		PP_DATA6 <= '0';
		PP_DATA7 <= '0';
--		led1 <= "1111";
		skip <= '0';
		upload <= (others=>(others=>'0'));
		upload(0) <= X"FE";		-- start of frame
		upload(1) <= X"00";		-- rpm low byte
		upload(2) <= X"01";		-- rpm high byte
		upload(3) <= X"02";		-- mph low byte
		upload(4) <= X"03";		-- mph high byte
		upload(5) <= X"04";		-- brightness param
		upload(6) <= X"05";
		upload(7) <= X"06";
		upload(8) <= X"07";
		upload(9) <= X"08";
		upload(10) <= X"09";
		rpm_factor <= std_logic_vector(conv_unsigned(RPM_DVND, DVSR_SIZE));
		mph_factor <= std_logic_vector(conv_unsigned(MPH_DVND, DVSR_SIZE));
		
	else if clk'event and clk = '1' then
		case pport_reg is
			when pp_idle =>
				PP_DATA0 <= upload(conv_integer(sPort))(0);
				PP_DATA1 <= upload(conv_integer(sPort))(1);
				PP_DATA2 <= upload(conv_integer(sPort))(2);
				PP_DATA3 <= upload(conv_integer(sPort))(3);
				PP_DATA4 <= upload(conv_integer(sPort))(4);
				PP_DATA5 <= upload(conv_integer(sPort))(5);
				PP_DATA6 <= upload(conv_integer(sPort))(6);
				PP_DATA7 <= upload(conv_integer(sPort))(7);
				pport_next <= pp_start1;
			
 			when pp_start1 =>
				PP_CS <= '1';
--				led1 <= "0111";
				skip <= not skip;
				if skip = '1' then
					sPort <= sPort + 1;
					if sPort > 8 then
						sPort <= (others=>'0');
					end if;
				end if;
				pport_next <= pp_start2;

			when pp_start2 =>
				if time_delay_reg > TIME_DELAY8 then
 					time_delay_next <= (others=>'0');
					PP_CS <= '0';
--					led1 <= "0111";
					pport_next <= pp_done;
				else
					time_delay_next <= time_delay_reg + 1;
				end if;

			when pp_done =>
				if PP_ACK = '1' then
					pport_next <= pp_done2;
--					led1 <= "1011";
				end if;

			when pp_done2 =>
				if time_delay_reg > TIME_DELAY8 then
					time_delay_next <= (others=>'0');
					temp := conv_integer(sPort);
					upload(1) <= rpm_result(7 downto 0);
					upload(2) <= rpm_result(15 downto 8);
					upload(3) <= mph_result(7 downto 0);
					upload(4) <= mph_result(15 downto 8);
					upload(5) <= LED_cmd;
					upload(6) <= download(0);
					upload(7) <= download(1);
					upload(8) <= download(2);
					upload(9) <= download(3);
					upload(10) <= download(4);
					-- upload(6) <= download(5);
					-- upload(7) <= download(6);
					-- upload(8) <= download(7);
					-- upload(9) <= download(8);
					-- upload(10) <= download(9);
					-- upload(11) <= download(10);
					-- upload(12) <= download(11);
					-- upload(13) <= download(12);
					-- upload(14) <= download(13);
					-- upload(15) <= download(14);
					pport_next <= pp_idle;
--					led1 <= "1101";
				else
					time_delay_next <= time_delay_reg + 1;
				end if;
		end case;
		pport_reg <= pport_next;
		time_delay_reg <= time_delay_next;
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
		time_delay_reg3 <= (others=>'0');
		time_delay_next3 <= (others=>'0');
		start_rx <= '0';
		mparam <= (others=>'0');
		mcmd <= (others=>'0');
--		led1 <= "1111";
		start_calc <= '0';
		data_sent <= '0';
		inc_params <= (others=>'0');
		stlv_flag <= (others=>'0');
		download <= (others=>(others=>'0'));
	
	else if clk'event and clk = '1' then
		case state_uart_reg2 is
			when pre_idle =>
				mcmd <= LCD_PWM;
				mparam <= PWM_30DC_PARAM;
				state_uart_next2 <= idle2;
--				led1 <= "0111";
			
			when idle2 =>
				data_sent <= '0';
				start_calc <= '0';
				if data_ready = '1' then
					start_rx <= '1';
					stlv_flag <= (others=>'0');
					state_uart_next2 <= next1;
--					led1 <= "0111";
				end if;

			when next1 =>
				if done_rx = '1' then
					start_rx <= '0';
					if cmd_param = '1' then
						mcmd <= data_rx;
--						led1 <= mcmd(3 downto 0);
						inc_params <= (others=>'0');
--						led1 <= "1011";
					else
						no_params := conv_integer(inc_params);
						download(no_params) <= data_rx;
						inc_params <= inc_params + 1;
						stlv_flag <= data_rx;
--						led1 <= "1101";
					end if;
					state_uart_next2 <= start1;
				end if;	
						
			when start1 =>
				if stlv_flag = X"FF" then
					start_calc <= '1';
--					led1 <= "1110";
				end if;
				state_uart_next2 <= done;

			when done =>
--				if start_calc = '1' then
					start_calc <= '0';
--					state_uart_next2 <= rx_delay;
--				end if;
				state_uart_next2 <= idle2;

			-- 40ms delay to allow sender finish sending manditory 8 bytes
			-- and then pull data_ready line back low
			when rx_delay =>
				if time_delay_reg3 > TIME_DELAY7 then
					time_delay_next3 <= (others=>'0');
					state_uart_next2 <= idle2;
				else
					time_delay_next3 <= time_delay_reg3 + 1;
				end if;

		end case;
		state_uart_reg2 <= state_uart_next2;
		time_delay_reg3 <= time_delay_next3;
		end if;
	end if;
end process;

-- ********************************************************************************
calc_proc1: process(clk,reset,main_reg1, tune1)
variable temp: integer range 0 to 11:= 0;
begin
	if reset = '0' then
		main_reg1 <= idle1a;
		main_next1 <= idle1a;
		stlv_duty_cycle <= (others=>'1');
		special <= '0';
		dtmf_index <= (others=>'0');
--		stdlv_transmit_update_rate <=  X"1FFFFF";
		led1 <= "1111";		
--		fp_override <= '0';	-- start off with relay open (need to send command to close it before starting)
--		rev_limit_max <= conv_std_logic_vector(RPM_MAXIMUM,7);	-- start out with defaults
--		rev_limit_min <= conv_std_logic_vector(RPM_MINIMUM,7);
-- 4000 = 0xFA0
-- 3888 = 0xF30
--		rev_limit_max <= X"FA";
--		rev_limit_min <= X"F3";
--		reset_rev_limits <= '0';
--		stlv_temp2 <= (others=>'1');
--		test_rpm_rev_limits <= '0';
		start_pwm2 <= '0';
		key_len <= X"14";
		stlv_duty_cycle2 <= "111111";
		tune1 <= load_tune_array(tune1);

	elsif clk'event and clk = '1' then
		case main_reg1 is
			when idle1a =>
--				reset_rev_limits <= '0';
				if start_calc = '1' then
--					led1 <= download(0)(3 downto 0);	-- first param is download(0)
					led1 <= mcmd(3 downto 0);
				
					case mcmd is
						when SET_UPDATE_RATE =>
--							stdlv_transmit_update_rate <= mparam & X"FFFF";
							main_next1 <= do_mcmd;
						when DTMF_TONE_ON =>
--							led1 <= "0111";
							dtmf_index <= mparam(4 downto 0);
							special <= '0';
							start_dtmf <= '1';
							main_next1 <= do_mcmd;
						when DTMF_TONE_OFF =>
--							led1 <= "1011";
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
--							fp_override <= mparam(0);
							main_next1 <= do_mcmd;
						when SET_MAX_REV_LIMITER =>
--							rev_limit_max <= mparam;
--							reset_rev_limits <= '1';
							main_next1 <= do_mcmd;
						when SET_MIN_REV_LIMITER =>
--							rev_limit_min <= mparam;
--							reset_rev_limits <= '1';
							main_next1 <= do_mcmd;
						when TEST_RPM_LIMIT =>
--							test_rpm_rev_limits <= mparam(0);
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
--							tune1 <= load_tune_array(tune1);
							main_next1 <= do_mcmd;
--							led1 <= "0111";
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

end truck_arch;
