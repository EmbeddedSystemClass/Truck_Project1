---------------------------------------------------------------------------------------------------
--*************************************************************************************************
--  CreateDate  :  2009-03-24 
--  ModifData   :  2009-03-24 
--  Description :  Frequency For KeyBoard 
--  Author      :  Explorer01 
--  Version     :  V1.0  
--*************************************************************************************************
---------------------------------------------------------------------------------------------------

-- VHDL library Declarations 
LIBRARY IEEE;
USE IEEE.std_logic_1164.ALL;
USE IEEE.STD_LOGIC_ARITH.ALL;
USE IEEE.std_logic_unsigned.ALL;

---------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------
-- The Entity Declarations 
ENTITY Frequency IS
	PORT 
	(
		reset: 		IN STD_LOGIC; 
		clk: 	IN STD_LOGIC; 		-- 50 MHz 
		manual_reset: in std_logic;
		ClockScan:	OUT STD_LOGIC;
		KeyScan:	OUT STD_LOGIC
	);
END Frequency;

---------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------
-- The Architecture of Entity Declarations 
ARCHITECTURE Frequency_arch OF Frequency IS
	--Clock: 
	signal Period: STD_LOGIC;
--	constant N: integer:= 18;	-- Period1uS = 2.6ms
--	constant N: integer:= 17;	-- 1.3ms
--	constant N: integer:= 16;	--  658us
--	constant N: integer:= 15;	-- 325us
	constant N: integer:= 14;	-- 153us
--	constant N: integer:= 12;	-- 41us -- anything shorter
	-- than this gives inconsistent results for valid output
	signal q_reg, q_next: unsigned(N-1 downto 0);
	signal t_reg, t_next: unsigned(N-1 downto 0);
	signal temp1: std_logic_vector(3 downto 0);
	signal temp2: std_logic_vector(3 downto 0);

begin
	
	-------------------------------------------------
	-- GCLK: 1MHz(1uS), 1KHz(1mS), 1Hz(1S) 
	clk_proc: process( reset, clk, temp1)
	begin 
		------------------------------------
		--Period: 1uS (Period <= clk; )
		if reset = '0' then
			q_reg <= (others=>'0');
			temp1 <= (others=>'0');
			Period <= '0';
		elsif  clk'event and clk='1'  then
			temp1 <= std_logic_vector(q_reg(N-1 downto N-4));
			if temp1 = "1000" then
				q_reg <= (others=>'0');
			else
				q_reg <= q_next;
			end if;
		end if;
		Period <= temp1(3);
	end process;
	
	q_next <= q_reg + 1;
	
	KeyScan <= Period;

	process(reset,Period,manual_reset)
	begin
		if reset = '0' then
			t_reg <= (others=>'0');
			t_next <= (others=>'0');
			temp2 <= (others=>'0');
		elsif Period'event and Period = '1' then
			if manual_reset = '1' then
				t_reg <= (others=>'0');
				t_next <= (others=>'0');
				temp2 <= (others=>'0');
			else
				temp2 <= std_logic_vector(t_reg(N-4 downto N-7));
	--			if temp2 > "1000" then
				if temp2(3) = '1' then
					t_reg <= (others=>'0');
				else
					t_reg <= t_next;
				end if;
			end if;
			t_next <= t_reg + 1;
		end if;
	end process;			
	------------------------------------
	
	ClockScan <= temp2(3); 		-- 176ms @ 50MHz
		
END Frequency_arch;
