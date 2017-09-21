
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/time.h>

//#define BAUDRATE B9600
#define BAUDRATE B19200
//#define BAUDRATE B115200
#define MODEMDEVICE "/dev/ttyS0"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define LEN 20
#define BUF2_LEN 200
#define TIME_DELAY 1000
#define READ_LEN 16

typedef unsigned char UCHAR;
typedef unsigned int UINT;
typedef unsigned char uint8_t;

int set_interface_attribs (int fd, int speed, int parity);
void set_blocking (int fd, int should_block);
int parse_sync(int fd);
void read_get_key(UCHAR key);

int syncup;

UCHAR sync_buf[10];
UCHAR buf2[BUF2_LEN];
int buf2index;

int set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                printf ("error %d from tcgetattr", errno);
				perror(" ");
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                printf ("error %d from tcsetattr", errno);
				perror(" ");
                return -1;
        }
        return 0;
}

void set_blocking (int fd, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                printf ("error %d from tggetattr", errno);
                perror(" ");
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 50;            // 0.5 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
                printf ("error %d setting term attributes", errno);
}


int main(void)
{
	int fd,c, res, res2, size, fp;
	int i,j,k;
	int done;
	unsigned char ch;
	struct termios oldtio,newtio;
	unsigned char buf[LEN];
	char filename[20];
	memset(&newtio, 0, sizeof newtio);
	syncup = 0;

	memset(sync_buf,0,sizeof(sync_buf));
	sync_buf[0] = 0x55;
	sync_buf[1] = 0x55;
	sync_buf[2] = 0x55;
	sync_buf[3] = 0xAA;
	sync_buf[4] = 0xAA;
	sync_buf[5] = 0xAA;

	fd = open (MODEMDEVICE, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd <0) {perror(MODEMDEVICE);
		exit(-1); }


	if(tcgetattr(fd,&oldtio) != 0) /* save current port settings */
	{
		printf("Error from tcgetattr: %d ",errno);
		close(fd);
		exit(1);
	}

	set_interface_attribs (fd, BAUDRATE, 0);
	set_blocking (fd, 1);

	ch = '*';

	for(j = 0;j < 93;j++)
		printf("*");
	printf("\n");

	done = 0;

	j = 0;
	k = 0;
	buf2index = 0;
	while(1)
	{
		if(parse_sync(fd))
		{
			if(!done)
				printf("got sequence\n");
			done = 1;
		}
	}
	ch = 0xff;
	write(fd,(void *)&ch,1);
	tcsetattr(fd,TCSANOW,&oldtio);
	close(fd);
	printf("\ndone\n");
	exit(0);
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
// read from serial port and if a sequence of 3 0x55's and 3 0xAA's are sent
// then set the syncup flag, otherwise call read_get_key with the other chars
int parse_sync(int fd)
{
	int i,j,got_seq;
	UCHAR buf[8];
	int res = 0;

	i = 0;
	got_seq = 1;

	do
	{
		res += read(fd,&buf[i],1);
		usleep(TIME_DELAY);
//		printf("%2x  %2d \n",buf[i],i);
		if(buf[i] != sync_buf[i])
			got_seq = 0;
		i++;

	}while(got_seq && i < 6);
	j = i;

	if(got_seq)
	{
//		printf("\ngot sync\n");
		syncup = 1;
		return 1;
	}
	else
	{
		for(i = 0;i < j;i++)
		{
			read_get_key(buf[i]);
		}
		return 0;
	}
	printf("\n");
}
void read_get_key(UCHAR key)
{
	int i = 0;

	buf2[buf2index] = key;
	if(++buf2index > BUF2_LEN-1)
	{
		for(i = 0;i < BUF2_LEN-1;i++)
			printf("%2x ",buf2[i]);
		printf("\n\n");
		buf2index = 0;
	}
}


