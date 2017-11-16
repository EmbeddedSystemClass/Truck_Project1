-- test_uart.vhd - copied from Truck_App1.vhd with just the uart stuff
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_signed.all;
--use IEEE.MATH_REAL.ALL;

library XESS;
use XESS.CommonPckg.all;
--use XESS.DelayPckg.all;
--use XESS.PulsePckg.all;

entity Truck_App is
	port(
		clk, reset: in std_logic;
		tx_uart: out std_logic;
		rx_uart: in std_logic;
		tx_uart2: out std_logic;
		rx_uart2: in std_logic;
		tx_rpm: out std_logic;
		tx_mph: out std_logic;
		rpm_signal : in std_logic;
		mph_signal : in std_logic;

		MOSI_o : out std_logic;
		MISO_i : in std_logic;
		SCLK_o : out std_logic;
		SS_o : out std_logic_vector(1 downto 0);
--		SS_o : out std_logic;
--		MOSI_i : in std_logic;
--		MISO_o : out std_logic;
--		SCLK_i : in std_logic;
--		SS_i : in std_logic;
		test : out std_logic_vector(3 downto 0);
		led1: out std_logic_vector(3 downto 0)
		);
end Truck_App;

architecture truck_arch of Truck_App is

	type state_uart1 is (idle1, start, delay);
	signal state_uart_reg1, state_uart_next1: state_uart1;
--	signal t_reg_uart1, t_next_uart1: unsigned(23 downto 0);
	
	type state_uart2 is (idle2, start2, delay2);
	signal state_uart_reg2, state_uart_next2: state_uart2;
--	signal t_reg_uart2, t_next_uart2: unsigned(23 downto 0);

	type type_state_get_rpm is (idle_r, start_r, done_r);
	signal state_reg_rpm, state_next_rpm: type_state_get_rpm;

	type type_state_get_mph is (idle_m, start_m, done_m);
	signal state_reg_mph, state_next_mph: type_state_get_mph;

	type type_state_get_spi is (idle_spi, mstart_in_spi, mstart_out_spi, sstart_in_spi);
	signal state_spi, state_spi_next: type_state_get_spi;

	type state_dout is (idle_dout, start_dout, done_dout);
	signal state_dout_reg, state_dout_next: state_dout;

	type state_dout2 is (idle_dout2, start_dout2, done_dout2);
	signal state_dout_reg2, state_dout_next2: state_dout2;

--	signal tempx: std_logic_vector(7 downto 0);
	signal temp3_int: std_logic_vector(7 downto 0);

	signal stdlv_rpm_update_rate: std_logic_vector(23 downto 0);
	signal rpm_update_rate: unsigned(23 downto 0);
	signal stdlv_mph_update_rate: std_logic_vector(23 downto 0);
	signal mph_update_rate: unsigned(23 downto 0);
	signal rpm_db, mph_db: std_logic;
--	signal cmd_rpm: std_logic_vector(2 downto 0):= (others=>'0');
--	signal cmd_mph: std_logic_vector(2 downto 0):= (others=>'0');
	signal param_mph: std_logic_vector(7 downto 0):= (others=>'0');
	signal param_rpm: std_logic_vector(7 downto 0):= (others=>'0');
	signal temp_param: std_logic_vector(7 downto 0):= (others=>'0');
	signal inc_frame: unsigned(3 downto 0);
	signal skip: std_logic;
	signal skip2: std_logic;
	signal skip3: std_logic;
	signal rpm_temp, mph_temp: std_logic;
	signal rpm_result: std_logic_vector(12 downto 0);
	signal mph_result: std_logic_vector(12 downto 0);
	signal mph_factor: std_logic_vector(5 downto 0):= (others=>'0');
	signal rpm_factor: std_logic_vector(5 downto 0):= (others=>'0');
--	signal map_result, o2_result, airt_result: std_logic_vector(7 downto 0);
--	signal trip_result, engt_result, oilp_result, oilt_result: std_logic_vector(7 downto 0);
--	signal coolant_result, baro_result: std_logic_vector(7 downto 0);
--	signal other3_result, other4_result, other5_result: std_logic_vector(7 downto 0);
	signal time_delay_mph: std_logic_vector(23 downto 0):= X"00FFFF";
	signal time_delay_rpm: std_logic_vector(23 downto 0):= X"00FFFF";
	signal temp1: unsigned(22 downto 0):= (others=>'0');
	signal stdlv_transmit_update_rate: std_logic_vector(24 downto 0):= '0' & X"0FFFFF";
	signal transmit_update_rate: unsigned(24 downto 0):= '0' & X"0FFFFF"; 
	signal bcda0_o, bcda1_o, bcda2_o, bcda3_o, bcdb0_o, bcdb1_o, bcdb2_o, bcdb3_o: std_logic_vector(3 downto 0);
	signal temp2: std_logic_vector(3 downto 0);
	signal temp3: std_logic_vector(7 downto 0);
	signal cmd, param: std_logic_vector(3 downto 0);
	signal send_cmd1, send_cmd2: std_logic;

	signal start_tx, done_tx, start_rx, done_rx: std_logic;
	signal data_tx, data_rx: std_logic_vector(7 downto 0);

	signal start_tx2, done_tx2, start_rx2, done_rx2: std_logic;
	signal data_tx2, data_rx2: std_logic_vector(7 downto 0);
	signal mph_done, rpm_done: std_logic;
	signal mph_done2, rpm_done2: std_logic;

	signal spi_din, spi_dout : std_logic_vector(7 downto 0);
	signal s_cnt : unsigned(7 downto 0);
	signal time_delay_reg, time_delay_next: unsigned(23 downto 0);
	signal time_delay_reg2, time_delay_next2: unsigned(23 downto 0);
	signal mspi_ready : std_logic;
	signal mspi_din_vld, mspi_dout_vld : std_logic;
	signal mosi, miso, sclk, ss: std_logic;
	signal mspi_din, mspi_dout : std_logic_vector(7 downto 0);
	signal stlv_temp1 : std_logic_vector(7 downto 0);
	signal stlv_temp1a : std_logic_vector(7 downto 0);
	signal stlv_temp2 : std_logic_vector(7 downto 0);
	signal stlv_temp2a : std_logic_vector(7 downto 0);
	signal trigger_send_spi : std_logic;
	signal addr: std_logic_vector(1 downto 0);

begin

--rx_temp <= tx_temp;
mph_wrapper_unit: entity work.wrapperLED(arch)
	generic map(TIME_DELAY=>TIME_DELAY7,DVSR_M=>DVSR_MU_9600,CLK_COUNT=>MPH_CLOCK_COUNT,DVND=>MPH_DVND)
	port map(clk=>clk, reset=>reset,
	tx=>tx_mph,
	sensor=>mph_db,
--	cmd=>cmd_mph,
	param=>param_mph,
	factor=>mph_factor,
	display_update_rate=>time_delay_mph,
	bcd0_o=>bcda0_o,
	bcd1_o=>bcda1_o,
	bcd2_o=>bcda2_o,
	bcd3_o=>bcda3_o,
	result=>mph_result,
	sensor_done1=>mph_done);

rpm_wrapper_unit: entity work.wrapperLED(arch)
	generic map(TIME_DELAY=>TIME_DELAY7,DVSR_M=>DVSR_MU_9600,CLK_COUNT=>RPM_CLOCK_COUNT,DVND=>RPM_DVND)
	port map(clk=>clk, reset=>reset,
	tx=>tx_rpm,
	sensor=>rpm_db,
--	cmd=>cmd_rpm,
	param=>param_rpm,
	factor=>rpm_factor,
	display_update_rate=>time_delay_rpm,
	bcd0_o=>bcdb0_o,
	bcd1_o=>bcdb1_o,
	bcd2_o=>bcdb2_o,
	bcd3_o=>bcdb3_o,
	result=>rpm_result,
	sensor_done1=>rpm_done);

tx_uart_wrapper_unit: entity work.uartLED(str_arch)
	generic map(DVSR_M=>DVSR_MU_115200)
	port map(clk=>clk, reset=>reset,
	tx_start=>start_tx,
	w_data=>data_tx,
	done_tick=>done_tx,
	tx=>tx_uart);

rx_uart_wrapper_unit: entity work.uartLED2(str_arch)
	generic map(DVSR_M=>DVSR_MU_115200)
	port map(clk=>clk, reset=>reset,
	rx_start=>start_rx,
	r_data=>data_rx,
	done_tick=>done_rx,
	rx=>rx_uart);

tx_uart_wrapper_unit2: entity work.uartLED(str_arch)
--	generic map(DVSR_M=>DVSR_MU_115200)
	generic map(DVSR_M=>DVSR_MU_19200)
	port map(clk=>clk, reset=>reset,
	tx_start=>start_tx2,
	w_data=>data_tx2,
	done_tick=>done_tx2,
	tx=>tx_uart2);

rx_uart_wrapper_unit2: entity work.uartLED2(str_arch)
--	generic map(DVSR_M=>DVSR_MU_115200)
	generic map(DVSR_M=>DVSR_MU_19200)
	port map(clk=>clk, reset=>reset,
	rx_start=>start_rx2,
	r_data=>data_rx2,
	done_tick=>done_rx2,
	rx=>rx_uart2);

db0_unit: entity work.db_fsm
	port map(clk=>clk,reset=>reset,
		sw=>rpm_signal,
		db=>rpm_db);
		
db1_unit: entity work.db_fsm
	port map(clk=>clk,reset=>reset,
		sw=>mph_signal,
		db=>mph_db);

spi_master_unit: entity work.SPI_MASTER(RTL)
	generic map(CLK_FREQ=>50000000,SCLK_FREQ=>5000,SLAVE_COUNT=>2)
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
		led1 <= (others=>'1');
		addr <= (others=>'1');
		mspi_din_vld <= '0';
		mspi_din <= (others=>'0');
		test <= (others=>'0');
		skip3 <= '0';
	else if clk'event and clk = '1' then
		case state_dout_reg is
			when idle_dout =>
				led1 <= "1110";
				if mspi_ready = '1' then
					led1 <= "0111";

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
					led1 <= "1011";
-- mspi_dout is what gets received by MISO
--					if addr(0) = '1' then
						stlv_temp1 <= mspi_dout;
--					elsif addr(1) = '1' then	
--						stlv_temp1a <= mspi_dout;
--					end if;	
--					cmd <= stlv_temp1(7 downto 4);
--					param <= stlv_temp1(3 downto 0);
--					test <= param;
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
							temp1:= 33;
						else temp1:= temp1 + 1;
						end if;	
						stlv_temp1 <= conv_std_logic_vector(temp1,8);
--						stlv_temp1 <= X"AA";
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

--					test <= "00" & addr;
					state_dout_next <= idle_dout;
				end if;
--				end if;
		end case;
		state_dout_reg <= state_dout_next;
		end if;
	end if;
end process;	

-- ********************************************************************************

--echo_dout_unit2: process(clk, reset, state_dout_reg2)
--variable temp_uart: integer range 0 to NUM_DATA_ARRAY-1:= 0;
--variable temp: integer range 0 to 255:= 0;
--begin
--	if reset = '0' then
--		state_dout_reg2 <= idle_dout2;
--		stlv_temp2 <= (others=>'0');
--		stlv_temp2a <= (others=>'0');
--	else if clk'event and clk = '1' then
--		case state_dout_reg2 is
--			when idle_dout2 =>
--				led1 <= "1110";
--				if mspi_ready = '1' then
--					led1 <= "0111";
--					mspi_din <= stlv_temp2;
--					mspi_din_vld <= '1';
--					state_dout_next <= start_dout;
--				end if;
--			when start_dout2 =>
--					mspi_din_vld <= '0';			
--					state_dout_next <= done_dout;
--			when done_dout2 =>
--				if SS_o(1) = '0' then
--				if mspi_dout_vld = '1' then
--					led1 <= "1011";
--					-- mspi_dout is what gets received by MISO
--					stlv_temp2a <= mspi_dout;
----					cmd <= stlv_temp2(7 downto 4);
----					param <= stlv_temp2(3 downto 0);
----					test <= param;
----					temp_uart := conv_integer(param);
----					if cmd(3) = '0' then
----						if temp_uart > NUM_DATA_ARRAY-1 then
----							temp_uart := 0;
----						end if;	
----						stlv_temp2 <= data_array1(temp_uart);
----					else
----						if temp_uart > 7 then
----							temp_uart := 0;
----						end if;	
----						stlv_temp <= "0000" & data_array2(temp_uart);
----					end if;	
--	
--					if temp > 126 then
--						temp:= 33;
--					else	
--						temp:= temp + 1;
--					end if;	
--					stlv_temp2 <= conv_std_logic_vector(temp,8);
--									
--					state_dout_next <= idle_dout;
--				end if;
--				end if;
--		end case;
--		state_dout_reg2 <= state_dout_next2;
--		end if;
--	end if;
--end process;	

-- ********************************************************************************

send_uart1: process(clk,reset,state_uart_reg1)
variable temp_uart: integer range 0 to 255:= 33;
begin
	if reset = '0' then
		state_uart_reg1 <= idle1;
		data_tx <= (others=>'0');
		start_tx <= '0';
		skip <= '0';
		start_rx <= '1';
		time_delay_reg <= (others=>'0');
		time_delay_next <= (others=>'0');
--		test <= (others=>'0');
	else if clk'event and clk = '1' then
		case state_uart_reg1 is
			when idle1 =>
				if done_rx = '1' then
					start_rx <= '0';
					state_uart_next1 <= start;
					data_tx <= conv_std_logic_vector(temp_uart,8);
--					data_tx <= stlv_temp1a;
--					data_tx <= data_rx;
--					test <= "1010";
					skip <= not skip;
					if skip = '1' then
						if temp_uart > 125 then
							temp_uart:= 33;
						else
							temp_uart:= temp_uart + 1;
						end if;	
--						test <= temp_uart(3 downto 0);
					end if;
					start_tx <= '1';
				end if;
			when start =>
				start_tx <= '0';
				state_uart_next1 <= delay;
			when delay =>
				start_rx <= '1';
				if time_delay_reg > TIME_DELAY8 then
					time_delay_next <= (others=>'0');
					state_uart_next1 <= idle1;
				else
					time_delay_next <= time_delay_reg + 1;
				end if;	
		end case;
		time_delay_reg <= time_delay_next;
		state_uart_reg1 <= state_uart_next1;
		end if;
	end if;
end process;

-- ********************************************************************************

send_uart2: process(clk,reset,state_uart_reg2)
variable temp_uart: integer range 0 to 255:= 33;
begin
	if reset = '0' then
		state_uart_reg2 <= idle2;
		data_tx2 <= (others=>'0');
		start_tx2 <= '0';
		skip2 <= '0';
	else if clk'event and clk = '1' then
		case state_uart_reg2 is
			when idle2 =>
				if done_rx2 = '1' then
					state_uart_next2 <= start2;
					data_tx2 <= conv_std_logic_vector(temp_uart,8);
	--				data_tx <= stlv_temp;

					skip2 <= not skip2;
					if skip2 = '1' then
						if temp_uart > 125 then
							temp_uart:= 33;
						else
							temp_uart:= temp_uart + 1;
						end if;	
					end if;
					start_tx2 <= '1';
				end if;
			when start2 =>
				start_tx2 <= '0';
				state_uart_next2 <= delay2;
			when delay2 =>
--				if time_delay_reg > TIME_DELAY8 then
--					time_delay_next <= (others=>'0');
					state_uart_next2 <= idle2;
--				else
--					time_delay_next <= time_delay_reg + 1;
--				end if;	
		end case;
--		time_delay_reg <= time_delay_next;
		state_uart_reg2 <= state_uart_next2;
		end if;
	end if;
end process;

-- ********************************************************************************

get_rpm_results: process(clk,reset,state_reg_rpm,rpm_done2)
begin
	if reset = '0' then
		state_reg_rpm <= idle_r;
--		data_array1 <= (others=>(others=>'0'));
--		data_array2 <= (others=>(others=>'0'));
		data_array2(0) <= "0001";
		data_array2(1) <= "0010";
		data_array2(2) <= "0011";
		data_array2(3) <= "0100";
		data_array2(4) <= "0101";
		data_array2(5) <= "0110";
		data_array2(6) <= "0111";
		data_array2(7) <= "1000";
		rpm_done2 <= '0';
		mph_done2 <= '0';
--		data_array1(8) <= (others=>'0');
--		data_array1(9) <= (others=>'0');
--		data_array1(10) <= (others=>'0');
--		data_array1(11) <= (others=>'0');
--		data_array1(12) <= (others=>'0');
--		data_array1(13) <= (others=>'0');
--		data_array1(14) <= (others=>'0');
--		data_array1(15) <= (others=>'0');
		data_array1(0) <= X"E2";
		data_array1(1) <= X"E3";
		data_array1(2) <= X"E4";
		data_array1(3) <= X"E5";
		data_array1(4) <= X"E6";
		data_array1(5) <= X"E7";
		data_array1(6) <= X"E8";
		data_array1(7) <= X"E9";
		data_array1(8) <= X"F0";
		data_array1(9) <= X"F1";
		data_array1(10) <= X"F2";
		data_array1(11) <= X"F3";
		data_array1(12) <= X"F4";
		data_array1(13) <= X"F5";
		data_array1(14) <= X"F6";
		data_array1(15) <= X"F7";
--		cmd_rpm <=  (others=>'0');
--		cmd_mph <=  (others=>'0');
		param_rpm <= (others=>'0');
		param_mph <= (others=>'0');
		time_delay_mph <= "000001111111111111111111";
		time_delay_rpm <= "000001111111111111111111";
	else if clk'event and clk = '1' then
		case state_reg_rpm is
			when idle_r =>
				if rpm_done2 = '1' then
					data_array1(0) <= rpm_result(7 downto 0);
					data_array1(1) <= "000" & rpm_result(12 downto 8);
					data_array2(0) <= bcdb0_o;
					data_array2(1) <= bcdb1_o;
					data_array2(2) <= bcdb2_o;
					data_array2(3) <= bcdb3_o;
					state_next_rpm <= start_r;
				end if;	
			when start_r =>
				state_next_rpm <= done_r;
			when done_r =>
				state_next_rpm <= idle_r;
		end case;
		state_reg_rpm <= state_next_rpm;
		end if;
	end if;
end process;

-- ********************************************************************************

get_mph_results: process(clk,reset,state_reg_mph,mph_done2)
begin
	if reset = '0' then
		state_reg_mph <= idle_m;
--		data_array1(2) <= (others=>'0');
--		data_array1(3) <= (others=>'0');
--		data_array2(4) <= (others=>'0');
--		data_array2(5) <= (others=>'0');
--		data_array2(6) <= (others=>'0');
--		data_array2(7) <= (others=>'0');
	else if clk'event and clk = '1' then
		case state_reg_mph is
			when idle_m =>
				if mph_done2 = '1' then
--					data_array1(2) <= mph_result(7 downto 0);
--					data_array1(3) <= "000" & mph_result(12 downto 8);
--					data_array2(4) <= bcda0_o;
--					data_array2(5) <= bcda1_o;
--					data_array2(6) <= bcda2_o;
--					data_array2(7) <= bcda3_o;
					state_next_mph <= start_m;
				end if;	
			when start_m =>
				state_next_mph <= done_m;
			when done_m =>
				state_next_mph <= idle_m;
		end case;
		state_reg_mph <= state_next_mph;
		end if;
	end if;
end process;

end truck_arch;

