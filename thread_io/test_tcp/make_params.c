/* make_params.c - actual values of rpm_update_rate->low_rev_limit & lights_on_delay is 
	the index into the choices in the xml files and the collections of the comboboxes
	in the DlgSetParams.cs of the client proj. and ParamsForm.cs of the server proj.
	
as of 11/7/2019 this file is prepended by an 0xAA and appended by an 0x55

high_rev_limit:
0 - 6000
1 - 5800
2 - 5600
3 - 5400
4 - 5200
5 - 5000
6 - 4800
7 - 4600
8 - 4200
9 - 4000

low_rev_limit:
0 - 4500
1 - 4400
2 - 4300
3 - 4200
4 - 4100
5 - 4000
6 - 3900
7 - 3800
8 - 3700

fpga_xmit_rate:
0 - 1000
1 - 2000
2 - 3000

rpm_update_rate:
0 - 1000
1 - 2000
2 - 3000

mph_update_rate:
0 - 1000
1 - 2000
2 - 3000

lights_on_delay:
0 - 1 second
1 - 2 seconds
2 - 3 seconds
3 - 5 seconds
4 - 10 seconds
5 - 15 seconds
6 - 30 seconds
7 - 1 minute
8 - 2 minutes
9 - 5 minutes
10 - 10 minutes
11 - 30 minutes
12 - 1 hour
*/

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
	UCHAR id = 0xAA;
	char password[5] = "4567\0";

	ps.rpm_update_rate = 1;
	ps.mph_update_rate = 2;
	ps.fpga_xmit_rate = 2;
	ps.high_rev_limit = 9;
	ps.low_rev_limit = 7;
	ps.cooling_fan_on = 180;
	ps.cooling_fan_off = 170;
	ps.blower_enabled = 50;
	ps.blower1_on = 32;
	ps.blower2_on = 20;
	ps.blower3_on = 10;
	ps.lights_on_delay = 6;
	ps.engine_temp_limit = 195;
	ps.batt_box_temp = 40;
	ps.test_bank = 2;
	ps.password_timeout = 1;
	ps.password_retries = 1;

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
	printf("password timeout:\t\t%d\r\n",ps.password_timeout);
	printf("password retries:\t\t%d\r\n",ps.password_retries);

	fp = open((const char *)fptr, O_WRONLY | O_CREAT, 666);
	write(fp,&id,1);
	write(fp,&ps,sizeof(PARAM_STRUCT));
	write(fp,&password,4);
	id = 0x55;
	write(fp,&id,1);
	close(fp);

	return 0;
}
