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
	constant TIME_DELAY6:  integer:= 4191300;	-- 167ms
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

	impure function average(a : in my_array3) return std_logic_vector;
	impure function shift_avg(a : in my_array3; b : in std_logic_vector) return my_array3;
	impure function block_ff(a : in std_logic_vector) return std_logic_vector;

end package;

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

package body CommonPckg is

-- 8192(0x2000) * 16 = 131056(0x20000)
-- to divide by 16 rol by 4 bits

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

