// create odata.dat and idata.dat files - 1st and 2nd param are names of dat files to create
// any param as a 3rd will create the xml file
// the data put into idata.dat & odata.dat files are read from the files: idata.csv and odata.csv
// the format for the odata.dat file is:
/*
0,0,0,2,10,0,0,0,STARTER
1,0,0,4,0,0,0,0,ACCON
2,0,0,4,0,0,0,0,FUELPUMP
3,0,0,0,0,0,0,0,COOLINGFAN
...
38,0,0,0,0,0,0,0,TESTOUTPUT36
39,0,0,0,0,0,0,0,NULL
*/
// the labels can be any text string and the 1st column must be in consecutive order with a total
// of 40 starting at 0, going to 39, no more or no less
//
// the same for the idata.dat files, except:
// the 2nd column is the output port that is affected by the input (column 0)
// if any of the others are less than 41 then that is just an additional output
// port that is affected by a change of the input
// e.g. the brake lights have 2 different ports but can only be activated by one brake switch
// don't ask why I need a relay for each brake light
/*
0,0,41,41,41,41,41,41,41,41,41,STARTER
1,1,41,41,41,41,41,41,41,41,41,ACCON
2,2,41,41,41,41,41,41,41,41,41,FUELPUMP
3,3,41,41,41,41,41,41,41,41,41,COOLINGFAN
...
38,38,41,41,41,41,41,41,41,41,41,TESTINPUT36
39,38,41,41,41,41,41,41,41,41,41,NULL
*/
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
#include "../queue/illist_threads_rw.h"
#include "../queue/ollist_threads_rw.h"
#include "../ioports.h"
#include "config_file.h"

extern int oWriteConfigXML(char *filename, O_DATA *curr_o_array,size_t size,char *errmsg);
extern int iWriteConfigXML(char *filename, I_DATA *curr_i_array,size_t size,char *errmsg);
extern int iWriteConfig(char *filename, I_DATA *curr_i_array,size_t size,char *errmsg);
extern int iLoadConfig(char *filename, I_DATA *curr_i_array,size_t size,char *errmsg);
extern int oWriteConfig(char *filename, O_DATA *curr_o_array,size_t size,char *errmsg);
extern int oLoadConfig(char *filename, O_DATA *curr_o_array,size_t size,char *errmsg);
int copy_labels(char *filename, void *array, int which);

static char buf[3000];
static char label_array[40][30];
static char int_array[40][10];

/***********************************************************************************/
int main(int argc, char *argv[])
{
	I_DATA *curr_i_array;
	I_DATA *pid;
	O_DATA *curr_o_array;
	O_DATA *pod;
	int i,j,k;
	size_t isize;
	size_t osize;
	char errmsg[60];
	int ret;
	int do_xml = 0;
	char filename[30];
	char org_idata[30];
	char org_odata[30];

	if(argc < 2)
	{
		printf("usage: %s [idata filename][odata filename]\n",argv[0]);
		printf("or add 3rd param to create xml format file\n");
		return 1;
	}
	else if(argc < 3)
	{
		printf("usage: %s %s [odata filename]\n",argv[0],argv[1]);
		printf("or add 3rd param to create xml format file\n");
		return 1;
	}
	strcpy(org_idata,argv[1]);
	strcpy(org_odata,argv[2]);
	printf("%s\n",org_idata);
	printf("%s\n",org_odata);

	if(argc > 3)
	{
		printf("creating XML file\n");
		do_xml = 1;
	}

	i = NUM_PORT_BITS;
//	printf("NUM_PORT_BITS: %d\n",i);
	isize = sizeof(I_DATA);
	isize *= i;
//	printf("total size: of i_data %lu\n",isize);

//	printf("\nsizeof O_DATA: %lu\n",sizeof(O_DATA));
	i = NUM_PORT_BITS;
//	printf("NUM_PORT_BITS: %d\n",i);
	osize = sizeof(O_DATA);
	osize *= i;
//	printf("total size of o_data: %lu\n",osize);

	curr_i_array = (I_DATA *)malloc(isize);
	memset((void *)curr_i_array,0,isize);

	pid = curr_i_array;

	// inputs

	if(copy_labels(org_idata, curr_i_array, 1) != 0)
	{
		printf("error in copy_labels\n");
		exit(0);
	}
	for(i = 0;i < 40;i++)
	{
		if(int_array[i][0] != i)
		{
			printf("bad index at: %d for I_DATA\n",i);
			printf("should be %d but is %d\n",i,int_array[i][0]);
			printf("all port no. must be in consecutive order starting at zero!\n");
			return 1;
		}
	}
	for(i = 0;i < isize/sizeof(I_DATA);i++)
	{
		pid->port = i;
		pid->affected_output[0] = int_array[i][1];
		for(j = 1;j < 10;j++)
			pid->affected_output[j] = int_array[i][j+1];
		pid++;
	}
//	exit(0);
	pid = curr_i_array;
/*
	for(i = 0;i < 40;i++)
	{
//		printf("%d,%d,%s\n",pid->port,pid->affected_output[0],pid->label);
		printf("%d,",pid->port);
		for(j = 0;j < 10;j++)
			printf("%d,",pid->affected_output[j]);
		printf("%s\n",pid->label);
		pid++;
	}
	pid = curr_i_array;
*/
	printf("filename: %s\n",org_idata);

	if(do_xml == 0)
	{
		ret = iWriteConfig(org_idata,curr_i_array,isize,errmsg);
		if(ret < 0)
			printf("iWriteConfig: %s\n",errmsg);
	}else  iWriteConfigXML(org_idata,curr_i_array,isize,errmsg);

/*
	memset((void *)curr_i_array,0,isize);

	ret = iLoadConfig(fptr1,curr_i_array,isize,errmsg);
	if(ret < 0)
		printf("iLoadConfig: %s\n",errmsg);

	pid = curr_i_array;
*/
	if(do_xml == 0)
	{
		printf("%s %s has %lu records \n",errmsg,org_idata,isize/sizeof(I_DATA));
		for(i = 0;i < isize/sizeof(I_DATA);i++)
		{
			printf("%d\t%s\n",pid->port,pid->label);
			pid++;
		}
	}
	// outputs

	curr_o_array = (O_DATA *)malloc(osize);
	memset((void *)curr_o_array,0,osize);

	pod = curr_o_array;

	if(copy_labels(org_odata, curr_o_array, 0) != 0)
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
		pod->polarity = int_array[i][2];
		pod->type = int_array[i][3];
		pod->time_delay = int_array[i][4];
		pod->time_left = int_array[i][5];
		pod->pulse_time = int_array[i][6];
		pod->reset = int_array[i][7];
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
		for(i = 0;i < osize/sizeof(O_DATA);i++)
		{
			printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%s\n",pod->port,pod->onoff,pod->polarity,pod->type,
					pod->time_delay,pod->time_left, pod->pulse_time,pod->reset,pod->label);
			pod++;
		}
		printf("sizeof: %ld %ld\n",sizeof(I_DATA),sizeof(O_DATA));
	}

	free(curr_i_array);
	free(curr_o_array);
}

/***********************************************************************************/
int copy_labels(char *filename2, void *array, int which)
{
	char *fptr3;
	int i,j,k,m,n,p;
	char tempx[100];
	char tempy[10];
	char *pch;
	char *pch2;
	int fp;
	off_t fsize;
	I_DATA *curr_i_array;
	I_DATA *pid;
	O_DATA *curr_o_array;
	O_DATA *pod;
	char errmsg[60];
	char filename[30];

	if(which == 1)
	{
		curr_i_array = (I_DATA *)array;
		pid = curr_i_array;
	}
	else
	{
		curr_o_array = (O_DATA *)array;
		pod = curr_o_array;
	}

	strcpy(filename,filename2);
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
		
//	pch++;

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

	if(which == 1)
		pid = curr_i_array;
	else pod = curr_o_array;

	for(i = 0;i < 40;i++)
	{
		if(which == 1)
		{
			strcpy(pid->label,label_array[i]);
//			printf("%s\n",pid->label);
			pid++;
		}else
		{
			strcpy(pod->label,label_array[i]);
			pod++;
		}
	}
	return 0;
}
