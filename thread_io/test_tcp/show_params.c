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

	fp = open((const char *)fptr, O_RDWR);
	read(fp, &ps, sizeof(PARAM_STRUCT));
	close(fp);

	printf("\r\nrpm_update_rate: %d\r\n",ps.rpm_update_rate);
	printf("mph_update_rate: %d\r\n",ps.mph_update_rate);
	printf("fpga_xmit_rate: %d\r\n",ps.fpga_xmit_rate);
	printf("high_rev_limit:  %d\r\n",ps.high_rev_limit);
	printf("low_rev_limit:   %d\r\n",ps.low_rev_limit);
	printf("cooling_fan_on:  %d\r\n",ps.cooling_fan_on);
	printf("cooling_fan_off: %d\r\n",ps.cooling_fan_off);
	printf("blower_enabled:  %d\r\n",ps.blower_enabled);
	printf("blower1_on:      %d\r\n",ps.blower1_on);
	printf("blower2_on:      %d\r\n",ps.blower2_on);
	printf("blower3_on:      %d\r\n",ps.blower3_on);
	printf("test_bank:       %d\r\n",ps.test_bank);
	
	return 0;
}
