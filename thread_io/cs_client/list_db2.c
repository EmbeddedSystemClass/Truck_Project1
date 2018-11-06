#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <assert.h>
#include "../queue/ollist_threads_rw.h"
#include "../ioports.h"

extern int olLoadConfig(char *filename, ollist_t *oll, size_t size,char *errmsg);
extern int GetFileFormat(char *filename);
extern int ollist_show(ollist_t *llistp);

ollist_t oll;

int main(int argc, char *argv[])
{
	O_DATA *otp;
	O_DATA **otpp = &otp;
	char *fptr1;
	int num_valids = 0;
	IP ip[40];

	int i,j,k;
	int comma_delim;
//	size_t isize;
	size_t osize;
	char errmsg[60];
	int rc;

	comma_delim = 0;
	if(argc < 2)
	{
		printf("usage: %s[odata filename][csv format]\n",argv[0]);
		return 1;
	}
	else if(argc > 2)
	{
		comma_delim = 1;
		printf("outputing odata as csv format\n");
	}
	fptr1 = argv[1];

	if(GetFileFormat(fptr1) != 1)
	{
		printf("%s does not have proper file format for input file\n",fptr1);
		return 1;
	}

	i = NUM_PORT_BITS;

	ollist_init(&oll);
	if(access(fptr1,F_OK) != -1)
	{
		rc = olLoadConfig(fptr1,&oll,osize,errmsg);
		if(rc > 0)
		{
			printf(errmsg);
		}
	}

/*
	for(i = 0;i < 40;i++)
	{
		ollist_find_data(i,&otp,&oll);
		if(otp->input_port != 0xFF)
			printf("port: %2d\t\tinput: %2d\t\t %s\n",otp->port,otp->input_port,otp->label);
	}
	printf("\n");
*/
	for(i = 0;i < 40;i++)
	{
		j = ollist_find_data_ip(i,&otp,&oll);
		if(j > -1)
			printf("port: %2d\t\tinput: %2d\t %s\n",i,j,otp->label);
//		if(otp->input_port != 0xFF)
//			printf("%d\t %02x\t %s\n",otp->port,otp->input_port,otp->label);
	}
	printf("\n");

	for(i = 0;i < 40;i++)
	{
		ip[i].port = -1;
		ip[i].input = 0;
		memset(ip[i].label,0,30);
	}

	j = 0;
	for(i = 0;i < 40;i++)
	{
		for(k = 0;k < 40;k++)
		{
			if(ollist_find_data_op(i,k,&otp,&oll) > -1)
			{
				ip[j].port = k;
				ip[j].input = i;
				strcpy(ip[j++].label,otp->label);
			}
		}
	}
	printf("\n");
	for(i = 0;i < 40;i++)
	{
		if(ip[i].port > -1)
			printf("%d:\t\tport: %d\t\tinput: %d\t\t %s \n",i,ip[i].port,ip[i].input,ip[i].label);
	}
	return 0;
}

