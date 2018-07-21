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
#define LCD_CLRSCR			5
#define LCD_MSG1			6
#define MENU_SETMODE		7
#define MENU_SETCONTEXT		8
#define PUT_STRING 			22

#define COLUMN              40      //Set column number to be e.g. 32 for 8x8 fonts, 2 pages
#define ROWS                16
#define SIZE_NUM			20

#define EEPROM_SIZE 0x400
char eepromString[EEPROM_SIZE] EEMEM;
volatile UCHAR xbyte;
volatile UCHAR high_delay;

volatile int onoff;
volatile int dc2;
volatile UCHAR spi_ret;
volatile UCHAR curr_num[SIZE_NUM];
volatile char eeprom[EEPROM_SIZE/2];

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
	UCHAR index;

	GDispInit();
//	GDispInitPort();
	_delay_ms(10);
    initUSART();
	_delay_ms(20);
//	initSPImaster();
//	initSPIslave();

//#if 0
	GDispSetMode(XOR_MODE);
	_delay_us(10);
	GDispSetMode(TEXT_ON);
	_delay_us(10);
	GDispClrTxt(0);
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
	GDispClrTxt(0);

	memset((void *)curr_num,0,SIZE_NUM);
	memcpy((void *)curr_num,"0123456\0",7);

	eeprom_read_block((void *)eeprom,(const void *)eepromString,EEPROM_SIZE/2);

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
					mode = buff[1];
					row = (UINT)buff[2];
					col = (UINT)buff[3];
					type = buff[4];
					GDispSetCursor (mode, row, col, type);
				break;
				case LCD_CLRSCR:
					GDispClrTxt(buff[1]);
				break;
				case LCD_MSG1:
					for(i = 0;i < SIZE_NUM;i++)
						transmitByte(curr_num[i]);
				break;
				case MENU_SETMODE:
				break;
				case MENU_SETCONTEXT:
				break;
				case PUT_STRING:
					row = (UINT)buff[1];
					col = (UINT)buff[2];
					index = buff[3];
					if(index > 0 && index < 23)
					{
						printHexByte(buff[0]);
						printHexByte(buff[1]);
						printHexByte(buff[2]);
						printHexByte(buff[3]);

						j = 0;
						i = 0;
						do{
							j++;
							if(eeprom[j] == 0)
							{
								i++;
					 			k = j;
							}

						}while(i < index);

						do{
							k--;
						}while(eeprom[k] != 0);

						memset((void *)str,0,sizeof(str));
						memcpy((void *)str,(const void *)&eeprom[k+1],j-k-1);
						GDispStringAt(row,col,str);
						printString(str);
					}
				break;
				default:
				break;
			}
			i = 0;
			transmitByte(0xFD);	// send 0xFD back to let PIC24 know we are finished with this command
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
