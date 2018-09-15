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
		start: in std_logic;
		cmd: in std_logic_vector(7 downto 0);
		param: in std_logic_vector(7 downto 0);
		LED0: in std_logic_vector(3 downto 0);
		LED1: in std_logic_vector(3 downto 0);
		LED2: in std_logic_vector(3 downto 0);
		LED3: in std_logic_vector(3 downto 0);
		display_done: out std_logic
   );
end sendcharLED;

architecture arch of sendcharLED is
	signal uart_done: std_logic;
	signal start_proc1: std_logic;
	signal set_char_out: std_logic_vector(7 downto 0);
	signal nodigits: unsigned(1 downto 0);
	signal higher_than_0: std_logic;
	
	type state_type is (clear_disp, set_init_bright, set_init_bright_param, idle, 
		start1, set_cursor_pos, send_cursor_param, wait_done1, wait_done2, 
			send_digit0,send_digit1, send_digit2, send_digit3, wait_done3, 
				set_bright, set_decimal, factory_reset, wait_send_param, 
						send_param, wait_done_send_param, state_done);

	signal send_state_reg, send_state_next: state_type;
--   	signal t_reg, t_next: unsigned(15 downto 0);
	signal u_time_reg, u_time_next: unsigned(25 downto 0);
   
--*****************************************************************************************************--
begin
   -- instantiate uart
tx_uart_wrapper_unit3: entity work.uartLED(str_arch)
	generic map(DVSR_M=>DVSR_M)
	port map(clk=>clk, reset=>reset,
	tx_start=>start_proc1,
	w_data=>set_char_out,
	done_tick=>uart_done,
--	done_tick=>open,
	tx=>tx);

--*****************************************************************************************************--
	SEND_DIGIT_FSM: process(clk, reset, send_state_reg, send_state_next, cmd, param, start)
	variable temp_int: integer range 0 to 3:= 0;
	variable temp_int2: integer range 0 to 255:= 0;
	begin
	if reset='0' then
		send_state_reg <= clear_disp;
		send_state_next <= clear_disp;
--		send_state_reg <= idle;
--		send_state_next <= idle;
		nodigits <= (others=>'0');
		set_char_out <= (others=>'0');
		display_done <= '0';
		start_proc1 <= '0';
		higher_than_0 <= '0';
		u_time_reg <= (others=>'0');
		u_time_next <= (others=>'0');
	elsif clk'event and clk = '1' then
		case send_state_reg is
			when clear_disp =>
				if u_time_reg > TIME_DELAY9 then
					u_time_next <= (others=>'0');
					set_char_out <= SET_BRIGHT_CTL;
					start_proc1 <= '1';
					send_state_next <= set_init_bright;
				else
					u_time_next <= u_time_reg + 1;
				end if;
				
			when set_init_bright =>
				start_proc1 <= '0';
				if u_time_reg > TIME_DELAY9 then
					u_time_next <= (others=>'0');
					set_char_out <= X"FF";
					start_proc1 <= '1';
					send_state_next <= set_init_bright_param;
				else
					u_time_next <= u_time_reg + 1;
				end if;					

			when set_init_bright_param =>
				start_proc1 <= '0';
				if u_time_reg > TIME_DELAY9 then
					u_time_next <= (others=>'0');
					set_char_out <= SET_CLEAR_CTL;
					start_proc1 <= '1';
					send_state_next <= wait_done3;
				else
					u_time_next <= u_time_reg + 1;
				end if;					
			when idle =>
				start_proc1 <= '0';
--				display_done <= '0';
				if start = '1' then
					higher_than_0 <= '0';
					nodigits <= (others=>'0');
					case cmd is
						when RPM_OFF_CMD => 
							send_state_next <= idle;
						when MPH_OFF_CMD => 
							send_state_next <= idle;
						when RPM_SEND_CHAR_CMD => 
							send_state_next <= start1;
						when MPH_SEND_CHAR_CMD => 
							send_state_next <= start1;
						when RPM_SET_BRIGHTNESS_CMD => 
							send_state_next <= set_bright;
						when RPM_SET_FACTORY_RESET =>
							send_state_next <= factory_reset;
						when MPH_SET_BRIGHTNESS_CMD => 
							send_state_next <= set_bright;
						when RPM_SET_CDECIMAL_CMD => 
							send_state_next <= set_decimal;
						when MPH_SET_CDECIMAL_CMD => 
							send_state_next <= set_decimal;
						when MPH_SET_FACTORY_RESET =>
							send_state_next <= factory_reset;
--						when SET_TEST_CMD1 => send_state_next <= idle;
--						when SET_TEST_CMD2 => send_state_next <= idle;
--						when SET_TEST_CMD3 => send_state_next <= idle;
--						when SET_TEST_CMD4 => send_state_next <= idle;
						when others => 
							send_state_next <= idle;
					end case;
					display_done <= '0';
				else
					send_state_next <= idle;
				end if;	
			when start1 =>
				send_state_next <= set_cursor_pos;
			when set_cursor_pos =>
				set_char_out <= SET_CURSOR_CTL;	-- set cursor command
				start_proc1 <= '1';
				send_state_next <= send_cursor_param;
			when send_cursor_param =>
				start_proc1 <= '0';
				if uart_done = '1' then
					set_char_out <= "00000000";	-- always start from the 1st position
					start_proc1 <= '1';
					send_state_next <= wait_done1;
				end if;	
			when wait_done1 =>
				start_proc1 <= '0';
				if uart_done = '1' then
					send_state_next <= wait_done2;
				end if;	
			when wait_done2 =>
				temp_int:= conv_integer(nodigits);
				case temp_int is
					when 0 =>
						send_state_next <= send_digit0;
					when 1 =>
						send_state_next <= send_digit1;
					when 2 =>
						send_state_next <= send_digit2;
					when 3 =>
						send_state_next <= send_digit3;
				end case;
			when send_digit0 =>
				if LED0 /= "0000" then
					set_char_out <= "0000" & LED0;
					higher_than_0 <= '1';
				else
					set_char_out <= "00010000";	-- set the leading 0's to blanks
				end if;	
				start_proc1 <= '1';
				send_state_next <= wait_done1;
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
				send_state_next <= wait_done1;
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
				send_state_next <= wait_done1;
			when send_digit3 =>
				set_char_out <= "0000" & LED3;
				start_proc1 <= '1';
				send_state_next <= wait_done3;
			when wait_done3 =>
				start_proc1 <= '0';
				if uart_done = '1' then
						send_state_next <= state_done;
				end if;
			when set_bright =>
				set_char_out <= SET_BRIGHT_CTL;
				start_proc1 <= '1';
				send_state_next <= wait_send_param;
			when set_decimal =>
				set_char_out <= SET_DECIMAL_CTL;	-- set decimal command
				start_proc1 <= '1';
				send_state_next <= wait_send_param;
			when factory_reset =>	
				set_char_out <= FACTORY_RESET_CTL;	-- factory_reset
				start_proc1 <= '1';
				send_state_next <= wait_done_send_param;
			when wait_send_param =>
				start_proc1 <= '0';
				if uart_done = '1' then
					send_state_next <= send_param;
				end if;	
			when send_param =>
				set_char_out <= param;
				start_proc1 <= '1';
				send_state_next <= wait_done_send_param;
			when wait_done_send_param =>
				start_proc1 <= '0';
				if uart_done = '1' then
					set_char_out <= (others=>'0');
					send_state_next <= state_done;
				end if;	
			when state_done =>
				display_done <= '1';
				send_state_next <= idle;
--				send_state_next <= start1;
				nodigits <= (others=>'0');
		end case;
		send_state_reg <= send_state_next;
		u_time_reg <= u_time_next;
	end if;
	end process;

end arch;				

