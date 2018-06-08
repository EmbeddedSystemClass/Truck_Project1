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
         tx_uart : OUT  std_logic;
         rx_uart : IN  std_logic;
         tx_rpm : OUT  std_logic;
         tx_mph : OUT  std_logic;
         rpm_signal : IN  std_logic;
         mph_signal : IN  std_logic;
         pwm_spk1 : OUT  std_logic;
         pwm_spk2 : OUT  std_logic;
         pwm_lcd : OUT  std_logic;
         d_start : OUT  std_logic;
         fp_shutoff : OUT  std_logic;
         rev_limit : OUT  std_logic;
         MOSI_o : OUT  std_logic;
         MISO_i : IN  std_logic;
         SCLK_o : OUT  std_logic;
         SS_o : OUT  std_logic_vector(1 downto 0);
         led1 : OUT  std_logic_vector(3 downto 0)
        );
    END COMPONENT;
    

   --Inputs
   signal clk : std_logic := '0';
   signal reset : std_logic := '0';
   signal rx_uart : std_logic := '0';
   signal rpm_signal : std_logic := '0';
   signal mph_signal : std_logic := '0';
   signal MISO_i : std_logic := '0';

 	--Outputs
   signal tx_uart : std_logic;
   signal tx_rpm : std_logic;
   signal tx_mph : std_logic;
   signal pwm_spk1 : std_logic;
   signal pwm_spk2 : std_logic;
   signal pwm_lcd : std_logic;
   signal d_start : std_logic;
   signal fp_shutoff : std_logic;
   signal rev_limit : std_logic;
   signal MOSI_o : std_logic;
   signal SCLK_o : std_logic;
   signal SS_o : std_logic_vector(1 downto 0);
   signal led1 : std_logic_vector(3 downto 0);

   -- Clock period definitions
   constant clk_period : time := 22 ns;
 
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: multi_byte PORT MAP (
          clk => clk,
          reset => reset,
          tx_uart => tx_uart,
          rx_uart => rx_uart,
          tx_rpm => tx_rpm,
          tx_mph => tx_mph,
          rpm_signal => rpm_signal,
          mph_signal => mph_signal,
          pwm_spk1 => pwm_spk1,
          pwm_spk2 => pwm_spk2,
          pwm_lcd => pwm_lcd,
          d_start => d_start,
          fp_shutoff => fp_shutoff,
          rev_limit => rev_limit,
          MOSI_o => MOSI_o,
          MISO_i => MISO_i,
          SCLK_o => SCLK_o,
          SS_o => SS_o,
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
