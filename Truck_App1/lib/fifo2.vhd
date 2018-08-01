--**********************************************************************
-- Copyright (c) 2011-2014 by XESS Corp <http://www.xess.com>.
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

library IEEE, XESS;
--use IEEE.math_real.all;
use IEEE.std_logic_1164.all;
use XESS.CommonPckg.all;

package FifoPckg is

  -- Adjustable FIFO with common read and write clock.
  component FifoCc is
    generic (
      WIDTH_G  : natural := 8;          -- FIFO word width.
      LENGTH_G : natural := 16          -- Number of words in the FIFO.
      );
    port (
      rst_i   : in  std_logic                            := '0';  -- Active-low reset.
      clk_i   : in  std_logic;          -- Master clock.
      add_i   : in  std_logic                            := YES;  -- Remove data from the front of the FIFO.
      rmv_i   : in  std_logic                            := YES;  -- Add data to the back of the FIFO.
      data_i  : in  std_logic_vector(WIDTH_G-1 downto 0) := (others => ZERO);  -- Input data to FIFO.
      data_o  : out std_logic_vector(WIDTH_G-1 downto 0);  -- Output data from FIFO.
      empty_o : out std_logic;          -- True when the FIFO is empty.
      full_o  : out std_logic;          -- True when the FIFO is full.
--      level_o : out std_logic_vector(natural(ceil(log2(real(LENGTH_G+1))))-1 downto 0)  -- # of data words currently in FIFO.
		level_o : out std_logic_vector(LENGTH_G-1 downto 0)
      );
  end component;

end package;

library IEEE, XESS;
use IEEE.math_real.all;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;
use XESS.CommonPckg.all;

--**********************************************************************
-- Adjustable FIFO with common read and write clock.
--**********************************************************************
entity FifoCc is
  generic (
    WIDTH_G  : natural := 8;            -- FIFO word width.
    LENGTH_G : natural := 16            -- Number of words in the FIFO.
    );
  port (
    rst_i   : in  std_logic                            := NO;  -- Active-high reset.
    clk_i   : in  std_logic;            -- Master clock.
    add_i   : in  std_logic                            := NO;  -- Remove data from the front of the FIFO.
    rmv_i   : in  std_logic                            := NO;  -- Add data to the back of the FIFO.
    data_i  : in  std_logic_vector(WIDTH_G-1 downto 0) := (others => ZERO);  -- Input data to FIFO.
    data_o  : out std_logic_vector(WIDTH_G-1 downto 0);  -- Output data from FIFO.
    empty_o : out std_logic;            -- True when the FIFO is empty.
    full_o  : out std_logic;            -- True when the FIFO is full.
--    level_o : out std_logic_vector(natural(ceil(log2(real(LENGTH_G+1))))-1 downto 0)  -- # of data words currently in FIFO.
		level_o : out std_logic_vector(LENGTH_G-1 downto 0)
    );
end entity;

architecture arch of FifoCc is
  signal full_s             : std_logic;
  signal empty_s            : std_logic;
  subtype Address_t is natural range 0 to LENGTH_G-1;
  signal rmvAddr_r          : Address_t := 0;
  signal addAddr_r          : Address_t := 0;
  subtype Level_t is natural range 0 to LENGTH_G;
  signal level_r            : Level_t   := 0;
  signal rmvAllow_s         : std_logic;
  signal addAllow_s         : std_logic;
  subtype RamWord_t is std_logic_vector(data_i'range);  -- RAM word type.
  type Ram_t is array (0 to LENGTH_G-1) of RamWord_t;  -- array of RAM words type.
  signal ram_r              : Ram_t;    -- RAM declaration.
  signal frontData_r        : RamWord_t;
  signal shortCircuitData_r : RamWord_t;
  signal shortCircuit_r     : std_logic;
begin

  full_s     <= YES when level_r = Level_t'high else NO;
  empty_s    <= YES when level_r = 0            else NO;
  rmvAllow_s <= rmv_i and not empty_s;
  addAllow_s <= add_i and not full_s;

  process (rst_i, clk_i)
  begin
    if rst_i = '0' then
      rmvAddr_r      <= 0;
      addAddr_r      <= 0;
      level_r        <= 0;
      shortCircuit_r <= NO;
    elsif rising_edge(clk_i) then
      shortCircuit_r <= NO;
      if rmvAllow_s = YES then
        rmvAddr_r   <= rmvAddr_r + 1;
        frontData_r <= ram_r(rmvAddr_r + 1);  -- Data at the front of the FIFO is always available on the output.
      else
        frontData_r <= ram_r(rmvAddr_r);  -- Data at the front of the FIFO is always available on the output.
      end if;
      if addAllow_s = YES then
        ram_r(addAddr_r) <= data_i;
        addAddr_r        <= addAddr_r + 1;
        if level_r = 0 then
          -- If data is added to an empty FIFO, then we have to "short-circuit" the
          -- input data directly onto the FIFO output.
          shortCircuit_r     <= YES;
          shortCircuitData_r <= data_i;
        end if;
      end if;
      if (addAllow_s and not rmvAllow_s) = YES then
        level_r <= level_r + 1;
      elsif (rmvAllow_s and not addAllow_s) = YES then
        level_r <= level_r - 1;
      end if;
    end if;
  end process;

  data_o  <= shortCircuitData_r when shortCircuit_r = YES else frontData_r;
  full_o  <= full_s;
  empty_o <= empty_s;
  level_o <= std_logic_vector(TO_UNSIGNED(level_r, level_o'length));

end architecture;
