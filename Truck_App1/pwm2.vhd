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
		duty_cycle: in std_logic_vector(5 downto 0);
		tune1: in tune_array;
		note_len: in std_logic_vector(7 downto 0);
		tune_ptr: in signed(2 downto 0)
		);
end pwm2;

architecture pwm_arch of pwm2 is

	type state_duty is (idle_duty, start_duty, check_duty1, check_duty2, done_duty);
	signal duty_reg, duty_next : state_duty;

	type state_pwm is (idle_pwm, start_pwm1, on_pwm, off_pwm);
	signal reg1, next1 : state_pwm;

	type state_pwm2 is (idle_pwm2, start_pwm12, on_pwm2, off_pwm2);
	signal reg12, next12 : state_pwm2;

	type state_vary is (idle_vary, start_vary, next_vary1, next_vary2, next_vary3, done_vary);
	signal vary_reg, vary_next : state_vary;

	type state_varya is (idle_varya, start_varya, next_varya1, next_varya2, next_varya3, done_varya);
	signal vary_rega, vary_nexta : state_varya;

	type state_pwm1 is (pwm_idle, pwm_start, pwm_next, pwm_next2, pwm_done);
	signal state_pwm_reg, state_pwm_next: state_pwm1;
	signal stemp: signed(2 downto 0);

	signal notes: notes_array;		-- array of notes(0)->(95)
	
	signal time_delay_reg1, time_delay_next1: signed(23 downto 0);
	signal time_delay_reg2, time_delay_next2: unsigned(24 downto 0);
	signal time_delay_reg3, time_delay_next3: unsigned(23 downto 0);
	signal start_pwm: std_logic;
	signal next_note: std_logic;
	signal sOn_Time1: signed(19 downto 0);
	signal sOff_Time1: signed(19 downto 0);
	signal sOn_Time2: signed(19 downto 0);
	signal sOff_Time2: signed(19 downto 0);

	signal iwhole_period1: signed(19 downto 0);
	signal ihalf_period1: signed(19 downto 0);
	signal iquarter_period1: signed(19 downto 0);
	signal isixteen_period1: signed(19 downto 0);
	signal whole_period1: std_logic_vector(19 downto 0);
	signal half_period1: std_logic_vector(19 downto 0);
	signal quarter_period1: std_logic_vector(19 downto 0);
	signal sixteen_period1: std_logic_vector(19 downto 0);

	signal iwhole_period2: signed(19 downto 0);
	signal ihalf_period2: signed(19 downto 0);
	signal iquarter_period2: signed(19 downto 0);
	signal isixteen_period2: signed(19 downto 0);
	signal whole_period2: std_logic_vector(19 downto 0);
	signal half_period2: std_logic_vector(19 downto 0);
	signal quarter_period2: std_logic_vector(19 downto 0);
	signal sixteen_period2: std_logic_vector(19 downto 0);

	signal skip: std_logic;
	signal skip2: std_logic;
	signal tune_done: std_logic;
	signal note_index1: signed(7 downto 0);
	signal note_index2: signed(7 downto 0);
	signal tune_index: signed(7 downto 0);
	signal mynote_index: signed(7 downto 0);
	signal slength: signed(7 downto 0);
	signal soctave: signed(7 downto 0);
	signal sDutyCycle1: std_logic_vector(2 downto 0);
	signal sDutyCycle2: std_logic_vector(2 downto 0);
	signal stlv_note_len: std_logic_vector(24 downto 0);
	signal sNoteLen: unsigned(24 downto 0);
--	signal tune2: tune_array;
begin

play_note1: process(clk, reset, vary_reg)
variable itemp: integer range 0 to 528290:= C6;
begin
	if reset = '0' then
		whole_period1 <= (others=>'0');
		sOn_Time1 <= (others=>'0');
		sOff_Time1 <= (others=>'0');
--		time_delay_reg3 <= (others=>'0');
--		time_delay_next3 <= (others=>'0');
		quarter_period1 <= (others=>'0');
		half_period1 <= (others=>'0');
		iquarter_period1 <= (others=>'0');
		ihalf_period1 <= (others=>'0');
		iwhole_period1 <= (others=>'0');
		sDutyCycle1 <= "111";
		
	elsif clk'event and clk = '1' then
		case vary_reg is
			when idle_vary =>
				if next_note = '1' then
					sDutyCycle1 <= duty_cycle(2 downto 0);
					vary_next <= start_vary;
				end if;	
			when start_vary =>
				itemp := conv_integer(notes(conv_integer(note_index1)));
				whole_period1 <= conv_std_logic_vector(itemp,20);
				vary_next <= next_vary1;
			when next_vary1 =>
				half_period1 <= '0' & whole_period1(19 downto 1);
				quarter_period1 <= '0' & half_period1(19 downto 1);
				sixteen_period1 <= '0' & quarter_period1(19 downto 1);
				vary_next <= next_vary2;
			when next_vary2 =>
--				quarter_period11 <= "0" & quarter_period11(19 downto 1);
				vary_next <= next_vary3;
			when next_vary3 =>
				iwhole_period1 <= conv_signed(conv_integer(whole_period1),20);
				ihalf_period1 <= conv_signed(conv_integer(half_period1),20);
				iquarter_period1 <= conv_signed(conv_integer(quarter_period1),20);
				isixteen_period1 <= conv_signed(conv_integer(sixteen_period1),20);
				vary_next <= done_vary;
			when done_vary =>
				case sDutyCycle1 is
					when "000" =>
						-- 12% duty_cycle
						sOn_Time1 <= iwhole_period1 - ihalf_period1 - iquarter_period1 - isixteen_period1;
						sOff_Time1 <= iwhole_period1 - ihalf_period1 + iquarter_period1 + isixteen_period1;
					when "001" =>
						-- 25% duty_cycle
						sOn_Time1 <= iwhole_period1 - ihalf_period1 - iquarter_period1;
						sOff_Time1 <= iwhole_period1 - ihalf_period1 + iquarter_period1;
					when "010" =>
						-- 30% duty_cycle
						sOn_Time1 <= iwhole_period1 - ihalf_period1 - iquarter_period1 + isixteen_period1;
						sOff_Time1 <= iwhole_period1 - ihalf_period1 + iquarter_period1 - isixteen_period1;
					when "011" =>
						-- square wave
						sOn_Time1 <= iwhole_period1 - ihalf_period1;
						sOff_Time1 <= iwhole_period1 - ihalf_period1;
					when "100" =>
						-- 60% duty_cycle
						sOn_Time1 <= iwhole_period1 - ihalf_period1 + iquarter_period1 - isixteen_period1;
						sOff_Time1 <= iwhole_period1 - ihalf_period1 - iquarter_period1 + isixteen_period1;
					when "101" =>
						-- 75% duty_cycle
						sOn_Time1 <= iwhole_period1 - ihalf_period1 + iquarter_period1;
						sOff_Time1 <= iwhole_period1 - ihalf_period1 - iquarter_period1;
					when "110" =>
						-- 80% duty_cycle
						sOn_Time1 <= iwhole_period1 - ihalf_period1 + iquarter_period1 + isixteen_period1;
						sOff_Time1 <= iwhole_period1 - ihalf_period1 - iquarter_period1 - isixteen_period1;
					when others =>	
						sOn_Time1 <= iwhole_period1 - ihalf_period1;
						sOff_Time1 <= iwhole_period1 - ihalf_period1;
				end case;
					-- sOn_Time <= iwhole_period1 - ihalf_period1 - iquarter_period1;
					-- sOff_Time <= iwhole_period1 - ihalf_period1 + iquarter_period1;
				-- elsif sDutyCycle = "10" then
					-- sOn_Time <= iwhole_period1 - ihalf_period1 + iquarter_period1;
					-- sOff_Time <= iwhole_period1 - ihalf_period1 - iquarter_period1;
				-- else	
				-- end if;
				vary_next <= idle_vary;
		end case;
		vary_reg <= vary_next;
--		time_delay_reg3 <= time_delay_next3;
	end if;
end process;
		
play_note2: process(clk, reset, vary_rega)
variable itemp: integer range 0 to 528290:= C6;
begin
	if reset = '0' then
		whole_period2 <= (others=>'0');
		sOn_Time2 <= (others=>'0');
		sOff_Time2 <= (others=>'0');
--		time_delay_reg3 <= (others=>'0');
--		time_delay_next3 <= (others=>'0');
		quarter_period2 <= (others=>'0');
		half_period2 <= (others=>'0');
		iquarter_period2 <= (others=>'0');
		ihalf_period2 <= (others=>'0');
		iwhole_period2 <= (others=>'0');
		sDutyCycle2 <= "111";
		
	elsif clk'event and clk = '1' then
		case vary_rega is
			when idle_varya =>
				if next_note = '1' then
					sDutyCycle2 <= duty_cycle(5 downto 3);
					vary_nexta <= start_varya;
				end if;	
			when start_varya =>
				itemp := conv_integer(notes(conv_integer(note_index2)));
				whole_period2 <= conv_std_logic_vector(itemp,20);
				vary_nexta <= next_varya1;
			when next_varya1 =>
				half_period2 <= '0' & whole_period2(19 downto 1);
				quarter_period2 <= '0' & half_period2(19 downto 1);
				sixteen_period2 <= '0' & quarter_period2(19 downto 1);
				vary_nexta <= next_varya2;
			when next_varya2 =>
--				quarter_period2 <= "0" & quarter_period2(19 downto 1);
				vary_nexta <= next_varya3;
			when next_varya3 =>
				iwhole_period2 <= conv_signed(conv_integer(whole_period2),20);
				ihalf_period2 <= conv_signed(conv_integer(half_period2),20);
				iquarter_period2 <= conv_signed(conv_integer(quarter_period2),20);
				isixteen_period2 <= conv_signed(conv_integer(sixteen_period2),20);
				vary_nexta <= done_varya;
			when done_varya =>
				case sDutyCycle2 is
					when "000" =>
						-- 12% duty_cycle
						sOn_Time2 <= iwhole_period2 - ihalf_period2 - iquarter_period2 - isixteen_period2;
						sOff_Time2 <= iwhole_period2 - ihalf_period2 + iquarter_period2 + isixteen_period2;
					when "001" =>
						-- 25% duty_cycle
						sOn_Time2 <= iwhole_period2 - ihalf_period2 - iquarter_period2;
						sOff_Time2 <= iwhole_period2 - ihalf_period2 + iquarter_period2;
					when "010" =>
						-- 30% duty_cycle
						sOn_Time2 <= iwhole_period2 - ihalf_period2 - iquarter_period2 + isixteen_period2;
						sOff_Time2 <= iwhole_period2 - ihalf_period2 + iquarter_period2 - isixteen_period2;
					when "011" =>
						-- square wave
						sOn_Time2 <= iwhole_period2 - ihalf_period2;
						sOff_Time2 <= iwhole_period2 - ihalf_period2;
					when "100" =>
						-- 60% duty_cycle
						sOn_Time2 <= iwhole_period2 - ihalf_period2 + iquarter_period2 - isixteen_period2;
						sOff_Time2 <= iwhole_period2 - ihalf_period2 - iquarter_period2 + isixteen_period2;
					when "101" =>
						-- 75% duty_cycle
						sOn_Time2 <= iwhole_period2 - ihalf_period2 + iquarter_period2;
						sOff_Time2 <= iwhole_period2 - ihalf_period2 - iquarter_period2;
					when "110" =>
						-- 80% duty_cycle
						sOn_Time2 <= iwhole_period2 - ihalf_period2 + iquarter_period2 + isixteen_period2;
						sOff_Time2 <= iwhole_period2 - ihalf_period2 - iquarter_period2 - isixteen_period2;
					when others =>	
						sOn_Time2 <= iwhole_period2 - ihalf_period2;
						sOff_Time2 <= iwhole_period2 - ihalf_period2;
				end case;
					-- sOn_Time <= iwhole_period1 - ihalf_period1 - iquarter_period1;
					-- sOff_Time <= iwhole_period1 - ihalf_period1 + iquarter_period1;
				-- elsif sDutyCycle = "10" then
					-- sOn_Time <= iwhole_period1 - ihalf_period1 + iquarter_period1;
					-- sOff_Time <= iwhole_period1 - ihalf_period1 - iquarter_period1;
				-- else	
				-- end if;
				vary_nexta <= idle_varya;
		end case;
		vary_rega <= vary_nexta;
--		time_delay_reg3 <= time_delay_next3;
	end if;
end process;

play_tune: process(clk, reset,state_pwm_reg, tune_ptr)
--variable note_index: integer range 0 to 71:= 30;
--variable tune_index: integer range 0 to 15:= 0;
--variable mynote_index: integer range 0 to 15:= 0;
variable temp: integer range 0 to 15:= 0;
variable temp1: integer range 0 to 95:= 0;
variable temp2: integer range 0 to 95:= 0;
variable temp3: integer range 0 to 95:= 0;
begin
	if reset = '0' then
		state_pwm_reg <= pwm_idle;
		state_pwm_next <= pwm_idle;
		time_delay_reg2 <= (others=>'0');
		time_delay_next2 <= (others=>'0');
		start_pwm <= '0';
		skip <= '0';
		skip2 <= '0';
		next_note <= '0';
		note_index1 <= (others=>'0');
		note_index2 <= (others=>'0');
		slength <= (others=>'0');
		
		notes <= load_notes_array(notes);
--		tune1 <= load_tune_array(tune1);

		tune_done <= '0';
		done <= '0';
		note_index1 <= conv_signed(tune1(0,0),8);
		note_index2 <= conv_signed(tune1(0,1),8);
		tune_index <= (others=>'0');
		
-- goes threw current tune passed in by calling program and plays all 6 notes 
-- then waits for next 'start' signal from calling program

	else if clk'event and clk = '1' then
		case state_pwm_reg is
			when pwm_idle =>
				if start = '1' then		-- wait for calling program to start the whole process
					slength <= len;
					if slength > 20 then
						slength <= X"14";
					end if;
					tune_index <= conv_signed(conv_integer(tune_ptr),8);	-- get index from calling program
					temp := conv_integer(tune_index);
--					temp_key <= mykeys(temp);
					state_pwm_next <= pwm_start;
					done <= '0';
					stlv_note_len <= '0' & note_len & X"FFFF";
 					sNoteLen <= unsigned(stlv_note_len);
				end if;
				tune_done <= '0';

			when pwm_start =>
				
				skip2 <= not skip2;
				if skip2 = '1' then
					temp1 := tune1(conv_integer(mynote_index),0);
					temp2 := tune1(conv_integer(mynote_index),1);
					temp3 := tune1(conv_integer(mynote_index),1);
					note_index1 <= conv_signed(temp1,8);
					note_index2 <= conv_signed(temp2,8);
					stemp <= conv_signed(temp3,3);
					-- sDutyCycle1 <= conv_std_logic_vector(stemp,3);
					-- sDutyCycle2 <= conv_std_logic_vector(stemp,3);
				end if;

--				note_index1 <= conv_signed(conv_integer(temp_key(conv_integer(mynote_index1))),8);
				
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
				next_note <= '1'; -- trigger the play_note process which sets the on/off times
				start_pwm <= '1';
				state_pwm_next <= pwm_next2;

			when pwm_next2 =>
				next_note <= '0';
--				if time_delay_reg2 > TIME_DELAY7 then
				if time_delay_reg2 > sNoteLen then
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
				next1 <= on_pwm;
			when on_pwm =>
				if time_delay_reg1 > sOn_Time1 then
					time_delay_next1 <= (others=>'0');
					next1 <= off_pwm;
					pwm_signal_1 <= '0';
				else
					time_delay_next1 <= time_delay_reg1 + 1;
				end if;
			when off_pwm =>
				if time_delay_reg1 > sOff_Time1 then
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

--	type state_pwm2 is (idle_pwm2, start_pwm12, on_pwm2, off_pwm2);
--	signal reg12, next12 : state_pwm2;

pwm_proc2: process(clk,reset,reg12,start_pwm)
begin
	if reset = '0' then
		reg12 <= idle_pwm2;
		next12 <= idle_pwm2;
		pwm_signal_2 <= '0';
		time_delay_reg3 <= (others=>'0');
		time_delay_next3 <= (others=>'0');
	elsif clk'event and clk = '1' then
		case reg12 is
			when idle_pwm2 =>
				if start_pwm = '1' then
					next12 <= start_pwm12;
				end if;
			when start_pwm12 =>
				pwm_signal_2 <= '1';
				next12 <= on_pwm2;
			when on_pwm2 =>
				if time_delay_reg3 > sOn_Time2 then
					time_delay_next3 <= (others=>'0');
					next12 <= off_pwm2;
					pwm_signal_2 <= '0';
				else
					time_delay_next3 <= time_delay_reg3 + 1;
				end if;
			when off_pwm2 =>
				if time_delay_reg3 > sOff_Time2 then
					time_delay_next3 <= (others=>'0');
					next12 <= idle_pwm2;
				else
					time_delay_next3 <= time_delay_reg3 + 1;
				end if;
		end case;
		time_delay_reg3 <= time_delay_next3;
		reg12 <= next12;
	end if;
end process;

end pwm_arch;
