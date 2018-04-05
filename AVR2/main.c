#if 1
//******************************************************************************************//
//*************************************** main.c  ******************************************//
//******************************************************************************************//
//LM34 -	Vout = 3v @ 300F
//			Vout = 750mV @ 75F
//			Vout = -500mV @ -50F
//			0.01V/degree F
//			0F = 0.5V
//			100F = 1.5V
//			250F = 2.5V

//LM35 -	-55 -> 150c
//			0.01v/degree C
//			0C	= 0.55V
//			150C = 1.5V
//			25C	= 0.25V
//			-55C = -550mV

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

#define LED_PORT PORTB
#define LED_DDR DDRB
#define LEN 100
//int channels[8][LEN];
char test_string[] = "what the fuck?\0";

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
	
	LED_DDR = 0xff;
/*
	for(j = 0;j < 8;j++)
		for(i = 0;i < LEN;i++)
			channels[j][i] = j+i;
*/
	// Set up ADC
//	ADMUX |= (1 << REFS0) | (1 << REFS1);       /* reference voltage on AVCC  (1.1V) */
//	ADMUX |= (1 << REFS0);
//	ADCSRA |= (1 << ADPS1) | (1 << ADPS0);     /* ADC clock prescaler /8 */
//	ADCSRA |= (1 << ADEN);                                 /* enable ADC */
	initUSART();
//******************************************************************************************//
//*********************************** start of main loop ***********************************//
//******************************************************************************************//
	_delay_us(10);

	xbyte = 0x21;
	read_byte = 0;	
	channel = 0;

	for(i = 0;i < sizeof(test_string);i++)
		transmitByte(test_string[i]);
	printString("\n\rWhat the fuck?\r\r\n\r");
	while (1)
	{

		if(++xbyte > 0x7e)
			xbyte = 0x21;
		LED_PORT = 1;
		_delay_ms(1);
		transmitByte(xbyte);	
		LED_PORT = 0;
		_delay_ms(1);
/*
		read_byte = readADC(channel);

//		if(++channel > 7)
//			channel = 0;

		transmitByte(0x55);
		_delay_ms(1);
		transmitByte(0xAA);
		_delay_ms(1);
		transmitByte(channel);
		_delay_ms(1);

		transmitByte((UCHAR)(read_byte >> 8));
		_delay_ms(1);
		transmitByte((UCHAR)read_byte);

		if(channel == 7)
			_delay_ms(1000);
*/
	}
	return (0);									  // this should never happen
}

