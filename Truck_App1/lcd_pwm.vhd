-- use duty_cycle as an index into an array of on and off times

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

entity lcd_pwm is
	generic(W2: integer:= W2_SIZE);
	port(
		clk, reset: in std_logic;
		pwm_signal: out std_logic;
		duty_cycle: in std_logic_vector(4 downto 0);
		done: out std_logic
		);
end lcd_pwm;

architecture lcd_pwm_arch of lcd_pwm is

	type state_pwm is (idle_pwm, start_pwm1, on_pwm, off_pwm);
	signal reg1, next1 : state_pwm;

	type state_pwm1 is (pwm_idle, pwm_off, pwm_on, pwm_next, pwm_next2, pwm_next3, pwm_done);
	signal state_pwm_reg, state_pwm_next: state_pwm1;

	signal time_delay_reg1, time_delay_next1: unsigned(W2-1 downto 0);
--	signal time_delay_reg1a, time_delay_next1a: signed(23 downto 0);
--	signal time_delay_reg2, time_delay_next2: unsigned(24 downto 0);
--	signal time_delay_reg3, time_delay_next3: unsigned(23 downto 0);
	signal start_pwm: std_logic;
	signal onoff: std_logic_vector(1 downto 0);
	signal sOn_Time: signed(W2-1 downto 0);
	signal sOff_Time: signed(W2-1 downto 0);
	signal skip: std_logic;
	signal stlv_DutyCycle: std_logic_vector(4 downto 0);
	signal count: unsigned(4 downto 0);
	signal count2: unsigned(4 downto 0);
	signal count3: unsigned(4 downto 0);
	signal stlv_temp: std_logic_vector(W2-1 downto 0);
	signal stlv_temp2: std_logic_vector(W2-1 downto 0);

begin


play_tune: process(clk, reset,state_pwm_reg)
begin
	if reset = '0' then
		state_pwm_reg <= pwm_idle;
		state_pwm_next <= pwm_idle;
--		time_delay_reg2 <= (others=>'0');
--		time_delay_next2 <= (others=>'0');
		skip <= '0';
		done <= '0';
		start_pwm <= '0';
		onoff <= (others=>'0');
		sOn_Time <= (others=>'0');
		sOff_Time <= (others=>'0');
		count <= (others=>'0');
		count2 <= (others=>'0');
		count3 <= (others=>'1');
		stlv_temp <= (others=>'0');
		stlv_temp2 <= (others=>'0');
		
	else if clk'event and clk = '1' then
		case state_pwm_reg is
			when pwm_idle =>
				done <= '0';
				if stlv_DutyCycle /= duty_cycle then
					stlv_DutyCycle <= duty_cycle;
					start_pwm <= '0';
					if duty_cycle = "00000" then	-- this isn't supposed to happen
						state_pwm_next <= pwm_idle;
					elsif duty_cycle = "00001" then	-- signal stays off
						state_pwm_next <= pwm_off;
					elsif duty_cycle = "11111" then	-- signal stays on
						state_pwm_next <= pwm_on;
					else 	
						state_pwm_next <= pwm_next;
					end if;
				end if;
			when pwm_off =>
				start_pwm <= '1';
				onoff <= "00";
				sOn_Time <= (others=>'0');
				sOff_Time <= (others=>'0');
				sOn_Time(0) <= '1';
				sOff_Time(0) <= '1';
				state_pwm_next <= pwm_done;
			when pwm_on =>
				start_pwm <= '1';
				onoff <= "11";
--				sOn_Time <=  "11" & X"FFF0";
--				sOff_Time <= "00" & X"0001";
				sOn_Time <=  "111" & X"FFF0";
				sOff_Time <= "000" & X"0001";
				state_pwm_next <= pwm_done;
			when pwm_next =>	
				count <= conv_unsigned(conv_integer(stlv_DutyCycle),5) - 1;
				count2 <= count3 - count;
				state_pwm_next <= pwm_next2;
			when pwm_next2 =>
				stlv_temp <= load_pwm_delay(conv_integer(count2));
				sOff_Time <= conv_signed(conv_integer(stlv_temp),W2);
				state_pwm_next <= pwm_next3;
			when pwm_next3 =>
				stlv_temp2 <= load_pwm_delay(conv_integer(count));
				sOn_Time <= conv_signed(conv_integer(stlv_temp2),W2);
				onoff <= "01";
				start_pwm <= '1';
				state_pwm_next <= pwm_done;
			when pwm_done =>
				done <= '1';
				state_pwm_next <= pwm_idle;
 		end case;
--		time_delay_reg2 <= time_delay_next2;
		state_pwm_reg <= state_pwm_next;
		end if;
	end if;
end process;

-- pulses the output signal (pwm_signal) according to sOff_Time and sOn_Time
-- this process waits on start_pwm 
pwm1_proc: process(clk,reset,reg1,start_pwm)
begin
	if reset = '0' then
		reg1 <= idle_pwm;
		next1 <= idle_pwm;
		pwm_signal <= '0';
		time_delay_next1 <= (others=>'0');
		time_delay_reg1 <= (others=>'0');
	elsif clk'event and clk = '1' then
		case reg1 is
			when idle_pwm =>
				if onoff = "01" then
					next1 <= start_pwm1;
					pwm_signal <= '0';
				elsif onoff = "00" then
					pwm_signal <= '0';
					next1 <= idle_pwm;
				elsif onoff = "11" then
					pwm_signal <= '1';
					next1 <= idle_pwm;
				end if;
			when start_pwm1 =>
				pwm_signal <= '1';
				next1 <= on_pwm;
			when on_pwm =>
				if time_delay_reg1 > sOn_Time then
					time_delay_next1 <= (others=>'0');
					next1 <= off_pwm;
					pwm_signal <= '0';
				else
					time_delay_next1 <= time_delay_reg1 + 1;
				end if;
			when off_pwm =>
				if time_delay_reg1 > sOff_Time then
					time_delay_next1 <= (others=>'0');
					next1 <= idle_pwm;
				else
					time_delay_next1 <= time_delay_reg1 + 1;
				end if;
		end case;
		time_delay_reg1 <= time_delay_next1;
		reg1 <= next1;
	end if;
end process;

end lcd_pwm_arch;
