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
#include "mytypes.h"
/**********************************************************************************************************/

int main(int argc, char **argv)
{
	int key;
	int key2;
	size_t mem_pagesize;
	size_t card_pagesize;
	int i, j;
	char filename[20];
	char scriptfilename[20];
	int fp, fp2;
	char *fptr;
	char errmsg[30];
	char script[200];
	int scr_ptr;

	if(argc > 1)
	{
		strcpy(scriptfilename,argv[1]);

		fptr = scriptfilename;
		fp2 = open((const char *)fptr, O_RDONLY);
		if(fp2 < 0)
		{
			strcpy(errmsg,strerror(errno));
			close(fp2);
			printf("%s  %s\n",errmsg,filename);
			fp2 = open((const char *)fptr, O_RDWR | O_CREAT | O_TRUNC, 666);
			printf("created new file: %s\n",fptr);
			exit(1);
		}
		lseek(fp2,0,SEEK_SET);
		j = lseek(fp2,0,SEEK_END);
		printf("\nscript file length: %d\n",j);
#if 0
			scr_ptr = malloc(j);
			if(scr_ptr == NULL)
			{
				printf("problem with malloc\n");
				exit(1);
			}
#endif
		lseek(fp2,0,SEEK_SET);
		if(fp2 < 0)
		{
			strcpy(errmsg,strerror(errno));
			close(fp2);
			printf("%s  %s\n",errmsg,filename);
			exit(1);
		}
		read(fp2,&script[0],j);
		close(fp2);
		for(i = 0;i < j;i++)
			printf("%c ",script[i]);
	}
	printf("\ndone\n");
	return 1;
}
