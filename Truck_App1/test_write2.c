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

//#define BAUDRATE B19200
#define BAUDRATE B115200
#define MODEMDEVICE "/dev/ttyS0"
#define _POSIX_SOURCE 1							  /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define LEN 10
#define TIME_DELAY 10
#define TIME_DELAY2 TIME_DELAY*10

int set_interface_attribs (int fd, int speed, int parity);
void set_blocking (int fd, int should_block);

//volatile int STOP=FALSE;

/******************************************************************/
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

	tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;	  // 8-bit chars
// disable IGNBRK for mismatched speed tests; otherwise receive break
// as \000 chars
	tty.c_iflag &= ~IGNBRK;						  // disable break processing
	tty.c_lflag = 0;							  // no signaling chars, no echo,
// no canonical processing
	tty.c_oflag = 0;							  // no remapping, no delays
	tty.c_cc[VMIN]  = 0;						  // read doesn't block
	tty.c_cc[VTIME] = 5;						  // 0.5 seconds read timeout

	tty.c_iflag &= ~(IXON | IXOFF | IXANY);		  // shut off xon/xoff ctrl

	tty.c_cflag |= (CLOCAL | CREAD);			  // ignore modem controls,
// enable reading
	tty.c_cflag &= ~(PARENB | PARODD);			  // shut off parity
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

/******************************************************************/
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
	tty.c_cc[VTIME] = 5;						  // 0.5 seconds read timeout

	if (tcsetattr (fd, TCSANOW, &tty) != 0)
		printf ("error %d setting term attributes", errno);
}

/******************************************************************/
int send_frame(int fd)
{
	unsigned char ch = 0xAA;
	write(fd,&ch,1);
	usleep(TIME_DELAY);
	ch = 0x55;
	write(fd,&ch,1);
	usleep(TIME_DELAY);
	return 1;
}

/******************************************************************/
int main(void)
{
	int fd,c, res, res2;
	int i,j;
	unsigned char ch;
	struct termios oldtio,newtio;
	memset(&newtio, 0, sizeof newtio);

	fd = open (MODEMDEVICE, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd <0)
	{
		perror(MODEMDEVICE);
		exit(-1);
	}

	if(tcgetattr(fd,&oldtio) != 0)				  /* save current port settings */
	{
		printf("Error from tcgetattr: %d ",errno);
		close(fd);
		exit(1);
	}

	set_interface_attribs (fd, BAUDRATE, 0);
	set_blocking (fd, 1);

	res2 = 0;
	ch = 0x20;
	for(j = 0;j < 60;j++)
	{
	for(i = 0;i < 16;i++)
	{
		ch = (unsigned char)i;
		res2 = write(fd,(void*)&ch,1);
		usleep(TIME_DELAY);

		res = read(fd,&ch,1);
		printf("%2x ",ch);
		usleep(TIME_DELAY);
	}
	printf("\n");
	usleep(TIME_DELAY2);
	}
	tcsetattr(fd,TCSANOW,&oldtio);
	close(fd);
	printf("done\n");
	exit(0);
}
