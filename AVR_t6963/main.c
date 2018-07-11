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
#define CHAR_CMD			2
#define GOTO_CMD			3
#define SET_MODE_CMD 		4
#define LCD_CLRSCR1			5
#define LCD_CLRSCR2			6
#define LCD_CLRSCR3			7
#define LCD_MSG1			8
#define MENU_SETMODE		9
#define MENU_SETCONTEXT		10

#define COLUMN              40      //Set column number to be e.g. 32 for 8x8 fonts, 2 pages
#define ROWS                16

volatile UCHAR xbyte;
volatile UCHAR high_delay;

volatile int onoff;
volatile int dc2;
volatile UCHAR spi_ret;

ISR(TIMER1_OVF_vect)
{

	if(+dc2 % 100 == 0)
	{
//		loop_until_bit_is_set(SPSR, SPIF);			  /* wait until done */
//		spi_ret = SPDR;
//		SPI_write(xbyte);
	}
//	TCNT1 = (UINT)((high_delay << 8) & 0xFFF0);
	TCNT1 = 0xF800;
}

int main(void)
{
	int i,j,k;
	UCHAR ch;
	UCHAR key;
	UCHAR buff[LEN];
	UCHAR mode, type;
	UINT row, col;
	char str[30];
	UCHAR str_len;
	UCHAR temp;

	GDispInit();
//	GDispInitPort();
	_delay_ms(10);
    initUSART();
	_delay_ms(20);
	initSPImaster();
//	initSPIslave();

//#if 0
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

	_delay_ms(1000);
	GDispStringAt(7,15,"          ");
//#endif

	xbyte = 0x21;

	TCNT1 = 0xFF00;
	TCCR1A = 0x00;
//	TCCR1B = (1<<CS10) | (1<<CS12);;  // Timer mode with 1024 prescler
	TCCR1B = (1<<CS11);
	TIMSK1 = (1 << TOIE1) ;   // Enable timer1 overflow interrupt(TOIE1)
//	sei(); // Enable global interrupts by setting global interrupt enable bit in SREG

	i = 0;
	dc2 = 0;
/*
	for(row = 0;row < ROWS;row++)
	{
		for(col = 0;col < COLUMN-1;col++)
		{
			GDispCharAt(row,col,xbyte);
			_delay_ms(1);
			if(++xbyte > 0x7e)
			{
				xbyte = 0x21;
			}
		}
	}

	_delay_ms(500);
*/
	GDispClrTxt();

	row = col = 0;

    while (1)
    {
		key = receiveByte();
//		GDispCharAt(0,0,key);
/*
		if(++dc2 % 2 == 0)
			_SB(PORTB,PORTB1);
		else
			_CB(PORTB,PORTB1);
*/
		buff[i] = key;
		i++;
//#if 0
		if(key == 0xfe)
		{
			switch(buff[0])
			{
				case CHAR_CMD:
					ch = buff[1];
					if(ch > 0x1F && ch < 0x7f)
					{
						GDispCharAt(row,col,ch);
						if(++col > COLUMN-1)
						{
							col = 0;
							if(++row > ROWS-1)
								row = 0;
						}
					}
				break;
				case GOTO_CMD:
					row = (UINT)buff[1];
					col = (UINT)buff[2];
					GDispGoto(row,col);
				break;
				case SET_MODE_CMD:
				break;
				case LCD_CLRSCR3:
					GDispClrTxt();
				break;
				case LCD_MSG1:
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
				case MENU_SETMODE:
				break;
				case MENU_SETCONTEXT:
				break;
				default:
				break;
			}
			i = 0;
		}
//#endif
	}
    return (0);		// this should never happen
}

#if 0
ISR(SPI_STC_vect)
{
	loop_until_bit_is_set(SPSR, SPIF);			  /* wait until done */
	spi_ret = SPDR;
	transmitByte(spi_ret);
}
#endif
