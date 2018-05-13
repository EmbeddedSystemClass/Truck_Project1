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

entity pwm1 is
	port(
		clk, reset: in std_logic;
		pwm_signal: out std_logic;
		start: in std_logic;
		tune: in std_logic_vector(7 downto 0)
		);
end pwm1;

architecture pwm_arch of pwm1 is

	type state_duty is (idle_duty, start_duty, check_duty1, check_duty2, done_duty);
	signal duty_reg, duty_next : state_duty;

	type state_pwm is (idle_pwm, start_pwm1, on_pwm, off_pwm);
	signal reg1, next1 : state_pwm;

	type state_vary is (idle_vary, start_vary, next_vary1, next_vary2, next_vary3, done_vary);
	signal vary_reg, vary_next : state_vary;

	type state_pwm1 is (pwm_idle, pwm_start, pwm_next, pwm_next2);
	signal state_pwm_reg, state_pwm_next: state_pwm1;

--	tune_array is array(0 to 7) of std_logic_vector(7 downto 0);
--  array of 

	signal mytunes: tunes;			-- array of tune1->tune6
	signal tune1: tune_array;		-- array of 
	signal tune2: tune_array;		-- tune1->6 are
	signal tune3: tune_array;		-- arrays of indexex into notes array
	signal tune4: tune_array;
	signal tune5: tune_array;
	signal tune6: tune_array;
	signal temp_tune: tune_array;
--	signal in_tune: std_logic_vector(7 downto 0);

	signal notes: notes_array;		-- array of notes(0)->(95)
	
	signal time_delay_reg1, time_delay_next1: signed(23 downto 0);
	signal time_delay_reg2, time_delay_next2: unsigned(23 downto 0);
	signal time_delay_reg3, time_delay_next3: unsigned(23 downto 0);
	signal time_delay_reg4, time_delay_next4: unsigned(23 downto 0);
	signal start_pwm: std_logic;
	signal stlv_note_index: std_logic_vector(7 downto 0);
	signal next_note: std_logic;
	signal sOn_Time: signed(19 downto 0);
	signal sOff_Time: signed(19 downto 0);
	signal uVary_Time: unsigned(19 downto 0);
	constant VARY_TIME:  integer:=  TIME_DELAY8b;
	signal temp1: signed(19 downto 0);
	signal temp2: signed(19 downto 0);
	signal temp3: signed(19 downto 0);
	signal temp4: signed(19 downto 0);
	signal stlv_temp: std_logic_vector(19 downto 0);
	signal stlv_temp2: std_logic_vector(19 downto 0);
	signal stlv_temp3: std_logic_vector(19 downto 0);
	signal skip: std_logic;
	signal skip2: std_logic;
	signal duty_dir: std_logic_vector(1 downto 0);
	signal dont_change_duty: std_logic;

begin
-- this is disabled for now - couldn't quite get it to work. 
-- inc/dec temp1 which is added/subtracted to/from the on time and off time
calc_duty_cycle: process(clk, reset, duty_reg)
variable temp: integer range 0 to 528000:= 0;
begin
	if reset = '0' then
		duty_reg <= idle_duty;
		duty_next <= idle_duty;
		time_delay_reg4 <= (others=>'0');
		time_delay_next4 <= (others=>'0');
		temp1 <= (others=>'0');
--		temp1 <= X"014bc";
		temp2 <= (others=>'1');
		stlv_temp2 <= (others=>'0');
		skip2 <= '0';
		duty_dir <= "01";		-- start out increasing duty cycle
		temp4 <= (others=>'0');
		
	elsif clk'event and clk = '1' then
		case duty_reg is
			when idle_duty =>
				if time_delay_reg4 > TIME_DELAY9/8 then
					time_delay_next4 <= (others=>'0');
					duty_next <= start_duty;
--					duty_next <= idle_duty;
				else
					time_delay_next4 <= time_delay_reg4 + 1;
				end if;
			when start_duty =>
					skip2 <= not skip2;
					if skip2 = '1' then
						if duty_dir = "01" then
							temp1 <= temp1 + 200;
						elsif duty_dir = "10" then
							temp1 <= temp1 - 200;
						end if;
					end if;
				duty_next <= check_duty1;
			when check_duty1 =>
				temp4 <= sOn_Time + temp1;
				if duty_dir = "01" and (temp4 > temp3) then
					temp1 <= (others=>'0');
					duty_dir <= "10";
				end if;	
--				duty_next <= check_duty2;
				duty_next <= done_duty;
			when check_duty2 =>	
				temp4 <= sOff_Time + temp1;
				if duty_dir = "10" and temp4 < 100 then
					duty_dir <= "01";
				end if;
				duty_next <= done_duty;
			when done_duty =>
				duty_next <= idle_duty;
		end case;
		duty_reg <= duty_next;
		time_delay_reg4 <= time_delay_next4;
	end if;
end process;

play_note: process(clk, reset, vary_reg)
variable freq1: integer range 0 to 528290:= C6;
variable freq2: integer range 0 to 528290:= C6;
variable freq3: integer range 0 to 528290:= C6;
variable note_index: integer range 0 to 71:= 0;
begin
	if reset = '0' then
		stlv_temp <= (others=>'0');
		sOn_Time <= (others=>'0');
		sOff_Time <= (others=>'0');
		time_delay_reg3 <= (others=>'0');
		time_delay_next3 <= (others=>'0');
		dont_change_duty <= '1';
		stlv_temp3 <= (others=>'0');
		temp3 <= (others=>'0');
		
	elsif clk'event and clk = '1' then
		case vary_reg is
			when idle_vary =>
				if next_note = '1' then
					uVary_Time <= conv_unsigned(VARY_TIME,20);
					vary_next <= start_vary;
				end if;	
			when start_vary =>
				dont_change_duty <= '1';
				note_index := conv_integer(stlv_note_index);
				freq1 := conv_integer(notes(note_index));
				stlv_temp <= conv_std_logic_vector(freq1,20);
				stlv_temp3 <= stlv_temp;
				vary_next <= next_vary1;
			when next_vary1 =>
				stlv_temp <= "00" & stlv_temp(19 downto 2);
				sOn_Time <= conv_signed(conv_integer(stlv_temp),20);
				vary_next <= next_vary2;
			when next_vary2 =>
--				sOn_Time <= sOn_Time + temp1;
				sOn_Time <= sOn_Time;
				temp3 <= conv_signed(conv_integer(stlv_temp3),20);
				vary_next <= next_vary3;
			when next_vary3 =>
				sOff_Time <= temp3 - sOn_Time;
				vary_next <= done_vary;
			when done_vary =>
				stlv_temp3 <= conv_std_logic_vector(sOff_Time + sOn_Time,20);
				vary_next <= idle_vary;
		end case;
		vary_reg <= vary_next;
		time_delay_reg3 <= time_delay_next3;
	end if;
end process;
		
test_pwm: process(clk, reset,state_pwm_reg, tune)
variable note_index: integer range 0 to 71:= 30;
variable tune_index: integer range 0 to 15:= 0;
variable mytune_index: integer range 0 to 5:= 0;
begin
	if reset = '0' then
		state_pwm_reg <= pwm_idle;
		state_pwm_next <= pwm_idle;
		time_delay_reg2 <= (others=>'0');
		time_delay_next2 <= (others=>'0');
		start_pwm <= '0';
		stlv_note_index <= (others=>'0');
--		stlv_note_index <= "01000101";	-- use this when in simulator so the pulses are shorter
		skip <= '0';
		next_note <= '1';
		temp_tune <= (others=>(others=>'0'));
--		in_tune <= tune;
		
		notes(0) <= conv_std_logic_vector(C2,20);	--	0
		notes(1) <= conv_std_logic_vector(CS2,20);	--	1
		notes(2) <= conv_std_logic_vector(D2,20);	--	2
		notes(3) <= conv_std_logic_vector(DS2,20);	--	3
		notes(4) <= conv_std_logic_vector(E2,20);	--	4
		notes(5) <= conv_std_logic_vector(F2,20);	--	5
		notes(6) <= conv_std_logic_vector(FS2,20);	--	6
		notes(7) <= conv_std_logic_vector(G2,20);	--	7
		notes(8) <= conv_std_logic_vector(GS2,20);	--	8
		notes(9) <= conv_std_logic_vector(A2,20);	--	9
		notes(10) <= conv_std_logic_vector(AS2,20);	--	A
		notes(11) <= conv_std_logic_vector(B2,20);	--	B

		notes(12) <= conv_std_logic_vector(C3,20);	--	C
		notes(13) <= conv_std_logic_vector(CS3,20);	--	D
		notes(14) <= conv_std_logic_vector(D3,20);	--	E
		notes(15) <= conv_std_logic_vector(DS3,20);	--	F
		notes(16) <= conv_std_logic_vector(E3,20);	--	10
		notes(17) <= conv_std_logic_vector(F3,20);	--	11
		notes(18) <= conv_std_logic_vector(FS3,20);	--	12
		notes(19) <= conv_std_logic_vector(G3,20);	--	13
		notes(20) <= conv_std_logic_vector(GS3,20);	--	14
		notes(21) <= conv_std_logic_vector(A3,20);	--	15
		notes(22) <= conv_std_logic_vector(AS3,20);	--	16
		notes(23) <= conv_std_logic_vector(B3,20);	--	17

		notes(24) <= conv_std_logic_vector(C4,20);	--	18
		notes(25) <= conv_std_logic_vector(CS4,20);	--	19
		notes(26) <= conv_std_logic_vector(D4,20);	--	1A
		notes(27) <= conv_std_logic_vector(DS4,20);	--	1B
		notes(28) <= conv_std_logic_vector(E4,20);	--	1C
		notes(29) <= conv_std_logic_vector(F4,20);	--	1D
		notes(30) <= conv_std_logic_vector(FS4,20);	--	1E
		notes(31) <= conv_std_logic_vector(G4,20);	--	1F
		notes(32) <= conv_std_logic_vector(GS4,20);	--	20
		notes(33) <= conv_std_logic_vector(A4,20);	--	21
		notes(34) <= conv_std_logic_vector(AS4,20);	--	22
		notes(35) <= conv_std_logic_vector(B4,20);	--	23

		notes(36) <= conv_std_logic_vector(C5,20);	--	24
		notes(37) <= conv_std_logic_vector(CS5,20);	--	25
		notes(38) <= conv_std_logic_vector(D5,20);	--	26
		notes(39) <= conv_std_logic_vector(DS5,20);	--	27
		notes(40) <= conv_std_logic_vector(E5,20);	--	28
		notes(41) <= conv_std_logic_vector(F5,20);	--	29
		notes(42) <= conv_std_logic_vector(FS5,20);	--	2A
		notes(43) <= conv_std_logic_vector(G5,20);	--	2B
		notes(44) <= conv_std_logic_vector(GS5,20);	--	2C
		notes(45) <= conv_std_logic_vector(A5,20);	--	2D
		notes(46) <= conv_std_logic_vector(AS5,20);	--	2E
		notes(47) <= conv_std_logic_vector(B5,20);	--	2F

		notes(48) <= conv_std_logic_vector(C6,20);	--	30
		notes(49) <= conv_std_logic_vector(CS6,20);	--	31
		notes(50) <= conv_std_logic_vector(D6,20);	--	32
		notes(51) <= conv_std_logic_vector(DS6,20);	--	33
		notes(52) <= conv_std_logic_vector(E6,20);	--	34
		notes(53) <= conv_std_logic_vector(F6,20);	--	35
		notes(54) <= conv_std_logic_vector(FS6,20);	--	36
		notes(55) <= conv_std_logic_vector(G6,20);	--	37
		notes(56) <= conv_std_logic_vector(GS6,20);	--	38
		notes(57) <= conv_std_logic_vector(A6,20);	--	39
		notes(58) <= conv_std_logic_vector(AS6,20);	--	3A
		notes(59) <= conv_std_logic_vector(B6,20);	--	3B

		notes(60) <= conv_std_logic_vector(C7,20);	--	3C
		notes(61) <= conv_std_logic_vector(CS7,20);	--	3D
		notes(62) <= conv_std_logic_vector(D7,20);	--	3E
		notes(63) <= conv_std_logic_vector(DS7,20);	--	3F
		notes(64) <= conv_std_logic_vector(E7,20);	--	40
		notes(65) <= conv_std_logic_vector(F7,20);	--	41
		notes(66) <= conv_std_logic_vector(FS7,20);	--	42
		notes(67) <= conv_std_logic_vector(G7,20);	--	43
		notes(68) <= conv_std_logic_vector(GS7,20);	--	44
		notes(69) <= conv_std_logic_vector(A7,20);	--	45
		notes(70) <= conv_std_logic_vector(AS7,20);	--	46
		notes(71) <= conv_std_logic_vector(B7,20);	--	47

		notes(72) <= conv_std_logic_vector(C8,20);	--	48
		notes(73) <= conv_std_logic_vector(CS8,20);	--	49
		notes(74) <= conv_std_logic_vector(D8,20);	--	4A
		notes(75) <= conv_std_logic_vector(DS8,20);	--	4B
		notes(76) <= conv_std_logic_vector(E8,20);	--	4C
		notes(77) <= conv_std_logic_vector(F8,20);	--	4D
		notes(78) <= conv_std_logic_vector(FS8,20);	--	4E
		notes(79) <= conv_std_logic_vector(G8,20);	--	4F
		notes(80) <= conv_std_logic_vector(GS8,20);	--	50
		notes(81) <= conv_std_logic_vector(A8,20);	--	51
		notes(82) <= conv_std_logic_vector(AS8,20);	--	52

		notes(83) <= conv_std_logic_vector(B8,20);	--	53
		notes(84) <= conv_std_logic_vector(C9,20);	--	54
		notes(85) <= conv_std_logic_vector(CS9,20);	--	55
		notes(86) <= conv_std_logic_vector(D9,20);	--	56
		notes(87) <= conv_std_logic_vector(DS9,20);	--	57
		notes(88) <= conv_std_logic_vector(E9,20);	--	58
		notes(89) <= conv_std_logic_vector(F9,20);	--	59
		notes(90) <= conv_std_logic_vector(FS9,20);	--	5A
		notes(91) <= conv_std_logic_vector(G9,20);	--	5B
		notes(92) <= conv_std_logic_vector(GS9,20);	--	5C
		notes(93) <= conv_std_logic_vector(A9,20);	--	5D

		notes(94) <= conv_std_logic_vector(AS9,20);	--	5E
		notes(95) <= conv_std_logic_vector(B9,20);	--	5F
		
		tune1(0) <= X"00";
		tune1(1) <= X"01";
		tune1(2) <= X"02";
		tune1(3) <= X"03";
		tune1(4) <= X"04";
		tune1(5) <= X"05";
		tune1(6) <= X"06";
		tune1(7) <= X"07";

		tune2(0) <= X"04";
		tune2(1) <= X"05";
		tune2(2) <= X"06";
		tune2(3) <= X"07";
		tune2(4) <= X"08";
		tune2(5) <= X"09";
		tune2(6) <= X"0A";
		tune2(7) <= X"0B";
		
		tune3(0) <= X"09";
		tune3(1) <= X"08";
		tune3(2) <= X"07";
		tune3(3) <= X"06";
		tune3(4) <= X"05";
		tune3(5) <= X"04";
		tune3(6) <= X"03";
		tune3(7) <= X"02";

		tune4(0) <= X"3B";
		tune4(1) <= X"3A";
		tune4(2) <= X"39";
		tune4(3) <= X"38";
		tune4(4) <= X"37";
		tune4(5) <= X"36";
		tune4(6) <= X"35";
		tune4(7) <= X"34";
		
		tune5(0) <= X"06";
		tune5(1) <= X"08";
		tune5(2) <= X"0A";
		tune5(3) <= X"12";
		tune5(4) <= X"14";
		tune5(5) <= X"16";
		tune5(6) <= X"06";
		tune5(7) <= X"12";

		tune6(0) <= X"02";
		tune6(1) <= X"06";
		tune6(2) <= X"09";
		tune6(3) <= X"0E";
		tune6(4) <= X"12";
		tune6(5) <= X"15";
		tune6(6) <= X"02";
		tune6(7) <= X"0E";

		mytunes(0) <= tune1;
		mytunes(1) <= tune2;
		mytunes(2) <= tune3;
		mytunes(3) <= tune4;
		mytunes(4) <= tune5;
		mytunes(5) <= tune6;
		
-- goes threw current tune passed in by calling program and plays all 6 notes 
-- then waits for next 'start' signal from calling program

	else if clk'event and clk = '1' then
		case state_pwm_reg is
			when pwm_idle =>
				if start = '1' then		-- wait for calling program to start the whole process
					state_pwm_next <= pwm_start;
				end if;

			when pwm_start =>
				tune_index := conv_integer(tune);	-- get index from calling program
				temp_tune <= mytunes(tune_index);	-- one of the tunes 1->6 of which there are 8 notes to play
				note_index := conv_integer(temp_tune(mytune_index));
				stlv_note_index <= conv_std_logic_vector(note_index,8);
				state_pwm_next <= pwm_next;
				
			when pwm_next =>
				next_note <= '1';		-- trigger the play_note process which sets the on/off times
				start_pwm <= '1';
				state_pwm_next <= pwm_next2;

			when pwm_next2 =>
				next_note <= '0';
				if time_delay_reg2 > TIME_DELAY6a then
					time_delay_next2 <= (others=>'0');
					skip <= not skip;
					if skip = '1' then
						if mytune_index > 6 then
							start_pwm <= '0';
							state_pwm_next <= pwm_idle;
						else
							mytune_index := mytune_index + 1;
							state_pwm_next <= pwm_start;
						end if;
					end if;
				else
					time_delay_next2 <= time_delay_reg2 + 1;
				end if;
 		end case;
		time_delay_reg2 <= time_delay_next2;
		state_pwm_reg <= state_pwm_next;
		end if;
	end if;
end process;

-- pulses the output signal (pwm_signal) according to sOff_Time and sOn_Time
-- temp1 is set above to modify the duty cycle
-- this process waits on start_pwm 
pwm_proc: process(clk,reset,reg1,start_pwm)
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
			when on_pwm =>
--				if time_delay_reg1 > sOn_Time + temp1 then
				if time_delay_reg1 > sOn_Time then
					time_delay_next1 <= (others=>'0');
					next1 <= off_pwm;
					pwm_signal <= '0';
				else
					time_delay_next1 <= time_delay_reg1 + 1;
				end if;
			when off_pwm =>
--				if time_delay_reg1 > sOff_Time - temp1 then
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
