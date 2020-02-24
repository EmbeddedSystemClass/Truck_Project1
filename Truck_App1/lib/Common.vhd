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

	constant pwm_delay1: integer:= 8187;
	constant pwm_delay2: integer:= 16374;
	constant pwm_delay3: integer:= 24561;
	constant pwm_delay4: integer:= 32748;
	constant pwm_delay5: integer:= 40935;
	constant pwm_delay6: integer:= 49122;
	constant pwm_delay7: integer:= 57309;
	constant pwm_delay8: integer:= 65496;
	constant pwm_delay9: integer:= 73683;
	constant pwm_delay10: integer:= 81870;
	constant pwm_delay11: integer:= 90057;
	constant pwm_delay12: integer:= 98244;
	constant pwm_delay13: integer:= 106431;
	constant pwm_delay14: integer:= 114618;
	constant pwm_delay15: integer:= 122805;
	constant pwm_delay16: integer:= 130992;
	constant pwm_delay17: integer:= 139179;
	constant pwm_delay18: integer:= 147366;
	constant pwm_delay19: integer:= 155553;
	constant pwm_delay20: integer:= 163740;
	constant pwm_delay21: integer:= 171927;
	constant pwm_delay22: integer:= 180114;
	constant pwm_delay23: integer:= 188301;
	constant pwm_delay24: integer:= 196488;
	constant pwm_delay25: integer:= 204675;
	constant pwm_delay26: integer:= 212862;
	constant pwm_delay27: integer:= 221049;
	constant pwm_delay28: integer:= 229236;
	constant pwm_delay29: integer:= 237423;
	constant pwm_delay30: integer:= 245610;
	constant pwm_delay31: integer:= 253797;
	constant pwm_delay32: integer:= 262000;

	
	-- constant pwm_delay1: integer:= 8191;
	-- constant pwm_delay2: integer:= 16382;
	-- constant pwm_delay3: integer:= 24573;
	-- constant pwm_delay4: integer:= 32764;
	-- constant pwm_delay5: integer:= 40955;
	-- constant pwm_delay6: integer:= 49146;
	-- constant pwm_delay7: integer:= 57337;
	-- constant pwm_delay8: integer:= 65528;
	-- constant pwm_delay9: integer:= 73719;
	-- constant pwm_delay10: integer:= 81910;
	-- constant pwm_delay11: integer:= 90101;
	-- constant pwm_delay12: integer:= 98292;
	-- constant pwm_delay13: integer:= 106483;
	-- constant pwm_delay14: integer:= 114674;
	-- constant pwm_delay15: integer:= 122865;
	-- constant pwm_delay16: integer:= 131056;
	-- constant pwm_delay17: integer:= 139247;
	-- constant pwm_delay18: integer:= 147438;
	-- constant pwm_delay19: integer:= 155629;
	-- constant pwm_delay20: integer:= 163820;
	-- constant pwm_delay21: integer:= 172011;
	-- constant pwm_delay22: integer:= 180202;
	-- constant pwm_delay23: integer:= 188393;
	-- constant pwm_delay24: integer:= 196584;
	-- constant pwm_delay25: integer:= 204775;
	-- constant pwm_delay26: integer:= 212966;
	-- constant pwm_delay27: integer:= 221157;
	-- constant pwm_delay28: integer:= 229348;
	-- constant pwm_delay29: integer:= 237539;
	-- constant pwm_delay30: integer:= 245730;
	-- constant pwm_delay31: integer:= 253921;
	-- constant pwm_delay32: integer:= 262143;
	
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
	constant TIME_DELAY7:  integer:= 1000000;	-- 40ms (25Hz)
	constant TIME_DELAY8:  integer:= 500000;	-- 20ms (50Hz)
	constant TIME_DELAY8a:  integer:= 300000;	-- 12ms
	constant TIME_DELAY8b:  integer:= 150000;	-- 6ms
	constant TIME_DELAY8c:  integer:= 100000;	-- 4ms
	constant TIME_DELAY9:  integer:=  40000;	-- 1.6ms
--	constant TIME_DELAY_500RPM:  integer:=  5454260;
	constant TIME_DELAY_500RPM:  integer:=  16000000;	-- 330 rpm
	
-- baud rates for 7-seg displays	
	constant DVSR_MU_9600: integer:= 320;	-- 9600 baud
	constant DVSR_MU_19200: integer:= 163;	-- 19200 baud
	constant DVSR_MU_115200: integer:= 27;	-- 115200 baud
-- mph is currently for (2) magnets on driveshaft
--	constant MPH_CLOCK_COUNT: integer:= 12500;
	constant MPH_CLOCK_COUNT: integer:= 8000;

--	constant MPH_DVND: integer:= 128514;

--	constant MPH_CLOCK_COUNT: integer:= 25000;
--	constant MPH_DVND: integer:= 128514;
	constant MPH_DVND: integer:= 20000;
												-- 26ms should be 2254
--	constant RPM_CLOCK_COUNT: integer:= 500;
--	constant RPM_CLOCK_COUNT: integer:= 454;
	constant RPM_CLOCK_COUNT: integer:= 908;
	constant RPM_DVND: integer:= 6000000;
	constant DVND_SIZE: integer:= 30;
	constant RESULT_SIZE: integer:= 18;
	constant PRD_SIZE: integer:= 17;
	constant DVSR_SIZE: integer:= 23;
	constant W2_SIZE: integer:= 19;
	constant MPH_P_REG_SIZE: integer:= 16;
	constant FP_RPM_MINIMUM: integer:= 100;
	constant RPM_MAXIMUM: integer:= 4000;
	constant RPM_MINIMUM: integer:= 3900;
	constant MAX_TIME:  integer:=  131071;
	constant SIZE_32: integer:= 32;
	constant MAX_TUNE_LEN: integer:= 20;

	constant SET_FPGA_SEND_UPDATE_RATE: std_logic_vector(7 downto 0):= X"01";
	constant SET_UPDATE_RATE: std_logic_vector(7 downto 0):= X"02";
	constant DTMF_TONE_ON: std_logic_vector(7 downto 0):= X"03";
	constant DTMF_TONE_OFF: std_logic_vector(7 downto 0):= X"04";
	constant SPECIAL_TONE_ON: std_logic_vector(7 downto 0):= X"05";
	constant LCD_PWM: std_logic_vector(7 downto 0):= X"06";
	constant TUNE_ON: std_logic_vector(7 downto 0):= X"07";
	constant TUNE_OFF: std_logic_vector(7 downto 0):= X"08";
	constant LOAD_TUNE: std_logic_vector(7 downto 0):= X"09";
	constant SHOW_DOWNLOAD: std_logic_vector(7 downto 0):= X"0A";
	constant ADC_CTL: std_logic_vector(7 downto 0):= X"0B";

	constant  PWM_OFF_PARAM: std_logic_vector(7 downto 0):= X"01";
	constant  PWM_ON_PARAM: std_logic_vector(7 downto 0):= X"1F";
	constant  PWM_80DC_PARAM: std_logic_vector(7 downto 0):= X"1A";
	constant  PWM_75DC_PARAM: std_logic_vector(7 downto 0):= X"16";
	constant  PWM_60DC_PARAM: std_logic_vector(7 downto 0):= X"12";
	constant  PWM_50DC_PARAM: std_logic_vector(7 downto 0):= X"0E";
	constant  PWM_30DC_PARAM: std_logic_vector(7 downto 0):= X"0A";
	constant  PWM_25DC_PARAM: std_logic_vector(7 downto 0):= X"06";
	constant  PWM_12DC_PARAM: std_logic_vector(7 downto 0):= X"02";

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
--	type dkey_array is array(0 to 82) of std_logic_vector(7 downto 0);
	type tune_array is array(0 to MAX_TUNE_LEN-1, 0 to 2) of integer range 0 to 95;
	type dl_array is array(0 to 25) of std_logic_vector(7 downto 0);
	type led_array is array(0 to 8) of std_logic_vector(7 downto 0);
	type mcp_array is array(0 to 9) of std_logic_vector(7 downto 0);
	
	type dtmf_array is array(0 to 1) of std_logic_vector(19 downto 0);
	type data_queue is array(0 to 100) of std_logic_vector(7 downto 0);

	impure function average(a : in my_array3) return std_logic_vector;
	impure function shift_avg(a : in my_array3; b : in std_logic_vector) return my_array3;
	impure function block_ff(a : in std_logic_vector) return std_logic_vector;
	impure function div_int(a : integer; b : integer) return integer;
--	impure function add_queue(a : std_logic_vector) return integer;
--	impure function add_queue(a : std_logic_vector) return integer;
	impure function manage_queue(a : std_logic_vector; dq : data_queue; stlv_ptr : std_logic_vector; add : integer) return std_logic_vector;
	impure function load_dtmf(a : integer) return dtmf_array;
	impure function load_pwm_delay(a : integer) return std_logic_vector;
	impure function load_notes_array(a : notes_array) return notes_array;
	impure function load_tune_array(a : tune_array) return tune_array;
	impure function convert_dl_to_tune_array(a : dl_array) return tune_array;

end package;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

package body CommonPckg is

-- 8192(0x2000) * 16 = 131056(0x20000)
-- to divide by 16 rol by 4 bits

impure function manage_queue(a : std_logic_vector; dq : data_queue; stlv_ptr : std_logic_vector; add : integer) return std_logic_vector is
variable data_in: std_logic_vector(7 downto 0);
variable data_ptr: std_logic_vector(7 downto 0);
variable dq1: data_queue;
variable ptr: integer range 0 to 255:= 0;
	begin
		data_in := a;
		dq1 := dq;
		data_ptr := stlv_ptr;
		ptr := conv_integer(data_ptr);

		if add > 0 then
			ptr := ptr + 1;
			dq1(ptr) := data_in;
		else 
			ptr := ptr - 1;
			data_in := dq1(ptr);
		end if;
		data_ptr := conv_std_logic_vector(ptr,8);
		return data_in;
	end function;

-- impure function remove_queue(a : std_logic_vector; dq : data_queue; ptr1 : unsigned) return unsigned is
-- variable data_in: std_logic_vector(7 downto 0);
-- variable dq1: data_queue;
-- variable ptr: integer range 0 to 255:= 0;
-- variable uptr: unsigned(7 downto 0):= (others=>'0');
	-- begin
		-- data_in := a;
		-- dq1 := dq;
		
		-- ptr := to_integer(ptr1);

		-- ptr := ptr - 1;
		-- uptr := to_unsigned(ptr,8);
		-- dq1(ptr) := data_in;
		-- return uptr;
	-- end function;

impure function convert_dl_to_tune_array(a : dl_array) return tune_array is
	 variable tune1: tune_array;
	 variable dl: dl_array;
	 variable temp1: integer range 0 to MAX_TUNE_LEN-1:= 0;
	 variable temp2: integer range 0 to 2:= 0;
	 variable temp3: integer range 0 to 60:= 0;
	 begin
		 dl := a;
		 
		 -- for i in 0 to 19 loop
			-- temp1:= conv_integer(i);
			-- for j in 0 to 2 loop
				-- temp2:= conv_integer(j);
				-- tune1(temp1,temp2) := conv_integer(dl(temp3));
				-- temp3:= temp3 + 1;
			-- end loop;
		-- end loop;	
		return tune1;
	end function;

-- impure function convert_dl_to_tune_array(a : dl_array) return tune_array is
	-- variable tune1: tune_array;
	-- variable dl: dl_array;
	-- begin
		-- dl := a;
		-- tune1(0,0) := conv_integer(dl(0));
		-- tune1(0,1) := conv_integer(dl(1));
		-- tune1(0,2) := conv_integer(dl(2));
		-- tune1(1,0) := conv_integer(dl(3));
		-- tune1(1,1) := conv_integer(dl(4));
		-- tune1(1,2) := conv_integer(dl(5));
		-- tune1(2,0) := conv_integer(dl(6));
		-- tune1(2,1) := conv_integer(dl(7));
		-- tune1(2,2) := conv_integer(dl(8));
		-- tune1(3,0) := conv_integer(dl(9));
		-- tune1(3,1) := conv_integer(dl(10));
		-- tune1(3,2) := conv_integer(dl(11));
		-- tune1(4,0) := conv_integer(dl(12));
		-- tune1(4,1) := conv_integer(dl(13));
		-- tune1(4,2) := conv_integer(dl(14));
		-- tune1(5,0) := conv_integer(dl(15));
		-- tune1(5,1) := conv_integer(dl(16));
		-- tune1(5,2) := conv_integer(dl(17));
		-- tune1(6,0) := conv_integer(dl(18));
		-- tune1(6,1) := conv_integer(dl(19));
		-- tune1(6,2) := conv_integer(dl(20));
		-- tune1(7,0) := conv_integer(dl(21));
		-- tune1(7,1) := conv_integer(dl(22));
		-- tune1(7,2) := conv_integer(dl(23));
		-- tune1(8,0) := conv_integer(dl(24));
		-- tune1(8,1) := conv_integer(dl(25));
		-- tune1(8,2) := conv_integer(dl(26));
		-- tune1(9,0) := conv_integer(dl(27));
		-- tune1(9,1) := conv_integer(dl(28));
		-- tune1(9,2) := conv_integer(dl(29));
		-- tune1(10,0) := conv_integer(dl(30));
		-- tune1(10,1) := conv_integer(dl(31));
		-- tune1(10,2) := conv_integer(dl(32));
		-- tune1(11,0) := conv_integer(dl(33));
		-- tune1(11,1) := conv_integer(dl(34));
		-- tune1(11,2) := conv_integer(dl(35));
		-- tune1(12,0) := conv_integer(dl(36));
		-- tune1(12,1) := conv_integer(dl(37));
		-- tune1(12,2) := conv_integer(dl(38));
		-- tune1(13,0) := conv_integer(dl(39));
		-- tune1(13,1) := conv_integer(dl(40));
		-- tune1(13,2) := conv_integer(dl(41));
		-- tune1(14,0) := conv_integer(dl(42));
		-- tune1(14,1) := conv_integer(dl(43));
		-- tune1(14,2) := conv_integer(dl(44));
		-- tune1(15,0) := conv_integer(dl(45));
		-- tune1(15,1) := conv_integer(dl(46));
		-- tune1(15,2) := conv_integer(dl(47));
		-- tune1(16,0) := conv_integer(dl(48));
		-- tune1(16,1) := conv_integer(dl(49));
		-- tune1(16,2) := conv_integer(dl(50));
		-- tune1(17,0) := conv_integer(dl(51));
		-- tune1(17,1) := conv_integer(dl(52));
		-- tune1(17,2) := conv_integer(dl(53));
		-- tune1(18,0) := conv_integer(dl(54));
		-- tune1(18,1) := conv_integer(dl(55));
		-- tune1(18,2) := conv_integer(dl(56));
		-- tune1(19,0) := conv_integer(dl(57));
		-- tune1(19,1) := conv_integer(dl(58));
		-- tune1(19,2) := conv_integer(dl(59));

		-- return tune1;
	-- end function;

impure function load_tune_array(a : tune_array) return tune_array is
	variable tune1: tune_array;
	begin
		tune1 := a;

		tune1(0,0) := 10;
		tune1(0,1) := 14;
		tune1(0,2) := 0;

		tune1(1,0) := 12;
		tune1(1,1) := 16;
		tune1(1,2) := 0;

		tune1(2,0) := 14;
		tune1(2,1) := 18;
		tune1(2,2) := 0;

		tune1(3,0) := 16;
		tune1(3,1) := 20;
		tune1(3,2) := 0;

		tune1(4,0) := 18;
		tune1(4,1) := 22;
		tune1(4,2) := 0;

		tune1(5,0) := 20;
		tune1(5,1) := 24;
		tune1(5,2) := 0;

		tune1(6,0) := 22;
		tune1(6,1) := 26;
		tune1(6,2) := 0;

		tune1(7,0) := 24;
		tune1(7,1) := 28;
		tune1(7,2) := 0;

		tune1(8,0) := 26;
		tune1(8,1) := 30;
		tune1(8,2) := 0;

		tune1(9,0) := 28;
		tune1(9,1) := 32;
		tune1(9,2) := 0;

		tune1(10,0) := 30;
		tune1(10,1) := 34;
		tune1(10,2) := 7;

		tune1(11,0) := 28;
		tune1(11,1) := 32;
		tune1(11,2) := 7;

		tune1(12,0) := 26;
		tune1(12,1) := 30;
		tune1(12,2) := 7;

		tune1(13,0) := 24;
		tune1(13,1) := 38;
		tune1(13,2) := 7;

		tune1(14,0) := 22;
		tune1(14,1) := 26;
		tune1(14,2) := 7;

		tune1(15,0) := 20;
		tune1(15,1) := 24;
		tune1(15,2) := 7;

		tune1(16,0) := 18;
		tune1(16,1) := 22;
		tune1(16,2) := 7;

		tune1(17,0) := 16;
		tune1(17,1) := 20;
		tune1(17,2) := 7;

		tune1(18,0) := 14;
		tune1(18,1) := 18;
		tune1(18,2) := 7;

		tune1(19,0) := 12;
		tune1(19,1) := 16;
		tune1(19,2) := 7;
	
		return tune1;
	
	end function;

impure function load_notes_array(a : notes_array) return notes_array is
	variable notes : notes_array;
	begin
		notes := a;
		notes(0) := conv_std_logic_vector(C2,20);	--	0
		notes(1) := conv_std_logic_vector(CS2,20);	--	1
		notes(2) := conv_std_logic_vector(D2,20);	--	2
		notes(3) := conv_std_logic_vector(DS2,20);	--	3
		notes(4) := conv_std_logic_vector(E2,20);	--	4
		notes(5) := conv_std_logic_vector(F2,20);	--	5
		notes(6) := conv_std_logic_vector(FS2,20);	--	6
		notes(7) := conv_std_logic_vector(G2,20);	--	7
		notes(8) := conv_std_logic_vector(GS2,20);	--	8
		notes(9) := conv_std_logic_vector(A2,20);	--	9
		notes(10) := conv_std_logic_vector(AS2,20);	--	A
		notes(11) := conv_std_logic_vector(B2,20);	--	B

		notes(12) := conv_std_logic_vector(C3,20);	--	C
		notes(13) := conv_std_logic_vector(CS3,20);	--	D
		notes(14) := conv_std_logic_vector(D3,20);	--	E
		notes(15) := conv_std_logic_vector(DS3,20);	--	F
		notes(16) := conv_std_logic_vector(E3,20);	--	10
		notes(17) := conv_std_logic_vector(F3,20);	--	11
		notes(18) := conv_std_logic_vector(FS3,20);	--	12
		notes(19) := conv_std_logic_vector(G3,20);	--	13
		notes(20) := conv_std_logic_vector(GS3,20);	--	14
		notes(21) := conv_std_logic_vector(A3,20);	--	15
		notes(22) := conv_std_logic_vector(AS3,20);	--	16
		notes(23) := conv_std_logic_vector(B3,20);	--	17

		notes(24) := conv_std_logic_vector(C4,20);	--	18		middle C
		notes(25) := conv_std_logic_vector(CS4,20);	--	19
		notes(26) := conv_std_logic_vector(D4,20);	--	1A
		notes(27) := conv_std_logic_vector(DS4,20);	--	1B
		notes(28) := conv_std_logic_vector(E4,20);	--	1C
		notes(29) := conv_std_logic_vector(F4,20);	--	1D
		notes(30) := conv_std_logic_vector(FS4,20);	--	1E
		notes(31) := conv_std_logic_vector(G4,20);	--	1F
		notes(32) := conv_std_logic_vector(GS4,20);	--	20
		notes(33) := conv_std_logic_vector(A4,20);	--	21
		notes(34) := conv_std_logic_vector(AS4,20);	--	22
		notes(35) := conv_std_logic_vector(B4,20);	--	23

		notes(36) := conv_std_logic_vector(C5,20);	--	24
		notes(37) := conv_std_logic_vector(CS5,20);	--	25
		notes(38) := conv_std_logic_vector(D5,20);	--	26
		notes(39) := conv_std_logic_vector(DS5,20);	--	27
		notes(40) := conv_std_logic_vector(E5,20);	--	28
		notes(41) := conv_std_logic_vector(F5,20);	--	29
		notes(42) := conv_std_logic_vector(FS5,20);	--	2A
		notes(43) := conv_std_logic_vector(G5,20);	--	2B
		notes(44) := conv_std_logic_vector(GS5,20);	--	2C
		notes(45) := conv_std_logic_vector(A5,20);	--	2D
		notes(46) := conv_std_logic_vector(AS5,20);	--	2E
		notes(47) := conv_std_logic_vector(B5,20);	--	2F

		notes(48) := conv_std_logic_vector(C6,20);	--	30
		notes(49) := conv_std_logic_vector(CS6,20);	--	31
		notes(50) := conv_std_logic_vector(D6,20);	--	32
		notes(51) := conv_std_logic_vector(DS6,20);	--	33
		notes(52) := conv_std_logic_vector(E6,20);	--	34
		notes(53) := conv_std_logic_vector(F6,20);	--	35
		notes(54) := conv_std_logic_vector(FS6,20);	--	36
		notes(55) := conv_std_logic_vector(G6,20);	--	37
		notes(56) := conv_std_logic_vector(GS6,20);	--	38
		notes(57) := conv_std_logic_vector(A6,20);	--	39
		notes(58) := conv_std_logic_vector(AS6,20);	--	3A
		notes(59) := conv_std_logic_vector(B6,20);	--	3B

		notes(60) := conv_std_logic_vector(C7,20);	--	3C
		notes(61) := conv_std_logic_vector(CS7,20);	--	3D
		notes(62) := conv_std_logic_vector(D7,20);	--	3E
		notes(63) := conv_std_logic_vector(DS7,20);	--	3F
		notes(64) := conv_std_logic_vector(E7,20);	--	40
		notes(65) := conv_std_logic_vector(F7,20);	--	41
		notes(66) := conv_std_logic_vector(FS7,20);	--	42
		notes(67) := conv_std_logic_vector(G7,20);	--	43
		notes(68) := conv_std_logic_vector(GS7,20);	--	44
		notes(69) := conv_std_logic_vector(A7,20);	--	45
		notes(70) := conv_std_logic_vector(AS7,20);	--	46
		notes(71) := conv_std_logic_vector(B7,20);	--	47

		notes(72) := conv_std_logic_vector(C8,20);	--	48
		notes(73) := conv_std_logic_vector(CS8,20);	--	49
		notes(74) := conv_std_logic_vector(D8,20);	--	4A
		notes(75) := conv_std_logic_vector(DS8,20);	--	4B
		notes(76) := conv_std_logic_vector(E8,20);	--	4C
		notes(77) := conv_std_logic_vector(F8,20);	--	4D
		notes(78) := conv_std_logic_vector(FS8,20);	--	4E
		notes(79) := conv_std_logic_vector(G8,20);	--	4F
		notes(80) := conv_std_logic_vector(GS8,20);	--	50
		notes(81) := conv_std_logic_vector(A8,20);	--	51
		notes(82) := conv_std_logic_vector(AS8,20);	--	52

		notes(83) := conv_std_logic_vector(C9,20);	--	53
		notes(84) := conv_std_logic_vector(CS9,20);	--	54
		notes(85) := conv_std_logic_vector(D9,20);	--	55
		notes(86) := conv_std_logic_vector(DS9,20);	--	56
		notes(87) := conv_std_logic_vector(E9,20);	--	57
		notes(88) := conv_std_logic_vector(F9,20);	--	58
		notes(89) := conv_std_logic_vector(FS9,20);	--	59
		notes(90) := conv_std_logic_vector(G9,20);	--	5A
		notes(91) := conv_std_logic_vector(GS9,20);	--	5B
		notes(92) := conv_std_logic_vector(A9,20);	--	5C

		notes(93) := conv_std_logic_vector(AS9,20);	--	5D
		notes(94) := conv_std_logic_vector(B9,20);	--	5E
		notes(95) := conv_std_logic_vector(B9,20);	--	5F
		return notes;
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

impure function load_pwm_delay(a : integer) return std_logic_vector is
variable delay: integer range 0 to 262145:= 0;
variable stlv_delay: std_logic_vector(W2_SIZE-1 downto 0):= (others=>'0');
	begin

	case a is
		when 0 =>
			delay:= pwm_delay1;
		
		when 1 =>
			delay:= pwm_delay2;

		when 2 =>
			delay:= pwm_delay3;

		when 3 =>
			delay:= pwm_delay4;

		when 4 =>
			delay:= pwm_delay5;

		when 5 =>
			delay:= pwm_delay6;

		when 6 =>
			delay:= pwm_delay7;

		when 7 =>
			delay:= pwm_delay8;

		when 8 =>
			delay:= pwm_delay9;

		when 9 =>
			delay:= pwm_delay10;

		when 10 =>
			delay:= pwm_delay11;

		when 11 =>
			delay:= pwm_delay12;

		when 12 =>
			delay:= pwm_delay13;

		when 13 =>
			delay:= pwm_delay14;

		when 14 =>
			delay:= pwm_delay15;

		when 15 =>
			delay:= pwm_delay16;

		when 16 =>
			delay:= pwm_delay17;

		when 17 =>
			delay:= pwm_delay18;

		when 18 =>
			delay:= pwm_delay19;

		when 19 =>
			delay:= pwm_delay20;

		when 20 =>
			delay:= pwm_delay21;

		when 21 =>
			delay:= pwm_delay22;

		when 22 =>
			delay:= pwm_delay23;

		when 23 =>
			delay:= pwm_delay24;

		when 24 =>
			delay:= pwm_delay25;

		when 25 =>
			delay:= pwm_delay26;

		when 26 =>
			delay:= pwm_delay27;

		when 27 =>
			delay:= pwm_delay28;

		when 28 =>
			delay:= pwm_delay29;

		when 29 =>
			delay:= pwm_delay30;

		when 30 =>
			delay:= pwm_delay31;

		when 31 =>
			delay:= pwm_delay32;

		when others =>
			delay:= 99;
	end case;

	stlv_delay:= conv_std_logic_vector(delay,W2_SIZE);

	return stlv_delay;
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
