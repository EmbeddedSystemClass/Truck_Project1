// for programming AVR:
//first pin: 	10 SS		(not connected to buspirate)
//				11 MOSI		yellow clip
//				12 MISO		black regular clip
//				13 SCLK		other green clip
//				RST			whilte clip

// for running AVR:
//	GRN
//	YEL
//	BLK
//	RED


// this test sends ascii chars out on spi and prints out what's received
// FPGA reflects back what's received on spi and prints it out to 1st UART

#include <avr/io.h>
#include "../avr8-gnu-toolchain-linux_x86/avr/include/util/delay.h"
#include "sfr_helper.h"
#include <avr/eeprom.h>
#include <stdlib.h>
#include "USART.h"
#include "spi.h"
//#include "t6963.h"
#include "macros.h"
#include <string.h>
#include "main.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	int i;
	uint16_t temp2;
	UCHAR ret_char, temp;
	int done;
	UCHAR xbyte, xbyte2;
	int start_spi = 0;
	UINT xword;
	int do_states = 1;

	initSPIslave();
//	initSPImaster();
	_delay_ms(100);
	initUSART();
	xbyte = 0x21;
//	xbyte2 = 0x21;
	xword = 0;
	_delay_ms(100);


	while (1)
	{
		xbyte2 = SPI_read();
		_delay_us(50);
		SPI_write(xbyte);
//		if(--xbyte < 0x21)
//			xbyte = 0x7e;
		xbyte++;
		transmitByte(xbyte2);
/*
		if(++xword > 200)
		{
			transmitByte(xbyte2);
			if(++xbyte2 > 0x7e)
				xbyte2 = 0x21;
			xword = 0;
		}
*/
	}
	return (0);									  // this should never happen
}

