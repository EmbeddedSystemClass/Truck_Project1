-- period.vhd
-- measure the number of times CLK_MS_COUNT rolls over - this is the result (prd)
-- t_reg counts the number of clk cycles for 10us (rpm) or 1ms (mph) and each time it rolls over, p_reg is incremented
-- divide t_reg into 6000000 to get rpm
-- divide t_reg into 3213 to get mph
-- there's a bug here somewhere - on the test bench, if I do:
-- si <= '0'; wait for 7 ms;
-- si <= '1'; wait for 8 ms;
-- wait for 10 ns;
-- then it skips over the 2nd edge
-- wait for 15 ns; works ok
		
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
entity period_counter is
	generic(
		CLK_COUNT: integer := 250;	-- default is for rpm
		P_REG_SIZE: integer := 9;
		T_REG_SIZE: integer := 15);
--		CLK_COUNT: integer := 50000;	-- use this for mph
--		P_REG_SIZE: integer := 16;
--		T_REG_SIZE: integer := 10);
   port(
		clk, reset: in std_logic;
		start, si: in std_logic;
		done_tick: out std_logic;
		prd: out std_logic_vector(T_REG_SIZE-1 downto 0));
end period_counter;

architecture arch of period_counter is
--   constant CLK_MS_COUNT: integer := 2097150;	-- 100 = 2 us, so 2097151 (0x1FFFFF) = 41.94 ms (set t_reg to 21 bits)
--   constant CLK_MS_COUNT: integer := 65535; -- 0xFFFF = 1.3 ms
--   constant CLK_MS_COUNT: integer := 50000; -- 1 ms (50 MHz clock)
--   constant CLK_MS_COUNT: integer := 10000; -- 100 us (50 MHz clock)
--	constant CLK_MS_COUNT: integer := 1000; -- 10 us (50 MHz clock)
--	constant CLK_MS_COUNT2: integer := 100000; -- 1ms (50 MHz clock)
	type state_type is (idle, waite, count, done);
	signal pstate_reg, pstate_next: state_type;
	signal t_reg, t_next: unsigned(P_REG_SIZE-1 downto 0);
	signal p_reg, p_next: unsigned(T_REG_SIZE-1 downto 0);

	signal delay_reg: std_logic;
	signal edge: std_logic;

begin
-- state and data register
rpm_proc: process(clk,reset)
	begin
		if reset='0' then
			pstate_reg <= idle;
			t_reg <= (others=>'0');
			p_reg <= (others=>'0');
			delay_reg <= '0';
		elsif (clk'event and clk='1') then
			pstate_reg <= pstate_next;
			t_reg <= t_next;
			p_reg <= p_next;
			delay_reg <= si;
	end if;
end process;

edge <= (not delay_reg) and si;

process(start,edge,pstate_reg,t_reg,t_next,p_reg)
	begin
	done_tick <= '0';
	pstate_next <= pstate_reg;
	p_next <= p_reg;
	t_next <= t_reg;
	case pstate_reg is
		when idle =>
			if (start='1') then
				pstate_next <= waite;
			end if;
		when waite => -- wait for the first edge
			if (edge='1') then
				pstate_next <= count;
				t_next <= (others=>'0');
				p_next <= (others=>'0');
			end if;
		when count =>
			if (edge='1') then   -- 2nd edge arrived
				pstate_next <= done;
			else -- otherwise count
			if t_reg = CLK_COUNT-1 then -- 10 us tick
				t_next <= (others=>'0');
				p_next <= p_reg + 1;
			else
				t_next <= t_reg + 1;
			end if;

			end if;
		when done =>
			done_tick <= '1';
			pstate_next <= idle;
	end case;
end process;

prd <= std_logic_vector(p_reg);

end arch;
