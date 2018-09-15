#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <assert.h>
#include "../queue/illist_threads_rw.h"
#include "../queue/ollist_threads_rw.h"
#include "../ioports.h"
#include "setiodata.h"
#include "config_file.h"

extern int iWriteConfig(char *filename, I_DATA *curr_i_array,size_t size,char *errmsg);
extern int iLoadConfig(char *filename, I_DATA *curr_i_array,size_t size,char *errmsg);
extern int oWriteConfig(char *filename, O_DATA *curr_o_array,size_t size,char *errmsg);
extern int oLoadConfig(char *filename, O_DATA *curr_o_array,size_t size,char *errmsg);

int main(int argc, char *argv[])
{
	I_DATA *curr_i_array;
	I_DATA *pid;
	O_DATA *curr_o_array;
	O_DATA *pod;
	int i,j;
	size_t isize;
	size_t osize;
	char *fptr1;
	char *fptr2;
	char errmsg[60];
	int ret;

	if(argc < 2)
	{
		printf("usage: %s [idata filename][odata filename]\n",argv[0]);
		return 1;
	}
	else if(argc < 3)
	{
		printf("usage: %s %s [odata filename]\n",argv[0],argv[1]);
		return 1;
	}
	fptr1 = argv[1];
	fptr2 = argv[2];

	printf("creating idata file: %s\n",fptr1);
	printf("creating odata file: %s\n",fptr2);

	printf("\nsizeof I_DATA: %lu\n",sizeof(I_DATA));
	i = NUM_PORT_BITS;
	printf("NUM_PORT_BITS: %d\n",i);
	isize = sizeof(I_DATA);
	isize *= i;
	printf("total size: of i_data %lu\n",isize);

	printf("\nsizeof O_DATA: %lu\n",sizeof(O_DATA));
	i = NUM_PORT_BITS;
	printf("NUM_PORT_BITS: %d\n",i);
	osize = sizeof(O_DATA);
	osize *= i;
	printf("total size of o_data: %lu\n",osize);

	curr_i_array = (I_DATA *)malloc(isize);
	memset((void *)curr_i_array,0,isize);

	pid = curr_i_array;

	// inputs

	for(i = 0;i < isize/sizeof(I_DATA);i++)
	{
		pid->port = i;
		pid->affected_output[0] = i;
		for(j = 1;j < 10;j++)
			pid->affected_output[j] = 41;
		pid++;
	}

	pid = curr_i_array;

	strcpy(pid->label,"STARTER\0");
	pid++;
	strcpy(pid->label,"ACCON\0");
	pid++;
	strcpy(pid->label,"FUELPUMP\0");
	pid++;
	strcpy(pid->label,"COOLINGFAN\0");
	pid++;
	strcpy(pid->label,"HEADLAMP\0");
	pid++;
	strcpy(pid->label,"BRIGHTS\0");
	pid++;
	strcpy(pid->label,"LEFTBLINKER\0");
	pid++;
	strcpy(pid->label,"RIGHTBLINKER\0");
	pid++;
	strcpy(pid->label,"RUNNINGLIGHTS\0");
	pid++;
	strcpy(pid->label,"ALARMSPEAKER\0");
	pid++;
	strcpy(pid->label,"ALARMLIGHT\0");
	pid++;
	strcpy(pid->label,"BATTERYCHARGERELAY\0");
	pid++;
	strcpy(pid->label,"DASHHEATER\0");
	pid++;
	strcpy(pid->label,"BATTERYCOMPHEATER\0");
	pid++;
	strcpy(pid->label,"CRANKCASEHEATER\0");
	pid++;
	strcpy(pid->label,"TRAILERBRAKES\0");
	pid++;
	strcpy(pid->label,"TRAILERLEFTBLINKER\0");
	pid++;
	strcpy(pid->label,"TRAILERRIGHTBLINKER\0");
	pid++;
	strcpy(pid->label,"INTRUDERALARM\0");
	pid++;
	strcpy(pid->label,"ESTOPSWITCH\0");
	pid++;
	strcpy(pid->label,"LIGHTBAR\0");
	pid++;
	strcpy(pid->label,"BLINKINDICATE\0");
	pid++;
	strcpy(pid->label,"BRAKELIGHTS\0");
	pid++;
	strcpy(pid->label,"BACKUPLIGHTS\0");
	pid++;
	strcpy(pid->label,"TESTINPUT24\0");
	pid++;
	strcpy(pid->label,"TESTINPUT25\0");
	pid++;
	strcpy(pid->label,"TESTINPUT26\0");
	pid++;
	strcpy(pid->label,"TESTINPUT27\0");
	pid++;
	strcpy(pid->label,"TESTINPUT28\0");
	pid++;
	strcpy(pid->label,"TESTINPUT29\0");
	pid++;
	strcpy(pid->label,"TESTINPUT30\0");
	pid++;
	strcpy(pid->label,"TESTINPUT31\0");
	pid++;
	strcpy(pid->label,"TESTINPUT32\0");
	pid++;
	strcpy(pid->label,"TESTINPUT33\0");
	pid++;
	strcpy(pid->label,"TESTINPUT34\0");
	pid++;
	strcpy(pid->label,"TESTINPUT35\0");
	pid++;
	strcpy(pid->label,"TESTINPUT36\0");
	pid++;
	strcpy(pid->label,"TESTINPUT37\0");
	pid++;
	strcpy(pid->label,"TESTINPUT38\0");
	pid++;
	strcpy(pid->label,"NULL\0");

	pid = curr_i_array;

	ret = iWriteConfig(fptr1,curr_i_array,isize,errmsg);
	if(ret < 0)
		printf("iWriteConfig: %s\n",errmsg);

/*
	memset((void *)curr_i_array,0,isize);

	ret = iLoadConfig(fptr1,curr_i_array,isize,errmsg);
	if(ret < 0)
		printf("iLoadConfig: %s\n",errmsg);

	pid = curr_i_array;
*/
	printf("%s %s has %lu records \n",errmsg,fptr1,isize/sizeof(I_DATA));
	for(i = 0;i < isize/sizeof(I_DATA);i++)
	{
		printf("%d\t%s\n",pid->port,pid->label);
		pid++;
	}
	// outputs

	curr_o_array = (O_DATA *)malloc(osize);
	memset((void *)curr_o_array,0,osize);

	pod = curr_o_array;

	for(i = 0;i < osize/sizeof(O_DATA);i++)
	{
		pod->port = i;
		pod->onoff = 0;
		pod->polarity = 0;
		pod->type = 0;
		pod->time_delay = 0;
		pod->time_left = 0;
		pod->pulse_time = 0;
		pod->reset = 0;
		pod++;
	}

	pod = curr_o_array;

	strcpy(pod->label,"STARTER\0");
	pod++;
	strcpy(pod->label,"ACCON\0");
	pod++;
	strcpy(pod->label,"FUELPUMP\0");
	pod++;
	strcpy(pod->label,"COOLINGFAN\0");
	pod++;
	strcpy(pod->label,"HEADLAMP\0");
	pod++;
	strcpy(pod->label,"BRIGHTS\0");
	pod++;
	strcpy(pod->label,"LEFTBLINKER\0");
	pod++;
	strcpy(pod->label,"RIGHTBLINKER\0");
	pod++;
	strcpy(pod->label,"RUNNINGLIGHTS\0");
	pod++;
	strcpy(pod->label,"ALARMSPEAKER\0");
	pod++;
	strcpy(pod->label,"ALARMLIGHT\0");
	pod++;
	strcpy(pod->label,"BATTERYCHARGERELAY\0");
	pod++;
	strcpy(pod->label,"DASHHEATER\0");
	pod++;
	strcpy(pod->label,"BATTERYCOMPHEATER\0");
	pod++;
	strcpy(pod->label,"CRANKCASEHEATER\0");
	pod++;
	strcpy(pod->label,"TRAILERBRAKES\0");
	pod++;
	strcpy(pod->label,"TRAILERLEFTBLINKER\0");
	pod++;
	strcpy(pod->label,"TRAILERRIGHTBLINKER\0");
	pod++;
	strcpy(pod->label,"INTRUDERALARM\0");
	pod++;
	strcpy(pod->label,"ESTOPSWITCH\0");
	pod++;
	strcpy(pod->label,"LIGHTBAR\0");
	pod++;
	strcpy(pod->label,"BLINKINDICATE\0");
	pod++;
	strcpy(pod->label,"BRAKELIGHTS\0");
	pod++;
	strcpy(pod->label,"BACKUPLIGHTS\0");
	pod++;
	strcpy(pod->label,"TESTOUTPUT24\0");
	pod++;
	strcpy(pod->label,"TESTOUTPUT25\0");
	pod++;
	strcpy(pod->label,"TESTOUTPUT26\0");
	pod++;
	strcpy(pod->label,"TESTOUTPUT27\0");
	pod++;
	strcpy(pod->label,"TESTOUTPUT28\0");
	pod++;
	strcpy(pod->label,"TESTOUTPUT29\0");
	pod++;
	strcpy(pod->label,"TESTOUTPUT30\0");
	pod++;
	strcpy(pod->label,"TESTOUTPUT31\0");
	pod++;
	strcpy(pod->label,"TESTOUTPUT32\0");
	pod++;
	strcpy(pod->label,"TESTOUTPUT33\0");
	pod++;
	strcpy(pod->label,"TESTOUTPUT34\0");
	pod++;
	strcpy(pod->label,"TESTOUTPUT35\0");
	pod++;
	strcpy(pod->label,"TESTOUTPUT36\0");
	pod++;
	strcpy(pod->label,"TESTOUTPUT37\0");
	pod++;
	strcpy(pod->label,"TESTOUTPUT38\0");
	pod++;
	strcpy(pod->label,"NULL\0");

	ret = oWriteConfig(fptr2,curr_o_array,osize,errmsg);
	if(ret < 0)
		printf("oWriteConfig: %s\n",errmsg);
	else printf("%s %s has %lu records \n",errmsg,fptr2,osize/sizeof(O_DATA));

/*
	memset((void *)curr_o_array,0,osize);

	ret = oLoadConfig(fptr2,curr_o_array,osize,errmsg);
	if(ret < 0)
		printf("oLoadConfig: %s\n",errmsg);
*/
	pod = curr_o_array;
	for(i = 0;i < osize/sizeof(O_DATA);i++)
	{
		printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%s\n",pod->port,pod->onoff,pod->polarity,pod->type,
				pod->time_delay,pod->time_left, pod->pulse_time,pod->reset,pod->label);
		pod++;
	}

	printf("sizeof: %ld %ld\n",sizeof(I_DATA),sizeof(O_DATA));

	free(curr_i_array);
	free(curr_o_array);
}

