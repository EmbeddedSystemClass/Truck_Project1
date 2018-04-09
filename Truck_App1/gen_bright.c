//******************************************************************************************//
//*************************************** main.c  ******************************************//
//******************************************************************************************//
#include <avr/io.h>
#include "/home/dan/dev/Atmel_other/avr8-gnu-toolchain-linux_x86/avr/include/util/delay.h"
#include "sfr_helper.h"
#include <avr/eeprom.h>
#include <stdlib.h>
#include "USART.h"
#include "macros.h"
//#include "spi.h"
#include <string.h>
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
//#include "pinDefines.h"

#define LED_PORT PORTB
#define LED_DDR DDRB

#define SEND_CHAR_CMD 0x20
#define SET_BRIGHTNESS_CMD 0x40
#define SET_CDECIMAL_CMD 0x60
#define SET_UPDATE_RATE_CMD 0x80
#define SET_FACTOR_CMD 0xA0

void flash(void);

int main(void)
{
	UCHAR cmd, bright, decimal;

	LED_DDR = 0xff;
	initUSART();
//******************************************************************************************//
//*********************************** start of main loop ***********************************//
//******************************************************************************************//
	_delay_ms(200);

	decimal = 0;
	cmd = decimal | SET_CDECIMAL_CMD;
	transmitByte(cmd);
	_delay_ms(10);

	bright = 0x1F;
	transmitByte(SET_BRIGHTNESS_CMD | bright);
	_delay_ms(10);
	
	cmd = SET_UPDATE_RATE_CMD;
	bright = 0;
	transmitByte(cmd | bright);

	while (1)
	{

		cmd = SET_CDECIMAL_CMD;

		bright = 1;
		transmitByte(cmd | bright);
		_delay_ms(1000);
		flash();

		bright = 2;
		transmitByte(cmd | bright);
		_delay_ms(1000);
		flash();

		bright = 1;
		transmitByte(cmd | bright);
		_delay_ms(1000);
		flash();

		bright = 2;
		transmitByte(cmd | bright);
		_delay_ms(1000);
		flash();

		bright = 4;
		transmitByte(cmd | bright);
		_delay_ms(1000);
		flash();

		bright = 2;
		transmitByte(cmd | bright);
		_delay_ms(1000);
		flash();

		bright = 4;
		transmitByte(cmd | bright);
		_delay_ms(1000);
		flash();

		bright = 2;
		transmitByte(cmd | bright);
		_delay_ms(1000);
		flash();

		bright = 0;
		transmitByte(cmd | bright);
		_delay_ms(20);

		cmd = SET_BRIGHTNESS_CMD;

		flash();
		_delay_ms(500);
		flash();

		bright = 0;
		transmitByte(cmd | bright);
		_delay_ms(1000);

		bright = 1;
		transmitByte(cmd | bright);
		_delay_ms(1000);

		bright = 3;
		transmitByte(cmd | bright);
		_delay_ms(1000);

		bright = 7;
		transmitByte(cmd | bright);
		_delay_ms(1000);

		bright = 15;
		transmitByte(cmd | bright);
		_delay_ms(1000);


		bright = 31;
		transmitByte(cmd | bright);
		_delay_ms(1000);

		cmd = SET_UPDATE_RATE_CMD;

		flash();
		_delay_ms(500);
		flash();

		bright = 0x1f;
		transmitByte(cmd | bright);
		_delay_ms(4000);

		flash();

		bright = 15;
		transmitByte(cmd | bright);
		_delay_ms(4000);

		flash();

		bright = 7;
		transmitByte(cmd | bright);
		_delay_ms(4000);

		flash();

		bright = 3;
		transmitByte(cmd | bright);
		_delay_ms(4000);

		flash();

		bright = 1;
		transmitByte(cmd | bright);
		_delay_ms(4000);

		flash();

		bright = 0;
		transmitByte(cmd | bright);
		_delay_ms(4000);

		flash();
		_delay_ms(500);
		flash();

	}
	return (0);									  // this should never happen
}

void flash(void)
{
	UCHAR cmd, bright;

	cmd = SET_BRIGHTNESS_CMD;

	bright = 0;
	transmitByte(cmd | bright);
	_delay_ms(300);

	bright = 0x1f;
	transmitByte(cmd | bright);
	_delay_ms(10);
}
