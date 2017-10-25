#include<unistd.h>
#include<sys/types.h>
#include<sys/mman.h>
#include<stdio.h>
#include<fcntl.h>
#include<assert.h>
#include<time.h>
#include<stdlib.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>

#define NUM_PORTS 6
#define NUM_PORT_BITS (NUM_PORTS*8)-8	// PORTC & F only has 6 bits each

#define			OUTPORTA_OFFSET		0
#define			OUTPORTB_OFFSET		1
#define			OUTPORTC_OFFSET		2
#define			OUTPORTD_OFFSET		3
#define			OUTPORTE_OFFSET		4
#define			OUTPORTF_OFFSET		5

#define			UCHAR unsigned char
#define			UINT unsigned int

int main(int argc, char *argv[])
{
	UCHAR mask;
	UCHAR outportstatus[OUTPORTF_OFFSET-OUTPORTA_OFFSET];
	UCHAR *pstate;
	UCHAR state = 0;
	UCHAR port;
	UCHAR temp;
	char *fptr;
	char filename[30];
	int fp;
	char errmsg[30];


	fptr = filename;
	strcpy(filename,"iostate.bin\0");
	fp = open((const char *)fptr, O_RDONLY);
	if(fp < 0)
	{
		strcpy(errmsg,strerror(errno));
		close(fp);
		printf("%s  %s\n",errmsg,filename);
		fp = open((const char *)fptr, O_RDWR | O_CREAT | O_TRUNC, 666);
		if(fp < 0)
		{
			strcpy(errmsg,strerror(errno));
			close(fp);
			printf("%s  %s\n",errmsg,filename);
			exit(1);
		}
	}
	read(fp,&outportstatus,NUM_PORTS);
	close(fp);


	if(argc < 2)
	{
		printf("usage: %s [port]\n",argv[0]);
		exit(1);
	}
	port = atoi(argv[1]);
	if(port > 7)
	{
		printf("port can't be > 7\n");
		exit(1);
	}
	printf("port: %d\n",port);

	mask = 1;
	port = (port<7?port:7);
	mask <<= port;
	temp = outportstatus[OUTPORTA_OFFSET] & mask;
	temp = ~temp;
	temp &= (mask | outportstatus[OUTPORTA_OFFSET]);

	outportstatus[OUTPORTA_OFFSET] = temp;
	state = outportstatus[OUTPORTA_OFFSET];
	pstate = &state;
	printf("outportstatus: %x\n",outportstatus[OUTPORTA_OFFSET]);
	temp = outportstatus[OUTPORTA_OFFSET];
	printf("bits:\n");
	printf("%2x ",temp & 1);
	temp >>= 1;
	printf("%2x ",temp & 1);
	temp >>= 1;
	printf("%2x ",temp & 1);
	temp >>= 1;
	printf("%2x ",temp & 1);
	temp >>= 1;
	printf("%2x ",temp & 1);
	temp >>= 1;
	printf("%2x ",temp & 1);
	temp >>= 1;
	printf("%2x ",temp & 1);
	temp >>= 1;
	printf("%2x ",temp & 1);
	printf("\n");
	fp = open((const char *)fptr, O_RDWR);
	if(fp < 0)
	{
		strcpy(errmsg,strerror(errno));
		close(fp);
		printf("%s  %s\n",errmsg,filename);
		fp = open((const char *)fptr, O_RDWR | O_CREAT | O_TRUNC, 666);
		if(fp < 0)
		{
			strcpy(errmsg,strerror(errno));
			close(fp);
			printf("%s  %s\n",errmsg,filename);
			exit(1);
		}
	}
	write(fp,&outportstatus,NUM_PORTS);
	close(fp);

}
