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
         rx_uart2 : IN  std_logic;
         tx_rpm : OUT  std_logic;
         tx_mph : OUT  std_logic;
         rpm_signal : IN  std_logic;
         mph_signal : IN  std_logic;
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

 	--Outputs
   signal tx_uart : std_logic;
   signal tx_rpm : std_logic;
   signal tx_mph : std_logic;
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
          rx_uart2 => rx_uart2,
          tx_rpm => tx_rpm,
          tx_mph => tx_mph,
          rpm_signal => rpm_signal,
          mph_signal => mph_signal,
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
--		rx_uart <= '1';
		wait for 110 ns;


		-- wait for 15 ms;	-- this comes out as 0x5F - SET_BRIGHTNESS_CMD with highest brightness (0xFF)
		-- rx_uart <= '0';		-- start bit
		-- wait for 52.1 us;
		-- rx_uart <= '1';
		-- wait for 52.1 us;
		-- rx_uart <= '1';
		-- wait for 52.1 us;
		-- rx_uart <= '1';
		-- wait for 52.1 us;
		-- rx_uart <= '1';
		-- wait for 52.1 us;
		-- rx_uart <= '1';
		-- wait for 52.1 us;
		-- rx_uart <= '0';
		-- wait for 52.1 us;
		-- rx_uart <= '0';
		-- wait for 52.1 us;
		-- rx_uart <= '1';
		-- wait for 52.1 us;
		-- rx_uart <= '0';
		-- wait for 52.1 us;
		-- rx_uart <= '1';

		-- wait for 15 ms;
		-- -- send a 0x3f - (low bit first) 11111100 then stop bit is high
		-- rx_uart <= '0';		-- start bit
		-- wait for 52.1 us;
		-- rx_uart <= '1';
		-- wait for 52.1 us;
		-- rx_uart <= '0';
		-- wait for 52.1 us;
		-- rx_uart <= '0';
		-- wait for 52.1 us;
		-- rx_uart <= '0';
		-- wait for 52.1 us;

		-- rx_uart <= '0';	-- this produces a 0x61 or SET_CDECIMAL w/ param = 1
		-- wait for 52.1 us;
		-- rx_uart <= '0';
		-- wait for 52.1 us;
		-- rx_uart <= '1';
		-- wait for 52.1 us;
		-- rx_uart <= '1';
		-- wait for 52.1 us;
		-- rx_uart <= '0';
		-- wait for 52.1 us;
		-- rx_uart <= '1';

		-- wait for 8 ms;
		-- -- send a 0x3f - (low bit first) 11111100 then stop bit is high
		-- rx_uart <= '0';		-- start bit
		-- wait for 52.1 us;
		-- rx_uart <= '1';
		-- wait for 52.1 us;
		-- rx_uart <= '1';
		-- wait for 52.1 us;
		-- rx_uart <= '1';
		-- wait for 52.1 us;
		-- rx_uart <= '1';
		
		-- wait for 52.1 us;	-- makes a 0x9f - SET_UPDATE_RATE_CMD = update_rate = 0x3FFFFFF (highest poss. update_rate)
		-- rx_uart <= '1';
		-- wait for 52.1 us;
		-- rx_uart <= '1';
		-- wait for 52.1 us;
		-- rx_uart <= '1';
		-- wait for 52.1 us;
		-- rx_uart <= '0';
		-- wait for 52.1 us;
		-- rx_uart <= '1';		-- stop bit (high)

		-- wait for 3 ms;
		-- -- send a 0x3f - (low bit first) 11111100 then stop bit is high
		-- rx_uart <= '0';		-- start bit
		-- wait for 52.1 us;
		-- rx_uart <= '1';
		-- wait for 52.1 us;
		-- rx_uart <= '1';
		-- wait for 52.1 us;
		-- rx_uart <= '1';
		-- wait for 52.1 us;
		-- rx_uart <= '1';
		
		-- wait for 52.1 us;	-- makes a 0x9f - SET_UPDATE_RATE_CMD = update_rate = 0x3FFFFFF (highest poss. update_rate)
		-- rx_uart <= '0';
		-- wait for 52.1 us;
		-- rx_uart <= '0';
		-- wait for 52.1 us;
		-- rx_uart <= '0';
		-- wait for 52.1 us;
		-- rx_uart <= '0';
		-- wait for 52.1 us;
		-- rx_uart <= '1';		-- stop bit (high)
		-- wait for 14 ms;
		-- -- send a 0x3f - (low bit first) 11111100 then stop bit is high
		-- rx_uart <= '0';		-- start bit
		-- wait for 52.1 us;
		-- rx_uart <= '1';
		-- wait for 52.1 us;
		-- rx_uart <= '1';
		-- wait for 52.1 us;
		-- rx_uart <= '1';
		-- wait for 52.1 us;
		-- rx_uart <= '1';
		
		-- wait for 52.1 us;	-- makes a 0x9f - SET_UPDATE_RATE_CMD = update_rate = 0x1FFFFFF (1.565 seconds)
		-- rx_uart <= '0';
		-- wait for 52.1 us;
		-- rx_uart <= '0';
		-- wait for 52.1 us;
		-- rx_uart <= '0';
		-- wait for 52.1 us;
		-- rx_uart <= '0';
		-- wait for 52.1 us;
		-- rx_uart <= '1';		-- stop bit (high)

		-- wait for 14 ms;
		-- -- send a 0x3f - (low bit first) 11111100 then stop bit is high
		-- rx_uart <= '0';		-- start bit
		-- wait for 52.1 us;
		-- rx_uart <= '1';
		-- wait for 52.1 us;
		-- rx_uart <= '1';
		-- wait for 52.1 us;
		-- rx_uart <= '1';
		-- wait for 52.1 us;
		-- rx_uart <= '0';
		
		-- wait for 52.1 us;	-- makes a 0x9f - SET_UPDATE_RATE_CMD = update_rate = 0xFFFFFF (760 ms)
		-- rx_uart <= '0';
		-- wait for 52.1 us;
		-- rx_uart <= '0';
		-- wait for 52.1 us;
		-- rx_uart <= '0';
		-- wait for 52.1 us;
		-- rx_uart <= '0';
		-- wait for 52.1 us;
		-- rx_uart <= '1';		-- stop bit (high)

		-- wait for 14 ms;
		-- -- send a 0x3f - (low bit first) 11111100 then stop bit is high
		-- rx_uart <= '0';		-- start bit
		-- wait for 52.1 us;
		-- rx_uart <= '1';
		-- wait for 52.1 us;
		-- rx_uart <= '1';
		-- wait for 52.1 us;
		-- rx_uart <= '0';
		-- wait for 52.1 us;
		-- rx_uart <= '0';
		
		-- wait for 52.1 us;	-- makes a 0x9f - SET_UPDATE_RATE_CMD = update_rate = 0x7FFFFF (391 ms) divide 0x7FFFFF by 21,454
		-- rx_uart <= '0';
		-- wait for 52.1 us;
		-- rx_uart <= '0';
		-- wait for 52.1 us;
		-- rx_uart <= '0';
		-- wait for 52.1 us;
		-- rx_uart <= '0';
		-- wait for 52.1 us;
		-- rx_uart <= '1';		-- stop bit (high)

		-- wait for 14 ms;
		-- -- send a 0x3f - (low bit first) 11111100 then stop bit is high
		-- rx_uart <= '0';		-- start bit
		-- wait for 52.1 us;
		-- rx_uart <= '1';
		-- wait for 52.1 us;
		-- rx_uart <= '0';
		-- wait for 52.1 us;
		-- rx_uart <= '0';
		-- wait for 52.1 us;
		-- rx_uart <= '0';
		
		-- wait for 52.1 us;	-- makes a 0x9f - SET_UPDATE_RATE_CMD  update_rate 0x3ffff = 184ms
		-- rx_uart <= '0';
		-- wait for 52.1 us;
		-- rx_uart <= '0';
		-- wait for 52.1 us;
		-- rx_uart <= '0';
		-- wait for 52.1 us;
		-- rx_uart <= '0';
		-- wait for 52.1 us;
		-- rx_uart <= '1';		-- stop bit (high)

		-- wait for 14 ms;
		-- -- send a 0x3f - (low bit first) 11111100 then stop bit is high
		-- rx_uart <= '0';		-- start bit
		-- wait for 52.1 us;
		-- rx_uart <= '0';
		-- wait for 52.1 us;
		-- rx_uart <= '0';
		-- wait for 52.1 us;
		-- rx_uart <= '0';
		-- wait for 52.1 us;
		-- rx_uart <= '0';
		
		-- wait for 52.1 us;	-- makes a 0x9f - SET_UPDATE_RATE_CMD  update_rate 0x1ffff = 92ms
		-- rx_uart <= '0';
		-- wait for 52.1 us;
		-- rx_uart <= '0';
		-- wait for 52.1 us;
		-- rx_uart <= '0';
		-- wait for 52.1 us;
		-- rx_uart <= '0';
		-- wait for 52.1 us;
		-- rx_uart <= '1';		-- stop bit (high)
		

		wait;
	end process;

END;
