-- wrapperLED.vhd - a wrapper for the rpm & mph signals to the LED's
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_signed.all;

--library XESS;
--use XESS.CommonPckg.all;

entity wrapperLED is
	generic (TIME_DELAY: integer:= 1000; DVSR_M: integer:= 326; CLK_COUNT: integer:= 5000;DVND:integer:= 12852);
	port(
		clk, reset: in std_logic;
		tx: out std_logic;
		sensor: in std_logic;
--		cmd: in std_logic_vector(2 downto 0);
		param: in std_logic_vector(7 downto 0);
		factor: in std_logic_vector(5 downto 0);
		display_update_rate: in std_logic_vector(23 downto 0);
		bcd0_o, bcd1_o, bcd2_o, bcd3_o: out std_logic_vector(3 downto 0);
		result: out std_logic_vector(12 downto 0);
		sensor_done1: out std_logic
		);
end wrapperLED;

architecture arch of wrapperLED is

	type u_state_type is (u_idle, u_start1, u_start2, u_done);
	signal u_state_reg, u_state_next: u_state_type;
	signal u_time_reg, u_time_next: unsigned(23 downto 0);

--	type m_state_type is (m_idle, state_set_bright, state_set_decimal, state_set_baud,  
--			wait_bright, wait_decimal, wait_baud, m_done);
--	signal m_state_reg, m_state_next: m_state_type;

	signal start_sendchar: std_logic;
	signal done_sendchar: std_logic;
--	signal start_bright: std_logic;
--	signal done_bright: std_logic;
--	signal start_decimal: std_logic;
--	signal done_decimal: std_logic;
--	signal start_baud: std_logic;
--	signal done_baud: std_logic;
	signal bcd0, bcd1, bcd2, bcd3: std_logic_vector(3 downto 0);
--	signal tx1, tx2, tx3, tx4: std_logic;
--	signal sendchar_busy: std_logic;
--	signal other_commands: std_logic;
	signal update_rate: unsigned(23 downto 0):= "000001111111111111111111";
	signal real_factor: std_logic_vector(22 downto 0);
--	constant TIME_DELAY_UPDATE: integer:= 8388000;	-- about 168ms
	signal last_factor: std_logic_vector(5 downto 0);
	signal done_sensor: std_logic;
--	signal tx: std_logic;
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

--tx <= tx1 and tx2 and tx3 and tx4;

sendcharLED_unit: entity work.sendcharLED(arch)
	generic map(TIME_DELAY=>TIME_DELAY,DVSR_M=>DVSR_M)
	port map(clk=>clk, reset=>reset,
	tx=>tx,
	start_cmd=>start_sendchar,
	LED0=>bcd0,
	LED1=>bcd1,
	LED2=>bcd2,
	LED3=>bcd3,
	display_done=>done_sendchar);

--set_brightnessLED_unit: entity work.set_brightnessLED(arch)
--	generic map(TIME_DELAY=>TIME_DELAY,DVSR_M=>DVSR_M)
--	port map(clk=>clk, reset=>reset,
--	tx=>tx2,
--	start_cmd=>start_bright,
--	param=>param,
--	done_bright=>done_bright);

--set_decimalLED_unit: entity work.set_decimalLED(arch)
--	generic map(TIME_DELAY=>TIME_DELAY,DVSR_M=>DVSR_M)
--	port map(clk=>clk, reset=>reset,
--	tx=>tx3,
--	start_cmd=>start_decimal,
--	param=>param,
--	done_decimal=>done_decimal);

--set_baudrateLED_unit: entity work.setbaudLED(arch)
--	generic map(TIME_DELAY=>TIME_DELAY,DVSR_M=>DVSR_M)
--	port map(clk=>clk, reset=>reset,
--	tx=>tx4,
--	start_cmd=>start_baud,
--	baudrate=>param(3 downto 0),
--	done_baud=>done_baud);sensor_done
--	
sensor_unit: entity work.sensor(arch)
	generic map(CLK_COUNT=>CLK_COUNT,DVND_FACTOR=>DVND)
	port map(clk=>clk, reset=>reset,
		sensor=>sensor,
		result=>result,
		factor=>real_factor,
		LED0m=>bcd0,
		LED1m=>bcd1,
		LED2m=>bcd2,
		LED3m=>bcd3,
		sensor_done=>done_sensor);

display_update_proc: process(clk, reset, u_state_reg)
begin
	if reset = '0' then
		u_state_reg <= u_idle;	
		u_state_next <= u_idle;
		u_time_reg <= (others=>'0');
		u_time_next <= (others=>'0');
		start_sendchar <= '0';
--		sendchar_busy <= '0';		-- flag to tell the other commands to wait
--		sendchar_busy <= '1';		-- flag to tell the other commands to wait
		bcd0_o <= (others=>'0');
		bcd1_o <= (others=>'0');
		bcd2_o <= (others=>'0');
		bcd3_o <= (others=>'0');
		sensor_done1 <= '0';
--		update_rate <= unsigned(display_update_rate);
	elsif clk'event and clk= '1' then
		case u_state_reg is
			when u_idle =>
--				sendchar_busy <= '1';
--				if other_commands = '0' then
				if done_sensor = '1' then
					bcd0_o <= bcd0;
					bcd1_o <= bcd1;
					bcd2_o <= bcd2;
					bcd3_o <= bcd3;
					sensor_done1 <= '1';
					u_state_next <= u_start2;
				end if;	
			when u_start1 =>					
				sensor_done1 <= '0';
--				if u_time_reg = TIME_DELAY3 then
--					u_time_next <= (others=>'0');
					u_state_next <= u_start2;
--				else
--					u_time_next <= u_time_reg + 1;
--				end if;
			when u_start2 =>
				start_sendchar <= '1';
				u_state_next <= u_done;
			when u_done =>
				start_sendchar <= '0';
--				if done_sendchar = '1' then
--					sendchar_busy <= '0';
					u_state_next <= u_idle;
--				end if;   
		end case;
		u_state_reg <= u_state_next;
		u_time_reg <= u_time_next;
	end if;
end process;

--main_proc: process(clk, reset, m_state_reg)
--variable temp1: integer range 0 to 3:= 0; 
--begin
--	if reset = '0' then
----		m_state_reg <= m_idle;	
----		m_state_next <= m_idle;	
--		m_state_reg <= m_done;
--		m_state_next <= m_done;	
--		other_commands <= '0';
--		start_bright <= '0';
--		start_decimal <= '0';
--		start_baud <= '0';
--	elsif clk'event and clk = '1' then
--		case m_state_reg is
--			when m_idle =>
--				m_state_next <= m_idle;
--				
----				if sendchar_busy = '0' then
----					temp1:= conv_integer(signed(cmd));
----					case temp1 is
----						when 0 =>
----							m_state_next <= m_idle;
----						when 1 =>	
----							other_commands <= '1';
----							start_bright <= '1';
----							m_state_next <= state_set_bright;
----						when 2 =>
----							other_commands <= '1';
----							start_decimal <= '1';
----							m_state_next <= state_set_decimal;
----						when 3 =>
----							other_commands <= '1';
----							start_baud <= '1';
----							m_state_next <= state_set_baud;
----					end case;
----				end if;	
--			when state_set_bright =>
--				start_bright <= '0';
--				m_state_next <= wait_bright;
--			when state_set_decimal =>
--				start_decimal <= '0';
--				m_state_next <= wait_decimal;
--			when state_set_baud =>
--				start_baud <= '0';
--				m_state_next <= wait_baud;
--			when wait_bright =>
--				if done_bright = '1' then
--					m_state_next <= m_done;
--				end if;	
--			when wait_decimal =>
--				if done_decimal = '1' then	
--					m_state_next <= m_done;
--				end if;	
--			when wait_baud =>
--				if done_baud = '1' then	
--					m_state_next <= m_done;
--				end if;	
--			when m_done =>
--				other_commands <= '0';
--				m_state_next <= m_idle;
--		end case;
--		m_state_reg <= m_state_next;
--	end if;
--end process;

end arch;
