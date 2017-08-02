--
-- uartLED.vhd
--
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
entity uartLED2 is
   generic(
     -- Default setting:
     -- 19,200 baud, 8 data bis, 1 stop its, 2^2 FIFO
     DBIT: integer:=8;     -- # data bits
      SB_TICK: integer:=16; -- # ticks for stop bits, 16/24/32
                            --   for 1/1.5/2 stop bits
--      DVSR: integer:= 163;  -- baud rate divisor
--		DVSR: integer:= 27; -- 115200 baud
		DVSR_M: integer:= 326; -- 9600 baud
                            -- DVSR = 50M/(16*baud rate)
      DVSR_BIT: integer:=9 -- # bits of DVSR
   );
   port(
      clk, reset: in std_logic;
	  rx_start: in std_logic;
      r_data: out std_logic_vector(7 downto 0);
	  done_tick: out std_logic;
      rx: in std_logic);
end uartLED2;

architecture str_arch of uartLED2 is
	signal tick1: std_logic;
	signal tick2: std_logic;
	signal rx_done_tick: std_logic;
	signal temp: std_logic;
	
begin
   baud_gen_unit: entity work.mod_m_counter(arch)
      generic map(M=>DVSR_M, N=>DVSR_BIT)
      port map(clk=>clk, reset=>reset,
		max_tick=>tick1);
			   
	uart_rx_unit: entity work.uart_rx(arch)
      generic map(DBIT=>DBIT, SB_TICK=>SB_TICK)
      port map(clk=>clk, reset=>reset,
		s_tick=>tick2,
		dout=>r_data,
		rx_done_tick=> rx_done_tick, 
		rx=>rx);

done_tick <= rx_done_tick;	

xrx_ok: process(clk,reset)
begin
	if reset = '0' then
		tick2 <= '0';
		temp <= '0';
	else if clk'event and clk = '1' then
		temp <= '1';
		if rx_start = '1' then
			tick2 <= tick1;
		else 
			tick2 <= '0';
		end if;
		end if;
	end if;	
end process;

end str_arch;

