#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/stat.h>
#include <string.h>
#include <assert.h>
#include "../../mytypes.h"

int main(void)
{
	int fp;
	char filename[20] = "waypoints.conf\0";
	char *fptr = filename;
	WAYPOINTS wp;
	UCHAR id;;
	long filesize;
	int i,j;
	int strlength;

	fp = open((const char *)fptr, O_RDWR);
	read(fp,&id,1);
	if(id != 0xAB)
	{
		printf("bad file marker at begin\r\n");
		close(fp);
		return -1;
	}
	filesize = lseek(fp, -1, SEEK_END);
	printf("filesize: %ld\n",++filesize);

	read(fp,&id,1);
	if(id != 0x54)
	{
		printf("bad file marker at end\r\n");
		close(fp);
		return -1;
	}
	lseek(fp, 1, SEEK_SET);
//	printf("seek to begin + 1: %d\r\n",i);

	j = 0;
	for(i = 0;i < filesize/sizeof(WAYPOINTS);i++)
	{
		read(fp, &wp, sizeof(WAYPOINTS));
		strlength = (int)strlen(wp.name);
		if(strlength < 8)
			printf("%s\t\t%f\t\t%f\n",wp.name, wp.latitude, wp.longitude);
		else printf("%s\t%f\t\t%f\n",wp.name, wp.latitude, wp.longitude);
		j++;
	}
	printf("no records: %d\n",j);

	close(fp);

	return 0;
}
