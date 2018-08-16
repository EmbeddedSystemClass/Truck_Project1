#include <avr/io.h>
//#include "../Atmel/toolchain/avr/include/util/delay.h"
#include "../../../Atmel_other/avr8-gnu-toolchain-linux_x86/avr/include/util/delay.h"
#include "sfr_helper.h"
#include <avr/eeprom.h>
#include "macros.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>








#define NUM_STR 51
char menu_labels[NUM_STR][20] = {
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
	{"STARTER"},			// RT_VALUES_OFFSET + 11
	{"ACCON"},
	{"FUELPUMP"},
	{"COOLINGFAN"},
	{"PRELUBE"},
	{"HEADLAMP"},
	{"BRIGHTS"},
	{"LEFTBLINKER"},
	{"RIGHTBLINKER"},
	{"RUNNINGLIGHTS"},
	{"ALARMSPEAKER"},
	{"ALARMLIGHT"},
	{"BATTERYCHARGERELAY"},
	{"DASHHEATER"},
	{"BATTERYCOMPHEATER"},
	{"CRANKCASEHEATER"},
	{"TRAILERBRAKES"},
	{"TRAILERLEFTBLINKER"},
	{"TRAILERRIGHTBLINKER"},
	{"INTRUDERALARM"},
	{"DOMELIGHT"},
	{"LIGHTBAR"},
	{"BLINKINDICATE"},
	{"BRAKELIGHTS"},
	{"BACKUPLIGHTS"},
	{"enable starter"},		// menu choices
	{"ignition"},
	{"fuel pump"},
	{"cooling fan"},
	{"running lights"},
	{"headlights"},
	{"set dim time"},
	{"enter password:"},	// various message string
	{"bad password"},
	{"start engine"},
	{"retries left"},
	{"ON   "},
	{"OFF  "},
	{"re-enter password"},
	{"in    seconds"},
	{"oh shitsky"}
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
