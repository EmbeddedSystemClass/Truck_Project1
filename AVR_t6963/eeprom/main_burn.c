// main_burn.c - run this from AVR_t6963/eeprom directory to burn menu_labes into the eeprom
// (make flash)
#include <avr/io.h>
//#include "../Atmel/toolchain/avr/include/util/delay.h"
#include "../../../Atmel_other/avr8-gnu-toolchain-linux_x86/avr/include/util/delay.h"
#include "sfr_helper.h"
#include <avr/eeprom.h>
#include "macros.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "t6963.h"
#include "../../mytypes.h"

static char *eeprom_str_lookup(int index, char *str);

// values start at line 20 so its easy to calculate the offset

char menu_labels[NUM_STR][24] = {
	{"OIL PRES\0"},
	{"AIR TEMP\0"},
	{"MAP\0"},
	{"OIL TEMP\0"},
	{"O2\0"},
	{"ODOM\0"},
	{"TRIP\0"},
	{"RPM\0"},
	{"MPH\0"},
	{"RUN TIME\0"},
	{"ENG TEMP\0"},
	{"FUEL LEVEL\0"},
	{"OUTDOOR TEMP\0"},
	{"INDOOR TEMP\0"},
	{"ENGINE\0"},				// status values (STATUS_VALUES_OFFSET)
	{"COOLING FAN\0"},
	{"HEAD LIGHTS\0"},
	{"BRIGHTS\0"},
	{"BRAKES\0"},
	{"RUNNING LIGHTS\0"},
	{"enter password:\0"},	// various message string	(VARIOUS_MSG_OFFSET)
	{"bad password\0"},
	{"start engine\0"},
	{"retries left\0"},
	{"ON   \0"},
	{"OFF  \0"},
	{"re-enter password\0"},
	{"in    seconds\0"},
	{"engine on:\0"},
	{"engine shutoff\0"},
	{"press   to\0"},
	{"                "}
};

#define STRING_LEN 0x400
#define STR_LEN 30
char eepromString[STRING_LEN] EEMEM;
//UCHAR temp[STRING_LEN];
UCHAR temp[STRING_LEN/2];

int main(void)
{
	int i,j;
	char str[30];
	i = 0;
	j = 0;
	UCHAR row, col;

	for(i = 0;i < NUM_STR;i++)
	{
		memcpy((void *)temp+j,(const void *)menu_labels[i],strlen(menu_labels[i]));
		j += strlen(menu_labels[i]);
		j++;
		*(temp+j) = 0;
	}

	eeprom_write_block((const void*)temp,(void *)eepromString,j);

	GDispInit();
	_delay_ms(1);
	GDispSetMode(XOR_MODE);
	_delay_us(10);
	GDispSetMode(TEXT_ON);
	_delay_us(10);
	GDispClrTxt();
	FONT_8X8();
	GDispStringAt(7,15,"LCD is on!");
	_delay_ms(1000);
	GDispClrTxt();

	col = 0;
	i = 0;
	for(row = 0;row < 16;row++)
	{
		strcpy(str,eeprom_str_lookup(i++, str));
		GDispGoto(row,col);
		GDispStringAt(row,col,str);
	}
	col = 20;
	for(row = 0;row < 16;row++)
	{
		strcpy(str,eeprom_str_lookup(i++, str));
		GDispGoto(row,col);
		GDispStringAt(row,col,str);
	}
	return 0;
}

static char *eeprom_str_lookup(int index, char *str)
{
	int i,j,k;
	i = j = k = 0;
	index++;

	do{
		j++;
		if(temp[j] == 0)
		{
			i++;
 			k = j;
		}
//		printf("%c",eeprom[j]);

	}while(i < index);

	do{
		k--;
	}while(temp[k] != 0);

//	printf("\ni: %d j: %d k: %d\n",i,j,k);
	memset(str,0,STR_LEN);
	memcpy(str,&temp[k+1],j-k);
	return str;
}

