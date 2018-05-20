LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
use ieee.std_logic_arith.all;
use ieee.std_logic_signed.all;
 
-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--USE ieee.numeric_std.ALL;
 
ENTITY test_pwm IS
END test_pwm;
 
ARCHITECTURE behavior OF test_pwm IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT multi_byte
    PORT(
         clk : IN  std_logic;
         reset : IN  std_logic;
         tx_uart : OUT  std_logic;
         rx_uart : IN  std_logic;
         tx_uart2 : OUT  std_logic;
         rx_uart2 : IN  std_logic;
         tx_rpm : OUT  std_logic;
         tx_mph : OUT  std_logic;
         pwm_signal : OUT  std_logic;
         rpm_signal : IN  std_logic;
         mph_signal : IN  std_logic;
         MOSI_o : OUT  std_logic;
         MISO_i : IN  std_logic;
         SCLK_o : OUT  std_logic;
         SS_o : OUT  std_logic_vector(1 downto 0);
         test : OUT  std_logic_vector(3 downto 0);
         led1 : OUT  std_logic_vector(3 downto 0)
        );
    END COMPONENT;
    

   --Inputs
   signal clk : std_logic := '0';
   signal reset : std_logic := '0';
   signal rx_uart : std_logic := '0';
   signal rx_uart2 : std_logic := '0';
   signal rpm_signal : std_logic := '0';
   signal mph_signal : std_logic := '0';
   signal MISO_i : std_logic := '0';

 	--Outputs
   signal tx_uart : std_logic;
   signal tx_uart2 : std_logic;
   signal tx_rpm : std_logic;
   signal tx_mph : std_logic;
   signal pwm_signal : std_logic;
   signal MOSI_o : std_logic;
   signal SCLK_o : std_logic;
   signal SS_o : std_logic_vector(1 downto 0);
   signal test : std_logic_vector(3 downto 0);
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
          tx_uart2 => tx_uart2,
          rx_uart2 => rx_uart2,
          tx_rpm => tx_rpm,
          tx_mph => tx_mph,
          pwm_signal => pwm_signal,
          rpm_signal => rpm_signal,
          mph_signal => mph_signal,
          MOSI_o => MOSI_o,
          MISO_i => MISO_i,
          SCLK_o => SCLK_o,
          SS_o => SS_o,
          test => test,
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
		wait for 100 ns;
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
