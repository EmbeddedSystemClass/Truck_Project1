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





// values start at line 20 so its easy to calculate the offset
#define NUM_STR 53
char menu_labels[NUM_STR][24] = {
	{"RPM"},			// rt values
	{"MPH"},
	{"ENG TEMP"},
	{"OIL PRES"},
	{"AIR TEMP"},
	{"MAP"},
	{"OIL TEMP"},
	{"O2"},
	{"ODOM"},
	{"TRIP"},
	{"RUN TIME"},  
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
	{"enable starter"},			// 1	start of menu strings
	{"start sequence"},			// 2
	{"shutdown"},				// 3
	{"key silent"},				// 4
	{"dim screen"},				// 5
	{"bright screen"},			// 6
	{"toggle lights"},			// 7
	{"set time/date"},			// 8
	{"shutdown io box"},		// 9
	{"reboot io box"},			// 0
	{"engine off time"},		// *
	{"fpga send rate"},			// #
	{"LED update rate"},		// 1
	{"high rev limit"},			// 2
	{"low rev limit"},			// 3
	{"set lights off"},			// 4
	{"fpga comm"},				// 5
	{"change password"},		// 6
	{"blk ht on temp"},			// 7
	{"blk ht off temp"},		// 8
	{"batt ht on temp"},		// 9
	{"batt ht off temp"},		// 0
	{"unused 1"},				// *
	{"unused 2"}				// #
};

#define STRING_LEN 0x400
char eepromString[STRING_LEN] EEMEM;
//UCHAR temp[STRING_LEN];
UCHAR temp[STRING_LEN/2];

int main(void)
{
	char ramString[STRING_LEN];
	int i,j;

	i = 0;
	j = 0;

	for(i = 0;i < NUM_STR;i++)
	{
		memcpy((void *)temp+j,(const void *)menu_labels[i],strlen(menu_labels[i]));
		j += strlen(menu_labels[i]);
		j++;
		*(temp+j) = 0;
	}
	eeprom_update_block((const void*)temp,(void *)eepromString,j);

	return 0;
}
