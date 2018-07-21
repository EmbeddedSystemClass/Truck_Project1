#include <avr/io.h>
//#include "../Atmel/toolchain/avr/include/util/delay.h"
#include "../../../Atmel_other/avr8-gnu-toolchain-linux_x86/avr/include/util/delay.h"
#include "sfr_helper.h"
#include <avr/eeprom.h>
#include "USART.h"
#include "t6963.h"
#include "macros.h"
//#include "main.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define dispCharAt(_row,_col,_char) GDispCharAt((UINT)_row,(UINT)_col,(UCHAR)_char)
#define dispSetCursor(_mode,_row,_col,_type) GDispSetCursor ((UCHAR)_mode, (UINT)_row, (UINT)_col, 

char menu_labels[23][17] = {
	{"enter password:"},
	{"bad password"},
	{"start engine"},
	{"test1"},
	{"test2"},
	{"hello1"},
	{"hello2"},
	{"hello3"},
	{"hello4"},
	{"hello5"},
	{"RPM"},
	{"ENG TEMP"},
	{"TRIP"},
	{"TIME"},
	{"AIR TEMP"},
	{"MPH"},
	{"OIL PRES"},
	{"MAP"},
	{"OIL TEMP"},
	{"O2"},
	{"AUX1"},
	{"AUX2"}
};

#define BURN_EEPROM				9
#define READ_EEPROM				10
#define DISPLAY_LABELS			11
#define DISPLAY_RTPARAMS		12
#define TEST_PATTERN1			13
#define TEST_PATTERN2			14
#define TEST_PATTERN3			15
#define TEST_PATTERN4			16
#define FAST_TEST_PATTERN1		17
#define FAST_TEST_PATTERN2		18
#define FAST_TEST_PATTERN3		19
#define FAST_TEST_PATTERN4		20
#define TRANSMIT_ASCII			21

void printMenu(void);
#define STRING_LEN 0x400
char eepromString[STRING_LEN] EEMEM;
//UCHAR temp[STRING_LEN];
UCHAR temp[STRING_LEN/2];

int main(void)
{
	char ramString[STRING_LEN];
	int i,j;
	char *ch;
    int row,col;
    UCHAR k, key;
	int offset = 0;
	UCHAR buff[20];

    initUSART();

	GDispInit();
	_delay_us(10);
	GDispSetMode(XOR_MODE);
	_delay_us(10);
	GDispSetMode(TEXT_ON);
	_delay_us(10);
	GDispClrTxt(0);
	printString("screen is enabled");
	GDispStringAt(7, 7, "LCD is On!");

	i = 0;
	memset(buff,0,20);
	while (1)
    {
		key = receiveByte();

		buff[i] = key;
		i++;

		if(key == 0xfe)
		{

			transmitByte(buff[0]-9+0x30);
			transmitByte(0xFE);
			switch(buff[0])
			{
				case BURN_EEPROM:
					j = 0;

					for(i = 0;i < 23;i++)
					{
						memcpy((void *)temp+j,(const void *)menu_labels[i],strlen(menu_labels[i]));
						j += strlen(menu_labels[i]);
						j++;
						*(temp+j) = 0;
					}
					eeprom_update_block((const void*)temp,(void *)eepromString,j);
					printString("~~~~ done burning eeprom ~~~~");
					break;
				case READ_EEPROM:
					break;
		        case DISPLAY_LABELS:
		            printString("displaying labels");
					break;
				case DISPLAY_RTPARAMS:
		            printString(" displaying rt_params");
		            break;
				case TEST_PATTERN1:
					printString("tesing pattern 1");

					GDispClrTxt(0);
					k = 0x20;
					for(row = 0;row < ROWS;row++)
					{
						for(col = 0;col < COLUMN;col++)
						{
							dispCharAt(row,col,k);
							_delay_ms(5);
	//						transmitByte(k);
							if(++k > 0x7e)
								k = 0x20;
						}
					}
					printString("done");
					break;
				case TEST_PATTERN2:
					printString("tesing pattern 2");
					GDispClrTxt(0);
					k = 0x30;
					for(row = 0;row < ROWS;row++)
					{
						for(col = 0;col < COLUMN;col++)
						{
							dispCharAt(row,col,k);
							_delay_ms(3);
	//						transmitByte(k);
							if(++k > 0x39)
								k = 0x30;
						}
					}
					printString("done");
					break;
				case TEST_PATTERN3:
					printString("tesing pattern 3");
					GDispClrTxt(0);
					k = 0x41;
					for(row = 0;row < ROWS;row++)
					{
						for(col = 0;col < COLUMN;col++)
						{
							dispCharAt(row,col,k);
							_delay_ms(2);
	//						transmitByte(k);
							if(++k > 0x5A)
								k = 0x41;
						}
					}
					printString("done");
					break;
				case TEST_PATTERN4:
					printString("tesing pattern 4");
					GDispClrTxt(0);
					k = 0x61;
					for(row = 0;row < ROWS;row++)
					{
						for(col = 0;col < COLUMN;col++)
						{
							dispCharAt(row,col,k);
							_delay_ms(1);
	//						transmitByte(k);
							if(++k > 0x7A)
								k = 0x61;
						}
					}
					printString("done");
					break;
				case FAST_TEST_PATTERN1:
					printString("tesing pattern 1 (fast)");
					GDispClrTxt(0);
					k = 0x20;
					for(row = 0;row < ROWS;row++)
					{
						for(col = 0;col < COLUMN;col++)
						{
							dispCharAt(row,col,k);
							if(++k > 0x7e)
								k = 0x20;
						}
					}
					printString("done");
					break;
				case FAST_TEST_PATTERN2:
					printString("tesing pattern 2 (fast)");
					GDispClrTxt(0);
					k = 0x30;
					for(row = 0;row < ROWS;row++)
					{
						for(col = 0;col < COLUMN;col++)
						{
							dispCharAt(row,col,k);
							if(++k > 0x39)
								k = 0x30;
						}
					}
					printString("done");
					break;
				case FAST_TEST_PATTERN3:
					printString("tesing pattern 3 (fast)");
					GDispClrTxt(0);
					k = 0x41;
					for(row = 0;row < ROWS;row++)
					{
						for(col = 0;col < COLUMN;col++)
						{
							dispCharAt(row,col,k);
							if(++k > 0x5A)
								k = 0x41;
						}
					}
					printString("done");
					break;
				case FAST_TEST_PATTERN4:
					printString("tesing pattern 4 (fast)");
					GDispClrTxt(0);
					k = 0x61;
					for(row = 0;row < ROWS;row++)
					{
						for(col = 0;col < COLUMN;col++)
						{
							dispCharAt(row,col,k);
							if(++k > 0x7A)
								k = 0x61;
						}
					}
					break;

				case TRANSMIT_ASCII:
					for(k = 0x21;k < 0x7e;k++)
						transmitByte(k);
					printString("done");
					break;
				case 'r':
					GDispClrTxt(0);
					break;
				default:
					break;
			}		// end of switch
	        i = 0;
        }		// end of 'if(key == 0xFE)
    }			// end if while
    return (0);
}
