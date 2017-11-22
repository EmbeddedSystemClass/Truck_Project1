#if 1
//******************************************************************************************//
//*************************************** main.c  ******************************************//
//******************************************************************************************//
// for programming AVR:
//first pin: 	10 SS		(not connected to buspirate)
//				11 MOSI		yellow clip
//				12 MISO		black regular clip
//				13 SCLK		other green clip
//				RST			whilte clip

// for running AVR:
//	GRN		first pin
//	YEL
//	BLK
//	RED

#include <avr/io.h>
#include "/home/dan/dev/Atmel_other/avr8-gnu-toolchain-linux_x86/avr/include/util/delay.h"
#include "sfr_helper.h"
#include <avr/eeprom.h>
#include <stdlib.h>
#include "USART.h"
#include "macros.h"
#include "spi.h"
#include <string.h>
#include "main.h"
#include <stdio.h>
#include <stdlib.h>

char eepromString[EEPROM_SIZE] EEMEM;

//PROMPT_STRUCT prompts[26];	// this must be large enough for no_prompts
#endif
int main(void)
{
	int i;
	uint16_t temp2;
	UCHAR ret_char, temp;
	int done;
	UCHAR xbyte;
	UINT xword;
//	initSPIslave();
	_delay_ms(20);
	initUSART();
//******************************************************************************************//
//*********************************** start of main loop ***********************************//
//******************************************************************************************//
	_delay_us(10);
	done = 0;

	xbyte = 0x21;
//	printString("\r\nstarting...\r\n");
	while (1)
	{
		ret_char = receiveByte();
		transmitByte(ret_char);
		xbyte = SPI_read();
		_delay_ms(5);
		SPI_write(xbyte);
		transmitByte(xbyte);
	}
	return (0);									  // this should never happen
}

