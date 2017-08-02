-- sendcharLED.vhd - if leading digits are '0' then just send a 0x0f which will
-- set the digit blank
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
--use ieee.numeric_std.all;

library XESS;
use XESS.CommonPckg.all;

--*****************************************************************************************************--
entity sendcharLED is
	generic(TIME_DELAY: integer:= 10;DVSR_M: integer:= 326);	-- default baudrate set to 9600
   port(
      clk, reset: in std_logic;
      tx: out std_logic;
      start_cmd: in std_logic;	-- signal to start
--      cmd: in std_logic_vector(3 downto 0);
	  LED0: in std_logic_vector(3 downto 0);
	  LED1: in std_logic_vector(3 downto 0);
	  LED2: in std_logic_vector(3 downto 0);
	  LED3: in std_logic_vector(3 downto 0);
--	  ledx: out std_logic_vector(3 downto 0);
	  display_done: out std_logic
   );
end sendcharLED;

architecture arch of sendcharLED is
	signal uart_done: std_logic;
	signal start_proc1: std_logic;
	signal set_char_out: std_logic_vector(7 downto 0);
	signal nodigits: unsigned(1 downto 0);
	signal higher_than_0: std_logic;
	
	type state_type is (idle, start, set_cursor_pos, 
			send_cursor_param, wait_done1, wait_done2, send_digit0,send_digit1, 
				send_digit2, send_digit3, wait_done3, state_done);

	signal state_reg_ch, state_next: state_type;
--   	signal t_reg, t_next: unsigned(15 downto 0);
   
--*****************************************************************************************************--
begin
   -- instantiate uart
tx_uart_wrapper_unit3: entity work.uartLED(str_arch)
	generic map(DVSR_M=>DVSR_M)
	port map(clk=>clk, reset=>reset,
	tx_start=>start_proc1,
	w_data=>set_char_out,
	done_tick=>uart_done,
	tx=>tx);

--	wrap_uartLED_unit: entity work.wrap_uartLED(arch)
--		generic map(TIME_DELAY=>TIME_DELAY,DVSR_M=>DVSR_M)
--		port map(clk=>clk, reset=>reset,
--		tx=>tx,
--		writeLED=>set_char_out,
--		done=>uart_done,
--		start_proc=>start_proc1);

--*****************************************************************************************************--
	SEND_DIGIT_FSM: process(clk,reset,state_reg_ch,start_cmd)
	variable temp_int: integer range 0 to 3:= 0;
	variable temp_int2: integer range 0 to 255:= 0;
	begin
	if reset='0' then
		state_reg_ch <= idle;
		state_next <= idle;
		nodigits <= (others=>'0');
		set_char_out <= (others=>'0');
		display_done <= '1';
		start_proc1 <= '0';
		higher_than_0 <= '0';
--		ledx <= (others=>'1');
	elsif clk'event and clk = '1' then
		case state_reg_ch is
			when idle =>
				display_done <= '0';
				higher_than_0 <= '0';
				if start_cmd = '1' then
					state_next <= start;
				end if;
			when start =>
				state_next <= set_cursor_pos;
			when set_cursor_pos =>
				set_char_out <= X"79";	-- set cursor command
				start_proc1 <= '1';
				state_next <= send_cursor_param;
			when send_cursor_param =>
				start_proc1 <= '0';
				if uart_done = '1' then
					set_char_out <= "00000000";	-- always start from the 1st position
					start_proc1 <= '1';
					state_next <= wait_done1;
				end if;	
			when wait_done1 =>
				start_proc1 <= '0';
				if uart_done = '1' then
					state_next <= wait_done2;
				end if;	
			when wait_done2 =>
				temp_int:= conv_integer(nodigits);
				case temp_int is
					when 0 =>
						state_next <= send_digit0;
					when 1 =>
						state_next <= send_digit1;
					when 2 =>
						state_next <= send_digit2;
					when 3 =>
						state_next <= send_digit3;
				end case;
			when send_digit0 =>
				if LED0 /= "0000" then
					set_char_out <= "0000" & LED0;
					higher_than_0 <= '1';
				else
					set_char_out <= "00010000";	-- set the leading 0's to blanks
				end if;	
				start_proc1 <= '1';
				state_next <= wait_done1;
				nodigits <= "01";
			when send_digit1 =>
				if LED1 /= "0000" then
					set_char_out <= "0000" & LED1;
					higher_than_0 <= '1';
				else
					if higher_than_0 = '1' then
						set_char_out <= "00000000";
					else	
						set_char_out <= "00010000"; -- set the leading 0's to blanks
					end if;	
				end if;	
				start_proc1 <= '1';
				nodigits <= "10";
				state_next <= wait_done1;
			when send_digit2 =>
				if LED2 /= "0000" then
					set_char_out <= "0000" & LED2;
					higher_than_0 <= '1';
				else
					if higher_than_0 = '1' then
						set_char_out <= "00000000";
					else	
						set_char_out <= "00010000"; -- set the leading 0's to blanks
					end if;	
				end if;	
				start_proc1 <= '1';
				nodigits <= "11";
				state_next <= wait_done1;
			when send_digit3 =>
				set_char_out <= "0000" & LED3;
				start_proc1 <= '1';
				state_next <= wait_done3;
			when wait_done3 =>
				start_proc1 <= '0';
				if uart_done = '1' then
						state_next <= state_done;
				end if;
			when state_done =>
--				ledx <= LED0;
				nodigits <= (others=>'0');
				display_done <= '1';
				state_next <= idle;
		end case;
		state_reg_ch <= state_next;
	end if;
	end process;

end arch;				

