#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>      /* open */
#include <unistd.h>     /* exit */
#include <sys/ioctl.h>      /* ioctl */
#include <errno.h>
#include <string.h>
//#define DEVICE "/dev/ide/host0/target0/lun0/disc"
//#define DEVICE "/mnt/cf/sched.log"
#define DEVICE "sched.log"

int main(void)
{
	int file_desc, ret_val;
	int odo;
	file_desc = open(DEVICE, O_RDWR);
	if(file_desc < 0)
	{
		printf("failed to open the device \n");
		exit(1);
	}
	write(file_desc, &odo, sizeof(int));
	printf("%d %d\n",odo);
	close(file_desc);
	return 0;
}

