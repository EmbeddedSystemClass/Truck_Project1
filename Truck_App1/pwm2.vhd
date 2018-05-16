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
		pwm_signal: out std_logic;
		start: in std_logic;
		done: out std_logic;
		len: in signed(7 downto 0);
		duty_cycle: in std_logic_vector(2 downto 0);
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

	type state_pwm1 is (pwm_idle, pwm_start, pwm_next, pwm_next2, pwm_done);
	signal state_pwm_reg, state_pwm_next: state_pwm1;

--	tune_array is array(0 to 7) of std_logic_vector(7 downto 0);
--  array of 

	signal mykeys: keys;
	signal keyOfC: key_array;
	signal keyOfCS: key_array;
	signal keyOfD: key_array;
	signal keyOfDS: key_array;
	signal keyOfE: key_array;
	signal keyOfF: key_array;
	signal keyOfFS: key_array;
	signal keyOfG: key_array;
	signal keyOfGS: key_array;
	signal keyOfA: key_array;
	signal keyOfAS: key_array;
	signal keyOfB: key_array;
	signal temp_key: key_array;
--	signal in_tune: std_logic_vector(7 downto 0);

	signal notes: notes_array;		-- array of notes(0)->(95)
	
	signal time_delay_reg1, time_delay_next1: signed(23 downto 0);
	signal time_delay_reg2, time_delay_next2: unsigned(23 downto 0);
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
	signal skip2: std_logic;
	signal tune_done: std_logic;
	signal note_index: signed(7 downto 0);
	signal tune_index: signed(7 downto 0);
	signal mynote_index: signed(7 downto 0);
	signal slength: signed(7 downto 0);
	signal soctave: signed(7 downto 0);
	signal sDutyCycle: std_logic_vector(2 downto 0);
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
		note_index <= (others=>'0');
		slength <= (others=>'0');
		
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

		notes(24) <= conv_std_logic_vector(C4,20);	--	18		middle C
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

		notes(83) <= conv_std_logic_vector(C9,20);	--	53
		notes(84) <= conv_std_logic_vector(CS9,20);	--	54
		notes(85) <= conv_std_logic_vector(D9,20);	--	55
		notes(86) <= conv_std_logic_vector(DS9,20);	--	56
		notes(87) <= conv_std_logic_vector(E9,20);	--	57
		notes(88) <= conv_std_logic_vector(F9,20);	--	58
		notes(89) <= conv_std_logic_vector(FS9,20);	--	59
		notes(90) <= conv_std_logic_vector(G9,20);	--	5A
		notes(91) <= conv_std_logic_vector(GS9,20);	--	5B
		notes(92) <= conv_std_logic_vector(A9,20);	--	5C

		notes(93) <= conv_std_logic_vector(AS9,20);	--	5D
		notes(94) <= conv_std_logic_vector(B9,20);	--	5E
		notes(95) <= conv_std_logic_vector(B9,20);	--	5F
		
		keyOfC(0) <= X"00";
		keyOfC(1) <= X"02";
		keyOfC(2) <= X"04";
		keyOfC(3) <= X"05";
		keyOfC(4) <= X"07";
		keyOfC(5) <= X"09";
		keyOfC(6) <= X"0b";
		keyOfC(7) <= X"0c";
		keyOfC(8) <= X"0e";
		keyOfC(9) <= X"10";
		keyOfC(10) <= X"11";
		keyOfC(11) <= X"13";
		keyOfC(12) <= X"15";
		keyOfC(13) <= X"17";
		keyOfC(14) <= X"18";
		keyOfC(15) <= X"1a";
		keyOfC(16) <= X"1c";
		keyOfC(17) <= X"1d";
		keyOfC(18) <= X"1f";
		keyOfC(19) <= X"21";
		keyOfC(20) <= X"23";
		keyOfC(21) <= X"24";
		keyOfC(22) <= X"26";
		keyOfC(23) <= X"28";
		keyOfC(24) <= X"29";
		keyOfC(25) <= X"2b";
		keyOfC(26) <= X"2d";
		keyOfC(27) <= X"2f";
		keyOfC(28) <= X"30";
		keyOfC(29) <= X"32";
		keyOfC(30) <= X"34";
		keyOfC(31) <= X"35";
		keyOfC(32) <= X"37";
		keyOfC(33) <= X"39";
		keyOfC(34) <= X"3b";
		keyOfC(35) <= X"3c";
		keyOfC(36) <= X"3e";
		keyOfC(37) <= X"40";
		keyOfC(38) <= X"41";
		keyOfC(39) <= X"43";
		keyOfC(40) <= X"45";
		keyOfC(41) <= X"47";

		keyOfCS(0) <= X"01";
		keyOfCS(1) <= X"03";
		keyOfCS(2) <= X"05";
		keyOfCS(3) <= X"06";
		keyOfCS(4) <= X"08";
		keyOfCS(5) <= X"0a";
		keyOfCS(6) <= X"0c";
		keyOfCS(7) <= X"0d";
		keyOfCS(8) <= X"0f";
		keyOfCS(9) <= X"11";
		keyOfCS(10) <= X"12";
		keyOfCS(11) <= X"14";
		keyOfCS(12) <= X"16";
		keyOfCS(13) <= X"18";
		keyOfCS(14) <= X"19";
		keyOfCS(15) <= X"1b";
		keyOfCS(16) <= X"1d";
		keyOfCS(17) <= X"1e";
		keyOfCS(18) <= X"20";
		keyOfCS(19) <= X"22";
		keyOfCS(20) <= X"24";
		keyOfCS(21) <= X"25";
		keyOfCS(22) <= X"27";
		keyOfCS(23) <= X"29";
		keyOfCS(24) <= X"2a";
		keyOfCS(25) <= X"2c";
		keyOfCS(26) <= X"2e";
		keyOfCS(27) <= X"30";
		keyOfCS(28) <= X"31";
		keyOfCS(29) <= X"33";
		keyOfCS(30) <= X"35";
		keyOfCS(31) <= X"36";
		keyOfCS(32) <= X"38";
		keyOfCS(33) <= X"3a";
		keyOfCS(34) <= X"3c";
		keyOfCS(35) <= X"3d";
		keyOfCS(36) <= X"3f";
		keyOfCS(37) <= X"41";
		keyOfCS(38) <= X"42";
		keyOfCS(39) <= X"44";
		keyOfCS(40) <= X"46";
		keyOfCS(41) <= X"48";

		keyOfD(0) <= X"01";
		keyOfD(1) <= X"02";
		keyOfD(2) <= X"04";
		keyOfD(3) <= X"06";
		keyOfD(4) <= X"07";
		keyOfD(5) <= X"09";
		keyOfD(6) <= X"0b";
		keyOfD(7) <= X"0d";
		keyOfD(8) <= X"0e";
		keyOfD(9) <= X"10";
		keyOfD(10) <= X"12";
		keyOfD(11) <= X"13";
		keyOfD(12) <= X"15";
		keyOfD(13) <= X"17";
		keyOfD(14) <= X"19";
		keyOfD(15) <= X"1a";
		keyOfD(16) <= X"1c";
		keyOfD(17) <= X"1e";
		keyOfD(18) <= X"1f";
		keyOfD(19) <= X"21";
		keyOfD(20) <= X"23";
		keyOfD(21) <= X"25";
		keyOfD(22) <= X"26";
		keyOfD(23) <= X"28";
		keyOfD(24) <= X"29";
		keyOfD(25) <= X"2a";
		keyOfD(26) <= X"2b";
		keyOfD(27) <= X"2d";
		keyOfD(28) <= X"2f";
		keyOfD(29) <= X"31";
		keyOfD(30) <= X"32";
		keyOfD(31) <= X"34";
		keyOfD(32) <= X"35";
		keyOfD(33) <= X"36";
		keyOfD(34) <= X"37";
		keyOfD(35) <= X"39";
		keyOfD(36) <= X"3b";
		keyOfD(37) <= X"3d";
		keyOfD(38) <= X"3e";
		keyOfD(39) <= X"40";
		keyOfD(40) <= X"41";
		keyOfD(41) <= X"42";

		keyOfDS(0) <= X"00";
		keyOfDS(1) <= X"02";
		keyOfDS(2) <= X"03";
		keyOfDS(3) <= X"05";
		keyOfDS(4) <= X"07";
		keyOfDS(5) <= X"08";
		keyOfDS(6) <= X"0a";
		keyOfDS(7) <= X"0c";
		keyOfDS(8) <= X"0e";
		keyOfDS(9) <= X"0f";
		keyOfDS(10) <= X"11";
		keyOfDS(11) <= X"13";
		keyOfDS(12) <= X"14";
		keyOfDS(13) <= X"16";
		keyOfDS(14) <= X"18";
		keyOfDS(15) <= X"1a";
		keyOfDS(16) <= X"1b";
		keyOfDS(17) <= X"1d";
		keyOfDS(18) <= X"1f";
		keyOfDS(19) <= X"20";
		keyOfDS(20) <= X"22";
		keyOfDS(21) <= X"24";
		keyOfDS(22) <= X"26";
		keyOfDS(23) <= X"27";
		keyOfDS(24) <= X"29";
		keyOfDS(25) <= X"2b";
		keyOfDS(26) <= X"2c";
		keyOfDS(27) <= X"2e";
		keyOfDS(28) <= X"30";
		keyOfDS(29) <= X"32";
		keyOfDS(30) <= X"33";
		keyOfDS(31) <= X"35";
		keyOfDS(32) <= X"37";
		keyOfDS(33) <= X"38";
		keyOfDS(34) <= X"3a";
		keyOfDS(35) <= X"3c";
		keyOfDS(36) <= X"3e";
		keyOfDS(37) <= X"3f";
		keyOfDS(38) <= X"41";
		keyOfDS(39) <= X"43";
		keyOfDS(40) <= X"44";
		keyOfDS(41) <= X"46";

		keyOfE(0) <= X"01";
		keyOfE(1) <= X"03";
		keyOfE(2) <= X"04";
		keyOfE(3) <= X"06";
		keyOfE(4) <= X"08";
		keyOfE(5) <= X"09";
		keyOfE(6) <= X"0b";
		keyOfE(7) <= X"0d";
		keyOfE(8) <= X"0f";
		keyOfE(9) <= X"10";
		keyOfE(10) <= X"12";
		keyOfE(11) <= X"14";
		keyOfE(12) <= X"15";
		keyOfE(13) <= X"17";
		keyOfE(14) <= X"19";
		keyOfE(15) <= X"1b";
		keyOfE(16) <= X"1c";
		keyOfE(17) <= X"1e";
		keyOfE(18) <= X"20";
		keyOfE(19) <= X"21";
		keyOfE(20) <= X"23";
		keyOfE(21) <= X"25";
		keyOfE(22) <= X"27";
		keyOfE(23) <= X"28";
		keyOfE(24) <= X"2a";
		keyOfE(25) <= X"2c";
		keyOfE(26) <= X"2d";
		keyOfE(27) <= X"2f";
		keyOfE(28) <= X"31";
		keyOfE(29) <= X"33";
		keyOfE(30) <= X"34";
		keyOfE(31) <= X"36";
		keyOfE(32) <= X"38";
		keyOfE(33) <= X"39";
		keyOfE(34) <= X"3b";
		keyOfE(35) <= X"3d";
		keyOfE(36) <= X"3f";
		keyOfE(37) <= X"40";
		keyOfE(38) <= X"42";
		keyOfE(39) <= X"44";
		keyOfE(40) <= X"45";
		keyOfE(41) <= X"47";

		keyOfF(0) <= X"00";
		keyOfF(1) <= X"02";
		keyOfF(2) <= X"04";
		keyOfF(3) <= X"05";
		keyOfF(4) <= X"07";
		keyOfF(5) <= X"09";
		keyOfF(6) <= X"0a";
		keyOfF(7) <= X"0c";
		keyOfF(8) <= X"0e";
		keyOfF(9) <= X"10";
		keyOfF(10) <= X"11";
		keyOfF(11) <= X"13";
		keyOfF(12) <= X"15";
		keyOfF(13) <= X"16";
		keyOfF(14) <= X"17";
		keyOfF(15) <= X"18";
		keyOfF(16) <= X"1a";
		keyOfF(17) <= X"1c";
		keyOfF(18) <= X"1d";
		keyOfF(19) <= X"1f";
		keyOfF(20) <= X"21";
		keyOfF(21) <= X"22";
		keyOfF(22) <= X"23";
		keyOfF(23) <= X"24";
		keyOfF(24) <= X"26";
		keyOfF(25) <= X"28";
		keyOfF(26) <= X"29";
		keyOfF(27) <= X"2b";
		keyOfF(28) <= X"2d";
		keyOfF(29) <= X"2e";
		keyOfF(30) <= X"2f";
		keyOfF(31) <= X"30";
		keyOfF(32) <= X"32";
		keyOfF(33) <= X"34";
		keyOfF(34) <= X"35";
		keyOfF(35) <= X"37";
		keyOfF(36) <= X"39";
		keyOfF(37) <= X"3a";
		keyOfF(38) <= X"3b";
		keyOfF(39) <= X"3c";
		keyOfF(40) <= X"3e";
		keyOfF(41) <= X"40";

		keyOfFS(0) <= X"01";
		keyOfFS(1) <= X"03";
		keyOfFS(2) <= X"05";
		keyOfFS(3) <= X"06";
		keyOfFS(4) <= X"08";
		keyOfFS(5) <= X"0a";
		keyOfFS(6) <= X"0b";
		keyOfFS(7) <= X"0d";
		keyOfFS(8) <= X"0f";
		keyOfFS(9) <= X"11";
		keyOfFS(10) <= X"12";
		keyOfFS(11) <= X"14";
		keyOfFS(12) <= X"16";
		keyOfFS(13) <= X"17";
		keyOfFS(14) <= X"19";
		keyOfFS(15) <= X"1b";
		keyOfFS(16) <= X"1d";
		keyOfFS(17) <= X"1e";
		keyOfFS(18) <= X"20";
		keyOfFS(19) <= X"22";
		keyOfFS(20) <= X"23";
		keyOfFS(21) <= X"25";
		keyOfFS(22) <= X"27";
		keyOfFS(23) <= X"29";
		keyOfFS(24) <= X"2a";
		keyOfFS(25) <= X"2c";
		keyOfFS(26) <= X"2e";
		keyOfFS(27) <= X"2f";
		keyOfFS(28) <= X"31";
		keyOfFS(29) <= X"33";
		keyOfFS(30) <= X"35";
		keyOfFS(31) <= X"36";
		keyOfFS(32) <= X"38";
		keyOfFS(33) <= X"3a";
		keyOfFS(34) <= X"3b";
		keyOfFS(35) <= X"3d";
		keyOfFS(36) <= X"3f";
		keyOfFS(37) <= X"41";
		keyOfFS(38) <= X"42";
		keyOfFS(39) <= X"44";
		keyOfFS(40) <= X"46";
		keyOfFS(41) <= X"47";

		keyOfG(0) <= X"00";
		keyOfG(1) <= X"02";
		keyOfG(2) <= X"04";
		keyOfG(3) <= X"06";
		keyOfG(4) <= X"07";
		keyOfG(5) <= X"09";
		keyOfG(6) <= X"0b";
		keyOfG(7) <= X"0c";
		keyOfG(8) <= X"0e";
		keyOfG(9) <= X"10";
		keyOfG(10) <= X"12";
		keyOfG(11) <= X"13";
		keyOfG(12) <= X"15";
		keyOfG(13) <= X"17";
		keyOfG(14) <= X"18";
		keyOfG(15) <= X"1a";
		keyOfG(16) <= X"1c";
		keyOfG(17) <= X"1e";
		keyOfG(18) <= X"1f";
		keyOfG(19) <= X"21";
		keyOfG(20) <= X"23";
		keyOfG(21) <= X"24";
		keyOfG(22) <= X"26";
		keyOfG(23) <= X"28";
		keyOfG(24) <= X"2a";
		keyOfG(25) <= X"2b";
		keyOfG(26) <= X"2d";
		keyOfG(27) <= X"2f";
		keyOfG(28) <= X"30";
		keyOfG(29) <= X"32";
		keyOfG(30) <= X"34";
		keyOfG(31) <= X"36";
		keyOfG(32) <= X"37";
		keyOfG(33) <= X"39";
		keyOfG(34) <= X"3b";
		keyOfG(35) <= X"3c";
		keyOfG(36) <= X"3e";
		keyOfG(37) <= X"40";
		keyOfG(38) <= X"42";
		keyOfG(39) <= X"43";
		keyOfG(40) <= X"45";
		keyOfG(41) <= X"47";

		keyOfGS(0) <= X"00";
		keyOfGS(1) <= X"01";
		keyOfGS(2) <= X"03";
		keyOfGS(3) <= X"05";
		keyOfGS(4) <= X"07";
		keyOfGS(5) <= X"08";
		keyOfGS(6) <= X"0a";
		keyOfGS(7) <= X"0c";
		keyOfGS(8) <= X"0d";
		keyOfGS(9) <= X"0f";
		keyOfGS(10) <= X"11";
		keyOfGS(11) <= X"13";
		keyOfGS(12) <= X"14";
		keyOfGS(13) <= X"16";
		keyOfGS(14) <= X"18";
		keyOfGS(15) <= X"19";
		keyOfGS(16) <= X"1b";
		keyOfGS(17) <= X"1d";
		keyOfGS(18) <= X"1f";
		keyOfGS(19) <= X"20";
		keyOfGS(20) <= X"22";
		keyOfGS(21) <= X"24";
		keyOfGS(22) <= X"25";
		keyOfGS(23) <= X"27";
		keyOfGS(24) <= X"29";
		keyOfGS(25) <= X"2b";
		keyOfGS(26) <= X"2c";
		keyOfGS(27) <= X"2e";
		keyOfGS(28) <= X"30";
		keyOfGS(29) <= X"31";
		keyOfGS(30) <= X"33";
		keyOfGS(31) <= X"35";
		keyOfGS(32) <= X"37";
		keyOfGS(33) <= X"38";
		keyOfGS(34) <= X"3a";
		keyOfGS(35) <= X"3c";
		keyOfGS(36) <= X"3d";
		keyOfGS(37) <= X"3f";
		keyOfGS(38) <= X"41";
		keyOfGS(39) <= X"43";
		keyOfGS(40) <= X"44";
		keyOfGS(41) <= X"46";

		keyOfA(0) <= X"01";
		keyOfA(1) <= X"02";
		keyOfA(2) <= X"04";
		keyOfA(3) <= X"06";
		keyOfA(4) <= X"08";
		keyOfA(5) <= X"09";
		keyOfA(6) <= X"0b";
		keyOfA(7) <= X"0d";
		keyOfA(8) <= X"0e";
		keyOfA(9) <= X"10";
		keyOfA(10) <= X"12";
		keyOfA(11) <= X"14";
		keyOfA(12) <= X"15";
		keyOfA(13) <= X"17";
		keyOfA(14) <= X"19";
		keyOfA(15) <= X"1a";
		keyOfA(16) <= X"1c";
		keyOfA(17) <= X"1e";
		keyOfA(18) <= X"20";
		keyOfA(19) <= X"21";
		keyOfA(20) <= X"23";
		keyOfA(21) <= X"25";
		keyOfA(22) <= X"26";
		keyOfA(23) <= X"28";
		keyOfA(24) <= X"2a";
		keyOfA(25) <= X"2c";
		keyOfA(26) <= X"2d";
		keyOfA(27) <= X"2f";
		keyOfA(28) <= X"31";
		keyOfA(29) <= X"32";
		keyOfA(30) <= X"34";
		keyOfA(31) <= X"36";
		keyOfA(32) <= X"38";
		keyOfA(33) <= X"39";
		keyOfA(34) <= X"3b";
		keyOfA(35) <= X"3d";
		keyOfA(36) <= X"3e";
		keyOfA(37) <= X"40";
		keyOfA(38) <= X"42";
		keyOfA(39) <= X"44";
		keyOfA(40) <= X"45";
		keyOfA(41) <= X"47";

		keyOfAS(0) <= X"00";
		keyOfAS(1) <= X"02";
		keyOfAS(2) <= X"03";
		keyOfAS(3) <= X"05";
		keyOfAS(4) <= X"07";
		keyOfAS(5) <= X"09";
		keyOfAS(6) <= X"0a";
		keyOfAS(7) <= X"0c";
		keyOfAS(8) <= X"0e";
		keyOfAS(9) <= X"0f";
		keyOfAS(10) <= X"11";
		keyOfAS(11) <= X"13";
		keyOfAS(12) <= X"15";
		keyOfAS(13) <= X"16";
		keyOfAS(14) <= X"18";
		keyOfAS(15) <= X"1a";
		keyOfAS(16) <= X"1b";
		keyOfAS(17) <= X"1d";
		keyOfAS(18) <= X"1f";
		keyOfAS(19) <= X"21";
		keyOfAS(20) <= X"22";
		keyOfAS(21) <= X"24";
		keyOfAS(22) <= X"26";
		keyOfAS(23) <= X"27";
		keyOfAS(24) <= X"29";
		keyOfAS(25) <= X"2b";
		keyOfAS(26) <= X"2d";
		keyOfAS(27) <= X"2e";
		keyOfAS(28) <= X"30";
		keyOfAS(29) <= X"32";
		keyOfAS(30) <= X"33";
		keyOfAS(31) <= X"35";
		keyOfAS(32) <= X"36";
		keyOfAS(33) <= X"37";
		keyOfAS(34) <= X"39";
		keyOfAS(35) <= X"3a";
		keyOfAS(36) <= X"3c";
		keyOfAS(37) <= X"3e";
		keyOfAS(38) <= X"3f";
		keyOfAS(39) <= X"41";
		keyOfAS(40) <= X"43";
		keyOfAS(41) <= X"45";

		keyOfB(0) <= X"01";
		keyOfB(1) <= X"03";
		keyOfB(2) <= X"04";
		keyOfB(3) <= X"06";
		keyOfB(4) <= X"08";
		keyOfB(5) <= X"0a";
		keyOfB(6) <= X"0b";
		keyOfB(7) <= X"0d";
		keyOfB(8) <= X"0f";
		keyOfB(9) <= X"10";
		keyOfB(10) <= X"12";
		keyOfB(11) <= X"14";
		keyOfB(12) <= X"16";
		keyOfB(13) <= X"17";
		keyOfB(14) <= X"19";
		keyOfB(15) <= X"1b";
		keyOfB(16) <= X"1c";
		keyOfB(17) <= X"1e";
		keyOfB(18) <= X"20";
		keyOfB(19) <= X"22";
		keyOfB(20) <= X"23";
		keyOfB(21) <= X"25";
		keyOfB(22) <= X"27";
		keyOfB(23) <= X"28";
		keyOfB(24) <= X"2a";
		keyOfB(25) <= X"2c";
		keyOfB(26) <= X"2e";
		keyOfB(27) <= X"2f";
		keyOfB(28) <= X"31";
		keyOfB(29) <= X"33";
		keyOfB(30) <= X"34";
		keyOfB(31) <= X"36";
		keyOfB(32) <= X"38";
		keyOfB(33) <= X"3a";
		keyOfB(34) <= X"3b";
		keyOfB(35) <= X"3d";
		keyOfB(36) <= X"3f";
		keyOfB(37) <= X"40";
		keyOfB(38) <= X"42";
		keyOfB(39) <= X"44";
		keyOfB(40) <= X"46";
		keyOfB(41) <= X"47";

		mykeys(0) <= keyOfC;
		mykeys(1) <= keyOfCS;
		mykeys(2) <= keyOfD;
		mykeys(3) <= keyOfDS;
		mykeys(4) <= keyOfE;
		mykeys(5) <= keyOfF;
		mykeys(6) <= keyOfFS;
		mykeys(7) <= keyOfG;
		mykeys(8) <= keyOfGS;
		mykeys(9) <= keyOfA;
		mykeys(10) <= keyOfAS;
		mykeys(11) <= keyOfB;

		tune_done <= '0';
		done <= '0';
		mynote_index <= (others=>'0');
		tune_index <= (others=>'0');
		
-- goes threw current tune passed in by calling program and plays all 6 notes 
-- then waits for next 'start' signal from calling program

	else if clk'event and clk = '1' then
		case state_pwm_reg is
			when pwm_idle =>
				if start = '1' then		-- wait for calling program to start the whole process
					slength <= len;
					if slength > 10 then
						slength <= X"0A";
					end if;
					tune_index <= conv_signed(conv_integer(key),8);	-- get index from calling program
					temp := conv_integer(tune_index);
					temp_key <= mykeys(temp);
					state_pwm_next <= pwm_start;
					done <= '0';
				end if;
				tune_done <= '0';

			when pwm_start =>
				
				note_index <= conv_signed(conv_integer(temp_key(conv_integer(mynote_index))),8);
				
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
				if time_delay_reg2 > TIME_DELAY8a then
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

end pwm_arch;
