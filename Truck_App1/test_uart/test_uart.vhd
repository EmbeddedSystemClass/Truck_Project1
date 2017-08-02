-- test_uart.vhd - copied from Truck_App1.vhd with just the uart stuff
library ieee;
--use ieee.std_logic_1164.all;
--use ieee.numeric_std.all;
--use ieee.std_logic_signed.all;

use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_signed.all;


--library XESS;
--use XESS.CommonPckg.all;
--use XESS.DelayPckg.all;
--use XESS.PulsePckg.all;

entity test_uart is
	port(
		clk, reset: in std_logic;
		tx_uart: out std_logic;
		rx_uart: in std_logic;
		tx_uart2: out std_logic;
		rx_uart2: in std_logic;
		led1: out std_logic_vector(3 downto 0)
		);
end test_uart;

architecture truck_arch of test_uart is

	type state_uart1 is (idle1, start1a, start1b, start1c, start1d, done1);
	signal state_uart_reg1, state_uart_next1: state_uart1;
	signal t_reg_uart1, t_next_uart1: unsigned(23 downto 0);
	
	type state_uart4 is (idle4, start4a, start4b, start4c, done4);
	signal state_uart_reg4, state_uart_next4: state_uart4;
	signal t_reg_uart4, t_next_uart4: unsigned(23 downto 0);
	
	constant TIME_DELAY2: integer:= 1000;
	constant TIME_DELAY: integer:= 8388000;  -- highest possible time delay for 23 bits (about 0.36 seconds)
--	constant TIME_DELAY: integer:= 83880;	-- about 3.6ms between frames (SS stays high this long)
--	constant TIME_DELAY: integer:= 4000;
-- 16 bits = FFFF (65,535)	1210us
-- 17 bits = 1FFFF (131,071)  2521us
-- 18 bits = 3FFFF (262,143)  5.14ms
-- 19 bits = 7FFFF (524,287)  10.38ms
-- 20 bits = FFFFF (1,048,575)  ~21ms
-- 21 bits = 1FFFFF (2,097,151)  ~42ms
-- 22 bits = 3FFFFF (4,191,303)  ~84ms
-- 23 bits = 7FFFFF (8,388,607)  ~168ms
-- 24 bits = FFFFFF (16,777,215) ~335ms
--	constant TIME_DELAY4: integer:= 318181;	-- this in a time delay should provide about 25ms
	constant TIME_DELAY5:  integer:= 16000000;
	constant TIME_DELAY6:  integer:= 1048000;
	constant TIME_DELAY7:  integer:= 150000;
	constant TIME_DELAY8:  integer:= 50000;
	constant TIME_DELAY9:  integer:= 25000; -- shortest time delay possible for 19200 baud
	constant TIME_DELAY10:  integer:=3000;	-- shortest time delay possible for 115200 baud
	constant TIME_DELAY11:  integer:=5000;	-- shortest time delay possible for 115200 baud
-- baud rates for 7-seg displays	
	constant DVSR_M1: integer:= 163;	-- 19200 baud	520us	1923
	constant DVSR_M2: integer:= 27;
--	constant DVSR_M: integer:= 27;	-- 115200 baud

	signal skip: std_logic;
	signal skip2: std_logic;
	constant MPH_CLOCK_COUNT: integer:= 12500;
	constant MPH_DVND: integer:= 128514;
	constant RPM_CLOCK_COUNT: integer:= 500;
	constant RPM_DVND: integer:= 6000000;
	
	signal start_tx, done_tx, start_rx, done_rx: std_logic;
	signal data_tx, data_rx: std_logic_vector(7 downto 0);

	signal start_tx2, done_tx2, start_rx2, done_rx2: std_logic;
	signal data_tx2, data_rx2: std_logic_vector(7 downto 0);

--	signal tempx: std_logic_vector(7 downto 0);
	signal temp3_int: std_logic_vector(7 downto 0);
	signal stdlv_transmit_update_rate: std_logic_vector(23 downto 0);
	signal transmit_update_rate: unsigned(23 downto 0);

	signal stdlv_rpm_update_rate: std_logic_vector(23 downto 0);
	signal rpm_update_rate: unsigned(23 downto 0);
	signal stdlv_mph_update_rate: std_logic_vector(23 downto 0);
	signal mph_update_rate: unsigned(23 downto 0);
	signal temp1, temp2: std_logic_vector(3 downto 0);
begin

--rx_temp <= tx_temp;

tx_uart_wrapper_unit: entity work.uartLED(str_arch)
	generic map(DVSR_M=>DVSR_M2)
	port map(clk=>clk, reset=>reset,
	tx_start=>start_tx,
	w_data=>data_tx,
	done_tick=>done_tx,
	tx=>tx_uart);

rx_uart_wrapper_unit: entity work.uartLED2(str_arch)
	generic map(DVSR_M=>DVSR_M2)
	port map(clk=>clk, reset=>reset,
	rx_start=>start_rx,
	r_data=>data_rx,
	done_tick=>done_rx,
	rx=>rx_uart);

tx_uart_wrapper_unit2: entity work.uartLED(str_arch)
	generic map(DVSR_M=>DVSR_M1)
	port map(clk=>clk, reset=>reset,
	tx_start=>start_tx2,
	w_data=>data_tx2,
	done_tick=>done_tx2,
	tx=>tx_uart2);

rx_uart_wrapper_unit2: entity work.uartLED2(str_arch)
	generic map(DVSR_M=>DVSR_M1)
	port map(clk=>clk, reset=>reset,
	rx_start=>start_rx2,
	r_data=>data_rx2,
	done_tick=>done_rx2,
	rx=>rx_uart2);

-- ******************************** 1st UART ***************************************
-- first 2 pins on lower right: 77 - TX; 75 - RX
-- this puts on TX what's received from RX2

-- SEND

send_uart: process(clk,reset,state_uart_reg1)
variable temp_uart: integer range 0 to 255:= 33;
begin
	if reset = '0' then
		state_uart_reg1 <= idle1;
		t_reg_uart1 <= (others=>'0');
		t_next_uart1 <= (others=>'0');
		t_reg_uart1 <= (others=>'0');
		t_next_uart1 <= (others=>'0');
		data_tx <= X"20";
		start_tx <= '0';
		data_tx2 <= (others=>'0');
		start_tx2 <= '0';
		skip <= '0';
		start_rx <= '0';
		start_rx2 <= '0';
		led1 <= (others=>'1');
	else if clk'event and clk = '1' then
		case state_uart_reg1 is
			when idle1 =>
				start_rx2 <= '1';
				if t_reg_uart1 >= TIME_DELAY11 then
					t_next_uart1 <= (others=>'0');
					state_uart_next1 <= start1a;
				else
					t_next_uart1 <= t_reg_uart1 + 1;
				end if;
			when start1a =>
				data_tx <= data_rx2;
				temp1 <= data_rx2(7 downto 4);
				temp2 <= data_rx2(3 downto 0);
				start_tx <= '1';
				skip <= not skip;
				if skip = '1' then
					if temp_uart > 125 then
						temp_uart := 33;
					else
						temp_uart := temp_uart + 1;
					end if;
				end if;
				state_uart_next1 <= start1b;
			when start1b =>	
				start_tx <= '0';
				state_uart_next1 <= start1c;
			when start1c =>
				if done_rx2 = '1' then
					led1 <= data_rx(7 downto 4);
					data_tx2(3 downto 0) <= temp1;
					data_tx2(7 downto 4) <= temp2;
					start_tx2 <= '1';
					state_uart_next1 <= start1d;
				end if;
			when start1d =>
				start_tx2 <= '0';		
				state_uart_next1 <= done1;
			when done1 =>
				state_uart_next1 <= idle1;
		end case;
		state_uart_reg1 <= state_uart_next1;
		t_reg_uart1 <= t_next_uart1;
		end if;
	end if;
end process;

end truck_arch;

