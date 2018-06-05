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
	port(
		clk, reset: in std_logic;
		pwm_signal: out std_logic;
		state: in std_logic_vector(1 downto 0);
		duty_cycle: in std_logic_vector(2 downto 0);
		done: out std_logic
		);
end lcd_pwm;

architecture lcd_pwm_arch of lcd_pwm is

	type state_duty is (idle_duty, start_duty, check_duty1, check_duty2, done_duty);
	signal duty_reg, duty_next : state_duty;

	type state_pwm is (idle_pwm, start_pwm1, on_pwm, off_pwm);
	signal reg1, next1 : state_pwm;

	type state_pwm2 is (idle_pwm2, start_pwm12, on_pwm2, off_pwm2);
	signal reg2, next2 : state_pwm2;

	type state_pwm1 is (pwm_idle, pwm_next, pwm_next2, pwm_done);
	signal state_pwm_reg, state_pwm_next: state_pwm1;

	type state_vary is (idle_vary, start_vary, next_vary1, next_vary2, next_vary3, done_vary);
	signal vary_reg, vary_next : state_vary;

	signal time_delay_reg1, time_delay_next1: signed(23 downto 0);
--	signal time_delay_reg1a, time_delay_next1a: signed(23 downto 0);
	signal time_delay_reg2, time_delay_next2: unsigned(24 downto 0);
	signal time_delay_reg3, time_delay_next3: unsigned(23 downto 0);
	signal start_pwm: std_logic;
	signal next_note: std_logic;
	signal sOn_Time: signed(19 downto 0);
	signal sOff_Time: signed(19 downto 0);
	signal skip: std_logic;
	signal sDutyCycle: std_logic_vector(2 downto 0);

	signal iwhole_period: signed(19 downto 0);
	signal ihalf_period: signed(19 downto 0);
	signal iquarter_period: signed(19 downto 0);
	signal isixteen_period: signed(19 downto 0);

	signal whole_period: std_logic_vector(19 downto 0);
	signal half_period: std_logic_vector(19 downto 0);
	signal quarter_period: std_logic_vector(19 downto 0);
	signal sixteen_period: std_logic_vector(19 downto 0);
	signal current_state: std_logic_vector(1 downto 0);
	signal onoff: std_logic;

begin

play_note: process(clk, reset, vary_reg)
variable itemp: integer range 0 to 33000:= 0;
begin
	if reset = '0' then
		sOn_Time <= (others=>'0');
		sOff_Time <= (others=>'0');
		time_delay_reg3 <= (others=>'0');
		time_delay_next3 <= (others=>'0');

		iwhole_period <= (others=>'0');
		ihalf_period <= (others=>'0');
		iquarter_period <= (others=>'0');
		isixteen_period <= (others=>'0');

		whole_period <= (others=>'0');
		half_period <= (others=>'0');
		quarter_period <= (others=>'0');
		sixteen_period <= (others=>'0');
		
	elsif clk'event and clk = '1' then
		case vary_reg is
			when idle_vary =>
				if next_note = '1' then
					vary_next <= start_vary;
				end if;	
			when start_vary =>
				-- itemp := conv_integer(notes(conv_integer(note_index)));
				-- whole_period <= conv_std_logic_vector(itemp,20);
				whole_period <= conv_std_logic_vector(B9,20);
				vary_next <= next_vary1;
			when next_vary1 =>
				half_period <= '0' & whole_period(19 downto 1);
				quarter_period <= '0' & half_period(19 downto 1);
				sixteen_period <= '0' & quarter_period(19 downto 1);
				vary_next <= next_vary2;
			when next_vary2 =>
--				quarter_period <= "0" & quarter_period(19 downto 1);
				vary_next <= next_vary3;
			when next_vary3 =>
				iwhole_period <= conv_signed(conv_integer(whole_period),20);
				ihalf_period <= conv_signed(conv_integer(half_period),20);
				iquarter_period <= conv_signed(conv_integer(quarter_period),20);
				isixteen_period <= conv_signed(conv_integer(sixteen_period),20);
				vary_next <= done_vary;
			when done_vary =>
				case sDutyCycle is
					when "000" =>
						-- 12% duty_cycle
						sOn_Time <= iwhole_period - ihalf_period - iquarter_period - isixteen_period;
						sOff_Time <= iwhole_period - ihalf_period + iquarter_period + isixteen_period;
					when "001" =>
						-- 25% duty_cycle
						sOn_Time <= iwhole_period - ihalf_period - iquarter_period;
						sOff_Time <= iwhole_period - ihalf_period + iquarter_period;
					when "010" =>
						-- 30% duty_cycle
						sOn_Time <= iwhole_period - ihalf_period - iquarter_period + isixteen_period;
						sOff_Time <= iwhole_period - ihalf_period + iquarter_period - isixteen_period;
					when "011" =>
						-- square wave
						sOn_Time <= iwhole_period - ihalf_period;
						sOff_Time <= iwhole_period - ihalf_period;
					when "100" =>
						-- 60% duty_cycle
						sOn_Time <= iwhole_period - ihalf_period + iquarter_period - isixteen_period;
						sOff_Time <= iwhole_period - ihalf_period - iquarter_period + isixteen_period;
					when "101" =>
						-- 75% duty_cycle
						sOn_Time <= iwhole_period - ihalf_period + iquarter_period;
						sOff_Time <= iwhole_period - ihalf_period - iquarter_period;
					when "110" =>
						-- 80% duty_cycle
						sOn_Time <= iwhole_period - ihalf_period + iquarter_period + isixteen_period;
						sOff_Time <= iwhole_period - ihalf_period - iquarter_period - isixteen_period;
					when others =>	
						sOn_Time <= iwhole_period - ihalf_period;
						sOff_Time <= iwhole_period - ihalf_period;
				end case;
				vary_next <= idle_vary;
		end case;
		vary_reg <= vary_next;
		time_delay_reg3 <= time_delay_next3;
	end if;
end process;
		
play_tune: process(clk, reset,state_pwm_reg)
begin
	if reset = '0' then
		state_pwm_reg <= pwm_idle;
		state_pwm_next <= pwm_idle;
		time_delay_reg2 <= (others=>'0');
		time_delay_next2 <= (others=>'0');
		skip <= '0';
		next_note <= '0';
		done <= '0';
		current_state <= (others=>'0');
		onoff <= '0';
		start_pwm <= '0';
		sDutyCycle <= "000";
		
	else if clk'event and clk = '1' then
		case state_pwm_reg is
			when pwm_idle =>
				if state /= current_state then
					current_state <= state;
				end if;
				done <= '0';
				case current_state is
					when "00" =>
						onoff <= '0';
						start_pwm <= '0';
						next_note <= '0';
					when "01" =>
						onoff <= '1';
						start_pwm <= '1'; 
						next_note <= '1';
					when "10" =>
						onoff <= '1';
						start_pwm <= '1'; 
						next_note <= '1';
					when others =>
						onoff <= '0';
						start_pwm <= '0'; 
						next_note <= '0';
				end case;
				state_pwm_next <= pwm_next;
				
			when pwm_next =>
				if sDutyCycle /= duty_cycle and current_state = "10" then
					sDutyCycle <= duty_cycle;
				end if;
				state_pwm_next <= pwm_next2;

			when pwm_next2 =>
				next_note <= '0';
				state_pwm_next <= pwm_done;
			when pwm_done =>
				state_pwm_next <= pwm_idle;
				done <= '1';
 		end case;
		time_delay_reg2 <= time_delay_next2;
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
				if start_pwm = '1' then
					next1 <= start_pwm1;
				end if;
			when start_pwm1 =>
				pwm_signal <= '1';
				next1 <= on_pwm;
				if current_state = "01" then
					next1 <= idle_pwm;
				end if;
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
