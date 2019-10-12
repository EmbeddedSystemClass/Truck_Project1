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
	PARAM_STRUCT params;

	params.rpm_update_rate = 1;
	params.mph_update_rate = 2;
	params.fpga_xmit_rate = 3;
	params.high_rev_limit = 4000;
	params.low_rev_limit = 3800;
	params.cooling_fan_on = 171;
	params.cooling_fan_off = 154;
	params.blower_enabled = 7;
	params.blower1_on = 8;
	params.blower2_on = 9;
	params.blower3_on = 10;
	params.engine_temp_limit = 182;
	params.batt_box_temp = 40;
	params.test_bank = 2;
	fp = open((const char *)fptr, O_WRONLY | O_CREAT, 666);
	write(fp,&params,sizeof(PARAM_STRUCT));
	close(fp);

	return 0;
}
