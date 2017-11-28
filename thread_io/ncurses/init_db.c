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
	int i;
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
		pid->affected_output = i;
		pid++;
	}

	pid = curr_i_array;

	strcpy(pid->label,"STARTSWITCH\0");			// 0
	pid++;
	strcpy(pid->label,"SHUTDOWN\0");			// 1
	pid++;	
	strcpy(pid->label,"BRAKES\0");				// 2
	pid++;
	strcpy(pid->label,"HEADLIGHTS\0");			// 3
	pid++;
	strcpy(pid->label,"LEFTBLINKERI\0");		// 4
	pid++;
	strcpy(pid->label,"RIGHTBLINKERI\0");		// 5
	pid++;
	strcpy(pid->label,"RUNNINGLIGHTSI\0");		// 6
	pid++;
	strcpy(pid->label,"LEFTDOOROPEN\0");		// 7
	pid++;
	strcpy(pid->label,"RIGHTDOOROPEN\0");		// 8
	pid++;
	strcpy(pid->label,"TESTINPUT9\0");			// 9
	pid++;
	strcpy(pid->label,"TESTINPUT10\0");			// 10
	pid++;
	strcpy(pid->label,"TESTINPUT11\0");			// 11
	pid++;
	strcpy(pid->label,"TESTINPUT12\0");			// 12
	pid++;
	strcpy(pid->label,"TESTINPUT13\0");			// 13
	pid++;
	strcpy(pid->label,"TESTINPUT14\0");			// 14
	pid++;
	strcpy(pid->label,"TESTINPUT15\0");			// 15
	pid++;
	strcpy(pid->label,"TESTINPUT16\0");			// 16
	pid++;
	strcpy(pid->label,"TESTINPUT17\0");			// 17
	pid++;
	strcpy(pid->label,"TESTINPUT18\0");			// 18
	pid++;
	strcpy(pid->label,"TESTINPUT19\0");			// 19
	pid++;
	strcpy(pid->label,"TESTINPUT20\0");			
	pid++;
	strcpy(pid->label,"TESTINPUT21\0");
	pid++;
	strcpy(pid->label,"TESTINPUT22\0");
	pid++;
	strcpy(pid->label,"TESTINPUT23\0");
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
		pod->onoff = ((i % 2) == 0);
		pod->type = i % 3;
		pod->time_delay = i*2;
		pod->pulse_time = i*3;
		pod++;
	}

	pod = curr_o_array;

	strcpy(pod->label,"COOLINGFAN\0");
	pod++;
	strcpy(pod->label,"FUELPUMP\0");
	pod++;
	strcpy(pod->label,"PRELUBE\0");
	pod++;
	strcpy(pod->label,"STARTER\0");
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
	strcpy(pod->label,"DOMELIGHT\0");
	pod++;
	strcpy(pod->label,"LIGHTBAR\0");
	pod++;
	strcpy(pod->label,"BLINKINDICATE\0");
	pod++;
	strcpy(pod->label,"ENGINERUN\0");
	pod++;
	strcpy(pod->label,"ACCON\0");
	pod++;
	strcpy(pod->label,"BRAKELIGHTS\0");
	pod++;
	strcpy(pod->label,"BACKUPLIGHTS\0");
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
		printf("%d\t%d\t%d\t%d\t%d\t%s\n",pod->port,pod->onoff,pod->type,pod->time_delay,pod->pulse_time,pod->label);
		pod++;
	}

	printf("sizeof: %ld %ld\n",sizeof(I_DATA),sizeof(O_DATA));

	free(curr_i_array);
	free(curr_o_array);
}

