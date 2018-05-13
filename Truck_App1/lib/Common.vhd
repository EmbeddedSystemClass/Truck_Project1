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
	constant SET_TEST_CMD4: std_logic_vector(4 downto 0):= "00111";

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
	type tune_array is array(0 to 7) of std_logic_vector(7 downto 0);
	type tunes is array(0 to 5) of tune_array;

	impure function average(a : in my_array3) return std_logic_vector;
	impure function shift_avg(a : in my_array3; b : in std_logic_vector) return my_array3;
	impure function block_ff(a : in std_logic_vector) return std_logic_vector;
	impure function div_int(a : integer; b : integer) return integer;

end package;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

package body CommonPckg is

-- 8192(0x2000) * 16 = 131056(0x20000)
-- to divide by 16 rol by 4 bits

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

