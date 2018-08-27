//******************************************************************************************//
//*************************************** main.c  ******************************************//
//******************************************************************************************//
// main program that drives the t6963 LCD (40 col x 16 row) display - text only
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
#include "../mytypes.h"

#define LEN 200

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
volatile char eeprom[EEPROM_SIZE];
volatile UINT xrow, xcol;

typedef struct
{
	UINT row;
	UINT col;
}RT_LABEL_POS;

// use timer to keep track of things like:
// - seconds before re-enter password
// - how long to wait before shutting down screen
//   if not char's entered
// - 
ISR(TIMER1_OVF_vect)
{
	GDispCharAt(xrow,xcol,spi_ret);
	if(++spi_ret > 0x41)
		spi_ret = 0x30;

	if(++xcol > 39)
	{
		xcol = 35;
		if(++xrow > 15)
			xrow = 13;
	}
//	TCNT1 = (UINT)((high_delay << 8) & 0xFFF0);
}

int main(void)
{
	int i,j,k;
	UCHAR ch;
	int chptr;
	UCHAR key;
	UCHAR buff[LEN];
	UCHAR mode, type;
	UCHAR byte_val;
	UINT int_val;
	UINT row, col;
	UCHAR srow, scol, erow, ecol;
	char str[30];
	UCHAR str_len;
	UCHAR index;
	RT_LABEL_POS pos[20];
	UCHAR no_rt_values;
	UCHAR blanks;
	int gdsip_strlen;
	UCHAR tspi_ret;

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

	TCCR1A = 0x00;
	TCCR1B = (1<<CS10) | (1<<CS12);;  // Timer mode with 1024 prescler
//	TCCR1B = (1<<CS11);
	TIMSK1 = (1 << TOIE1) ;   // Enable timer1 overflow interrupt(TOIE1)
//	sei(); // Enable global interrupts by setting global interrupt enable bit in SREG

	chptr = 0;
	dc2 = 0;
	spi_ret = 0x30;
	xrow = 13;
	xcol = 35;

/*
	for(i = 0;i < 100;i++)
	for(row = 0;row < ROWS;row++)
	{
		for(col = 0;col < COLUMN-1;col++)
		{
			GDispCharAt(row,col,xbyte);
			_delay_ms(2);
			if(++xbyte > 0x7e)
			{
				xbyte = 0x21;
			}
		}
	}
*/
	_delay_ms(50);

	GDispClrTxt();

	memset((void *)curr_num,0,SIZE_NUM);
	memcpy((void *)curr_num,"0123456\0",7);

	eeprom_read_block((void *)eeprom,(const void *)eepromString,EEPROM_SIZE);

	row = col = 0;
	
    while (1)
    {
		key = receiveByte();
/*
		if(++dc2 % 2 == 0)
			_SB(PORTB,PORTB1);
		else
			_CB(PORTB,PORTB1);
*/
		buff[chptr] = key;
		chptr++;
//#if 0
		if(key == 0xfe)
		{

//			for(i = 0;i < 5;i++)
//				transmitByte(buff[i]);
/*
			printHexByte(buff[0]);
			printHexByte(buff[1]);
			printHexByte(buff[2]);
			printHexByte(buff[3]);
			printHexByte(buff[4]);
*/
//			GDispCharAt(0,0,buff[0]+0x30);

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

				// could not get this to work as far as being able to select starting, ending
				// cols & rows so for now it just clears a predetermined section in the upper right
				// of the screen
				case LCD_CLRSCR2:
					srow = buff[1];
					scol = buff[2];
					erow = buff[3];
					ecol = buff[4];	
					GDispClrSection(srow, scol, erow, ecol);
				break;

				case LCD_CLRSCR:
					GDispClrTxt();
				break;

				// not used
				case LCD_MSG1:
					for(i = 0;i < SIZE_NUM;i++)
						transmitByte(curr_num[i]);
				break;
				// params after cmd:
				// 1 row
				// 2 col
				// 3 index into eeprom + 1
				// 4 no of spaces to put down starting a beginning of string
				// to blank out what was there before
				case EEPROM_STR:
					row = (UINT)buff[1];
					col = (UINT)buff[2];
					index = buff[3];
					blanks = buff[4];

					if(index >= 0 && index < 52)
					{
						j = 0;
						i = 0;
						k = 0;
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

						GDispGoto(row,col);
						for(i = 0;i < blanks;i++)
 							GDispChar(0x20);
						gdsip_strlen = GDispStringAt(row,col,str);
/*
						gdsip_strlen = GDispStringAt(row,col,str);
						GDispGoto(row,col+gdsip_strlen);
						for(i = 0;i < blanks;i++)
 							GDispChar(0x20);
*/
					}
				break;

				case SEND_BYTE_RT_VALUES:
				// 1st param is row
				// 2nd param is col
				// 3rd is int value up to 255
					byte_val = buff[3];
					sprintf(str,"%02d",byte_val);
					GDispStringAt((UINT)buff[1],(UINT)buff[2],str);
				break;

				case SEND_BYTE_HEX_VALUES:
				// 1st param is row
				// 2nd param is col
				// 3rd is int value up to 255
					byte_val = buff[3];
					sprintf(str,"%02x",byte_val);
					GDispStringAt((UINT)buff[1],(UINT)buff[2],str);
				break;

				case SEND_INT_RT_VALUES:
				// 1st param is row
				// 2nd param is col
				// 3rd/4th is int value up to 65635
					int_val = (UINT)buff[3];
					int_val <<= 8;
					int_val |= (UINT)buff[4];
					sprintf(str,"%04d",int_val);
					GDispStringAt((UINT)buff[1],(UINT)buff[2],str);
				break;

				default:
				break;
			}
			chptr = 0;
//			_delay_ms(5);
			transmitByte(0xFD);	// send 0xFD back to let PIC24 know we are finished with this command
//			printHexByte(buff[5]+0x30);
//			printString(" ");
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
