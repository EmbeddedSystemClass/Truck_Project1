
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
#define LEN 10
#define TIME_DELAY 2000
#define READ_LEN 16

typedef unsigned char UCHAR;
typedef unsigned int UINT;
typedef unsigned char uint8_t;

int set_interface_attribs (int fd, int speed, int parity);
void set_blocking (int fd, int should_block);

//volatile int STOP=FALSE; 

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
	unsigned char ch;
	struct termios oldtio,newtio;
	unsigned char buf[LEN] =
	{0x55, 0x55, 0x55, 0xAA, 0xAA, 0xAA, 0,0,0,0};

	char filename[20];


	memset(&newtio, 0, sizeof newtio);

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

	res = 0;
	res2 = 0;
	k = 0;
	ch = '*';
	j = 0;
	for(j = 0;j < 93;j++)
		printf("*");
	printf("\n");

	while(1)
	{
		ch = 0xFF;
		res2 = 0;

		for(i = 0;i < 10;i++)
		{

			res2 += write(fd,&ch,1);
			usleep(TIME_DELAY);
//			printf("inc: %d \n",ch);
			ch--;
		}
		printf("res2: %d ",res2);

		res = 0;

		res2 = 0;

		// try writing almost the entire sequence
		for(i = 0;i < 4;i++)
		{
			write(fd,&buf[i],1);
			usleep(TIME_DELAY);
		}

		for(i = 0;i < 10;i++)
		{
			write(fd,&i,1);
			usleep(TIME_DELAY);
		}

		for(i = 0;i < 6;i++)
		{
			res2 += write(fd,&buf[i],1);
			usleep(TIME_DELAY);
//			printf("frame: %x \n",buf[i]);
		}
		printf("res2: %d ",res2);

		ch = 20;
		for(i = 0;i < 20;i++)
		{
			res2 += write(fd,&ch,1);
			usleep(TIME_DELAY);
			ch++;
		}
		printf("\n");
	}
	ch = 0xff;
	write(fd,(void *)&ch,1);
	tcsetattr(fd,TCSANOW,&oldtio);
	close(fd);
	printf("\ndone\n");
	exit(0);
}


