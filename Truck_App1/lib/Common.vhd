--**********************************************************************
-- Copyright (c) 1997-2014 by XESS Corp <http://www.xess.com>.
-- All rights reserved.
--
-- This library is free software; you can redistribute it and/or
-- modify it under the terms of the GNU Lesser General Public
-- License as published by the Free Software Foundation; either
-- version 3.0 of the License, or (at your option) any later version.
-- 
-- This library is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
-- Lesser General Public License for more details.
-- 
-- You should have received a copy of the GNU Lesser General Public
-- License along with this library.  If not, see 
-- <http://www.gnu.org/licenses/>.
--**********************************************************************

----------------------------------------------------------------------------------
-- Commonly-used functions and constants.
----------------------------------------------------------------------------------

library IEEE;
use IEEE.std_logic_1164.all;
--use IEEE.numeric_std.all;
use ieee.std_logic_signed.all;
use ieee.std_logic_arith.all;
package CommonPckg is

	constant YES  : std_logic := '1';
	constant NO   : std_logic := '0';
	constant HI   : std_logic := '1';
	constant LO   : std_logic := '0';
	constant ONE  : std_logic := '1';
	constant ZERO : std_logic := '0';
	constant HIZ  : std_logic := 'Z';

	constant C2:  integer:=  347491;	-- C octave 2
	constant CS2:  integer:=  327984;	-- C# octave 2
	constant D2:  integer:=  309578;	-- D
	constant DS2:  integer:=  292201;	-- D#
	constant E2:  integer:=  275802;	-- E
	constant F2:  integer:=  260323;	-- F
	constant FS2:  integer:=  245711;
	constant G2:  integer:=  231921;
	constant GS2:  integer:=  218905;
	constant A2:  integer:=  206618;
	constant AS2:  integer:=  195021;
	constant B2:  integer:=  184076;

	constant C3:  integer:=  173744;
	constant CS3:  integer:=  163993;
	constant D3:  integer:=  154789;
	constant DS3:  integer:=  146102;
	constant E3:  integer:=  137901;
	constant F3:  integer:=  130161;
	constant FS3:  integer:=  122856;
	constant G3:  integer:=  115960;
	constant GS3:  integer:=  109452;
	constant A3:  integer:=  103309;
	constant AS3:  integer:=  97511;
	constant B3:  integer:=  92038;

	constant C4:  integer:=  86872;		-- middle C
	constant CS4:  integer:=  81996;
	constant D4:  integer:=  77394;
	constant DS4:  integer:=  73051;
	constant E4:  integer:=  68950;
	constant F4:  integer:=  65081;
	constant FS4:  integer:=  61428;
	constant G4:  integer:=  57980;
	constant GS4:  integer:=  54726;
	constant A4:  integer:=  51655;
	constant AS4:  integer:=  48755;
	constant B4:  integer:=  46019;

	constant C5:  integer:=  43436;
	constant CS5:  integer:=  40998;
	constant D5:  integer:=  38697;
	constant DS5:  integer:=  36525;
	constant E5:  integer:=  34475;
	constant F5:  integer:=  32540;
	constant FS5:  integer:=  30714;
	constant G5:  integer:=  28990;
	constant GS5:  integer:=  27363;
	constant A5:  integer:=  25827;
	constant AS5:  integer:=  24378;
	constant B5:  integer:=  23009;

	constant C6:  integer:=  21718;
	constant CS6:  integer:=  20499;
	constant D6:  integer:=  19349;
	constant DS6:  integer:=  18263;
	constant E6:  integer:=  17238;
	constant F6:  integer:=  16270;
	constant FS6:  integer:=  15357;
	constant G6:  integer:=  14495;
	constant GS6:  integer:=  13682;
	constant A6:  integer:=  12914;
	constant AS6:  integer:=  12189;
	constant B6:  integer:=  11505;

	constant C7:  integer:=  10859;
	constant CS7:  integer:=  10250;
	constant D7:  integer:=  9674;
	constant DS7:  integer:=  9131;
	constant E7:  integer:=  8619;
	constant F7:  integer:=  8135;
	constant FS7:  integer:=  7678;
	constant G7:  integer:=  7248;
	constant GS7:  integer:=  6841;
	constant A7:  integer:=  6457;
	constant AS7:  integer:=  6094;
	constant B7:  integer:=  5752;

	constant C8:  integer:=  5430;
	constant CS8:  integer:=  5125;
	constant D8:  integer:=  4837;
	constant DS8:  integer:=  4566;
	constant E8:  integer:=  4309;
	constant F8:  integer:=  4068;
	constant FS8:  integer:=  3839;
	constant G8:  integer:=  3624;
	constant GS8:  integer:=  3420;
	constant A8:  integer:=  3228;
	constant AS8:  integer:=  3047;
	constant B8:  integer:=  2876;

	constant C9:  integer:=  2715;
	constant CS9:  integer:=  2562;
	constant D9:  integer:=  2419;
	constant DS9:  integer:=  2283;
	constant E9:  integer:=  2155;
	constant F9:  integer:=  2034;
	constant FS9:  integer:=  1920;
	constant G9:  integer:=  1812;
	constant GS9:  integer:=  1710;
	constant A9:  integer:=  1614;
	constant AS9:  integer:=  1524;
	constant B9:  integer:=  1438;

	constant OneA: integer:= 32608;
	constant OneB: integer:= 18799;

	constant TwoA: integer:= 32608;
	constant TwoB: integer:= 17012;

	constant ThreeA: integer:= 32608;
	constant ThreeB: integer:= 15388;

	constant FourA: integer:= 29517;
	constant FourB: integer:= 18799;

	constant FiveA: integer:= 29517;
	constant FiveB: integer:= 17012;

	constant SixA: integer:= 29517;
	constant SixB: integer:= 15388;

	constant SevenA: integer:= 26676;
	constant SevenB: integer:= 18799;

	constant EightA: integer:= 26676;
	constant EightB: integer:= 17012;

	constant NineA: integer:= 26676;
	constant NineB: integer:= 15388;

	constant AA: integer:= 32608;
	constant AB: integer:= 13918;

	constant BA: integer:= 29517;
	constant BB: integer:= 13918;

	constant CA: integer:= 26676;
	constant CB: integer:= 13918;

	constant DA: integer:= 24153;
	constant DB: integer:= 13918;

	constant PoundA: integer:= 24153;
	constant PoundB: integer:= 15388;

	constant AsterikA: integer:= 24153;
	constant AsterikB: integer:= 18799;

	constant ZeroA: integer:= 24153;
	constant ZeroB: integer:= 17012;
	
	constant TIME_DELAY0: integer:= 40000;
-- 16 bits = FFFF (65,535)		~1.5ms
-- 17 bits = 1FFFF (131,071)  5.2ms
-- 18 bits = 3FFFF (262,143)  10.4ms
-- 19 bits = 7FFFF (524,287)  20.8ms
-- 20 bits = FFFFF (1,048,575)  41.6ms
-- 21 bits = 1FFFFF (2,097,151)  83.2ms
-- 22 bits = 3FFFFF (4,191,303)  166.2ms
-- 23 bits = 7FFFFF (8,388,607)  332.8ms
-- 24 bits = FFFFFF (16,777,215) 665.6ms
-- 25 bits = 1FFFFFF (33,554,431) 1331.1ms (1.3sec)
	constant TIME_DELAY4: integer:= 318181;	-- this in a time delay should provide about 25ms
--	constant TIME_DELAY1:  integer:= 262000;
--	constant TIME_DELAY1:  integer:= 131000;
	constant TIME_DELAY1:  integer:= 100000;
	constant TIME_DELAY3:  integer:= 33554430;	-- 1.342 seconds
	constant TIME_DELAY5:  integer:= 16000000;	-- 640ms
	constant TIME_DELAY5a:  integer:= 8000000;	-- 320ms
	constant TIME_DELAY6:  integer:=  4191300;	-- 167ms
	constant TIME_DELAY6a:  integer:= 3000000;
	constant TIME_DELAY6b:  integer:= 2000000;
	constant TIME_DELAY7:  integer:= 1000000;	-- 40ms
	constant TIME_DELAY8:  integer:= 500000;	-- 20ms
	constant TIME_DELAY8a:  integer:= 300000;	-- 12ms
	constant TIME_DELAY8b:  integer:= 150000;	-- 12ms
	constant TIME_DELAY8c:  integer:= 100000;
	constant TIME_DELAY9:  integer:=  40000;
	
-- baud rates for 7-seg displays	
	constant DVSR_MU_9600: integer:= 320;	-- 9600 baud
	constant DVSR_MU_19200: integer:= 163;	-- 19200 baud
	constant DVSR_MU_115200: integer:= 27;	-- 115200 baud
-- mph is currently for (2) magnets on driveshaft
	constant MPH_CLOCK_COUNT: integer:= 12500;
	constant MPH_DVND: integer:= 128514;
												-- 26ms should be 2254
	constant RPM_CLOCK_COUNT: integer:= 500;	-- 26ms = 2409
--	constant RPM_CLOCK_COUNT: integer:= 510;	-- 2362
--	constant RPM_CLOCK_COUNT: integer:= 520;	-- 2316
--	constant RPM_CLOCK_COUNT: integer:= 525;	-- 2294
--	constant RPM_CLOCK_COUNT: integer:= 527;	-- 2281
--	constant RPM_CLOCK_COUNT: integer:= 530;	-- 2272
	constant RPM_DVND: integer:= 6000000;
	constant DVND_SIZE: integer:= 30;
	constant RESULT_SIZE: integer:= 18;
	constant PRD_SIZE: integer:= 17;
	constant DVSR_SIZE: integer:= 23;
	constant MPH_P_REG_SIZE: integer:= 16;
	constant OFF_CMD: std_logic_vector(4 downto 0):= "00000";
	constant SEND_CHAR_CMD: std_logic_vector(4 downto 0):= "00001";
	constant SET_BRIGHTNESS_CMD: std_logic_vector(4 downto 0):= "00010";
	constant SET_CDECIMAL_CMD: std_logic_vector(4 downto 0):= "00011";
	constant SET_UPDATE_RATE_CMD: std_logic_vector(4 downto 0):= "00100";
	constant SET_FACTOR_CMD: std_logic_vector(4 downto 0):= "00101";
	constant SET_DISPLAY_UPDATE_RATE: std_logic_vector(4 downto 0):= "00110";
	constant DTMF_TONE_ON: std_logic_vector(4 downto 0):=  "00111";
	constant DTMF_TONE_OFF: std_logic_vector(4 downto 0):= "01000";

	constant SET_CLEAR_CTL: std_logic_vector(7 downto 0):= X"76";
	constant SET_CURSOR_CTL: std_logic_vector(7 downto 0):= X"79";
	constant SET_BRIGHT_CTL: std_logic_vector(7 downto 0):= X"7A";
	constant SET_DECIMAL_CTL: std_logic_vector(7 downto 0):= X"77";
	constant SET_SEG1_CTL: std_logic_vector(7 downto 0):= X"7B";
	constant SET_SEG2_CTL: std_logic_vector(7 downto 0):= X"7C";
	constant SET_SEG3_CTL: std_logic_vector(7 downto 0):= X"7D";
	constant SET_SEG4_CTL: std_logic_vector(7 downto 0):= X"7E";

	constant NUM_DATA_ARRAY : integer:= 16;
	constant RT_RPM		: std_logic_vector := X"70";
	constant RT_ENGT	: std_logic_vector := X"71"; 
	constant RT_TRIP	: std_logic_vector := X"72";
	constant RT_TIME	: std_logic_vector := X"73";
	constant RT_AIRT	: std_logic_vector := X"74";
	constant RT_MPH		: std_logic_vector := X"75";
	constant RT_OILP	: std_logic_vector := X"76";
	constant RT_MAP		: std_logic_vector := X"77";
	constant RT_OILT	: std_logic_vector := X"78";
	constant RT_O2		: std_logic_vector := X"79";
	
	constant RT_LOW		: std_logic_vector := X"01";		-- UCHAR without high bit set
	constant RT_HIGH0	: std_logic_vector := X"02";		-- UCHAR with bit 7 set
	constant RT_HIGH1	: std_logic_vector := X"03";		-- UINT with neither 7 or 15 set
	constant RT_HIGH2	: std_logic_vector := X"04";		-- bit 7 of UINT set
	constant RT_HIGH3	: std_logic_vector := X"05";		-- bit 15 of UINT set
	constant SLAVE_COUNT : natural := 2;

	-- FPGA chip families.
	type FpgaFamily_t is (SPARTAN3A_E, SPARTAN6_E);

	-- XESS FPGA boards.
	type XessBoard_t is (XULA_E, XULA2_E);

	type my_array3 is array(0 to 15) of std_logic_vector(16 downto 0);
	type notes_array is array(0 to 95) of std_logic_vector(19 downto 0);
	type tune_array is array(0 to 11) of std_logic_vector(7 downto 0);
	type tunes is array(0 to 7) of tune_array;
	type key_array is array(0 to 41) of std_logic_vector(7 downto 0);
--	type dkey_array is array(0 to 82) of std_logic_vector(7 downto 0);
	type keys is array(0 to 11) of key_array;
	type i_keys is array(0 to 11) of std_logic_vector(7 downto 0);
	type dtmf_array is array(0 to 1) of std_logic_vector(19 downto 0);
		
	impure function average(a : in my_array3) return std_logic_vector;
	impure function shift_avg(a : in my_array3; b : in std_logic_vector) return my_array3;
	impure function block_ff(a : in std_logic_vector) return std_logic_vector;
	impure function div_int(a : integer; b : integer) return integer;
	impure function load_notes(a : integer) return notes_array;
	impure function get_note(notes : notes_array; a : integer) return std_logic_vector;

	impure function load_pattern1(a : key_array) return key_array;
	impure function load_pattern2(a : key_array) return key_array;
	impure function load_pattern2a(a : key_array) return key_array;
	impure function load_pattern3(a : key_array) return key_array;
	impure function load_pattern3a(a : key_array) return key_array;
	impure function load_pattern4(a : key_array) return key_array;
	impure function load_pattern4a(a : key_array) return key_array;
--	impure function load_minor7(a : key_array) return key_array;
	
	impure function load_keyOfC(a : integer) return key_array;
	impure function load_keyOfCS(a : integer) return key_array;
	impure function load_keyOfD(a : integer) return key_array;
	impure function load_keyOfDS(a : integer) return key_array;
	impure function load_keyOfE(a : integer) return key_array;
	impure function load_keyOfF(a : integer) return key_array;
	impure function load_keyOfFS(a : integer) return key_array;
	impure function load_keyOfG(a : integer) return key_array;
	impure function load_keyOfGS(a : integer) return key_array;
	impure function load_keyOfA(a : integer) return key_array;
	impure function load_keyOfAS(a : integer) return key_array;
	impure function load_keyOfB(a : integer) return key_array;
	impure function load_dtmf(a : integer) return dtmf_array;

end package;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

package body CommonPckg is

-- 8192(0x2000) * 16 = 131056(0x20000)
-- to divide by 16 rol by 4 bits

impure function load_pattern1(a : key_array) return key_array is
variable org: key_array;
variable pattern: key_array;
variable x: integer range 0 to 41;
variable y: integer range 0 to 41;
	begin
		org := a;
		x := 0;
		y := 0;

		for i in 0 to 20 loop
			pattern(x) := org(y);
			x := x + 1;
			y := y + 1;
			if y > 40 then
				y := 0;
			end if;
		end loop;
		y := 21;
		for i in 0 to 20 loop
			pattern(x) := org(y);
			x := x + 1;
 			y := y - 1;
			if y  < 1 then
				y := 40;
			end if;
		end loop;

		return pattern;
	end function;

impure function load_pattern2(a : key_array) return key_array is
variable org: key_array;
variable pattern: key_array;
variable x: integer range 0 to 41;
variable y: integer range 0 to 41;
	begin
		org := a;
		x := 0;
		y := 0;

		for j in 0 to 13 loop
			for i in 0 to 2 loop
				pattern(x) := org(y);
				x := x + 1;
				y := y + 1;
			end loop;
			y := y - 2;
		end loop;

		return pattern;
	end function;

impure function load_pattern2a(a : key_array) return key_array is
variable org: key_array;
variable pattern: key_array;
variable x: integer range 0 to 41;
variable y: integer range 0 to 41;
	begin
		org := a;
		x := 0;
		y := 20;

		for j in 0 to 13 loop
			for i in 0 to 2 loop
				pattern(x) := org(y);
				x := x + 1;
				y := y - 1;
			end loop;
			y := y + 2;
		end loop;

		return pattern;
	end function;

impure function load_pattern3(a : key_array) return key_array is
variable org: key_array;
variable pattern: key_array;
variable x: integer range 0 to 41;
variable y: integer range 0 to 41;
	begin
		org := a;
		x := 0;
		y := 0;

		for j in 0 to 7 loop
			for i in 0 to 3 loop
				pattern(x) := org(y);
				x := x + 1;
				y := y + 1;
			end loop;
			y := y - 2;
			pattern(x) := org(y);
			y := y - 1;
			x := x + 1;
			if x > 40 then x := 0;
			end if;
		end loop;

		return pattern;
	end function;

impure function load_pattern3a(a : key_array) return key_array is
variable org: key_array;
variable pattern: key_array;
variable x: integer range 0 to 41;
variable y: integer range 0 to 41;
	begin
		org := a;
		x := 0;
		y := 20;

		for j in 0 to 7 loop
			for i in 0 to 3 loop
				pattern(x) := org(y);
				x := x + 1;
				y := y - 1;
			end loop;
			y := y + 2;
			pattern(x) := org(y);
			y := y + 1;
			x := x + 1;
			if x > 40 then x := 0;
			end if;
		end loop;

		return pattern;
	end function;

impure function load_pattern4(a : key_array) return key_array is
variable org: key_array;
variable pattern: key_array;
variable x: integer range 0 to 41;
variable y: integer range 0 to 41;
	begin
		org := a;
		x := 0;
		y := 0;

		pattern(x) := org(y);
		for j in 0 to 19 loop
			y := y + 2;
			x := x + 1;
			pattern(x) := org(y);
			y := y - 1;
			x := x + 1;
			pattern(x) := org(y);
		end loop;

		return pattern;
	end function;
	
impure function load_pattern4a(a : key_array) return key_array is
variable org: key_array;
variable pattern: key_array;
variable x: integer range 0 to 41;
variable y: integer range 0 to 41;
	begin
		org := a;
		x := 0;
		y := 22;

		pattern(x) := org(y);
		for j in 0 to 18 loop
			y := y - 2;
			x := x + 1;
			pattern(x) := org(y);
			y := y + 1;
			x := x + 1;
			pattern(x) := org(y);
		end loop;

		return pattern;
	end function;

impure function load_dtmf(a : integer) return dtmf_array is
variable dtmf: dtmf_array;
variable tone1: integer range 0 to 33000:= 0;
variable tone2: integer range 0 to 33000:= 0;
variable stlv_tone1: std_logic_vector(19 downto 0):= (others=>'0');
variable stlv_tone2: std_logic_vector(19 downto 0):= (others=>'0');
	begin

	case a is
		when 0 =>
			tone1:= OneA;
			tone2:= OneB;

		when 1 =>
			tone1:= TwoA;
			tone2:= TwoB;

		when 2 =>
			tone1:= ThreeA;
			tone2:= ThreeB;

		when 3 =>
			tone1:= FourA;
			tone2:= FourB;

		when 4 =>
			tone1:= FiveA;
			tone2:= FiveB;

		when 5 =>
			tone1:= SixA;
			tone2:= SixB;

		when 6 =>
			tone1:= SevenA;
			tone2:= SevenB;

		when 7 =>
			tone1:= EightA;
			tone2:= EightB;

		when 8 =>
			tone1:= NineA;
			tone2:= NineB;

		when 9 =>
			tone1:= AA;
			tone2:= AB;

		when 10 =>
			tone1:= BA;
			tone2:= BB;

		when 11 =>
			tone1:= CA;
			tone2:= CB;

		when 12 =>
			tone1:= DA;
			tone2:= DB;

		when 13 =>
			tone1:= PoundA;
			tone2:= PoundB;

		when 14 =>
			tone1:= AsterikA;
			tone2:= AsterikB;

		when 15 =>
			tone1:= ZeroA;
			tone2:= ZeroB;

		when others =>
	end case;

		stlv_tone1 := conv_std_logic_vector(tone1,20);
		stlv_tone2 := conv_std_logic_vector(tone2,20);
		dtmf(0) := stlv_tone1;
		dtmf(1) := stlv_tone2;
		
		return dtmf;
	end function;
	
impure function load_keyOfC(a : integer) return key_array is
variable keyOfC: key_array;
	begin
		keyOfC(0) := X"00";
		keyOfC(1) := X"02";
		keyOfC(2) := X"04";
		keyOfC(3) := X"05";
		keyOfC(4) := X"07";
		keyOfC(5) := X"09";
		keyOfC(6) := X"0b";
		keyOfC(7) := X"0c";
		keyOfC(8) := X"0e";
		keyOfC(9) := X"10";
		keyOfC(10) := X"11";
		keyOfC(11) := X"13";
		keyOfC(12) := X"15";
		keyOfC(13) := X"17";
		keyOfC(14) := X"18";
		keyOfC(15) := X"1a";
		keyOfC(16) := X"1c";
		keyOfC(17) := X"1d";
		keyOfC(18) := X"1f";
		keyOfC(19) := X"21";
		keyOfC(20) := X"23";
		keyOfC(21) := X"24";
		keyOfC(22) := X"26";
		keyOfC(23) := X"28";
		keyOfC(24) := X"29";
		keyOfC(25) := X"2b";
		keyOfC(26) := X"2d";
		keyOfC(27) := X"2f";
		keyOfC(28) := X"30";
		keyOfC(29) := X"32";
		keyOfC(30) := X"34";
		keyOfC(31) := X"35";
		keyOfC(32) := X"37";
		keyOfC(33) := X"39";
		keyOfC(34) := X"3b";
		keyOfC(35) := X"3c";
		keyOfC(36) := X"3e";
		keyOfC(37) := X"40";
		keyOfC(38) := X"41";
		keyOfC(39) := X"43";
		keyOfC(40) := X"45";
		keyOfC(41) := X"47";

		return keyOfC;
	end function;

impure function load_keyOfCS(a : integer) return key_array is
variable keyOfCS: key_array;
	begin
		keyOfCS(0) := X"01";
		keyOfCS(1) := X"03";
		keyOfCS(2) := X"05";
		keyOfCS(3) := X"06";
		keyOfCS(4) := X"08";
		keyOfCS(5) := X"0a";
		keyOfCS(6) := X"0c";
		keyOfCS(7) := X"0d";
		keyOfCS(8) := X"0f";
		keyOfCS(9) := X"11";
		keyOfCS(10) := X"12";
		keyOfCS(11) := X"14";
		keyOfCS(12) := X"16";
		keyOfCS(13) := X"18";
		keyOfCS(14) := X"19";
		keyOfCS(15) := X"1b";
		keyOfCS(16) := X"1d";
		keyOfCS(17) := X"1e";
		keyOfCS(18) := X"20";
		keyOfCS(19) := X"22";
		keyOfCS(20) := X"24";
		keyOfCS(21) := X"25";
		keyOfCS(22) := X"27";
		keyOfCS(23) := X"29";
		keyOfCS(24) := X"2a";
		keyOfCS(25) := X"2c";
		keyOfCS(26) := X"2e";
		keyOfCS(27) := X"30";
		keyOfCS(28) := X"31";
		keyOfCS(29) := X"33";
		keyOfCS(30) := X"35";
		keyOfCS(31) := X"36";
		keyOfCS(32) := X"38";
		keyOfCS(33) := X"3a";
		keyOfCS(34) := X"3c";
		keyOfCS(35) := X"3d";
		keyOfCS(36) := X"3f";
		keyOfCS(37) := X"41";
		keyOfCS(38) := X"42";
		keyOfCS(39) := X"44";
		keyOfCS(40) := X"46";
		keyOfCS(41) := X"48";

		return keyOfCS;
	end function;

impure function load_keyOfD(a : integer) return key_array is
variable keyOfD: key_array;
	begin
		keyOfD(0) := X"01";
		keyOfD(1) := X"02";
		keyOfD(2) := X"04";
		keyOfD(3) := X"06";
		keyOfD(4) := X"07";
		keyOfD(5) := X"09";
		keyOfD(6) := X"0b";
		keyOfD(7) := X"0d";
		keyOfD(8) := X"0e";
		keyOfD(9) := X"10";
		keyOfD(10) := X"12";
		keyOfD(11) := X"13";
		keyOfD(12) := X"15";
		keyOfD(13) := X"17";
		keyOfD(14) := X"19";
		keyOfD(15) := X"1a";
		keyOfD(16) := X"1c";
		keyOfD(17) := X"1e";
		keyOfD(18) := X"1f";
		keyOfD(19) := X"21";
		keyOfD(20) := X"23";
		keyOfD(21) := X"25";
		keyOfD(22) := X"26";
		keyOfD(23) := X"28";
		keyOfD(24) := X"29";
		keyOfD(25) := X"2a";
		keyOfD(26) := X"2b";
		keyOfD(27) := X"2d";
		keyOfD(28) := X"2f";
		keyOfD(29) := X"31";
		keyOfD(30) := X"32";
		keyOfD(31) := X"34";
		keyOfD(32) := X"35";
		keyOfD(33) := X"36";
		keyOfD(34) := X"37";
		keyOfD(35) := X"39";
		keyOfD(36) := X"3b";
		keyOfD(37) := X"3d";
		keyOfD(38) := X"3e";
		keyOfD(39) := X"40";
		keyOfD(40) := X"41";
		keyOfD(41) := X"42";

		return keyOfD;
	end function;

impure function load_keyOfDS(a : integer) return key_array is
variable keyOfDS: key_array;
	begin
		keyOfDS(0) := X"00";
		keyOfDS(1) := X"02";
		keyOfDS(2) := X"03";
		keyOfDS(3) := X"05";
		keyOfDS(4) := X"07";
		keyOfDS(5) := X"08";
		keyOfDS(6) := X"0a";
		keyOfDS(7) := X"0c";
		keyOfDS(8) := X"0e";
		keyOfDS(9) := X"0f";
		keyOfDS(10) := X"11";
		keyOfDS(11) := X"13";
		keyOfDS(12) := X"14";
		keyOfDS(13) := X"16";
		keyOfDS(14) := X"18";
		keyOfDS(15) := X"1a";
		keyOfDS(16) := X"1b";
		keyOfDS(17) := X"1d";
		keyOfDS(18) := X"1f";
		keyOfDS(19) := X"20";
		keyOfDS(20) := X"22";
		keyOfDS(21) := X"24";
		keyOfDS(22) := X"26";
		keyOfDS(23) := X"27";
		keyOfDS(24) := X"29";
		keyOfDS(25) := X"2b";
		keyOfDS(26) := X"2c";
		keyOfDS(27) := X"2e";
		keyOfDS(28) := X"30";
		keyOfDS(29) := X"32";
		keyOfDS(30) := X"33";
		keyOfDS(31) := X"35";
		keyOfDS(32) := X"37";
		keyOfDS(33) := X"38";
		keyOfDS(34) := X"3a";
		keyOfDS(35) := X"3c";
		keyOfDS(36) := X"3e";
		keyOfDS(37) := X"3f";
		keyOfDS(38) := X"41";
		keyOfDS(39) := X"43";
		keyOfDS(40) := X"44";
		keyOfDS(41) := X"46";

		return keyOfDS;
	end function;

impure function load_keyOfE(a : integer) return key_array is
variable keyOfE: key_array;
	begin
		keyOfE(0) := X"01";
		keyOfE(1) := X"03";
		keyOfE(2) := X"04";
		keyOfE(3) := X"06";
		keyOfE(4) := X"08";
		keyOfE(5) := X"09";
		keyOfE(6) := X"0b";
		keyOfE(7) := X"0d";
		keyOfE(8) := X"0f";
		keyOfE(9) := X"10";
		keyOfE(10) := X"12";
		keyOfE(11) := X"14";
		keyOfE(12) := X"15";
		keyOfE(13) := X"17";
		keyOfE(14) := X"19";
		keyOfE(15) := X"1b";
		keyOfE(16) := X"1c";
		keyOfE(17) := X"1e";
		keyOfE(18) := X"20";
		keyOfE(19) := X"21";
		keyOfE(20) := X"23";
		keyOfE(21) := X"25";
		keyOfE(22) := X"27";
		keyOfE(23) := X"28";
		keyOfE(24) := X"2a";
		keyOfE(25) := X"2c";
		keyOfE(26) := X"2d";
		keyOfE(27) := X"2f";
		keyOfE(28) := X"31";
		keyOfE(29) := X"33";
		keyOfE(30) := X"34";
		keyOfE(31) := X"36";
		keyOfE(32) := X"38";
		keyOfE(33) := X"39";
		keyOfE(34) := X"3b";
		keyOfE(35) := X"3d";
		keyOfE(36) := X"3f";
		keyOfE(37) := X"40";
		keyOfE(38) := X"42";
		keyOfE(39) := X"44";
		keyOfE(40) := X"45";
		keyOfE(41) := X"47";

		return keyOfE;
	end function;

impure function load_keyOfF(a : integer) return key_array is
variable keyOfF: key_array;
	begin
		keyOfF(0) := X"00";
		keyOfF(1) := X"02";
		keyOfF(2) := X"04";
		keyOfF(3) := X"05";
		keyOfF(4) := X"07";
		keyOfF(5) := X"09";
		keyOfF(6) := X"0a";
		keyOfF(7) := X"0c";
		keyOfF(8) := X"0e";
		keyOfF(9) := X"10";
		keyOfF(10) := X"11";
		keyOfF(11) := X"13";
		keyOfF(12) := X"15";
		keyOfF(13) := X"16";
		keyOfF(14) := X"17";
		keyOfF(15) := X"18";
		keyOfF(16) := X"1a";
		keyOfF(17) := X"1c";
		keyOfF(18) := X"1d";
		keyOfF(19) := X"1f";
		keyOfF(20) := X"21";
		keyOfF(21) := X"22";
		keyOfF(22) := X"23";
		keyOfF(23) := X"24";
		keyOfF(24) := X"26";
		keyOfF(25) := X"28";
		keyOfF(26) := X"29";
		keyOfF(27) := X"2b";
		keyOfF(28) := X"2d";
		keyOfF(29) := X"2e";
		keyOfF(30) := X"2f";
		keyOfF(31) := X"30";
		keyOfF(32) := X"32";
		keyOfF(33) := X"34";
		keyOfF(34) := X"35";
		keyOfF(35) := X"37";
		keyOfF(36) := X"39";
		keyOfF(37) := X"3a";
		keyOfF(38) := X"3b";
		keyOfF(39) := X"3c";
		keyOfF(40) := X"3e";
		keyOfF(41) := X"40";

		return keyOfF;
	end function;

impure function load_keyOfFS(a : integer) return key_array is
variable keyOfFS: key_array;
	begin
		keyOfFS(0) := X"01";
		keyOfFS(1) := X"03";
		keyOfFS(2) := X"05";
		keyOfFS(3) := X"06";
		keyOfFS(4) := X"08";
		keyOfFS(5) := X"0a";
		keyOfFS(6) := X"0b";
		keyOfFS(7) := X"0d";
		keyOfFS(8) := X"0f";
		keyOfFS(9) := X"11";
		keyOfFS(10) := X"12";
		keyOfFS(11) := X"14";
		keyOfFS(12) := X"16";
		keyOfFS(13) := X"17";
		keyOfFS(14) := X"19";
		keyOfFS(15) := X"1b";
		keyOfFS(16) := X"1d";
		keyOfFS(17) := X"1e";
		keyOfFS(18) := X"20";
		keyOfFS(19) := X"22";
		keyOfFS(20) := X"23";
		keyOfFS(21) := X"25";
		keyOfFS(22) := X"27";
		keyOfFS(23) := X"29";
		keyOfFS(24) := X"2a";
		keyOfFS(25) := X"2c";
		keyOfFS(26) := X"2e";
		keyOfFS(27) := X"2f";
		keyOfFS(28) := X"31";
		keyOfFS(29) := X"33";
		keyOfFS(30) := X"35";
		keyOfFS(31) := X"36";
		keyOfFS(32) := X"38";
		keyOfFS(33) := X"3a";
		keyOfFS(34) := X"3b";
		keyOfFS(35) := X"3d";
		keyOfFS(36) := X"3f";
		keyOfFS(37) := X"41";
		keyOfFS(38) := X"42";
		keyOfFS(39) := X"44";
		keyOfFS(40) := X"46";
		keyOfFS(41) := X"47";

		return keyOfFS;
	end function;

impure function load_keyOfG(a : integer) return key_array is
variable keyOfG: key_array;
	begin
		keyOfG(0) := X"00";
		keyOfG(1) := X"02";
		keyOfG(2) := X"04";
		keyOfG(3) := X"06";
		keyOfG(4) := X"07";
		keyOfG(5) := X"09";
		keyOfG(6) := X"0b";
		keyOfG(7) := X"0c";
		keyOfG(8) := X"0e";
		keyOfG(9) := X"10";
		keyOfG(10) := X"12";
		keyOfG(11) := X"13";
		keyOfG(12) := X"15";
		keyOfG(13) := X"17";
		keyOfG(14) := X"18";
		keyOfG(15) := X"1a";
		keyOfG(16) := X"1c";
		keyOfG(17) := X"1e";
		keyOfG(18) := X"1f";
		keyOfG(19) := X"21";
		keyOfG(20) := X"23";
		keyOfG(21) := X"24";
		keyOfG(22) := X"26";
		keyOfG(23) := X"28";
		keyOfG(24) := X"2a";
		keyOfG(25) := X"2b";
		keyOfG(26) := X"2d";
		keyOfG(27) := X"2f";
		keyOfG(28) := X"30";
		keyOfG(29) := X"32";
		keyOfG(30) := X"34";
		keyOfG(31) := X"36";
		keyOfG(32) := X"37";
		keyOfG(33) := X"39";
		keyOfG(34) := X"3b";
		keyOfG(35) := X"3c";
		keyOfG(36) := X"3e";
		keyOfG(37) := X"40";
		keyOfG(38) := X"42";
		keyOfG(39) := X"43";
		keyOfG(40) := X"45";
		keyOfG(41) := X"47";

		return keyOfG;
	end function;

impure function load_keyOfGS(a : integer) return key_array is
variable keyOfGS: key_array;
	begin
		keyOfGS(0) := X"00";
		keyOfGS(1) := X"01";
		keyOfGS(2) := X"03";
		keyOfGS(3) := X"05";
		keyOfGS(4) := X"07";
		keyOfGS(5) := X"08";
		keyOfGS(6) := X"0a";
		keyOfGS(7) := X"0c";
		keyOfGS(8) := X"0d";
		keyOfGS(9) := X"0f";
		keyOfGS(10) := X"11";
		keyOfGS(11) := X"13";
		keyOfGS(12) := X"14";
		keyOfGS(13) := X"16";
		keyOfGS(14) := X"18";
		keyOfGS(15) := X"19";
		keyOfGS(16) := X"1b";
		keyOfGS(17) := X"1d";
		keyOfGS(18) := X"1f";
		keyOfGS(19) := X"20";
		keyOfGS(20) := X"22";
		keyOfGS(21) := X"24";
		keyOfGS(22) := X"25";
		keyOfGS(23) := X"27";
		keyOfGS(24) := X"29";
		keyOfGS(25) := X"2b";
		keyOfGS(26) := X"2c";
		keyOfGS(27) := X"2e";
		keyOfGS(28) := X"30";
		keyOfGS(29) := X"31";
		keyOfGS(30) := X"33";
		keyOfGS(31) := X"35";
		keyOfGS(32) := X"37";
		keyOfGS(33) := X"38";
		keyOfGS(34) := X"3a";
		keyOfGS(35) := X"3c";
		keyOfGS(36) := X"3d";
		keyOfGS(37) := X"3f";
		keyOfGS(38) := X"41";
		keyOfGS(39) := X"43";
		keyOfGS(40) := X"44";
		keyOfGS(41) := X"46";

		return keyOfGS;
	end function;

impure function load_keyOfA(a : integer) return key_array is
variable keyOfA: key_array;
	begin
		keyOfA(0) := X"01";
		keyOfA(1) := X"02";
		keyOfA(2) := X"04";
		keyOfA(3) := X"06";
		keyOfA(4) := X"08";
		keyOfA(5) := X"09";
		keyOfA(6) := X"0b";
		keyOfA(7) := X"0d";
		keyOfA(8) := X"0e";
		keyOfA(9) := X"10";
		keyOfA(10) := X"12";
		keyOfA(11) := X"14";
		keyOfA(12) := X"15";
		keyOfA(13) := X"17";
		keyOfA(14) := X"19";
		keyOfA(15) := X"1a";
		keyOfA(16) := X"1c";
		keyOfA(17) := X"1e";
		keyOfA(18) := X"20";
		keyOfA(19) := X"21";
		keyOfA(20) := X"23";
		keyOfA(21) := X"25";
		keyOfA(22) := X"26";
		keyOfA(23) := X"28";
		keyOfA(24) := X"2a";
		keyOfA(25) := X"2c";
		keyOfA(26) := X"2d";
		keyOfA(27) := X"2f";
		keyOfA(28) := X"31";
		keyOfA(29) := X"32";
		keyOfA(30) := X"34";
		keyOfA(31) := X"36";
		keyOfA(32) := X"38";
		keyOfA(33) := X"39";
		keyOfA(34) := X"3b";
		keyOfA(35) := X"3d";
		keyOfA(36) := X"3e";
		keyOfA(37) := X"40";
		keyOfA(38) := X"42";
		keyOfA(39) := X"44";
		keyOfA(40) := X"45";
		keyOfA(41) := X"47";

		return keyOfA;
	end function;

impure function load_keyOfAS(a : integer) return key_array is
variable keyOfAS: key_array;
	begin
		keyOfAS(0) := X"00";
		keyOfAS(1) := X"02";
		keyOfAS(2) := X"03";
		keyOfAS(3) := X"05";
		keyOfAS(4) := X"07";
		keyOfAS(5) := X"09";
		keyOfAS(6) := X"0a";
		keyOfAS(7) := X"0c";
		keyOfAS(8) := X"0e";
		keyOfAS(9) := X"0f";
		keyOfAS(10) := X"11";
		keyOfAS(11) := X"13";
		keyOfAS(12) := X"15";
		keyOfAS(13) := X"16";
		keyOfAS(14) := X"18";
		keyOfAS(15) := X"1a";
		keyOfAS(16) := X"1b";
		keyOfAS(17) := X"1d";
		keyOfAS(18) := X"1f";
		keyOfAS(19) := X"21";
		keyOfAS(20) := X"22";
		keyOfAS(21) := X"24";
		keyOfAS(22) := X"26";
		keyOfAS(23) := X"27";
		keyOfAS(24) := X"29";
		keyOfAS(25) := X"2b";
		keyOfAS(26) := X"2d";
		keyOfAS(27) := X"2e";
		keyOfAS(28) := X"30";
		keyOfAS(29) := X"32";
		keyOfAS(30) := X"33";
		keyOfAS(31) := X"35";
		keyOfAS(32) := X"36";
		keyOfAS(33) := X"37";
		keyOfAS(34) := X"39";
		keyOfAS(35) := X"3a";
		keyOfAS(36) := X"3c";
		keyOfAS(37) := X"3e";
		keyOfAS(38) := X"3f";
		keyOfAS(39) := X"41";
		keyOfAS(40) := X"43";
		keyOfAS(41) := X"45";

		return keyOfAS;
	end function;

impure function load_keyOfB(a : integer) return key_array is
variable keyOfB: key_array;
	begin
		keyOfB(0) := X"01";
		keyOfB(1) := X"03";
		keyOfB(2) := X"04";
		keyOfB(3) := X"06";
		keyOfB(4) := X"08";
		keyOfB(5) := X"0a";
		keyOfB(6) := X"0b";
		keyOfB(7) := X"0d";
		keyOfB(8) := X"0f";
		keyOfB(9) := X"10";
		keyOfB(10) := X"12";
		keyOfB(11) := X"14";
		keyOfB(12) := X"16";
		keyOfB(13) := X"17";
		keyOfB(14) := X"19";
		keyOfB(15) := X"1b";
		keyOfB(16) := X"1c";
		keyOfB(17) := X"1e";
		keyOfB(18) := X"20";
		keyOfB(19) := X"22";
		keyOfB(20) := X"23";
		keyOfB(21) := X"25";
		keyOfB(22) := X"27";
		keyOfB(23) := X"28";
		keyOfB(24) := X"2a";
		keyOfB(25) := X"2c";
		keyOfB(26) := X"2e";
		keyOfB(27) := X"2f";
		keyOfB(28) := X"31";
		keyOfB(29) := X"33";
		keyOfB(30) := X"34";
		keyOfB(31) := X"36";
		keyOfB(32) := X"38";
		keyOfB(33) := X"3a";
		keyOfB(34) := X"3b";
		keyOfB(35) := X"3d";
		keyOfB(36) := X"3f";
		keyOfB(37) := X"40";
		keyOfB(38) := X"42";
		keyOfB(39) := X"44";
		keyOfB(40) := X"46";
		 keyOfB(41) := X"47";

		return keyOfB;
	end function;
	
impure function get_note(notes : notes_array; a : integer) return std_logic_vector is
variable mynote: integer range 0 to 95:= 0;
variable note: std_logic_vector(19 downto 0);
variable n : notes_array;
	begin
		mynote := a;
		n := notes;
		note := n(mynote);
		return note;
	end function;

impure function load_notes(a : integer) return notes_array is
variable mynotes: notes_array;
	begin
		mynotes(0) := conv_std_logic_vector(C2,20);	--	0
		mynotes(1) := conv_std_logic_vector(CS2,20);	--	1
		mynotes(2) := conv_std_logic_vector(D2,20);	--	2
		mynotes(3) := conv_std_logic_vector(DS2,20);	--	3
		mynotes(4) := conv_std_logic_vector(E2,20);	--	4
		mynotes(5) := conv_std_logic_vector(F2,20);	--	5
		mynotes(6) := conv_std_logic_vector(FS2,20);	--	6
		mynotes(7) := conv_std_logic_vector(G2,20);	--	7
		mynotes(8) := conv_std_logic_vector(GS2,20);	--	8
		mynotes(9) := conv_std_logic_vector(A2,20);	--	9
		mynotes(10) := conv_std_logic_vector(AS2,20);	--	A
		mynotes(11) := conv_std_logic_vector(B2,20);	--	B

		mynotes(12) := conv_std_logic_vector(C3,20);	--	C
		mynotes(13) := conv_std_logic_vector(CS3,20);	--	D
		mynotes(14) := conv_std_logic_vector(D3,20);	--	E
		mynotes(15) := conv_std_logic_vector(DS3,20);	--	F
		mynotes(16) := conv_std_logic_vector(E3,20);	--	10
		mynotes(17) := conv_std_logic_vector(F3,20);	--	11
		mynotes(18) := conv_std_logic_vector(FS3,20);	--	12
		mynotes(19) := conv_std_logic_vector(G3,20);	--	13
		mynotes(20) := conv_std_logic_vector(GS3,20);	--	14
		mynotes(21) := conv_std_logic_vector(A3,20);	--	15
		mynotes(22) := conv_std_logic_vector(AS3,20);	--	16
		mynotes(23) := conv_std_logic_vector(B3,20);	--	17

		mynotes(24) := conv_std_logic_vector(C4,20);	--	18		middle C
		mynotes(25) := conv_std_logic_vector(CS4,20);	--	19
		mynotes(26) := conv_std_logic_vector(D4,20);	--	1A
		mynotes(27) := conv_std_logic_vector(DS4,20);	--	1B
		mynotes(28) := conv_std_logic_vector(E4,20);	--	1C
		mynotes(29) := conv_std_logic_vector(F4,20);	--	1D
		mynotes(30) := conv_std_logic_vector(FS4,20);	--	1E
		mynotes(31) := conv_std_logic_vector(G4,20);	--	1F
		mynotes(32) := conv_std_logic_vector(GS4,20);	--	20
		mynotes(33) := conv_std_logic_vector(A4,20);	--	21
		mynotes(34) := conv_std_logic_vector(AS4,20);	--	22
		mynotes(35) := conv_std_logic_vector(B4,20);	--	23

		mynotes(36) := conv_std_logic_vector(C5,20);	--	24
		mynotes(37) := conv_std_logic_vector(CS5,20);	--	25
		mynotes(38) := conv_std_logic_vector(D5,20);	--	26
		mynotes(39) := conv_std_logic_vector(DS5,20);	--	27
		mynotes(40) := conv_std_logic_vector(E5,20);	--	28
		mynotes(41) := conv_std_logic_vector(F5,20);	--	29
		mynotes(42) := conv_std_logic_vector(FS5,20);	--	2A
		mynotes(43) := conv_std_logic_vector(G5,20);	--	2B
		mynotes(44) := conv_std_logic_vector(GS5,20);	--	2C
		mynotes(45) := conv_std_logic_vector(A5,20);	--	2D
		mynotes(46) := conv_std_logic_vector(AS5,20);	--	2E
		mynotes(47) := conv_std_logic_vector(B5,20);	--	2F

		mynotes(48) := conv_std_logic_vector(C6,20);	--	30
		mynotes(49) := conv_std_logic_vector(CS6,20);	--	31
		mynotes(50) := conv_std_logic_vector(D6,20);	--	32
		mynotes(51) := conv_std_logic_vector(DS6,20);	--	33
		mynotes(52) := conv_std_logic_vector(E6,20);	--	34
		mynotes(53) := conv_std_logic_vector(F6,20);	--	35
		mynotes(54) := conv_std_logic_vector(FS6,20);	--	36
		mynotes(55) := conv_std_logic_vector(G6,20);	--	37
		mynotes(56) := conv_std_logic_vector(GS6,20);	--	38
		mynotes(57) := conv_std_logic_vector(A6,20);	--	39
		mynotes(58) := conv_std_logic_vector(AS6,20);	--	3A
		mynotes(59) := conv_std_logic_vector(B6,20);	--	3B

		mynotes(60) := conv_std_logic_vector(C7,20);	--	3C
		mynotes(61) := conv_std_logic_vector(CS7,20);	--	3D
		mynotes(62) := conv_std_logic_vector(D7,20);	--	3E
		mynotes(63) := conv_std_logic_vector(DS7,20);	--	3F
		mynotes(64) := conv_std_logic_vector(E7,20);	--	40
		mynotes(65) := conv_std_logic_vector(F7,20);	--	41
		mynotes(66) := conv_std_logic_vector(FS7,20);	--	42
		mynotes(67) := conv_std_logic_vector(G7,20);	--	43
		mynotes(68) := conv_std_logic_vector(GS7,20);	--	44
		mynotes(69) := conv_std_logic_vector(A7,20);	--	45
		mynotes(70) := conv_std_logic_vector(AS7,20);	--	46
		mynotes(71) := conv_std_logic_vector(B7,20);	--	47

		mynotes(72) := conv_std_logic_vector(C8,20);	--	48
		mynotes(73) := conv_std_logic_vector(CS8,20);	--	49
		mynotes(74) := conv_std_logic_vector(D8,20);	--	4A
		mynotes(75) := conv_std_logic_vector(DS8,20);	--	4B
		mynotes(76) := conv_std_logic_vector(E8,20);	--	4C
		mynotes(77) := conv_std_logic_vector(F8,20);	--	4D
		mynotes(78) := conv_std_logic_vector(FS8,20);	--	4E
		mynotes(79) := conv_std_logic_vector(G8,20);	--	4F
		mynotes(80) := conv_std_logic_vector(GS8,20);	--	50
		mynotes(81) := conv_std_logic_vector(A8,20);	--	51
		mynotes(82) := conv_std_logic_vector(AS8,20);	--	52

		mynotes(83) := conv_std_logic_vector(C9,20);	--	53
		mynotes(84) := conv_std_logic_vector(CS9,20);	--	54
		mynotes(85) := conv_std_logic_vector(D9,20);	--	55
		mynotes(86) := conv_std_logic_vector(DS9,20);	--	56
		mynotes(87) := conv_std_logic_vector(E9,20);	--	57
		mynotes(88) := conv_std_logic_vector(F9,20);	--	58
		mynotes(89) := conv_std_logic_vector(FS9,20);	--	59
		mynotes(90) := conv_std_logic_vector(G9,20);	--	5A
		mynotes(91) := conv_std_logic_vector(GS9,20);	--	5B
		mynotes(92) := conv_std_logic_vector(A9,20);	--	5C

		mynotes(93) := conv_std_logic_vector(AS9,20);	--	5D
		mynotes(94) := conv_std_logic_vector(B9,20);	--	5E
		mynotes(95) := conv_std_logic_vector(B9,20);	--	5F
		return mynotes;
	end function;

impure function div_int(a : integer; b : integer) return integer is
variable temp1: integer range 0 to 528290:= 0;
variable temp2: integer range 0 to 528290:= 0;
variable temp4: unsigned(20 downto 0);

	begin
		temp1 := a;
		temp2 := b;
--		temp2 := a/b;
		return temp2;
	end function;

impure function average(a : in my_array3) return std_logic_vector is
	variable temp: integer range 0 to 1966065:= 0;
	variable temp2: std_logic_vector(16 downto 0);
	variable temp3: my_array3;
	variable temp4: unsigned(20 downto 0);
	
	begin
		temp3:= a;
		for i in 0 to 15 loop
			temp:= temp + conv_integer(temp3(i));
		end loop;
		temp4:= to_unsigned(temp,21);
		temp4:= temp4 srl 4;	-- divide by 16
		temp:= to_integer(temp4);
		temp2:= conv_std_logic_vector(temp,17);
	return temp2;
	end function;

impure function shift_avg(a : in my_array3; b : in std_logic_vector) return my_array3 is
	variable temp3: my_array3;
	begin
		temp3:= a;
		for i in 0 to 14 loop
			temp3(i) := temp3(i+1);
		end loop;
		temp3(15) := b;
		return temp3;
	end function;

impure function block_ff(a : in std_logic_vector) return std_logic_vector is
		variable temp: integer range 0 to 255:= 0;
		variable temp2: std_logic_vector(7 downto 0);
		
		begin
			temp2:= a;
			temp:= conv_integer(temp2);
			if temp = 255 then
				temp:= 254;
			end if;
			return conv_std_logic_vector(temp,8);
		end function;	
	
end package body;

--signal quotient: integer := 0;
--signal remainder: integer := 0;
--signal division_complete: std_logic := '0';
---- Begin architecture
--process(clk, dividend, divisor, quotient, remainder)
--  variable n_quotient: integer := 0;
--  variable n_remainder: integer := 0;
--  variable n_division_complete: std_logic := '0';
--begin
--  n_quotient := quotient;
--  n_remainder := remainder;
--  n_division_complete := '0';
--  if (dividend < divisor) then
--    -- End of division
--    n_division_complete := '1';
--  elif (division_complete = '1') then
--    -- Start of next division
--    n_division_complete := '1';
--    n_quotient := 0;
--  else
--    -- Division in progress
--    n_quotient := n_quotient + 1;
--    n_remainder := dividend - divisor;
--  end if;
--  if (clk'event and clk = '1') then
--    quotient <= n_quotient;
--    remainder <= n_remainder;
--    division_complete <= n_division_complete;
--  end if;
--end process;

--	function get_array(a : in std_logic_vector) return std_logic_vector is
--	variable temp1 : std_logic_vector(7 downto 0);
--	variable temp2 : integer range 0 to NUM_DATA_ARRAY:= 0;
--	begin
--		temp1 := conv_std_logic_vector(a,8);
--		temp2 := conv_integer(a);
--		if temp2 > NUM_DATA_ARRAY then
--			temp2 := 0;
--		end if;
--		temp1 <= data_array1(temp2);
--		return temp1;
--	end function;

