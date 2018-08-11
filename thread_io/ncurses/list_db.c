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

extern int iLoadConfig(char *filename, I_DATA *curr_i_array,size_t size,char *errmsg);
extern int oLoadConfig(char *filename, O_DATA *curr_o_array,size_t size,char *errmsg);
extern int GetFileFormat(char *filename);

int main(int argc, char *argv[])
{
	I_DATA *curr_i_array;
	I_DATA *pid;
	O_DATA *curr_o_array;
	O_DATA *pod;
	char *fptr1;
	char *fptr2;
	int file1 = 1;
	int file2 = 1;
	int num_valids = 0;

	int i;
	int comma_delim;
	size_t isize;
	size_t osize;
	char errmsg[60];

	comma_delim = 0;
	if(argc < 2)
	{
		printf("usage: %s [idata filename][odata filename][csv format]\n",argv[0]);
		return 1;
	}
	else if(argc < 3)
	{
		printf("usage: %s %s [odata filename]\n",argv[0],argv[1]);
		file2 = 0;
	}
	else if(argc > 3)
	{
		comma_delim = 1;
		printf("outputing odata as csv format\n");
	}
		
	fptr1 = argv[1];
	fptr2 = argv[2];

	if(GetFileFormat(fptr1) != 0)
	{
		printf("%s does not have proper file format for input file\n",fptr1);
		file1 = 0;
	}else printf("reading idata file: %s\n",fptr1);

	if(file2 != 0)
	{
		if(GetFileFormat(fptr2) != 1)
		{
			printf("%s does not have proper file format for output file\n",fptr2);
			file2 = 0;
		} else printf("reading odata file: %s\n",fptr2);
	}
	i = NUM_PORT_BITS;

	isize = sizeof(I_DATA);
	isize *= i;

	osize = sizeof(O_DATA);
	osize *= i;

	curr_i_array = (I_DATA *)malloc(isize);
	memset((void *)curr_i_array,0,isize);

	if(file1)
	{
		pid = curr_i_array;

		memset((void *)curr_i_array,0,isize);

		if(iLoadConfig(fptr1,curr_i_array,isize,errmsg) < 0)
		{
			printf("%s\n",errmsg);
			return -1;
		}

		pid = curr_i_array;

		printf("\n");
		printf("port\taffected_output\t\tlabel\n\n");
		for(i = 0;i < isize/sizeof(I_DATA);i++)
		{
			if(pid->label[0] != 0)

				if(comma_delim == 1)
				{
					printf("%d,%d,%s\n",pid->port,pid->affected_output,pid->label);
				}else
				{
					printf("%d\t%d\t\t\t%s\n",pid->port,pid->affected_output,pid->label);
				}
				pid++;
		}

		pid = curr_i_array;
		for(i = 0;i < NUM_PORT_BITS;i++)
		{
			if(pid->label[0] != 0)
				num_valids++;
			pid++;
		}
		printf("num valid records: %d\n",num_valids);

		printf("\n");
	}
	// outputs

	curr_o_array = (O_DATA *)malloc(osize);
	memset((void *)curr_o_array,0,osize);

	if(file2)
	{
		if(oLoadConfig(fptr2,curr_o_array,osize,errmsg) < 0)
		{
			printf("%s\n",errmsg);
			return -1;
		}

		pod = curr_o_array;

		printf("\n");
		printf("port\tonoff\tpolarity\ttype\ttime_delay\tpulse_time\tlabel\n\n");
		for(i = 0;i < osize/sizeof(O_DATA);i++)
		{
			if(pod->label[0] != 0)
			{
				if(comma_delim == 1)
				{
					printf("%d,%d,%d,%d,%d,%d,%s\n",
						pod->port, pod->onoff, pod->polarity, pod->type, pod->time_delay, 
							pod->pulse_time, pod->label);
				}else
				{
					printf("%d\t%d\t%d\t\t%d\t%d\t\t%d\t\t%s\n",
						pod->port, pod->onoff, pod->polarity, pod->type, pod->time_delay, 
							pod->pulse_time, pod->label);
				}
			}
			pod++;
		}

		pod = curr_o_array;
		num_valids = 0;
		for(i = 0;i < NUM_PORT_BITS;i++)
		{
			if(pod->label[0] != 0)
				num_valids++;
			pod++;
		}
		printf("num valid records: %d\n",num_valids);
		printf("\n\n");

	}

	printf("sizeof: %ld %ld\n",sizeof(I_DATA),sizeof(O_DATA));

	free(curr_i_array);
	free(curr_o_array);
}

