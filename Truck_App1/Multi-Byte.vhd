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
		pwm_signal: out std_logic;
		rpm_signal : in std_logic;
		mph_signal : in std_logic;
		pwm_spk1 : out std_logic;
		pwm_spk2 : out std_logic;
		pwm_lcd : out std_logic;
		MOSI_o : out std_logic;
		MISO_i : in std_logic;
		SCLK_o : out std_logic;
		SS_o : out std_logic_vector(1 downto 0);
		led1: out std_logic_vector(3 downto 0)
		);
end multi_byte;

architecture truck_arch of multi_byte is

	-- send_uart1
	type state_uart1 is (idle10, idle1, start, start_a, start_b, start_c, start_d, delay);
	signal state_tx1_reg, state_tx1_next: state_uart1;

	-- send_uart2
	-- type state_uart3 is (idle3, start3, delay3);
	-- signal state_tx2_reg, state_tx2_next: state_uart3;
	
	-- recv_uart1
	type state_uart2 is (idle2, start2, delay2);
--	type state_uart2 is (idle2, idle2b, idle2c, start2, delay2, delay3);
	signal state_uart_reg2, state_uart_next2: state_uart2;

	-- recv_uart2
	-- type state_uart4 is (idle4, start4, delay4);
-- --	type state_uart2 is (idle2, idle2b, idle2c, start2, delay2, delay3);
	-- signal state_rx2_reg, state_rx2_next: state_uart4;

	type calc_type is (idle0, idle1_a, calc3, calc4, done, donex);
	signal cstate_reg, cstate_next: calc_type;

	type calc_type2 is (idle1a, spin, pre_mcmd, exec_mcmd, exec_other_mcmds, wait_mcmd, wait_mcmd2);
	signal main_reg1, main_next1: calc_type2;

	type state_dout is (idle_dout, start_dout, done_dout, wait_dout);
	signal state_dout_reg, state_dout_next: state_dout;

	type state_pwm is (pwm_idle, pwm_start, pwm_next1, pwm_next2, pwm_next3, pwm_done);
	signal drive_pwm_reg, drive_pwm_next: state_pwm;
	
	signal time_delay_reg, time_delay_next: unsigned(23 downto 0);		-- send_uart1
--	signal time_delay_reg2, time_delay_next2: unsigned(23 downto 0);	-- calc_proc
	signal time_delay_reg3, time_delay_next3: unsigned(23 downto 0);	-- calc_proc2
--	signal time_delay_reg4, time_delay_next4: unsigned(23 downto 0);
--	signal time_delay_reg5, time_delay_next5: unsigned(23 downto 0);
--	signal time_delay_reg6, time_delay_next6: unsigned(23 downto 0);
	signal time_delay_reg7, time_delay_next7: unsigned(23 downto 0);
	signal time_delay_reg8, time_delay_next8: unsigned(24 downto 0);

	--	signal tempx: std_logic_vector(7 downto 0);
--	signal temp3_int: std_logic_vector(7 downto 0);

--	signal stdlv_rpm_update_rate: std_logic_vector(23 downto 0);
--	signal rpm_update_rate: unsigned(23 downto 0);
--	signal stdlv_mph_update_rate: std_logic_vector(23 downto 0);
--	signal mph_update_rate: unsigned(23 downto 0);
	signal rpm_db, mph_db: std_logic;
	signal cmd_rpm: std_logic_vector(4 downto 0):= (others=>'0');
	signal cmd_mph: std_logic_vector(4 downto 0):= (others=>'0');
	signal param_mph: std_logic_vector(7 downto 0):= (others=>'0');
	signal param_rpm: std_logic_vector(7 downto 0):= (others=>'0');
--	signal inc_frame: unsigned(3 downto 0);
--	signal skip: std_logic;
	signal skip2: std_logic;
	signal skip3: std_logic;
	signal skip4: std_logic;
--	signal rpm_temp, mph_temp: std_logic;
	signal rpm_result: std_logic_vector(16 downto 0);
	signal mph_result: std_logic_vector(16 downto 0);
	signal mph_factor: std_logic_vector(5 downto 0):= (others=>'0');
	signal rpm_factor: std_logic_vector(5 downto 0):= (others=>'0');
--	signal map_result, o2_result, airt_result: std_logic_vector(7 downto 0);
--	signal trip_result, engt_result, oilp_result, oilt_result: std_logic_vector(7 downto 0);
--	signal coolant_result, baro_result: std_logic_vector(7 downto 0);
--	signal other3_result, other4_result, other5_result: std_logic_vector(7 downto 0);
--	signal time_delay_mph: std_logic_vector(25 downto 0):= "00" & X"00FFFF";
--	signal time_delay_rpm: std_logic_vector(25 downto 0):= "00" & X"00FFFF";
	signal time_delay_mph: std_logic_vector(25 downto 0);
	signal time_delay_rpm: std_logic_vector(25 downto 0);
	signal stdlv_transmit_update_rate: std_logic_vector(23 downto 0):= X"00FFFF";
--	signal transmit_update_rate: unsigned(24 downto 0):=         '0' & X"0FFFFF"; 
	signal bcda, bcdb, bcdc: std_logic_vector(15 downto 0);
--	signal temp2: std_logic_vector(3 downto 0);
--	signal temp3: std_logic_vector(7 downto 0);
--	signal cmd, param: std_logic_vector(3 downto 0);
--	signal send_cmd1, send_cmd2: std_logic;

	signal start_tx, done_tx, start_rx, done_rx: std_logic;
	signal data_tx, data_rx: std_logic_vector(7 downto 0);

--	signal start_tx2, done_tx2, start_rx2, done_rx2: std_logic;
--	signal data_tx2, data_rx2: std_logic_vector(7 downto 0);
	signal mph_done, rpm_done: std_logic;
	signal mph_done2, rpm_done2: std_logic;

--	signal spi_din, spi_dout : std_logic_vector(7 downto 0);
--	signal s_cnt : unsigned(7 downto 0);
	signal stlv_temp1 : std_logic_vector(7 downto 0);
	signal stlv_temp1a : std_logic_vector(7 downto 0);
	signal stlv_temp2 : std_logic_vector(7 downto 0);
--	signal stlv_temp2a : std_logic_vector(7 downto 0);
--	signal trigger_send_spi : std_logic;
	signal addr: std_logic_vector(1 downto 0);
	signal mph_signal_test: std_logic;
	signal rpm_signal_test: std_logic;
	signal start_calc_proc: std_logic;
	signal start_mph_wrapper: std_logic;
	signal start_rpm_wrapper: std_logic;
	signal mcmd: std_logic_vector(4 downto 0);
	signal mparam: std_logic_vector(6 downto 0);
	signal mph_or_not: std_logic;
	signal low_byte, high_byte: std_logic_vector(7 downto 0);
	signal mspi_ready : std_logic;
	signal mspi_din_vld, mspi_dout_vld : std_logic;
	signal mosi, miso, sclk, ss: std_logic;
	signal mspi_din, mspi_dout : std_logic_vector(7 downto 0);
	signal start_dtmf: std_logic;
	signal dtmf_done1: std_logic;
	signal key_index: signed(7 downto 0);
	signal key_len: signed(7 downto 0);
	signal mykey: std_logic_vector(7 downto 0);
	signal ikeys: i_keys;
	signal stlv_duty_cycle: std_logic_vector(2 downto 0);
	signal note_len: std_logic_vector(3 downto 0);
	signal dtmf_index: std_logic_vector(4 downto 0);
	
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
	bcd_o=>bcda,
	result=>mph_result,
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
	bcd_o=>bcdb,
	result=>rpm_result,
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
--		sw=>rpm_signal_test,	-- this is from calc_proc and used to simulate a signal
		db=>rpm_db);

db1_unit: entity work.db_fsm
	port map(clk=>clk,reset=>reset,
		sw=>mph_signal,
--		sw=>mph_signal_test,
		db=>mph_db);

spi_master_unit: entity work.SPI_MASTER(RTL)
	generic map(CLK_FREQ=>50000000,SCLK_FREQ=>100000,SLAVE_COUNT=>2)
	port map(CLK=>clk, RST=>reset,
	SCLK=>SCLK_o,
	CS_N=>SS_o,
	MOSI=>MOSI_o,
	MISO=>MISO_i,
	ADDR=>addr,
	READY=>mspi_ready,
	DIN=>mspi_din,
	DIN_VLD=>mspi_din_vld,
	DOUT=>mspi_dout,
	DOUT_VLD=>mspi_dout_vld);

--lcd_pwm_unit: entity work.lcd_pwm
--	port map(clk=>clk, reset=>reset,
	
dtmf_unit: entity work.dtmf
	port map(clk=>clk,reset=>reset,
		dtmf_index=>dtmf_index,
		dtmf_signal_1=>pwm_spk1,
		dtmf_signal_2=>pwm_spk2,
		start=>start_dtmf,
		len=>note_len,
		done=>dtmf_done1,
		duty_cycle=>stlv_duty_cycle);
	
-- test_dtmf: process(clk, reset, drive_pwm_reg)
-- begin
	-- if reset = '0' then
		-- drive_pwm_reg <= pwm_idle;
		-- drive_pwm_next <= pwm_idle;
		-- start_dtmf <= '0';
		-- time_delay_reg8 <= (others=>'0');
		-- time_delay_next8 <= (others=>'0');
		-- pwm_lcd <= '0';
-- --		mykey <= "000";
		-- key_len <= X"29";
		-- dtmf_index <= (others=>'0');
		-- key_index <= (others=>'0');
		-- skip4 <= '0';
		-- stlv_duty_cycle <= "000";
-- --		note_len <= X"7";
		
	-- else if clk'event and clk = '1' then
		-- case drive_pwm_reg is
			-- when pwm_idle =>
				-- if time_delay_reg8 > TIME_DELAY5a then
					-- time_delay_next8 <= (others=>'0');
-- --					drive_pwm_next <= pwm_start;
					-- drive_pwm_next <= pwm_idle;
				-- else
					-- time_delay_next8 <= time_delay_reg8 + 1;
				-- end if;
			-- when pwm_start =>
				
				-- skip4 <= not skip4;
				-- if skip4 = '1' then
					-- if key_index > 10 then
						-- key_index <= (others=>'0');
					-- else 
						-- case dtmf_index is
							-- when "00000" => dtmf_index <= "00001";
							-- when "00001" => dtmf_index <= "00010";
							-- when "00010" => dtmf_index <= "00011";
							-- when "00011" => dtmf_index <= "00100";
							-- when "00100" => dtmf_index <= "00101";
							-- when "00101" => dtmf_index <= "00110";
							-- when "00110" => dtmf_index <= "00111";
							-- when "00111" => dtmf_index <= "01000";
							-- when "01000" => dtmf_index <= "01001";
							
							-- -- when "01001" => dtmf_index <= "01010";
							-- -- when "01010" => dtmf_index <= "01011";
							-- -- when "01011" => dtmf_index <= "01100";
							-- -- when "01100" => dtmf_index <= "01101";
							-- -- when "01101" => dtmf_index <= "01110";
							-- -- when "01110" => dtmf_index <= "01111";
							-- when others => dtmf_index <= "00000";
						-- end case;
						-- case stlv_duty_cycle is
							-- when "000" => stlv_duty_cycle <= "001";
							-- when "001" => stlv_duty_cycle <= "010";
							-- when "010" => stlv_duty_cycle <= "011";
							-- when "011" => stlv_duty_cycle <= "100";
							-- when "100" => stlv_duty_cycle <= "101";
							-- when "101" => stlv_duty_cycle <= "000";
							-- when others => stlv_duty_cycle <= "000";
						-- end case;
						-- key_index <= key_index + 1;
					-- end if;
				-- end if;
				-- start_dtmf <= '1';
				-- drive_pwm_next <= pwm_next1;
			-- when pwm_next1 =>
				-- start_dtmf <= '0';
				-- drive_pwm_next <= pwm_next2;
			-- when pwm_next2 =>
				-- if dtmf_done1 = '1' then
					-- drive_pwm_next <= pwm_idle;
				-- end if;
				-- -- if time_delay_reg8 > TIME_DELAY3 then
					-- -- time_delay_next8 <= (others=>'0');
					-- -- drive_pwm_next <= pwm_idle;
				-- -- else
					-- -- time_delay_next8 <= time_delay_reg8 + 1;
				-- -- end if;
			-- when pwm_next3 =>
				-- if time_delay_reg8 > TIME_DELAY3 then
					-- time_delay_next8 <= (others=>'0');
					-- drive_pwm_next <= pwm_idle;
				-- else
					-- time_delay_next8 <= time_delay_reg8 + 1;
				-- end if;
			-- when pwm_done =>
				-- if time_delay_reg8 > TIME_DELAY3 then
					-- time_delay_next8 <= (others=>'0');
					-- drive_pwm_next <= pwm_start;
-- --					drive_pwm_next <= pwm_done;
				-- else
					-- time_delay_next8 <= time_delay_reg8 + 1;
				-- end if;
		-- end case;
		-- time_delay_reg8 <= time_delay_next8;
		-- drive_pwm_reg <= drive_pwm_next;
		-- end if;
	-- end if;
-- end process;
	
-- ********************************************************************************
--	type state_dout is (idle_dout, start_dout, end_dout, done_dout, delay_dout);

echo_dout_unit1: process(clk, reset, state_dout_reg)
variable temp_uart: integer range 0 to NUM_DATA_ARRAY-1:= 0;
variable temp1: integer range 0 to 255:= 0;
variable temp2: integer range 0 to 255:= 255;
variable temp3: integer range 0 to 7:= 1;
begin
	if reset = '0' then
		state_dout_reg <= idle_dout;
		stlv_temp1 <= (others=>'0');
		stlv_temp1a <= (others=>'0');
--		led1 <= (others=>'1');
		addr <= (others=>'1');
		mspi_din_vld <= '0';
		mspi_din <= (others=>'0');
		skip3 <= '0';
		time_delay_reg7 <= (others=>'0');
		time_delay_next7 <= (others=>'0');
	else if clk'event and clk = '1' then
		case state_dout_reg is
			when idle_dout =>
--				led1 <= "1110";
				if mspi_ready = '1' then
--					led1 <= "0111";

					mspi_din <= stlv_temp1;		-- write
					mspi_din_vld <= '1';
					state_dout_next <= start_dout;
				end if;
			when start_dout =>
					state_dout_next <= done_dout;
			when done_dout =>
				mspi_din_vld <= '0';			
--				if SS_o(0) = '0' then
				if mspi_dout_vld = '1' then
--					led1 <= "1011";
-- mspi_dout is what gets received by MISO
--					if addr(0) = '1' then
						stlv_temp1 <= mspi_dout;
--					elsif addr(1) = '1' then	
--						stlv_temp1a <= mspi_dout;
--					end if;	
--					cmd <= stlv_temp1(7 downto 4);
--					param <= stlv_temp1(3 downto 0);
--					temp_uart := conv_integer(param);
--					if cmd(3) = '0' then
--						if temp_uart > NUM_DATA_ARRAY-1 then
--							temp_uart := 0;
--						end if;	
--						stlv_temp1 <= data_array1(temp_uart);
--					else
--						if temp_uart > 7 then
--							temp_uart := 0;
--						end if;	
--						stlv_temp1 <= "0000" & data_array2(temp_uart);
--					end if;	
					skip3 <= not skip3;
					if skip3 = '1' then
						if temp1 > 125 then
							temp1:= 36;
						else temp1:= temp1 + 1;
						end if;	
						stlv_temp1 <= conv_std_logic_vector(temp1,8);
--						stlv_temp1 <= X"AA";
--						led1 <= "1011";
						addr <= "01";					
					else	
						if temp2 < 33 then
							temp2:= 125;
						else temp2:= temp2 - 1;	
						end if;	
						stlv_temp1 <= conv_std_logic_vector(temp2,8);
--						stlv_temp1 <= X"55";
						addr <= "10";
					end if;
					state_dout_next <= wait_dout;
				end if;
			when wait_dout =>
				if time_delay_reg7 > TIME_DELAY9/8 then
					time_delay_next7 <= (others=>'0');
					state_dout_next <= idle_dout;
				else
					time_delay_next7 <= time_delay_reg7 + 1;
				end if;
--				end if;
		end case;
		time_delay_reg7 <= time_delay_next7;
		state_dout_reg <= state_dout_next;
		end if;
	end if;
end process;	

-- ********************************************************************************
send_uart1: process(clk,reset)
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
		skip2 <= '0';
	else if clk'event and clk = '1' then
		case state_tx1_reg is
			when idle10 =>
				start_tx <= '0';
 				if time_delay_reg > TIME_DELAY8c then	-- this should be a settable param (update_rate)
					time_delay_next <= (others=>'0');
					state_tx1_next <= idle1;
				else
					time_delay_next <= time_delay_reg + 1;
				end if;	
			when idle1 =>
				data_tx <= conv_std_logic_vector(temp_uart,8);
				skip2 <= not skip2;
				if skip2 = '1' then
					if temp_uart < 33 then
						temp_uart:= 125;
					else
						temp_uart:= temp_uart - 1;
					end if;	
				end if;
				data_tx <= bcdb(7 downto 0);
				state_tx1_next <= idle10;
				start_tx <= '1';
				-- low_byte <= rpm_result(7 downto 0);
				-- high_byte <= rpm_result(15 downto 8);
				-- data_tx <= X"FF";
				-- start_tx <= '1';
				-- state_tx1_next <= start;
			when start =>
				start_tx <= '0';
					if done_tx = '1' then
						data_tx <= bcdc(15 downto 8);
						start_tx <= '1';
						state_tx1_next <= start_a;
					end if;
			when start_a =>
				start_tx <= '0';
				if done_tx = '1' then
					data_tx <= bcdc(7 downto 0);
					start_tx <= '1';
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
					state_tx1_next <= start_d;
				end if;
			when start_d =>	
				data_tx <= low_byte;
				start_tx <= '1';
				state_tx1_next <= delay;
			when delay =>
				start_tx <= '0';
				if time_delay_reg > unsigned(stdlv_transmit_update_rate) - 1 then
 				-- if time_delay_reg > TIME_DELAY5 then	-- this should be a settable param (update_rate)
					time_delay_next <= (others=>'0');
					bcdc <= bcdb;
					state_tx1_next <= idle1;
				else
					time_delay_next <= time_delay_reg + 1;
				end if;	
--				state_tx1_next <= idle1;
		end case;
		time_delay_reg <= time_delay_next;
		state_tx1_reg <= state_tx1_next;
		end if;
	end if;
end process;

-- ********************************************************************************
recv_uart1: process(clk,reset,state_uart_reg2)
begin
	if reset = '0' then
		state_uart_reg2 <= idle2;
		start_rx <= '1';
--		time_delay_reg4 <= (others=>'0');
--		time_delay_next4 <= (others=>'0');
		stlv_temp2 <= (others=>'1');
--		led1 <= "1111";
	else if clk'event and clk = '1' then
		case state_uart_reg2 is
			when idle2 =>
				if done_rx = '1' then
					start_rx <= '0';
					stlv_temp2 <= data_rx;
					state_uart_next2 <= start2;
				end if;
			when start2 =>
				start_rx <= '1';
--				led1 <= stlv_temp2(3 downto 0);
				state_uart_next2 <= delay2;
			when delay2 =>
				stlv_temp2 <= X"FF";
				state_uart_next2 <= idle2;

			-- when idle2 =>
				-- if time_delay_reg4 > TIME_DELAY8 then
-- --				if time_delay_reg4 > TIME_DELAY3 then
					-- time_delay_next4 <= (others=>'0');
					-- stlv_temp2 <= X"01";	-- simulate sending the param
-- --					stlv_temp2 <= X"ff";	-- simulate sending the param
					-- state_uart_next2 <= idle2b;
-- --					state_uart_next2 <= idle2;
				-- else
					-- time_delay_next4 <= time_delay_reg4 + 1;
				-- end if;	
			-- when idle2b =>
				-- if time_delay_reg4 > TIME_DELAY9 then
					-- time_delay_next4 <= (others=>'0');
					-- stlv_temp2 <= X"ff";	-- simulate turning off the recv uart
					-- state_uart_next2 <= idle2c;
				-- else
					-- time_delay_next4 <= time_delay_reg4 + 1;
				-- end if;	
			-- when idle2c =>
				-- if time_delay_reg4 > TIME_DELAY8b then
					-- time_delay_next4 <= (others=>'0');
					-- state_uart_next2 <= start2;
					-- stlv_temp2 <= X"83";
				-- else
					-- time_delay_next4 <= time_delay_reg4 + 1;
				-- end if;	
			-- when start2 =>
				-- if time_delay_reg4 > TIME_DELAY9 then
					-- time_delay_next4 <= (others=>'0');
-- --					state_uart_next2 <= delay2;
					-- state_uart_next2 <= delay2;
					-- stlv_temp2 <= X"ff";
				-- else
					-- time_delay_next4 <= time_delay_reg4 + 1;
				-- end if;	
			-- when delay2 =>
				-- if time_delay_reg4 > TIME_DELAY8 + TIME_DELAY9 then
-- --				if time_delay_reg4 > TIME_DELAY8 then
					-- time_delay_next4 <= (others=>'0');
					-- state_uart_next2 <= delay3;
					-- stlv_temp2 <= X"82";
				-- else
					-- time_delay_next4 <= time_delay_reg4 + 1;
				-- end if;	
			-- when delay3 =>
				-- if time_delay_reg4 > TIME_DELAY9 then
					-- time_delay_next4 <= (others=>'0');
-- --					state_uart_next2 <= idle2;
					-- state_uart_next2 <= delay3;
					-- stlv_temp2 <= X"ff";
				-- else
					-- time_delay_next4 <= time_delay_reg4 + 1;
				-- end if;	
		end case;
--		time_delay_reg4 <= time_delay_next4;
		state_uart_reg2 <= state_uart_next2;
		end if;
	end if;
end process;

-- ********************************************************************************
-- recv_uart2: process(clk,reset)
-- begin
	-- if reset = '0' then
		-- state_rx2_reg <= idle4;
		-- start_rx2 <= '1';
		-- time_delay_reg6 <= (others=>'0');
		-- time_delay_next6 <= (others=>'0');
		-- stlv_temp2a <= (others=>'1');
	-- else if clk'event and clk = '1' then
		-- case state_rx2_reg is
			-- when idle4 =>
				-- if done_rx = '1' then
					-- start_rx2 <= '0';
					-- stlv_temp2a <= data_rx;
					-- state_rx2_next <= start4;
				-- end if;
			-- when start4 =>
				-- state_rx2_next <= delay4;
			-- when delay4 =>
				-- start_rx2 <= '1';
				-- if time_delay_reg6 > TIME_DELAY9 then
					-- time_delay_next6 <= (others=>'0');
					-- state_rx2_next <= idle4;
					-- stlv_temp2a <= X"FF";
				-- else
					-- time_delay_next6 <= time_delay_reg6 + 1;
				-- end if;	
		-- end case;
		-- time_delay_reg6 <= time_delay_next6;
		-- state_rx2_reg <= state_rx2_next;
		-- end if;
	-- end if;
-- end process;

-- ********************************************************************************
calc_proc2: process(clk,reset,main_reg1)
begin
	if reset = '0' then
		main_reg1 <= idle1a;
		main_next1 <= idle1a;
		time_delay_reg3 <= (others=>'0');
		time_delay_next3 <= (others=>'0');
		cmd_mph <= SEND_CHAR_CMD;
		cmd_rpm <= SEND_CHAR_CMD;
--		cmd_mph <= SEND_CHAR_CMD;
		param_mph <= (others=>'1');
		param_rpm <= (others=>'1');
		start_calc_proc <= '0';
		time_delay_mph <= "00" & X"FFFFFF";
		time_delay_rpm <= "00" & X"7FFFFF";
--		time_delay_rpm <= "00" & X"3FFFFF";		-- 184ms
--		time_delay_rpm <= "00" & X"1FFFFF";		-- 92ms
--		time_delay_rpm <= "00" & X"0FFFFF";		-- 46ms
--		time_delay_rpm <= "00" & X"07FFFF";		-- 23ms
--		time_delay_rpm <= "00" & X"03FFFF";		-- 11ms
		led1 <= "1111";
		start_rpm_wrapper <= '0';
		start_mph_wrapper <= '0';
		mph_signal_test <= '0';
		mparam <= (others=>'1');
		mcmd <= (others=>'1');
		mph_or_not <= '0';
		note_len <= X"7";
		start_dtmf <= '0';
		stlv_duty_cycle <= (others=>'0');
		pwm_lcd <= '0';
--		stlv_temp2 <= (others=>'1');	
	elsif clk'event and clk = '1' then
		case main_reg1 is
			when idle1a =>
				if time_delay_reg3 > TIME_DELAY9/2 then
					time_delay_next3 <= (others=>'0');
--					main_next1 <= idle1a;
					start_rpm_wrapper <= '1';
					cmd_rpm <= SEND_CHAR_CMD;
					main_next1 <= spin;
				else
					time_delay_next3 <= time_delay_reg3 + 1;
				end if;					
			when spin =>
				if stlv_temp2 /= X"FF" then
					if stlv_temp2(7) = '1' then		-- if high bit set, its a command
						mph_or_not <= stlv_temp2(6);
						mcmd <= stlv_temp2(4 downto 0);
						if mcmd = SET_BRIGHTNESS_CMD
								or mcmd = SET_CDECIMAL_CMD or
										mcmd = SEND_CHAR_CMD then
							if mph_or_not = '0' then
								start_rpm_wrapper <= '0';
							else
								start_mph_wrapper <= '0';
							end if;
						end if;	
						main_next1 <= pre_mcmd;
					else 
						mparam <= stlv_temp2(6 downto 0);		-- else it's a param
					end if;
				end if;
			when pre_mcmd =>
--				if rpm_done = '1' then
					main_next1 <= exec_mcmd;
--				end if;
			when exec_mcmd =>
				if (rpm_done = '1' and mph_or_not = '0') or (mph_done = '1' and mph_or_not = '1') then
					case mcmd is
						when SEND_CHAR_CMD =>
							if mph_or_not = '0' then
								cmd_rpm <= SEND_CHAR_CMD;
							else cmd_mph <= SEND_CHAR_CMD;
							end if;
						when SET_BRIGHTNESS_CMD =>
							if mph_or_not = '0' then
								cmd_rpm <= SET_BRIGHTNESS_CMD;
							else cmd_mph <= SET_BRIGHTNESS_CMD;
							end if;
							param_rpm <= mparam & "1";
	--						led1 <= param_rpm(3 downto 0);
						when SET_CDECIMAL_CMD =>
							if mph_or_not = '0' then
								cmd_rpm <= SET_CDECIMAL_CMD;
							else cmd_mph <= SET_CDECIMAL_CMD;
							end if;
							param_rpm <= "000000" & mparam(1 downto 0);
	--						led1 <= param_rpm(3 downto 0);
	--						led1 <= stlv_temp2(3 downto 0);
						when others =>
					end case;
					main_next1 <= exec_other_mcmds;
				end if;
			when exec_other_mcmds =>
				case mcmd is
					when SET_UPDATE_RATE_CMD =>
						if mph_or_not = '0' then
							time_delay_rpm <= mparam & "111" & X"FFFF";
						else time_delay_mph <= mparam & "111" & X"FFFF";
						end if;
--						led1 <= stlv_temp2(3 downto 0);
					when SET_FACTOR_CMD =>	
						if mph_or_not = '0' then
							rpm_factor <= mparam(5 downto 0);
						else mph_factor <= mparam(5 downto 0);
						end if;
					when SET_DISPLAY_UPDATE_RATE =>	
						if mph_or_not = '0' then
							stdlv_transmit_update_rate <= mparam & "1" & X"FFFF";
						else stdlv_transmit_update_rate <= mparam & "1" & X"FFFF";
						end if;
					when DTMF_TONE_ON =>
							dtmf_index <= '0' & mparam(3 downto 0);
							note_len <= '0' & mparam(6 downto 4);
							start_dtmf <= '1';
							led1 <= dtmf_index(3 downto 0);
--							led1 <= mparam(6 downto 3);
					when DTMF_TONE_OFF =>
							start_dtmf <= '0';
--							led1 <= "0011";
							led1 <= mparam(3 downto 0);
					when others =>	
					end case;
				main_next1 <= wait_mcmd;	
			when wait_mcmd =>
				if time_delay_reg3 > TIME_DELAY9/2 then
					time_delay_next3 <= (others=>'0');
					mcmd <= (others=>'1');
					mparam <= (others=>'1');
					start_rpm_wrapper <= '1';
					start_mph_wrapper <= '1';
					main_next1 <= wait_mcmd2;
				else
					time_delay_next3 <= time_delay_reg3 + 1;
				end if;
			when wait_mcmd2 =>
				if time_delay_reg3 > TIME_DELAY9/2 then
					time_delay_next3 <= (others=>'0');
					cmd_mph <= SEND_CHAR_CMD;
					cmd_rpm <= SEND_CHAR_CMD;
					main_next1 <= spin;
				else
					time_delay_next3 <= time_delay_reg3 + 1;
				end if;
		end case;
		main_reg1 <= main_next1;
		time_delay_reg3 <= time_delay_next3;
	end if;
end process;		

end truck_arch;
