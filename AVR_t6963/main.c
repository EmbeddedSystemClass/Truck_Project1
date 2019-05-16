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

#define EEPROM_SIZE 0x400
#define STR_LEN 30

char eepromString[EEPROM_SIZE] EEMEM;
volatile UCHAR xbyte;
volatile UCHAR high_delay;
static char str[STR_LEN];

volatile int onoff;
volatile int dc2;
volatile UCHAR spi_ret;
volatile UCHAR curr_num[SIZE_NUM];
static int num_entry_mode;
static char eeprom[EEPROM_SIZE];
volatile UINT xrow, xcol;
volatile UCHAR buff[LEN];
static char *eeprom_str_lookup(int index, char *str);
static float convertF(int raw_data);

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
	int i,j;
	int hr,min,sec;
	UCHAR ch;
	int chptr;
	UCHAR key;
	UCHAR mode, type;
	UCHAR byte_val;
	UINT int_val;
	UINT row, col;
//	UCHAR srow, scol, erow, ecol;
	UINT starting_row, starting_col, col_width, ending_row, num_labels;
	UCHAR index;
//	UCHAR no_rt_values;
	UCHAR blanks;
//	int gdsip_strlen;
//	UCHAR tspi_ret;
	int num_entry_ptr = 0;
	num_entry_mode = 0;

///	char disp_str[STR_LEN];

	GDispInit();
//	GDispInitPort();
	_delay_ms(1);
    initUSART();
	_delay_ms(1);
//	initSPImaster();
//	initSPIslave();

//#if 0
	GDispSetMode(XOR_MODE);
	_delay_us(10);
	GDispSetMode(TEXT_ON);
	_delay_us(10);
	GDispClrTxt();
	FONT_6X8();
	GDispStringAt(7,7,"LCD is on!");
	_delay_ms(1000);
	FONT_8X8();

//	initSPImaster();
//******************************************************************************************//
//*********************************** start of main loop ***********************************//
//******************************************************************************************//

//	_delay_ms(1000);
//	GDispStringAt(7,15,"          ");
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
	GDispClrTxt();
	FONT_8X8();
		
	memset((void *)curr_num,0,SIZE_NUM);
	memcpy((void *)curr_num,"0123456\0",7);

	eeprom_read_block((void *)eeprom,(const void *)eepromString,EEPROM_SIZE);

	row = col = 0;

    while (1)
    {
		key = receiveByte();
		buff[chptr] = key;
		chptr++;

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
						if(num_entry_mode == 1)
							if(num_entry_ptr < SIZE_NUM)
								curr_num[num_entry_ptr++] = ch;
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
/*
					srow = buff[1];
					scol = buff[2];
					erow = buff[3];
					ecol = buff[4];	
					GDispClrSection(srow, scol, erow, ecol);
					GDispGoto(0,0);
*/
				break;

				case LCD_CLRSCR:
					GDispClrTxt();
					GDispGoto(0,0);
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

					if(index >= 0 && index < NUM_STR)
					{
						strcpy(str,eeprom_str_lookup(index, str));
						GDispGoto(row,col);
						for(i = 0;i < blanks;i++)
 							GDispChar(0x20);
						GDispStringAt(row,col,str);
/*
						gdsip_strlen = GDispStringAt(row,col,str);
						GDispGoto(row,col+gdsip_strlen);
						for(i = 0;i < blanks;i++)
 							GDispChar(0x20);
*/
					}
				break;

				case EEPROM_STR2:
					blanks = 6;
					col = 15;
					switch(buff[1])
					{
						case START_SEQ:
						case SHUTDOWN:
							row =  STATUS_ENGINE;
							break;
						case ON_FAN:
						case OFF_FAN:
							row = STATUS_COOLING_FAN;
							break;
						case ON_LIGHTS:
						case OFF_LIGHTS:
							row = STATUS_HEAD_LIGHTS;
							break;
						case ON_BRIGHTS:
						case OFF_BRIGHTS:
							row = STATUS_BRIGHTS;
							break;
						case BLOWER_OFF:
						case BLOWER1:
						case BLOWER2:
						case BLOWER3:
							row = STATUS_BLOWER;
							break;
						case ON_RUNNING_LIGHTS:
						case OFF_RUNNING_LIGHTS:
							row = STATUS_RUNNING_LIGHTS;
							break;
						case ON_BRAKES:
						case OFF_BRAKES:
							row = STATUS_BRAKES;
							break;
					}
					GDispGoto(row,col);
					_delay_ms(2);
					for(i = 0;i < blanks;i++)
						GDispChar(0x20);

					if(buff[2] == 2)	// blower 1
					{
						strcpy(str,"ONE\0");
						GDispStringAt(row,col,str);
					}
					else if(buff[2] == 3)	// blower 2
					{
						strcpy(str,"TWO\0");
						GDispStringAt(row,col,str);
					}
					else if(buff[2] == 4)	// blower 3
					{
						strcpy(str,"THREE\0");
						GDispStringAt(row,col,str);
					}else
					{
						strcpy(str,eeprom_str_lookup((buff[2] == 0?STATUS_ON:STATUS_OFF), str));
						GDispStringAt(row,col,str);
					}
					GDispGoto(0,0);
				break;

				case DISPLAY_RTLABELS:
					index = 1;
					blanks = 6;
//					col = START_RT_VALUE_COL;
					starting_row = (UINT)buff[1];
					col = (UINT)buff[2];
					ending_row = (UINT)buff[3];
					col_width = (UINT)buff[4];
					num_labels = (UINT)buff[5];

					for(row = starting_row;index < num_labels+1;index++,row++)
					{
						strcpy(str,eeprom_str_lookup(index, str));
						GDispGoto(row,col);
						for(i = 0;i < blanks;i++)
 							GDispChar(0x20);

						if(row == ending_row)
						{
							row = starting_row;
							col += col_width;
						}
						GDispStringAt(row,col,str);
					}
				break;

				case DISPLAY_STATUSLABELS:
					index = STATUS_VALUES_OFFSET;
					blanks = 10;
					col = 0;

					for(row = 0;index < NUM_STATUS_LABELS+STATUS_VALUES_OFFSET;index++,row++)
					{
						strcpy(str,eeprom_str_lookup(index, str));
						GDispGoto(row,col);
						for(i = 0;i < blanks;i++)
 							GDispChar(0x20);

						GDispStringAt(row,col,str);
					}
				break;

				case SEND_BYTE_RT_VALUES:
				// 1st param is row
				// 2nd param is col
				// 3rd is int value up to 255
					byte_val = buff[3];
					sprintf(str,"%02d ",byte_val);
					GDispStringAt((UINT)buff[1],(UINT)buff[2],str);
				break;

				case SEND_BYTE_HEX_VALUES:
				// 1st param is row
				// 2nd param is col
				// 3rd is int value up to 255
					byte_val = buff[3];
					sprintf(str,"%02x ",byte_val);
					GDispStringAt((UINT)buff[1],(UINT)buff[2],str);
				break;

				case SEND_INT_RT_VALUES:
				// 1st param is row
				// 2nd param is col
				// 3rd/4th is int value up to 65635 (high byte 1st)
					int_val = (UINT)buff[3];
					int_val <<= 8;
					int_val |= (UINT)buff[4];
					sprintf(str,"%04d ",int_val);
					GDispStringAt((UINT)buff[1],(UINT)buff[2],str);
				break;

				case SHOW_EEPROM:
				break;

				case PASSWORD_MODE:
					GDispClrTxt();
					row = 7;
					col = (UINT)buff[1];
					GDispGoto(row,col);
					_delay_ms(2);
					strcpy(str,eeprom_str_lookup(VARIOUS_MSG_OFFSET, str));
					GDispStringAt(6,0,str);
					_delay_ms(2);
					GDispSetCursor (TEXT_ON | CURSOR_BLINK_ON, 7, 0, LINE_2_CURSOR);
				break;

				case SET_NUM_ENTRY_MODE:
					num_entry_mode = buff[1];
					num_entry_ptr = 0;
				break;

				// display string at x,y
				case DISPLAY_STR:
//					memset(disp_str,0,sizeof(disp_str));
//					j = (int)buff[4];
//					GDispGoto((UINT)buff[1],(UINT)buff[2]);
//					for(i = 0;i < j;i++)
//						GDispChar(0x20);
					i = 0;
					do{
						str[i] = buff[i+3];
						i++;
					}while(i < STR_LEN && buff[i] != 0);

//					strncpy(disp_str,(char *)&buff[4],disp_str_len);
					GDispStringAt((UINT)buff[1],(UINT)buff[2],str);
				break;

				// display int as hr:min:sec at x,y
				case DISPLAY_ELAPSED_TIME:
					int_val = (UINT)buff[3];
					int_val <<= 8;
					int_val |= (UINT)buff[4];
					hr = (int_val/3600); 
					min = (int_val -(3600*hr))/60;
					sec = (int_val -(3600*hr)-(min*60));
					sprintf(str,"%02d:%02d:%02d",hr,min,sec);
					GDispStringAt((UINT)buff[1],(UINT)buff[2],str);
				break;
				case DISPLAY_TEMP:
/*
					byte_val = buff[3];
					sprintf(str,"%02x ",byte_val);
					GDispStringAt((UINT)buff[1],(UINT)buff[2]+4,str);

					byte_val = buff[4];
					sprintf(str,"%02x ",byte_val);
					GDispStringAt((UINT)buff[1],(UINT)buff[2]+6,str);
*/
					int_val = (UINT)buff[3];
					int_val <<= 8;
					int_val |= (UINT)buff[4];
					int_val = convertF(int_val);
					if(int_val < 100)
						sprintf(str,"%2d ",int_val);
					else	
						sprintf(str,"%3d ",int_val);
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
	}
    return (0);		// this should never happen
}

static char *eeprom_str_lookup(int index, char *str)
{
	int i,j,k;
	i = j = k = 0;

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

	memset(str,0,STR_LEN);
	memcpy(str,&eeprom[k+1],j-k);
	return str;
}

static float convertF(int raw_data)
{
	float T_F, T_celcius;
	int ret;
	if ((raw_data & 0x100) != 0)
	{
		raw_data = -(((~raw_data)+1) & 0xff); /* take 2's comp */   
	}
	T_celcius = raw_data * 0.5;
	T_F = (T_celcius * 1.8) + 32;
	ret = (int)T_F;
	return ret;	// returns 257 -> -67
}

