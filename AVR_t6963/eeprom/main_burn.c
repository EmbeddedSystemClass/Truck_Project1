#include <avr/io.h>
//#include "../Atmel/toolchain/avr/include/util/delay.h"
#include "../../../Atmel_other/avr8-gnu-toolchain-linux_x86/avr/include/util/delay.h"
#include "sfr_helper.h"
#include <avr/eeprom.h>
#include "macros.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char menu_labels[23][17] = {
	{"enter password:"},
	{"bad password"},
	{"start engine"},
	{"retries left"},
	{"enable starter"},
	{"ignition"},
	{"fuel pump"},
	{"cooling fan"},
	{"running lights"},
	{"headlights"},
	{"RPM"},
	{"MPH"},
	{"TIME"},
	{"ODOM"},
	{"TRIP"},
	{"ENG TEMP"},
	{"AIR TEMP"},
	{"OIL PRES"},
	{"MAP"},
	{"OIL TEMP"},
	{"O2"},
	{"AUX1"},
	{"AUX2"}
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

	for(i = 0;i < 23;i++)
	{
		memcpy((void *)temp+j,(const void *)menu_labels[i],strlen(menu_labels[i]));
		j += strlen(menu_labels[i]);
		j++;
		*(temp+j) = 0;
	}
	eeprom_update_block((const void*)temp,(void *)eepromString,j);

	return 0;
}
