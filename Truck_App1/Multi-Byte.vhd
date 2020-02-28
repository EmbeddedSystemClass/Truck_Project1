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
		fp_shutoff : out std_logic;
		rev_limit : out std_logic;
		rpm1_signal : in std_logic;
--		rpm2_signal : in std_logic;
		mph1_signal : in std_logic;
--		mph2_signal : in std_logic;
		tx_rpm_mph : out std_logic;
		TS7800TX : out std_logic;
		TS7800RX : in std_logic;
		monitorTX : out std_logic;
		monitorRX : in std_logic;
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
		led1: out std_logic_vector(3 downto 0)
		);
end multi_byte;

architecture truck_arch of multi_byte is

	type pport_type is (pp_preidle, pp_idle, pp_start1, pp_start2, pp_done, pp_done2);
	signal pport_reg, pport_next: pport_type;

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

	type state_test is (test_idle, test_start1, test_start2, test_done);
	signal test_reg, test_next: state_test;

	type state_rev_limit is (rev_idle, rev_start1, rev_start2, rev_done);
	signal rev_reg, rev_next: state_rev_limit;

	type state_mcp is (mcp_idle, mcp_start1, mcp_start2, mcp_done);
	signal mcp_reg, mcp_next: state_mcp;

	signal time_delay_reg, time_delay_next: unsigned(25 downto 0);
	signal time_delay_reg1, time_delay_next1: unsigned(24 downto 0);
	signal time_delay_reg3, time_delay_next3: unsigned(27 downto 0);
	signal time_delay_reg4, time_delay_next4: unsigned(24 downto 0);
	signal time_delay_reg5, time_delay_next5: unsigned(25 downto 0);
	signal time_delay_reg6, time_delay_next6: unsigned(25 downto 0);

	signal rpm_db, mph_db: std_logic;
	signal rpm_result: std_logic_vector(15 downto 0);
	signal urpm_result: unsigned(15 downto 0);
	signal mph_result: std_logic_vector(15 downto 0);
	signal mph_factor: std_logic_vector(22 downto 0);
	signal rpm_factor: std_logic_vector(22 downto 0);
	signal start_tx, done_tx, start_rx, done_rx: std_logic;
	signal data_tx, data_rx: std_logic_vector(7 downto 0);

	signal stlv_duty_cycle: std_logic_vector(4 downto 0);

	signal stdlv_temp1, stdlv_temp2: std_logic_vector(16 downto 0);
	signal skip, skip2: std_logic;
	signal xmit_stdlv: std_logic_vector(7 downto 0);
	signal done_rpm, done_mph: std_logic;
	signal stlv_duty_cycle2: std_logic_vector(5 downto 0);
	signal sPort, sPort2: unsigned(7 downto 0);
	signal download: dl_array;	-- array(0 to 25) of std_logic_vector(7 downto 0);
	signal upload: dl_array;
	signal LED_cmd: std_logic_vector(7 downto 0);
--	signal uLED_cmd: unsigned(7 downto 0);
	signal mcmd: std_logic_vector(7 downto 0);
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
	signal led_dl_array: led_array;
	signal start_mcp: std_logic;
	signal done_mcp: std_logic;
	signal mcp_results: mcp_array;
	signal compact: std_logic;
	signal adc_gate: std_logic;
	signal adc_delay: unsigned(25 downto 0);
	signal send_delay: unsigned(25 downto 0);
	signal rpm_mph_delay: unsigned(25 downto 0);
	signal high_rev_limit: unsigned(25 downto 0);
	signal low_rev_limit: unsigned(25 downto 0);
	signal dl_param: std_logic_vector(3 downto 0);
	signal stlv_rev_limit_override: std_logic;
	signal stlv_fuel_pump_override: std_logic;
	signal stlv_status1: std_logic_vector(7 downto 0);
	signal stlv_status2: std_logic_vector(7 downto 0);
	signal stlv_status3: std_logic_vector(7 downto 0);
	signal stlv_status4: std_logic_vector(7 downto 0);
	signal stlv_engine_running: std_logic;

begin

MCP_wrapper_unit: entity work.mcp_wrapper(truck_arch)
	port map(clk=>clk, reset=>reset,
		MCP_CS1=>MCP_CS1,
		MCP_CLK1=>MCP_CLK1,
		MCP_DIN1=>MCP_DIN1,
		MCP_DOUT1=>MCP_DOUT1,
		MCP_CS2=>MCP_CS2,
		MCP_CLK2=>MCP_CLK2,
		MCP_DIN2=>MCP_DIN2,
		MCP_DOUT2=>MCP_DOUT2,
		MCP_CS3=>MCP_CS3,
		MCP_CLK3=>MCP_CLK3,
		MCP_DIN3=>MCP_DIN3,
		MCP_DOUT3=>MCP_DOUT3,
		MCP_CS4=>MCP_CS4,
		MCP_CLK4=>MCP_CLK4,
		MCP_DIN4=>MCP_DIN4,
		MCP_DOUT4=>MCP_DOUT4,
		start=>start_mcp,
		done=>done_mcp,
		compact=>compact,
		results=>mcp_results);

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
				led_dl_array(1) <= rpm_result(7 downto 0);
				led_dl_array(2) <= rpm_result(15 downto 8);
				led_dl_array(3) <= mph_result(7 downto 0);
				led_dl_array(4) <= mph_result(15 downto 8);
				led_dl_array(5) <= LED_cmd;
				state_led_next <= led_delay;

			when led_delay =>
				if time_delay_reg1 > rpm_mph_delay then
					time_delay_next1 <= (others=>'0');
					state_led_next <= led_start_xmit;
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
		end case;		
		state_led_reg <= state_led_next;
		time_delay_reg1 <= time_delay_next1;
		end if;
	end if;
end process;
-- ********************************************************************************
pport_unit: process(clk, reset, pport_reg)
begin
	if reset = '0' then
		pport_reg <= pp_preidle;
		pport_next <= pp_preidle;
		time_delay_reg <= (others=>'0');
		time_delay_next <= (others=>'0');
		sPort <= (others=>'0');
		led1 <= "1111";
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
		skip <= '0';
		upload <= (others=>(others=>'0'));
		upload(0) <= X"FE";		-- start of frame
		upload(1) <= X"00";		-- rpm low byte
		upload(2) <= X"01";		-- rpm high byte
		upload(3) <= X"02";		-- mph low byte
		upload(4) <= X"03";		-- mph high byte
		upload(5) <= X"04";		-- brightness param
		upload(6) <= X"05";		-- oil pressure
		upload(7) <= X"06";		-- gas gauge
		upload(8) <= X"07";		-- MAP
		upload(9) <= X"08";		-- O2
		upload(10) <= X"09";	-- TPS
		upload(11) <= X"0A";	-- light sensor on dash
		upload(12) <= X"0B";	-- not used 
		upload(13) <= X"0C";	-- not used 
		rpm_factor <= std_logic_vector(conv_unsigned(RPM_DVND, DVSR_SIZE));
		mph_factor <= std_logic_vector(conv_unsigned(MPH_DVND, DVSR_SIZE));
		
	else if clk'event and clk = '1' then
		case pport_reg is
			when pp_preidle =>
				if time_delay_reg > TIME_DELAY7 then
 					time_delay_next <= (others=>'0');
					pport_next <= pp_idle;
				else
					time_delay_next <= time_delay_reg + 1;
				end if;
			
			when pp_idle =>
				PP_DATA0 <= upload(conv_integer(sPort))(0);
				PP_DATA1 <= upload(conv_integer(sPort))(1);
				PP_DATA2 <= upload(conv_integer(sPort))(2);
				PP_DATA3 <= upload(conv_integer(sPort))(3);
				PP_DATA4 <= upload(conv_integer(sPort))(4);
				PP_DATA5 <= upload(conv_integer(sPort))(5);
				PP_DATA6 <= upload(conv_integer(sPort))(6);
				PP_DATA7 <= upload(conv_integer(sPort))(7);
				led1 <= "0111";
				pport_next <= pp_start1;
			
 			when pp_start1 =>
				PP_CS <= '1';
				skip <= not skip;
				if skip = '1' then
					sPort <= sPort + 1;
					if sPort > 17 then
						sPort <= (others=>'0');
					end if;
				end if;
				pport_next <= pp_start2;

			when pp_start2 =>
				if time_delay_reg > TIME_DELAY8c then
--				if time_delay_reg > TIME_DELAY7 then
 					time_delay_next <= (others=>'0');
					PP_CS <= '0';
					led1 <= "1011";
					pport_next <= pp_done;
				else
					time_delay_next <= time_delay_reg + 1;
				end if;

			when pp_done =>
				if PP_ACK = '1' then
					pport_next <= pp_done2;
					led1 <= "1101";
				end if;

			when pp_done2 =>
				if time_delay_reg > send_delay then
--				if time_delay_reg > TIME_DELAY7 then
					time_delay_next <= (others=>'0');
					upload(1) <= rpm_result(7 downto 0);
					upload(2) <= rpm_result(15 downto 8);
					upload(3) <= mph_result(7 downto 0);
					upload(4) <= mph_result(15 downto 8);
					upload(5) <= mcp_results(0);
					upload(6) <= mcp_results(1);
					upload(7) <= mcp_results(2);
					upload(8) <= mcp_results(3);
					upload(9) <= mcp_results(4);
					upload(10) <= mcp_results(5);
					upload(11) <= mcp_results(6);
					upload(12) <= mcp_results(7);
--					upload(13) <= mcp_results(8);	-- these not used because compact = 1 for now
--					upload(14) <= mcp_results(9);	--  "
					upload(13) <= stlv_status1;
					upload(14) <= stlv_status2;
					upload(15) <= stlv_status3;
					upload(16) <= stlv_status4;
					led1 <= "1110";
					pport_next <= pp_idle;
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
		mcmd <= (others=>'0');
		start_calc <= '0';
		data_sent <= '0';
		inc_params <= (others=>'0');
		stlv_flag <= (others=>'0');
		download <= (others=>(others=>'0'));
	
	else if clk'event and clk = '1' then
		case state_uart_reg2 is
			when pre_idle =>
				state_uart_next2 <= idle2;
			
			when idle2 =>
				data_sent <= '0';
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
				if stlv_flag = X"FF" then
					start_calc <= '1';
				end if;
				state_uart_next2 <= done;

			when done =>
--				if start_calc = '1' then
					start_calc <= '0';
--					state_uart_next2 <= rx_delay;
--				end if;
				state_uart_next2 <= idle2;

			-- 40ms delay to allow sender finish sending mandatory 8 bytes
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
variable temp: integer range 0 to 16000000:= 16000000;
begin
	if reset = '0' then
		main_reg1 <= idle1a;
		main_next1 <= idle1a;
		stlv_duty_cycle <= (others=>'1');
		special <= '0';
		dtmf_index <= (others=>'0');
--		stdlv_transmit_update_rate <=  X"1FFFFF";
--		led1 <= "1111";
		start_pwm2 <= '0';
		key_len <= X"14";
		stlv_duty_cycle2 <= "111111";
		tune1 <= load_tune_array(tune1);
		adc_gate <= '0';
		send_delay <=  "00" & X"07FFFF";
		adc_delay  <=  "00" & X"07FFFF";
		dl_param <= (others=>'0');
		stlv_rev_limit_override <= '1';
		stlv_fuel_pump_override <= '1';
		high_rev_limit <= conv_unsigned(HREV_LIMIT_4200,26);
		low_rev_limit 	<= conv_unsigned(LREV_LIMIT_3700,26);
		stlv_status1 <= (others=>'0');
		stlv_status2 <= (others=>'0');
		stlv_status3 <= (others=>'0');
		stlv_status4 <= (others=>'0');
--		stlv_status4 <= X"AA";

	elsif clk'event and clk = '1' then
		case main_reg1 is
			when idle1a =>
				-- always set this bit so svr can tell if engine is running
				stlv_status3(3) <= stlv_engine_running;
--				reset_rev_limits <= '0';
				if start_calc = '1' then
--					led1 <= download(0)(3 downto 0);	-- first param is download(0)
--					led1 <= mcmd(3 downto 0);
					case mcmd is
						when SET_FPGA_SEND_UPDATE_RATE =>
							dl_param <= download(0)(3 downto 0);
							stlv_status1(7 downto 4) <= download(0)(3 downto 0);
							case dl_param is
								when "0000" =>
									send_delay <= conv_unsigned(TIME_DELAY3/8,26);
								when "0001" =>
									send_delay <= conv_unsigned(TIME_DELAY5/8,26);
								when "0010" =>
									send_delay <= conv_unsigned(TIME_DELAY5a/8,26);
								when "0011" =>
									send_delay <= conv_unsigned(TIME_DELAY6/8,26);
								when "0100" =>
									send_delay <= conv_unsigned(TIME_DELAY6a/8,26);
								when "0101" =>
									send_delay <= conv_unsigned(TIME_DELAY6b/8,26);
								when "0110" =>
									send_delay <= conv_unsigned(TIME_DELAY7/8,26);
								when "0111" =>
									send_delay <= conv_unsigned(TIME_DELAY8/8,26);
								when "1000" =>
									send_delay <= conv_unsigned(TIME_DELAY8a/8,26);
								when "1001" =>
									send_delay <= conv_unsigned(TIME_DELAY8b/8,26);
								when "1010" =>
									send_delay <= conv_unsigned(TIME_DELAY8c/8,26);
								when others =>	
									send_delay <= conv_unsigned(TIME_DELAY5/8,26);
							end case;
							main_next1 <= do_mcmd;
						when SET_RPM_MPH_UPDATE_RATE =>
							dl_param <= download(0)(3 downto 0);
							stlv_status1(3 downto 0) <= download(0)(3 downto 0);
							case dl_param is
								when "0000" =>
									rpm_mph_delay <= conv_unsigned(TIME_DELAY3/8,26);
								when "0001" =>
									rpm_mph_delay <= conv_unsigned(TIME_DELAY5/8,26);
								when "0010" =>
									rpm_mph_delay <= conv_unsigned(TIME_DELAY5a/8,26);
								when "0011" =>
									rpm_mph_delay <= conv_unsigned(TIME_DELAY6/8,26);
								when "0100" =>
									rpm_mph_delay <= conv_unsigned(TIME_DELAY6a/8,26);
								when "0101" =>
									rpm_mph_delay <= conv_unsigned(TIME_DELAY6b/8,26);
								when "0110" =>
									rpm_mph_delay <= conv_unsigned(TIME_DELAY7/8,26);
								when "0111" =>
									rpm_mph_delay <= conv_unsigned(TIME_DELAY8/8,26);
								when "1000" =>
									rpm_mph_delay <= conv_unsigned(TIME_DELAY8a/8,26);
								when "1001" =>
									rpm_mph_delay <= conv_unsigned(TIME_DELAY8b/8,26);
								when "1010" =>
									rpm_mph_delay <= conv_unsigned(TIME_DELAY8c/8,26);
								when others =>	
									rpm_mph_delay <= conv_unsigned(TIME_DELAY5,26);
							end case;
							main_next1 <= do_mcmd;
						when DTMF_TONE_ON =>
--							led1 <= "0111";
							dtmf_index <= download(0)(4 downto 0);
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
							stlv_duty_cycle <= download(0)(4 downto 0);
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
--							led1 <= "0111";
						when ADC_CTL =>
							adc_gate <= download(0)(0);
							stlv_status3(2) <= download(0)(0);
							main_next1 <= do_mcmd;
						when SET_ADC_RATE2 =>
							dl_param <= download(0)(3 downto 0);
							stlv_status3(7 downto 4) <= download(0)(3 downto 0);
							case dl_param is
								when "0000" =>
									adc_delay <= conv_unsigned(TIME_DELAY5,26);
								when "0001" =>
									adc_delay <= conv_unsigned(TIME_DELAY5a,26);
								when "0010" =>
									adc_delay <= conv_unsigned(TIME_DELAY6,26);
								when "0011" =>
									adc_delay <= conv_unsigned(TIME_DELAY6a,26);
								when "0100" =>
									adc_delay <= conv_unsigned(TIME_DELAY6b,26);
								when "0101" =>
									adc_delay <= conv_unsigned(TIME_DELAY7,26);
								when "0110" =>
									adc_delay <= conv_unsigned(TIME_DELAY8,26);
								when "0111" =>
									adc_delay <= conv_unsigned(TIME_DELAY8a,26);
								when "1000" =>
									adc_delay <= conv_unsigned(TIME_DELAY8b,26);
								when "1001" =>
									adc_delay <= conv_unsigned(TIME_DELAY8c,26);
								when "1010" =>
									adc_delay <= conv_unsigned(TIME_DELAY9,26);
								when others =>	
									adc_delay <= conv_unsigned(TIME_DELAY5,26);
							end case;
							main_next1 <= do_mcmd;
						when SET_HIGH_REV_LIMIT =>	
							dl_param <= download(0)(3 downto 0);
							stlv_status2(7 downto 4) <= download(0)(3 downto 0);
							case dl_param is
								when "0000" =>
									high_rev_limit <= conv_unsigned(HREV_LIMIT_6000,26);
								when "0001" =>
									high_rev_limit <= conv_unsigned(HREV_LIMIT_5800,26);
								when "0010" =>
									high_rev_limit <= conv_unsigned(HREV_LIMIT_5600,26);
								when "0011" =>
									high_rev_limit <= conv_unsigned(HREV_LIMIT_5400,26);
								when "0100" =>
									high_rev_limit <= conv_unsigned(HREV_LIMIT_5200,26);
								when "0101" =>
									high_rev_limit <= conv_unsigned(HREV_LIMIT_5000,26);
								when "0110" =>
									high_rev_limit <= conv_unsigned(HREV_LIMIT_4800,26);
								when "0111" =>
									high_rev_limit <= conv_unsigned(HREV_LIMIT_4600,26);
								when "1000" =>
									high_rev_limit <= conv_unsigned(HREV_LIMIT_4400,26);
								when "1001" =>
									high_rev_limit <= conv_unsigned(HREV_LIMIT_4200,26);
								when "1010" =>
									high_rev_limit <= conv_unsigned(HREV_LIMIT_4000,26);
								when others =>	
									high_rev_limit <= conv_unsigned(HREV_LIMIT_4400,26);
							end case;
							main_next1 <= do_mcmd;
						when SET_LOW_REV_LIMIT =>	
							dl_param <= download(0)(3 downto 0);
							stlv_status2(3 downto 0) <= download(0)(3 downto 0);
							case dl_param is
								when "0000" =>
									low_rev_limit <= conv_unsigned(LREV_LIMIT_4500,26);
								when "0001" =>
									low_rev_limit <= conv_unsigned(LREV_LIMIT_4400,26);
								when "0010" =>
									low_rev_limit <= conv_unsigned(LREV_LIMIT_4300,26);
								when "0011" =>
									low_rev_limit <= conv_unsigned(LREV_LIMIT_4200,26);
								when "0100" =>
									low_rev_limit <= conv_unsigned(LREV_LIMIT_4100,26);
								when "0101" =>
									low_rev_limit <= conv_unsigned(LREV_LIMIT_4000,26);
								when "0110" =>
									low_rev_limit <= conv_unsigned(LREV_LIMIT_3900,26);
								when "0111" =>
									low_rev_limit <= conv_unsigned(LREV_LIMIT_3800,26);
								when others =>	
									low_rev_limit <= conv_unsigned(LREV_LIMIT_3700,26);
							end case;
							main_next1 <= do_mcmd;
						when REV_LIMIT_OVERRIDE =>	
							stlv_rev_limit_override <= download(0)(0);
							stlv_status3(0) <= download(0)(0);
							main_next1 <= do_mcmd;
						when FUEL_PUMP_OVERRIDE =>	
							stlv_fuel_pump_override <= download(0)(0);
							stlv_status3(1) <= download(0)(0);
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
-- test the current rpm_result to see if has changed over the last 640ms or so
-- if not, it must be off so then set the rpm_result back to zero
rpm_off_unit: process(clk, reset, test_reg)
variable temp: integer range 0 to 40:= 40;
begin
	if reset = '0' then
		test_reg <= test_idle;
		test_next <= test_idle;
		time_delay_reg4 <= (others=>'0');
		time_delay_next4 <= (others=>'0');
		TS7800TX <= '0';
		monitorTX <= '0';

	else if clk'event and clk = '1' then
		case test_reg is
			when test_idle =>
--				if done_rpm = '1' then 
					
--				end if;
				test_next <= test_start1;

			when test_start1 =>
				if time_delay_reg4 > TIME_DELAY5 then
					time_delay_next4 <= (others=>'0');
					test_next <= test_start2;
--					test_next <= test_idle;
				else
					time_delay_next4 <= time_delay_reg4 + 1;
				end if;

			when test_start2 =>
				if time_delay_reg4 > TIME_DELAY7 then
					time_delay_next4 <= (others=>'0');
					test_next <= test_done;
				else
					time_delay_next4 <= time_delay_reg4 + 1;
				end if;

			when test_done =>
				test_next <= test_idle;

		end case;
		test_reg <= test_next;
		time_delay_reg4 <= time_delay_next4;
		end if;
	end if;
end process;

-- ********************************************************************************
rev_limit_unit: process(clk, reset, rev_reg)
begin
	if reset = '0' then
		rev_reg <= rev_idle;
		rev_next <= rev_idle;
		time_delay_reg6 <= (others=>'0');
		time_delay_next6 <= (others=>'0');
		urpm_result <= (others=>'0');
		rev_limit <= '0';	-- turn relay off to begin with
		fp_shutoff <= '1';	-- start off with fuel pump enabled
		stlv_engine_running <= '0';

	else if clk'event and clk = '1' then
		case rev_reg is
			when rev_idle =>
				if stlv_rev_limit_override = '1' then
					rev_limit <= '1';
					rev_next <= rev_idle;
				else
					urpm_result <= conv_unsigned(conv_integer(rpm_result),16);
					if urpm_result > high_rev_limit then
						rev_limit <= '0';
					elsif urpm_result < low_rev_limit then 
						rev_limit <= '1';
					end if;	
					rev_next <= rev_start1;
				end if;

			when rev_start1 =>
				if time_delay_reg6 > TIME_DELAY6 then
					time_delay_next6 <= (others=>'0');
					
					rev_next <= rev_start2;
--					test_next <= test_idle;
				else
					time_delay_next6 <= time_delay_reg6 + 1;
				end if;

			when rev_start2 =>
				if time_delay_reg6 > TIME_DELAY6 then
					time_delay_next6 <= (others=>'0');
					rev_next <= rev_done;
					if urpm_result > TIME_DELAY_500RPM then
						stlv_engine_running <= '1';
					else 
						stlv_engine_running <= '0';
					end if;
				else
					time_delay_next6 <= time_delay_reg6 + 1;
				end if;

			when rev_done =>
				rev_next <= rev_idle;

		end case;
		rev_reg <= rev_next;
		time_delay_reg6 <= time_delay_next6;
		end if;
	end if;
end process;

-- ********************************************************************************
mcp_unit: process(clk, reset, test_reg)
begin
	if reset = '0' then
		mcp_reg <= mcp_idle;
		mcp_next <= mcp_idle;
		time_delay_reg5 <= (others=>'0');
		time_delay_next5 <= (others=>'0');
		start_mcp <= '0';
		compact <= '1';
--		led1 <= "1111";

	else if clk'event and clk = '1' then
		case mcp_reg is
			when mcp_idle =>
				if time_delay_reg5 > adc_delay then
					time_delay_next5 <= (others=>'0');
					if adc_gate = '1' then
						start_mcp <= '1';
						mcp_next <= mcp_start1;
					else
						mcp_next <= mcp_idle;
					end if;	
--					led1 <= "0111";
				else
					time_delay_next5 <= time_delay_reg5 + 1;
				end if;

			when mcp_start1 =>
				start_mcp <= '0';
				if done_mcp = '1' then
					mcp_next <= mcp_start2;
--					led1 <= "1011";
				end if;

			when mcp_start2 =>
				if time_delay_reg5 > adc_delay then
					time_delay_next5 <= (others=>'0');
					mcp_next <= mcp_done;
--					led1 <= "1101";
				else
					time_delay_next5 <= time_delay_reg5 + 1;
				end if;

			when mcp_done =>
				mcp_next <= mcp_idle;

		end case;
		mcp_reg <= mcp_next;
		time_delay_reg5 <= time_delay_next5;
		end if;
	end if;
end process;

end truck_arch;