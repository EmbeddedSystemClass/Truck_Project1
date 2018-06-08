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

entity dtmf is
	port(
		clk, reset: in std_logic;
		dtmf_index: in std_logic_vector(4 downto 0);
		special_cmd: in std_logic;
		dtmf_signal_1: out std_logic;
		dtmf_signal_2: out std_logic;
		start: in std_logic;
		done: out std_logic);
end dtmf;

architecture dtmf_arch of dtmf is

	type state_duty is (idle_duty, start_duty, check_duty1, check_duty2, done_duty);
	signal duty_reg, duty_next : state_duty;

	type state_dtmf is (idle_dtmf, start_dtmf1, on_dtmf, off_dtmf);
	signal reg1, next1 : state_dtmf;

	type state_dtmf2 is (idle_dtmf2, start_dtmf12, on_dtmf2, off_dtmf2);
	signal reg2, next2 : state_dtmf2;

	type state_dtmf1 is (dtmf_idle, dtmf_next, dtmf_next2, dtmf_done);
	signal state_dtmf_reg, state_dtmf_next: state_dtmf1;

	type state_vary is (idle_vary, start_vary, next_vary1, next_vary2, next_vary3, modify_tone, done_vary);
	signal vary_reg, vary_next : state_vary;

	signal time_delay_reg1, time_delay_next1: signed(23 downto 0);
	signal time_delay_reg1a, time_delay_next1a: signed(23 downto 0);
	signal time_delay_reg2, time_delay_next2: unsigned(25 downto 0);
	signal time_delay_reg3, time_delay_next3: unsigned(23 downto 0);
	signal start_dtmf: std_logic;
	signal next_note: std_logic;
	signal sOn_Time1: signed(19 downto 0);
	signal sOff_Time1: signed(19 downto 0);
	signal sOn_Time2: signed(19 downto 0);
	signal sOff_Time2: signed(19 downto 0);
	signal skip: std_logic;
	signal tune_done: std_logic;
	signal whole_period1: std_logic_vector(19 downto 0);
	signal half_period1: std_logic_vector(19 downto 0);
	signal whole_period2: std_logic_vector(19 downto 0);
	signal half_period2: std_logic_vector(19 downto 0);
	signal iwhole_period1: signed(19 downto 0);
	signal ihalf_period1: signed(19 downto 0);
	signal iwhole_period2: signed(19 downto 0);
	signal ihalf_period2: signed(19 downto 0);
	signal dtmf: dtmf_array;
	signal reverse: std_logic;
begin

play_note: process(clk, reset, vary_reg)
variable itemp1: integer range 0 to 33000:= 0;
variable itemp2: integer range 0 to 33000:= 0;
begin
	if reset = '0' then
		sOn_Time1 <= (others=>'0');
		sOff_Time1 <= (others=>'0');
		sOn_Time2 <= (others=>'0');
		sOff_Time2 <= (others=>'0');
		time_delay_reg3 <= (others=>'0');
		time_delay_next3 <= (others=>'0');
		ihalf_period1 <= (others=>'0');
		iwhole_period1 <= (others=>'0');
		whole_period1 <= (others=>'0');
		half_period1 <= (others=>'0');
		ihalf_period2 <= (others=>'0');
		iwhole_period2 <= (others=>'0');
		whole_period2 <= (others=>'0');
		half_period2 <= (others=>'0');
--		dtmf <= load_dtmf(conv_integer("0000"));
		dtmf <= (others=>(others=>'0'));
		reverse <= '0';
		
	elsif clk'event and clk = '1' then
		case vary_reg is
			when idle_vary =>
				if next_note = '1' then
					vary_next <= start_vary;
				end if;	
			when start_vary =>
				if special_cmd = '0' then
					dtmf <= load_dtmf(conv_integer(dtmf_index));
					itemp1 := conv_integer(dtmf(0));
					whole_period1 <= conv_std_logic_vector(itemp1,20);
					itemp2 := conv_integer(dtmf(1));
					whole_period2 <= conv_std_logic_vector(itemp2,20);
				else
					whole_period1 <= conv_std_logic_vector(G4,20);
					whole_period2 <= conv_std_logic_vector(GS4,20);
				end if;	
				vary_next <= next_vary1;
			when next_vary1 =>
				half_period1 <= '0' & whole_period1(19 downto 1);
				half_period2 <= '0' & whole_period2(19 downto 1);
				vary_next <= next_vary2;
			when next_vary2 =>
				iwhole_period1 <= conv_signed(conv_integer(whole_period1),20);
				ihalf_period1 <= conv_signed(conv_integer(half_period1),20);
				vary_next <= next_vary3;
			when next_vary3 =>
				iwhole_period2 <= conv_signed(conv_integer(whole_period2),20);
				ihalf_period2 <= conv_signed(conv_integer(half_period2),20);
				vary_next <= modify_tone;
			when modify_tone =>	
				sOn_Time1 <= iwhole_period1 - ihalf_period1;
				sOff_Time1 <= iwhole_period1 - ihalf_period1;
				sOn_Time2 <= iwhole_period2 - ihalf_period2;
				sOff_Time2 <= iwhole_period2 - ihalf_period2;
				if special_cmd = '0' then
					vary_next <= idle_vary;
				else vary_next <= done_vary;
				end if;
			when done_vary =>
				if time_delay_reg3 > TIME_DELAY9/8 then
					time_delay_next3 <= (others=>'0');
					sOn_Time1 <= sOn_Time1 - 2;
					sOn_Time2 <= sOn_Time2 - 2;
					sOff_Time1 <= sOff_Time1 - 1;
					sOff_Time2 <= sOff_Time2 - 1;
				else
					time_delay_next3 <= time_delay_reg3 + 1;
					if start = '0' then
						time_delay_next3 <= (others=>'0');
						vary_next <= idle_vary;
					end if;	
				end if;
		end case;
		vary_reg <= vary_next;
		time_delay_reg3 <= time_delay_next3;
	end if;
end process;
		
play_tune: process(clk, reset,state_dtmf_reg)
begin
	if reset = '0' then
		state_dtmf_reg <= dtmf_idle;
		state_dtmf_next <= dtmf_idle;
		time_delay_reg2 <= (others=>'0');
		time_delay_next2 <= (others=>'0');
		start_dtmf <= '0';
		skip <= '0';
		next_note <= '0';
		tune_done <= '0';
		done <= '0';

	else if clk'event and clk = '1' then
		case state_dtmf_reg is
			when dtmf_idle =>
				if start = '1' then		-- wait for calling program to start the whole process
					done <= '0';
					state_dtmf_next <= dtmf_next;
				end if;	
				tune_done <= '0';
				
			when dtmf_next =>
				next_note <= '1';		-- trigger the play_note process which sets the on/off times
				start_dtmf <= '1';
				state_dtmf_next <= dtmf_next2;
			when dtmf_next2 =>
				next_note <= '0';
				if time_delay_reg2 > TIME_DELAY5a then
					time_delay_next2 <= (others=>'0');
					state_dtmf_next <= dtmf_done;
				else
					time_delay_next2 <= time_delay_reg2 + 1;
					if start = '0' then
						time_delay_next2 <= (others=>'0');
						state_dtmf_next <= dtmf_done;
					end if;	
				end if;
			when dtmf_done =>
				state_dtmf_next <= dtmf_idle;
				start_dtmf <= '0';
				done <= '1';
 		end case;
		time_delay_reg2 <= time_delay_next2;
		state_dtmf_reg <= state_dtmf_next;
		end if;
	end if;
end process;

-- pulses the output signal (dtmf_signal) according to sOff_Time and sOn_Time
-- this process waits on start_dtmf 
dtmf1_proc: process(clk,reset,reg1,start_dtmf)
begin
	if reset = '0' then
		reg1 <= idle_dtmf;
		next1 <= idle_dtmf;
		dtmf_signal_1 <= '0';
		time_delay_next1 <= (others=>'0');
		time_delay_reg1 <= (others=>'0');
	elsif clk'event and clk = '1' then
		case reg1 is
			when idle_dtmf =>
				if start_dtmf = '1' then
					next1 <= start_dtmf1;
				else
					dtmf_signal_1 <= '0';
				end if;
			when start_dtmf1 =>
				dtmf_signal_1 <= '1';
				next1 <= on_dtmf;
			when on_dtmf =>
				if time_delay_reg1 > sOn_Time1 then
					time_delay_next1 <= (others=>'0');
					next1 <= off_dtmf;
					dtmf_signal_1 <= '0';
				else
					time_delay_next1 <= time_delay_reg1 + 1;
				end if;
			when off_dtmf =>
				if time_delay_reg1 > sOff_Time1 then
					time_delay_next1 <= (others=>'0');
					next1 <= idle_dtmf;
				else
					time_delay_next1 <= time_delay_reg1 + 1;
				end if;
		end case;
		time_delay_reg1 <= time_delay_next1;
		reg1 <= next1;
	end if;
end process;

dtmf2_proc: process(clk,reset,reg2,start_dtmf)
begin
	if reset = '0' then
		reg2 <= idle_dtmf2;
		next2 <= idle_dtmf2;
		dtmf_signal_2 <= '0';
		time_delay_next1a <= (others=>'0');
		time_delay_reg1a <= (others=>'0');
	elsif clk'event and clk = '1' then
		case reg2 is
			when idle_dtmf2 =>
				if start_dtmf = '1' then
					next2 <= start_dtmf12;
				else
					dtmf_signal_2 <= '0';
				end if;
			when start_dtmf12 =>
				dtmf_signal_2 <= '1';
				next2 <= on_dtmf2;
			when on_dtmf2 =>
				if time_delay_reg1a > sOn_Time2 then
					time_delay_next1a <= (others=>'0');
					next2 <= off_dtmf2;
					dtmf_signal_2 <= '0';
				else
					time_delay_next1a <= time_delay_reg1a + 1;
				end if;
			when off_dtmf2 =>
				if time_delay_reg1a > sOff_Time2 then
					time_delay_next1a <= (others=>'0');
					next2 <= idle_dtmf2;
				else
					time_delay_next1a <= time_delay_reg1a + 1;
				end if;
		end case;
		time_delay_reg1a <= time_delay_next1a;
		reg2 <= next2;
	end if;
end process;

end dtmf_arch;
