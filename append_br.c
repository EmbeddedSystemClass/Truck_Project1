// append a "<br />" to the end of each line
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include "AVR_t6963/main.h"
//#include "AVR_t6963/PIC_main.h"

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
	char label[10];

	if(argc < 3)
	{
		printf("usage: %s <file to open> <file to create>\n",argv[0]);
		printf("%s will append <br /> to end of each newline\n",argv[0]);
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
			printf("open: bytes read: %d filesize: %d\n",res,filesize);
			close(fp);
		}
	}
	else exit(1);
	
	j = 0;
	k = 0;

	for(i = 0;i < filesize;i++)
	{
		if(*(buff+i) == 0x0A)
			k++;
	}
	printf("no of nl's: %d \n",k);

	buff2 = (char *)malloc(filesize+(k*7));
	if(buff2 == NULL)
	{
		printf("problem with 2nd malloc\n");
		exit(1);
	}
	strcpy(label,"<br />\0");
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
		k = 0;
		j = 0;
		for(i = 0;i < filesize;i++)
		{
			*(buff2+k) = *(buff+i);
			k++;
			if(*(buff+i) == 0x0A)
			{
				if(*(buff+i-1) == 0x0D)
					k--;
				k--;
				memcpy((void*)(buff2+k),(void *)label,6);
				k += 6;
				*(buff2+k) = 0x0a;
				k++;
			}	
		}
			
		res = 0;
		res = write(fp,buff2,filesize+(k-i));
		free(buff);
		free(buff2);

		close(fp);
	}
	printf("diff: %d  bytes written: %d\n",k,res);
	return 0;
}
