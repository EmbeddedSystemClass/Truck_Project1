--------------------------------------------------------------------------------
LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
 
-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--USE ieee.numeric_std.ALL;
 
ENTITY testmain IS
END testmain;
 
ARCHITECTURE behavior OF testmain IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT multi_byte
    PORT(
         clk : IN  std_logic;
         reset : IN  std_logic;
         pwm_lcd : OUT  std_logic;
         led1 : OUT  std_logic_vector(3 downto 0)
        );
    END COMPONENT;
    

   --Inputs
   signal clk : std_logic := '0';
   signal reset : std_logic := '0';

 	--Outputs
   signal pwm_lcd : std_logic;
   signal led1 : std_logic_vector(3 downto 0);

   -- Clock period definitions
   constant clk_period : time := 22 ns;
 
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: multi_byte PORT MAP (
          clk => clk,
          reset => reset,
          pwm_lcd => pwm_lcd,
          led1 => led1
        );

   -- Clock process definitions
   clk_process :process
   begin
		clk <= '0';
		wait for clk_period/2;
		clk <= '1';
		wait for clk_period/2;
   end process;

	reset_proc: process
	begin
		reset <= '0';
		wait for 100 ns;
		reset <= '1';
		wait;
	end process;

   -- Stimulus process
   stim_proc: process
   begin		
      -- hold reset state for 100 ns.
      wait for 100 ns;	

      wait for clk_period*10;

      -- insert stimulus here 

      wait;
   end process;

END;
