// create odata.dat file - 1st and 2nd param are names of dat files to create
// any param as a 3rd will create the xml file (2nd param should be <filename>.xml)
// the data put into odata.dat file is read from the files: odata.csv
// the format for the odata.dat file is:
/*
typedef struct o_data (from 
{
	char label[OLABELSIZE];
	UCHAR port;
	UCHAR onoff;			// current state: 1 if on; 0 if off
	UCHAR input_port;		// input port which affects this output (if not set to 0xFF)
	UCHAR polarity;			// 0 - on input turns output on; off input turns output off
							// 1 - on input turns output off; off input turns output on
	UCHAR type;				// see below
	UINT time_delay;		// when type 2-4 this is used as the time delay
	UINT time_left;			// gets set to time_delay and then counts down
	UCHAR pulse_time;		// not used
	UCHAR reset;			// used to make 2nd pass
} O_DATA;

0,0,5,0,2,10,0,0,0,STARTER
1,0,7,0,4,0,0,0,0,ACCON
2,0,7,0,4,0,0,0,0,FUELPUMP
3,0,6,0,0,0,0,0,0,COOLINGFAN
4,0,0,0,0,0,0,0,0,LHEADLAMP
...
36,0,-1,0,0,0,0,0,0,TESTOUTPUT34
37,0,-1,0,0,0,0,0,0,TESTOUTPUT35
38,0,-1,0,0,0,0,0,0,TESTOUTPUT36
39,0,-1,0,0,0,0,0,0,NULL
(see mytypes.h for input/output defines)

the labels can be any text string and the 1st column must be in consecutive order with a total
 of 40 starting at 0, going to 39, no more or no less
*/

// TODO: if no csv file exists then just create a bare-bones dat file

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <assert.h>
#include <ctype.h>
#include "../queue/ollist_threads_rw.h"
#include "../ioports.h"
#include "config_file.h"

extern int oWriteConfigXML(char *filename, O_DATA *curr_o_array,size_t size,char *errmsg);
extern int oWriteConfig(char *filename, O_DATA *curr_o_array,size_t size,char *errmsg);
extern int oLoadConfig(char *filename, O_DATA *curr_o_array,size_t size,char *errmsg);
int copy_labels(char *filename, O_DATA *curr_o_data);

static char buf[3000];
static char label_array[40][30];
static char int_array[40][10];

/***********************************************************************************/
int main(int argc, char *argv[])
{
	O_DATA *curr_o_array;
	O_DATA *pod;
	int i,j,k;
	size_t osize;
	char errmsg[60];
	int ret;
	int do_xml = 0;
	char filename[30];
	char org_odata[30];

	if(argc < 2)
	{
		printf("usage: %s[odata filename]\n",argv[0]);
		printf("or add 2nd param to create xml format file\n");
		return 1;
	}
	strcpy(org_odata,argv[1]);
	printf("%s\n",org_odata);

	if(argc > 2)
	{
		printf("creating XML file\n");
		do_xml = 1;
	}

	printf("\nsizeof O_DATA: %lu\n",sizeof(O_DATA));
	i = NUM_PORT_BITS;
	printf("NUM_PORT_BITS: %d\n",i);
	osize = sizeof(O_DATA);
	osize *= i;
	printf("total size of o_data: %lu\n",osize);
	curr_o_array = (O_DATA *)malloc(osize);
	memset((void *)curr_o_array,0,osize);

	pod = curr_o_array;

	if(copy_labels(org_odata, curr_o_array) != 0)
	{
		printf("error in copy_labels\n");
		exit(0);
	}
	for(i = 0;i < 40;i++)
	{
		if(int_array[i][0] != i)
		{
			printf("bad index at: %d for O_DATA\n",i);
			printf("should be %d but is %d\n",i,int_array[i][0]);
			printf("all port no. must be in consecutive order starting at zero!\n");
			return 1;
		}
	}

	pod = curr_o_array;
	for(i = 0;i < osize/sizeof(O_DATA);i++)
	{
		pod->port = i;
		pod->onoff = int_array[i][1];
		pod->input_port = int_array[i][2];
		pod->polarity = int_array[i][3];
		pod->type = int_array[i][4];
		pod->time_delay = int_array[i][5];
		pod->time_left = int_array[i][6];
		pod->pulse_time = int_array[i][7];
		pod->reset = int_array[i][8];
		pod++;
	}

	if(do_xml == 1)
		ret = oWriteConfigXML(org_odata,curr_o_array,osize,errmsg);
	else
		ret = oWriteConfig(org_odata,curr_o_array,osize,errmsg);
	if(ret < 0)
		printf("oWriteConfig: %s\n",errmsg);
	else printf("%s %s has %lu records \n",errmsg,org_odata,osize/sizeof(O_DATA));

	pod = curr_o_array;
	if(do_xml == 0)
	{
//		printf("port\tonoff\tinput_port\tpolarity\ttype\ttime_delay\ttime_left\tpulse_time\treset\tlabel\n\n");
		for(i = 0;i < osize/sizeof(O_DATA);i++)
		{
			printf("%d %d %d %d %d %d %d %d %d %s\n",pod->port,pod->onoff,pod->input_port, 
					pod->polarity,pod->type,pod->time_delay,pod->time_left, 
					pod->pulse_time,pod->reset,pod->label);
			pod++;
		}
		printf("sizeof: %ld\n",sizeof(O_DATA));
	}
	free(curr_o_array);
}

/***********************************************************************************/
int copy_labels(char *filename2, O_DATA *curr_o_data)
{
	char *fptr3;
	int i,j,k,m,n,p;
	char tempx[100];
	char tempy[10];
	char *pch;
	char *pch2;
	int fp;
	off_t fsize;
	O_DATA *pod;
	char errmsg[60];
	char filename[30];

	strcpy(filename,filename2);
	printf("filename: %s\n",filename);
	pch = &filename[0];
	i = 0;
	while(*pch != '.' && i < 30)
	{
		i++;
		printf("%c",*pch);
		pch++;
	}
	if(i > 25)
	{
		printf("filename doesn't have dot ext\n");
		return 1;
	}
	*pch = 0;
	strcat(filename,".csv\0");
	printf("\ncsv filename: %s\n",filename);

	fptr3 = filename;
	fp = open((const char *)fptr3, O_RDWR);
	if(fp < 0)
	{
		strcpy(errmsg,strerror(errno));
		close(fp);
		printf("%s  %s\n",errmsg,filename);
		printf("no csv file found!\n");
		return 1;
	}
	fsize = lseek(fp,0,SEEK_END);
	printf("fsize: %lu\n",fsize);
	
	i = lseek(fp,0,SEEK_SET);
	i = read(fp,(void*)&buf[0],fsize);
	close(fp);
	j = 0;
	k = 0;
	i = 0;
	pch = &buf[0];
	// search for 1st nl because the csv files have 1 nl at beginning
	while(*pch != '\n' && i < 2000)
	{
		pch++;
		j++;
	}
		
	memset(int_array,0,sizeof(int_array));
	pch2 = pch;
	
	memset(label_array,0,sizeof(label_array));
	k = 0;
	for(j = 0;j < 40;j++)
	{
		m = 0;
		n = 0;
		p = 0;
		memset(tempx,0,sizeof(tempx));
		pch2 = pch;
		while(!isalpha(*pch))
		{
//			printf("%c",*pch);
			pch++;
			k++;
			tempx[m++] = *pch;
			n++;
			if(*pch == ',')
			{
				memset(tempy,0,sizeof(tempy));
				memcpy(tempy,(void*)(pch-n+1),n-1);
//				printf("%s ",tempy);
				int_array[j][p++] = atoi(tempy);
				n = 0;
				pch2 = pch;
			}
//			printf("\n");
		}
		tempx[m-1] = 0;
//		printf("%s ",tempx);
//		for(p = 0;p < 9;p++)
//			printf("%d ",int_array[j][p]);
//		printf("\n\n");
		pch2 = pch;
		i = 0;
		while(*pch2 != '\n' && k < fsize)
		{
			i++;
			pch2++;
			k++;
		}
		strncpy(&label_array[j][0],pch,i);
//		printf("%s\n",&label_array[j][0]);
		pch = pch2;
	}
//	printf("fp:%d  read: %d bytes in oLoadConfig\n",fp,i);

	pod = curr_o_data;

	for(i = 0;i < 40;i++)
	{
		strcpy(pod->label,label_array[i]);
		pod++;
	}
	return 0;
}
