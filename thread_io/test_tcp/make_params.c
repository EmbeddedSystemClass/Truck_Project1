#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/stat.h>
#include <assert.h>
#include "../../mytypes.h"

int main(void)
{
	int fp;
	char filename[20] = "param.conf\0";
	char *fptr = filename;
	PARAM_STRUCT ps;

	ps.rpm_update_rate = 1;
	ps.mph_update_rate = 2;
	ps.fpga_xmit_rate = 3;
	ps.high_rev_limit = 4000;
	ps.low_rev_limit = 3800;
	ps.cooling_fan_on = 171;
	ps.cooling_fan_off = 154;
	ps.blower_enabled = 7;
	ps.blower1_on = 8;
	ps.blower2_on = 9;
	ps.blower3_on = 10;
	ps.lights_on_delay = 30;
	ps.engine_temp_limit = 182;
	ps.batt_box_temp = 40;
	ps.test_bank = 2;

	printf("\r\nrpm_update_rate:\t\t%d\r\n",ps.rpm_update_rate);
	printf("mph_update_rate:\t\t%d\r\n",ps.mph_update_rate);
	printf("fpga_xmit_rate:\t\t\t%d\r\n",ps.fpga_xmit_rate);
	printf("high_rev_limit:\t\t\t%d\r\n",ps.high_rev_limit);
	printf("low_rev_limit:\t\t\t%d\r\n",ps.low_rev_limit);
	printf("cooling_fan_on:\t\t\t%d\r\n",ps.cooling_fan_on);
	printf("cooling_fan_off:\t\t%d\r\n",ps.cooling_fan_off);
	printf("blower_enabled:\t\t\t%d\r\n",ps.blower_enabled);
	printf("blower1_on:\t\t\t%d\r\n",ps.blower1_on);
	printf("blower2_on:\t\t\t%d\r\n",ps.blower2_on);
	printf("blower3_on:\t\t\t%d\r\n",ps.blower3_on);
	printf("lights delay:\t\t\t%d\r\n",ps.lights_on_delay);
	printf("engine_temp_limit:\t\t%d\r\n",ps.engine_temp_limit);
	printf("battery box temp:\t\t%d\r\n",ps.batt_box_temp);
	printf("test_bank:\t\t\t%d\r\n",ps.test_bank);

	fp = open((const char *)fptr, O_WRONLY | O_CREAT, 666);
	write(fp,&ps,sizeof(PARAM_STRUCT));
	close(fp);

	return 0;
}
