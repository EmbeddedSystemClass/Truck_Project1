//******************************************************************************************//
//*************************************** main.c  ******************************************//
//******************************************************************************************//
// main program that drives the t6963 LCD (32 col x 15 row) display - text only
// see t6963_notes.txt for more details
#include <avr/io.h>
#include<avr/interrupt.h>
//#include "../avr8-gnu-toolchain-linux_x86/avr/include/util/delay.h"
#include "../../Atmel_other/avr8-gnu-toolchain-linux_x86/avr/include/util/delay.h"
#include "sfr_helper.h"
#include <avr/eeprom.h>
#include <stdlib.h>
#include "USART.h"
#include "t6963.h"
#include "spi.h"
#include "macros.h"
#include <string.h>
//#include "main.h"
#include <stdio.h>
#include <stdlib.h>

#define LEN 200
#define DEBUG_CHAR_AT		0
#define DEBUG_STRING_AT		1
#define DEBUG_CHAR			2
#define DEBUG_GOTO			3
#define DEBUG_SET_MODE		4
#define DEBUG_CLRSCR1		5
#define DEBUG_CLRSCR2		6
#define DEBUG_CLRSCR3		7
#define DEBUG_MSG1			8

#define COLUMN              40      //Set column number to be e.g. 32 for 8x8 fonts, 2 pages
#define ROWS                16
// really cranking
#define TIME_DELAY 2
volatile int timer_c;
volatile UCHAR timer_x;
volatile int dc;

ISR(TIMER1_OVF_vect) 
{ 
	if(dc)
	{
		if(timer_c % timer_x == 0)
			SET_PWM();
		else
			CLR_PWM();
	}else
	{
		if(timer_c % timer_x == 0)
			CLR_PWM();
		else
			SET_PWM();
	}
	timer_c++;

	TCNT1 = 0xFF00;
	return 0;	
}

int main(void)
{
	int i,j,k;
	UCHAR xbyte, xbyte2;
	UCHAR ch;
	UCHAR key;
	UCHAR buff[LEN];
	UCHAR mode, type;
	UINT row, col;
	char str[30];
	UCHAR str_len;
	UCHAR spi_ret;

	GDispInitPort();
#if 0
	_delay_ms(10);
	GDispInit();
	_delay_ms(10);
//	initSPImaster();
//	initSPIslave();
	_delay_us(10);

	GDispSetMode(XOR_MODE);
	_delay_us(10);
	GDispSetMode(TEXT_ON);
	_delay_us(10);
	GDispClrTxt();
	GDispStringAt(7,15,"LCD is on!");

//	initSPImaster();
//******************************************************************************************//
//*********************************** start of main loop ***********************************//
//******************************************************************************************//
	_delay_ms(100);
	GDispStringAt(7,15,"          ");
#endif
	xbyte = 0x21;

    initUSART();
	SET_PWM();

	TCNT1 = 0x3FFF;

	TCCR1A = 0x00;
//	TCCR1B = (1<<CS10) | (1<<CS12);;  // Timer mode with 1024 prescler
	TCCR1B = (1<<CS10);
	TIMSK1 = (1 << TOIE1) ;   // Enable timer1 overflow interrupt(TOIE1)
	sei(); // Enable global interrupts by setting global interrupt enable bit in SREG

	_delay_ms(100);
//#if 0
	timer_x = 2;
	dc = 1;
	
	while(1)
	{
		if(++xbyte > 0x7e)
			xbyte = 0x21;
		transmitByte(xbyte);
		_delay_ms(250);
		if(++timer_x > 20)
		{
			timer_x = 2;
		}
		if(timer_c > 20000)
		{
			if(dc)
				dc = 0;
			else dc = 1;
			timer_c = 0;
		}
	}

	while(1)
	{
		for(row = 0;row < ROWS;row++)
		{
			for(col = 0;col < COLUMN-1;col++)
			{
/*
				xbyte = receiveByte();
				if(xbyte == 0xFE)
				{
					GDispClrTxt();
					row = col = 0;
					xbyte = 0x21;
				}
*/
				GDispCharAt(row,col,xbyte);
				if(++xbyte > 0x7e)
					xbyte = 0x21;
				transmitByte(xbyte);	
				_delay_ms(10);
			}
			if ( OCR2B < 63 )
				OCR2B += 5;
			else
				OCR2B = 0;
		}
	}
	_delay_ms(200);
//#endif
	GDispClrTxt();

	i = 0;
    while (1)
    {
		key = receiveByte();
//		GDispCharAt(0,0,key);
		buff[i] = key;
		i++;
		if(key == 0xfe)
		{
			switch(buff[0])
			{
				case DEBUG_CHAR_AT:
					row = (UINT)buff[1];
					col = (UINT)buff[2];
					ch = buff[3];
					GDispCharAt(row,col,ch);
				break;
				case DEBUG_STRING_AT:
					row = (UINT)buff[1];
					col = (UINT)buff[2];
					str_len = buff[3];

					memset(str,0x20,sizeof(str));	
					memcpy(str,&buff[4],str_len);
					str[str_len] = 0;
					GDispStringAt(row,col,str);
				break;
				case DEBUG_CHAR:
					ch = buff[1];
					GDispChar(ch);
				break;
				case DEBUG_GOTO:
					row = (UINT)buff[1];
					col = (UINT)buff[2];
					GDispGoto(row,col);
				break;
				case DEBUG_CLRSCR3:
					GDispClrTxt();
				break;
				case DEBUG_MSG1:
					row = buff[2];
					row <<= 8;
					row |= buff[1];
					col = buff[4];
					col <<= 8;
					col |= buff[3];
					str_len = buff[5];
					memset(str,0x20,sizeof(str));	
					memcpy(str,&buff[7],str_len);
					str[str_len] = 0;
				break;
				default:
				break;
			}
			i = 0;
		}
	}
    return (0);		// this should never happen
}

#if 0
ISR(SPI_STC_vect)
{
//	loop_until_bit_is_set(SPSR, SPIF);			  /* wait until done */
//	spi_ret = SPDR;
//	transmitByte(spi_ret);
//}
#endif

