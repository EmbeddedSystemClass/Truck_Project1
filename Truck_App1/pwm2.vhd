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

entity pwm2 is
	port(
		clk, reset: in std_logic;
		pwm_signal_1: out std_logic;
		pwm_signal_2: out std_logic;
		start: in std_logic;
		done: out std_logic;
		len: in signed(7 downto 0);
		duty_cycle: in std_logic_vector(2 downto 0);
		note_len_index: std_logic_vector(2 downto 0);
		key: in std_logic_vector(7 downto 0)
		);
end pwm2;

architecture pwm_arch of pwm2 is

	type state_duty is (idle_duty, start_duty, check_duty1, check_duty2, done_duty);
	signal duty_reg, duty_next : state_duty;

	type state_pwm is (idle_pwm, start_pwm1, on_pwm, off_pwm);
	signal reg1, next1 : state_pwm;

	type state_vary is (idle_vary, start_vary, next_vary1, next_vary2, next_vary3, done_vary);
	signal vary_reg, vary_next : state_vary;

	type state_pwm1 is (pwm_idle, pwm_idle1, pwm_idle2, pwm_start, pwm_next, pwm_next2, pwm_done);
	signal state_pwm_reg, state_pwm_next: state_pwm1;

--	tune_array is array(0 to 7) of std_logic_vector(7 downto 0);
--  array of 

	signal mykeys: keys;
	signal temp_key: key_array;
	signal temp_key2: key_array;
--	signal in_tune: std_logic_vector(7 downto 0);

	signal notes: notes_array;		-- array of notes(0)->(95)
	
	signal time_delay_reg1, time_delay_next1: signed(23 downto 0);
	signal time_delay_reg2, time_delay_next2: unsigned(24 downto 0);
	signal time_delay_reg3, time_delay_next3: unsigned(23 downto 0);
	signal start_pwm: std_logic;
	signal next_note: std_logic;
	signal sOn_Time: signed(19 downto 0);
	signal sOff_Time: signed(19 downto 0);
	signal iwhole_period: signed(19 downto 0);
	signal ihalf_period: signed(19 downto 0);
	signal iquarter_period: signed(19 downto 0);
	signal isixteen_period: signed(19 downto 0);
	signal whole_period: std_logic_vector(19 downto 0);
	signal half_period: std_logic_vector(19 downto 0);
	signal quarter_period: std_logic_vector(19 downto 0);
	signal sixteen_period: std_logic_vector(19 downto 0);
	signal skip: std_logic;
	signal tune_done: std_logic;
	signal note_index: signed(7 downto 0);
	signal tune_index: signed(7 downto 0);
	signal mynote_index: signed(7 downto 0);
	signal slength: signed(7 downto 0);
	signal sDutyCycle: std_logic_vector(2 downto 0);
	signal which_pattern: std_logic_vector(2 downto 0);
	signal next_pattern: std_logic_vector(2 downto 0);
	signal note_len: unsigned(24 downto 0);
begin

play_note: process(clk, reset, vary_reg)
variable itemp: integer range 0 to 528290:= C6;
begin
	if reset = '0' then
		whole_period <= (others=>'0');
		sOn_Time <= (others=>'0');
		sOff_Time <= (others=>'0');
		time_delay_reg3 <= (others=>'0');
		time_delay_next3 <= (others=>'0');
		quarter_period <= (others=>'0');
		half_period <= (others=>'0');
		iquarter_period <= (others=>'0');
		ihalf_period <= (others=>'0');
		iwhole_period <= (others=>'0');
		sDutyCycle <= "000";
		
	elsif clk'event and clk = '1' then
		case vary_reg is
			when idle_vary =>
				if next_note = '1' then
					sDutyCycle <= duty_cycle;
					vary_next <= start_vary;
				end if;	
			when start_vary =>
				itemp := conv_integer(notes(conv_integer(note_index)));
				whole_period <= conv_std_logic_vector(itemp,20);
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
					-- sOn_Time <= iwhole_period - ihalf_period - iquarter_period;
					-- sOff_Time <= iwhole_period - ihalf_period + iquarter_period;
				-- elsif sDutyCycle = "10" then
					-- sOn_Time <= iwhole_period - ihalf_period + iquarter_period;
					-- sOff_Time <= iwhole_period - ihalf_period - iquarter_period;
				-- else	
				-- end if;
				vary_next <= idle_vary;
		end case;
		vary_reg <= vary_next;
		time_delay_reg3 <= time_delay_next3;
	end if;
end process;
		
play_tune: process(clk, reset,state_pwm_reg, key)
--variable note_index: integer range 0 to 71:= 30;
--variable tune_index: integer range 0 to 15:= 0;
--variable mynote_index: integer range 0 to 15:= 0;
variable temp: integer range 0 to 15:= 0;

begin
	if reset = '0' then
		state_pwm_reg <= pwm_idle;
		state_pwm_next <= pwm_idle;
		time_delay_reg2 <= (others=>'0');
		time_delay_next2 <= (others=>'0');
		start_pwm <= '0';
		skip <= '0';
		next_note <= '0';
		temp_key <= (others=>(others=>'0'));
		temp_key2 <= (others=>(others=>'0'));
		note_index <= (others=>'0');
		slength <= (others=>'0');
		
		notes <= load_notes(1);

		mykeys(0) <= load_keyOfC(1);
		mykeys(1) <= load_keyOfCS(1);
		mykeys(2) <= load_keyOfD(1);
		mykeys(3) <= load_keyOfDS(1);
		mykeys(4) <= load_keyOfE(1);
		mykeys(5) <= load_keyOfF(1);
		mykeys(6) <= load_keyOfFS(1);
		mykeys(7) <= load_keyOfG(1);
		mykeys(8) <= load_keyOfGS(1);
		mykeys(9) <= load_keyOfA(1);
		mykeys(10) <= load_keyOfAS(1);
		mykeys(11) <= load_keyOfB(1);

		tune_done <= '0';
		done <= '0';
		mynote_index <= (others=>'0');
		tune_index <= (others=>'0');
		which_pattern <= "000";
		next_pattern <= "000";
-- goes threw current tune passed in by calling program and plays all 6 notes 
-- then waits for next 'start' signal from calling program

	else if clk'event and clk = '1' then
		case state_pwm_reg is
			when pwm_idle =>
				if start = '1' then		-- wait for calling program to start the whole process
					slength <= len;
					if slength > 41 then
						slength <= X"29";
					end if;
					tune_index <= conv_signed(conv_integer(key),8);	-- get index from calling program
					temp := conv_integer(tune_index);
					temp_key <= mykeys(temp);
					done <= '0';
					state_pwm_next <= pwm_idle1;
				end if;	
				tune_done <= '0';

			when pwm_idle1 =>
				if which_pattern = "000" then
					temp_key2 <= load_pattern1(temp_key);
				elsif which_pattern = "001" then
					temp_key2 <= load_pattern2(temp_key);
				elsif which_pattern = "010" then
					temp_key2 <= load_pattern2a(temp_key);
				elsif which_pattern = "011" then
					temp_key2 <= load_pattern3(temp_key);
				elsif which_pattern = "100" then
					temp_key2 <= load_pattern3a(temp_key);
				elsif which_pattern = "101" then
					temp_key2 <= load_pattern4(temp_key);
				elsif which_pattern = "110" then
					temp_key2 <= load_pattern4a(temp_key);
				end if;
				if note_len_index = "000" then
					note_len <= conv_unsigned(TIME_DELAY5a,25);
				elsif note_len_index = "001" then
					note_len <= conv_unsigned(TIME_DELAY6,25);
				elsif note_len_index = "010" then
					note_len <= conv_unsigned(TIME_DELAY6a,25);
				elsif note_len_index = "011" then
					note_len <= conv_unsigned(TIME_DELAY6b,25);
				elsif note_len_index = "100" then
					note_len <= conv_unsigned(TIME_DELAY6a,25);
				elsif note_len_index = "101" then
					note_len <= conv_unsigned(TIME_DELAY6b,25);
				elsif note_len_index = "110" then
					note_len <= conv_unsigned(TIME_DELAY6,25);
				end if;
				which_pattern <= next_pattern;
				state_pwm_next <= pwm_idle2;
			when pwm_idle2 =>
				case which_pattern is
					when "000" => next_pattern <= "001";
					when "001" => next_pattern <= "010";
					when "010" => next_pattern <= "011";
					when "011" => next_pattern <= "100";
					when "100" => next_pattern <= "101";
					when "101" => next_pattern <= "110";
					when "110" => next_pattern <= "000";
--					when "111" => next_pattern <= "000";
					when others => next_pattern <= "000";
				end case;	
				state_pwm_next <= pwm_start;
				
			when pwm_start =>
				
				note_index <= conv_signed(conv_integer(temp_key2(conv_integer(mynote_index))),8);
				
				-- one of the tunes 1->6 of which there are 12 notes to play				

				skip <= not skip;
				if skip = '1' then
					if mynote_index > slength-1 then
						mynote_index <= (others=>'0');
						tune_done <= '1';
					else
						mynote_index <= mynote_index + 1;
					end if;
				end if;
				
				state_pwm_next <= pwm_next;
				
			when pwm_next =>
				next_note <= '1';		-- trigger the play_note process which sets the on/off times
				start_pwm <= '1';
				state_pwm_next <= pwm_next2;

			when pwm_next2 =>
				next_note <= '0';
--				if time_delay_reg2 > TIME_DELAY5a then
				if time_delay_reg2 > note_len then
					time_delay_next2 <= (others=>'0');
					state_pwm_next <= pwm_done;
				else
					time_delay_next2 <= time_delay_reg2 + 1;
				end if;
			when pwm_done =>
				if tune_done = '1' then
					state_pwm_next <= pwm_idle;
					start_pwm <= '0';
					done <= '1';
				else	
					state_pwm_next <= pwm_start;
				end if;
 		end case;
		time_delay_reg2 <= time_delay_next2;
		state_pwm_reg <= state_pwm_next;
		end if;
	end if;
end process;

-- pulses the output signal (pwm_signal) according to sOff_Time and sOn_Time
-- this process waits on start_pwm 
pwm_proc: process(clk,reset,reg1,start_pwm)
begin
	if reset = '0' then
		reg1 <= idle_pwm;
		next1 <= idle_pwm;
		pwm_signal_1 <= '0';
		pwm_signal_2 <= '0';
		time_delay_next1 <= (others=>'0');
		time_delay_reg1 <= (others=>'0');
	elsif clk'event and clk = '1' then
		case reg1 is
			when idle_pwm =>
				if start_pwm = '1' then
					next1 <= start_pwm1;
				end if;
			when start_pwm1 =>
				pwm_signal_1 <= '1';
				pwm_signal_2 <= '1';
				next1 <= on_pwm;
			when on_pwm =>
				if time_delay_reg1 > sOn_Time then
					time_delay_next1 <= (others=>'0');
					next1 <= off_pwm;
					pwm_signal_1 <= '0';
					pwm_signal_2 <= '0';
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

end pwm_arch;
