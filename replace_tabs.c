// replace_tabs.c - replace all occurances of tabs with 4 spaces and also insert 4 extra spaces at
// the beginning of each line - used to format source code before posting to stackoverflow.com
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef unsigned char UCHAR;
typedef unsigned int UINT;
typedef unsigned char uint8_t;


int main(int argc, char *argv[])
{
	int i,j,k;
	int fp;
	int res;
	char filename[20];
	int filesize;
	char *buff;
	char *buff2;
	int no_nl;
	int no_tabs;
	char space_ch = 0x20;
	char *psp_ch = &space_ch;

	if(argc < 3)
	{
		printf("usage: %s <file to open> <file to create>\n",argv[0]);
		printf("%s will insert 4 spaces for each tab\n",argv[0]);
		exit(1);
	}

	strcpy(filename,argv[1]);
	if(access(filename,F_OK) != -1)
	{
		fp = open((const char *)filename, O_RDWR);
		if(fp < 0)
		{
			printf("can't open file for writing\n");
		}else
		{
			res = 0;
			lseek(fp,0,SEEK_SET);
			filesize = lseek(fp,0,SEEK_END);
			lseek(fp,0,SEEK_SET);
			buff = (char *)malloc(filesize);
			if(buff == NULL)
			{
				close(fp);
				printf("problem with malloc\n");
				exit(1);
			}
			res = read(fp,buff,filesize);
//			printf("open: bytes read: %d filesize: %d\n",res,filesize);
			close(fp);
		}
	}
	else exit(1);
	
	j = 0;
	k = 0;

    no_nl = 0;
	for(i = 0;i < filesize;i++)
	{
		if(*(buff+i) == 0x0A)
			no_nl++;
	}
//	printf("no of nl's: %d \n",no_nl);
    
    no_tabs = 0;
	for(i = 0;i < filesize;i++)
	{
		if(*(buff+i) == 0x09)
			no_tabs++;
	}
//	printf("no of tabs: %d\n",no_tabs);

	buff2 = (char *)malloc(filesize+(no_tabs*4)+(no_nl*4));

	if(buff2 == NULL)
	{
		printf("problem with 2nd malloc\n");
		exit(1);
	}
	strcpy(filename,argv[2]);

	fp = open((const char *)filename,
		 O_RDWR | O_CREAT | O_TRUNC,
		 S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	if(fp < 0)
	{
		printf("can't open file for writing\n");
		exit(1);
	}

	if(1)
	{
		res = 0;
		lseek(fp,0,SEEK_SET);
		j = 0;
		for(j = 0;j < 4;j++)
		    *(buff2 + j) = 0x20;
		k = 4;

		for(i = 0;i < filesize;i++)
		{
			if(*(buff+i) == 0x0A || *(buff+i) == 0x0D)
			{
				*(buff2 + k++) = 0x0A;
				for(j = 0;j < 4;j++)
					*(buff2 + k++) = 0x20;
			}
			else if(*(buff+i) == 0x09)
			{
				for(j = 0;j < 4;j++)
					memcpy((void*)(buff2+k++),(void*)psp_ch,1);
			}
			else
			{
				memcpy((void*)(buff2+k++),(void *)(buff+i),1);
			}
		}
			
		res = 0;
		res = write(fp,buff2,filesize+(k-i));
		free(buff);
		free(buff2);

		close(fp);
	}
//	printf("diff: %d  bytes written: %d\n",k,res);
	return 0;
}
