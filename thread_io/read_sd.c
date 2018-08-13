#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/stat.h>
#include <assert.h>
#include <string.h>
#include "../mytypes.h"
//#define DEVICE "/dev/ide/host0/target0/lun0/disc"
//#define DEVICE "/mnt/cf/sched.log"
#define DEVICE "sched.log"

char out_types[25][30] = {
	{"STARTER"},
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
	{"BACKUPLIGHTS"}
};

int main(void)
{
	int file_desc, ret_val;
	unsigned char buf[200];
	long file_len;
	int i;
	char out_str[30];
	char onoff[5];

	file_desc = open(DEVICE, O_RDWR);
	if(file_desc < 0)
	{
		printf("failed to open the device \n");
		exit(1);
	}
	file_len = lseek(file_desc,0,SEEK_END);
	printf("file length: %lu\n",file_len);
	lseek(file_desc,1,SEEK_SET);
	printf("time       type tdelay tleft reset pol\n");
	for(i = 0;i < file_len/10;i++)
	{
		read(file_desc, &buf, 10);
		strcpy(out_str,out_types[buf[0]]);
		if(buf[1] == 1)
			strcpy(onoff," ON\0");
		else
			strcpy(onoff,"OFF\0");
		printf("%02d:%02d:%02d - %02d   %02d     %02d    %02d    %02d %s %s\n",
		buf[7],buf[8],buf[9],buf[2],buf[3],	buf[4],buf[5],buf[6],onoff,out_str);
	}
	close(file_desc);
	return 0;
}

