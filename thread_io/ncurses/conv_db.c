// conv_db - first run list_db with extra param at end - can be anything: 
// ./list_db idata.dat odata.dat 1 > list_db.txt
// then edit the list_db.txt to 2 sep files: idatat.csv and odata.csv 
// cut out all the extra garbage except the records with comma delimiters
// you can also pull these files into a spreadsheet
// but leave and extra line at the top
// you can edit the values in the text file instead of using the clucky ncurses editor
// then run ./conv_db idata.dat odata.dat
// this will create the new dat files from the csv files

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
	size_t isize;
	size_t osize;
	char *fptr1;
	char *fptr2;
	char fptr1_org[20];
	char fptr2_org[20];
	char errmsg[60];
	int ret;
	char *fptr;
	char dat[5] = "dat\0";
	char csv[5] = "csv\0";
    FILE* fd = NULL;
    char buff[2000];
    char records[41][50];
    char *ch, *ch2;
	int j, k,l;
	int i;
    size_t fsize;
	size_t size;
	char temp[40];
	char cp;
	
	if(argc < 2)
	{
		printf("%s takes the idata.csv and the odata.csv files and converts them\n",argv[0]);
		printf("to idata.dat and odata.dat files\n");
		printf("usage: %s [idata filename][odata filename]\n",argv[0]);
		return 1;
	}
	else if(argc < 3)
	{
		printf("%s takes the idata.csv and the odata.csv files and converts them\n",argv[0]);
		printf("to idata.dat and odata.dat files\n");
		printf("usage: %s %s [odata filename]\n",argv[0],argv[1]);
		return 1;
	}
	fptr1 = argv[1];
	strcpy((char *)fptr1_org,fptr1);
	fptr = fptr1;
	fptr += strlen(fptr1);
	while(*fptr != '.')
		fptr--;
	fptr++;
	*fptr = 0;
	strcat(fptr1,csv);

	fptr2 = argv[2];
	strcpy((char *)fptr2_org,fptr2);
	fptr = fptr2;
	fptr += strlen(fptr1);
	while(*fptr != '.')
		fptr--;
	fptr++;
	*fptr = 0;
	strcat(fptr2,csv);

/*
	printf("opening csv file: %s\n",fptr1);
	printf("opening csv file: %s\n",fptr2);

	printf("creating idata file: %s\n",fptr1);
	printf("creating odata file: %s\n",fptr2);

	printf("\nsizeof I_DATA: %lu\n",sizeof(I_DATA));
*/
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
	
//    fd = fopen("idata.csv","rw+");
    fd = fopen(fptr1,"rw+");

    if(NULL == fd)
    {
        printf("\n fopen() Error!!!\n");
        return 1;
    }

	fseek(fd,0,SEEK_END);
	fsize = ftell(fd);
	fseek(fd,0,SEEK_SET);
	memset(buff,0,sizeof(buff));
	memset(records,0,41*50);
    size = fread((void*)&buff[0],1,1500,fd);
	fclose(fd);
    ch = buff;
    size = 0;
    i = j = k = 0;

    do{
    	size++;
    	j++;
    	k++;
    	if(*ch == '\n')
    	{
			ch--;
	    	fsize = (size_t)j;
	    	ch -= j - 1;
    		memcpy((void*)records[i],(const void*)ch,fsize);
	   		ch += j;
    		j = 0;
    		i++;
    		ch++;
    	}
    	ch++;
    }while(size < 1300);

	cp = ',';
    for(i = 1;i < 41;i++)
    {
    	ch = records[i];
    	do{
    		if(*ch == cp)
    		{
    			*ch = 0;
    		}
    	}while(*(ch++) != 'n');
    }
//    	printf("%s\n",records[i]);

	for(i = 1;i < isize/sizeof(I_DATA)+1;i++)
	{
		ch = records[i];
		ch2 = ch;
		memset(temp,0,sizeof(temp));

		strcpy(temp,ch);
//		printf("%s \n",temp);

		pid->port = atoi(ch);

		do{
			ch++;
		}while(*ch != 0);
		ch++;

		strcpy(temp,ch);
//		printf("%s \n",temp);

		pid->affected_output = atoi(ch);

		strcpy(temp,ch);
//		printf("%s \n",temp);
		pid->port = atoi(ch);
		do{
			ch++;
		}while(*ch != '\n' && *ch != 0);
		ch++;

		strcpy(temp,ch);
//		printf("%s \n",temp);

		strcpy(pid->label,ch);

		pid++;
	}

	pid = curr_i_array;
	for(i = 0;i < isize/sizeof(I_DATA);i++)
	{
		printf("%d %d\t%s\n",pid->port,pid->affected_output,pid->label);
		pid++;
	}

	pid = curr_i_array;

	ret = iWriteConfig(fptr1_org,curr_i_array,isize,errmsg);
	if(ret < 0)
		printf("iWriteConfig: %s\n",errmsg);

/*
	memset((void *)curr_i_array,0,isize);

	ret = iLoadConfig(fptr1,curr_i_array,isize,errmsg);
	if(ret < 0)
		printf("iLoadConfig: %s\n",errmsg);

	pid = curr_i_array;
*/
//	printf("%s %s has %lu records \n",errmsg,fptr1,isize/sizeof(I_DATA));
/*
	for(i = 0;i < isize/sizeof(I_DATA);i++)
	{
		printf("%d\t%s\n",pid->port,pid->label);
		pid++;
	}
*/

	// outputs


	curr_o_array = (O_DATA *)malloc(osize);
	memset((void *)curr_o_array,0,osize);

	pod = curr_o_array;

    fd = fopen(fptr2,"rw+");

    if(NULL == fd)
    {
        printf("\n fopen() Error!!!\n");
        return 1;
    }

	fseek(fd,0,SEEK_END);
	fsize = ftell(fd);
	fseek(fd,0,SEEK_SET);
	memset(buff,0,sizeof(buff));
	memset(records,0,41*50);
    size = fread((void*)&buff[0],1,1500,fd);
	fclose(fd);
    ch = buff;
    size = 0;
    i = j = k = 0;

    do{
    	size++;
    	j++;
    	k++;
    	if(*ch == '\n')
    	{
			ch--;
	    	fsize = (size_t)j;
	    	ch -= j - 1;
    		memcpy((void*)records[i],(const void*)ch,fsize);
	   		ch += j;
    		j = 0;
    		i++;
    		ch++;
    	}
    	ch++;
    }while(size < 1200);

//    for(i = 1;i < 41;i++)
//    	printf("%s\n",records[i]);

/*
	UCHAR port;
	UCHAR onoff;			// current state: 1 of on; 0 if off
	UCHAR polarity;			// 0 - on input turns output on; off input turns output off
							// 1 - on input turns output off; off input turns output on
							// but if type is 1 then this tells whether to reset or not
	UCHAR type;				// see below
	UINT time_delay;
	UCHAR pulse_time;
	UCHAR reset;
	char label[OLABELSIZE];
*/

    for(i = 1;i < 41;i++)
    {
    	ch = records[i];
    	do{
    		if(*ch == cp)
    		{
    			*ch = 0;
    		}
    	}while(*(ch++) != 'n');
    }

	for(i = 1;i < osize/sizeof(O_DATA)+1;i++)
	{
		ch = records[i];
		ch2 = ch;
		memset(temp,0,sizeof(temp));

		strcpy(temp,ch);
//		printf("%s \n",temp);

		pod->port = atoi(ch);

		do{
			ch++;
		}while(*ch != 0);
		ch++;

		strcpy(temp,ch);
//		printf("%s \n",temp);

		pod->onoff = atoi(ch);

		do{
			ch++;
		}while(*ch != 0);
		ch++;

		strcpy(temp,ch);
//		printf("%s \n",temp);

		pod->polarity = atoi(ch);

		do{
			ch++;
		}while(*ch != 0);
		ch++;

		strcpy(temp,ch);
//		printf("%s \n",temp);

		pod->type = atoi(ch);

		do{
			ch++;
		}while(*ch != 0);
		ch++;

		strcpy(temp,ch);
//		printf("%s \n",temp);

		pod->time_delay = atoi(ch);

		do{
			ch++;
		}while(*ch != 0);
		ch++;

		strcpy(temp,ch);
//		printf("%s \n",temp);

		pod->time_left = atoi(ch);

		do{
			ch++;
		}while(*ch != 0);
		ch++;

		strcpy(temp,ch);
//		printf("%s \n",temp);

		pod->pulse_time = atoi(ch);

		do{
			ch++;
		}while(*ch != 0);
		ch++;

		strcpy(temp,ch);
//		printf("%s \n",temp);

		pod->reset = atoi(ch);

		do{
			ch++;
		}while(*ch != 0 && *ch != '\n');
		ch++;

		strcpy(temp,ch);
//		printf("%s\n",temp);

		strcpy(pod->label,temp);

		pod++;
	}

	pod = curr_o_array;

	ret = oWriteConfig(fptr2_org,curr_o_array,osize,errmsg);
	if(ret < 0)
		printf("oWriteConfig: %s\n",errmsg);
	else printf("%s %s has %lu records \n",errmsg,fptr2,osize/sizeof(O_DATA));

	pod = curr_o_array;

	for(i = 0;i < osize/sizeof(O_DATA);i++)
	{
		printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%s\n",pod->port,pod->onoff,pod->polarity,pod->type,
				pod->time_delay,pod->time_left, pod->pulse_time,pod->reset,pod->label);
		pod++;
	}

//	printf("sizeof: %ld %ld\n",sizeof(I_DATA),sizeof(O_DATA));

	free(curr_i_array);
	free(curr_o_array);
}

