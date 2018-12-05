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
	{"OIL PRES"},
	{"AIR TEMP"},
	{"MAP"},
	{"OIL TEMP"},
	{"O2"},
	{"ODOM"},
	{"TRIP"},
	{"RPM"},
	{"MPH"},
	{"RUN TIME"},  
	{"ENG TEMP"},
	{"OUTDOOR TEMP"},
	{"INDOOR TEMP"},
	{"ENGINE"},				// status values (STATUS_VALUES_OFFSET)
	{"COOLING FAN"},
	{"HEAD LIGHTS"},
	{"BRIGHTS"},
	{"BRAKES"},
	{"RUNNING LIGHTS"},
	{"BLOWER"},
	{"enter password:"},	// various message string	(VARIOUS_MSG_OFFSET)
	{"bad password"},
	{"start engine"},
	{"retries left"},
	{"ON   "},
	{"OFF  "},
	{"re-enter password"},
	{"in    seconds"},
	{"engine on:"},
	{"engine shutoff"},
	{"press   to"},
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
	eeprom_update_block((const void*)temp,(void *)eepromString,j);

	GDispInit();
	_delay_ms(1);
	GDispSetMode(XOR_MODE);
	_delay_us(10);
	GDispSetMode(TEXT_ON);
	_delay_us(10);
	GDispClrTxt();
	GDispStringAt(7,15,"LCD is on!");
	_delay_ms(1000);
	GDispClrTxt();

	col = 0;
	i = 1;		// lookup has to start at 1
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

