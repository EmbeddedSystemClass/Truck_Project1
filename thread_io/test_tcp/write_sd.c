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
	int odo;
	file_desc = open(DEVICE, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

	if(file_desc < 0)
	{
		printf("failed to open the device \n");
		exit(1);
	}
	odo = 10;
	write(file_desc, &odo, sizeof(int));
	printf("odometer: %d\n",odo);
	close(file_desc);
	return 0;
}

