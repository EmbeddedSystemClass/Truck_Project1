#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/stat.h>
#include <assert.h>
#include <string.h>
//#define DEVICE "/dev/ide/host0/target0/lun0/disc"
//#define DEVICE "/mnt/cf/sched.log"
#define DEVICE "sched.log"

int main(void)
{
	int file_desc, ret_val;
	unsigned char buf[200];
	long file_len;
	int i;

	file_desc = open(DEVICE, O_RDWR);
	if(file_desc < 0)
	{
		printf("failed to open the device \n");
		exit(1);
	}
	file_len = lseek(file_desc,0,SEEK_END);
	printf("file length: %d\n",file_len);
	lseek(file_desc,1,SEEK_SET);
	for(i = 0;i < file_len/5;i++)
	{
		read(file_desc, &buf, 5);
		printf("%02d %02d %02d:%02d:%02d\n",buf[0],buf[1],buf[2],buf[3],buf[4]);
	}
	close(file_desc);
	return 0;
}

