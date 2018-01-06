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

#define LEN 100
int channels[8][LEN];

//char eepromString[EEPROM_SIZE] EEMEM;

uint16_t readADC(uint8_t channel) {
  ADMUX = (0xf0 & ADMUX) | channel;
  ADCSRA |= (1 << ADSC);
  loop_until_bit_is_clear(ADCSRA, ADSC);
  return (ADC);
}

//PROMPT_STRUCT prompts[26];	// this must be large enough for no_prompts
#endif
int main(void)
{
	int i,j;
	UCHAR xbyte;
	UINT read_byte;
	uint8_t channel;

/*
	for(j = 0;j < 8;j++)
		for(i = 0;i < LEN;i++)
			channels[j][i] = j+i;
*/
	// Set up ADC
	ADMUX |= (1 << REFS0);                  /* reference voltage on AVCC */
	ADCSRA |= (1 << ADPS1) | (1 << ADPS0);     /* ADC clock prescaler /8 */
	ADCSRA |= (1 << ADEN);                                 /* enable ADC */

	_delay_ms(20);
	initUSART();
//******************************************************************************************//
//*********************************** start of main loop ***********************************//
//******************************************************************************************//
	_delay_us(10);

	xbyte = 0x21;
	read_byte = 0;	
	channel = 0;
//	printString("\r\nstarting...\r\n");

	while (1)
	{
//		if(++xbyte > 0x7e)
//			xbyte = 0x21;
		read_byte = readADC(channel);
		if(++channel > 7)
			channel = 0;

		if(++i > LEN)
			i = 0;			

		transmitByte(0x55);
		_delay_ms(1);
		transmitByte(0xAA);
		_delay_ms(1);
		transmitByte(channel);
		_delay_ms(1);
//		read_byte = channels[channel][i];
		transmitByte((UCHAR)(read_byte >> 8));
		_delay_ms(1);
		transmitByte((UCHAR)read_byte);
		if(channel == 7)
			_delay_ms(700);
//		xbyte = SPI_read();
//		_delay_ms(500);
//		SPI_write(xbyte);
//		transmitByte(xbyte);
	}
	return (0);									  // this should never happen
}

