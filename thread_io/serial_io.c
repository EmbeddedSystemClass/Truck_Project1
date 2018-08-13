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
#include "../mytypes.h"
#include "serial_io.h"

//#define BAUDRATE B19200
#define BAUDRATE B115200
#ifdef MAKE_SIM
#define MODEMDEVICE "/dev/ttyS0"
#else

#ifdef TS_7800
#define MODEMDEVICE "/dev/ttyS1"				  // 7800 uses ttyS1 as the 2nd serial port
#else

#define MODEMDEVICE "/dev/ttyAM0"				  // 7200 uses ttyAM0 if console disabled
#define MODEMDEVICE2 "/dev/ttyAM1"				  // 7200 uses ttyAM1 as 2nd serial port
#endif	// end of ifdef TS_7800
#endif	// end of ifdef MAKE_SIM
#define _POSIX_SOURCE 1							  /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define LEN 200

static int set_interface_attribs (int fd, int speed, int parity);
static void set_blocking (int fd, int should_block);
static struct termios oldtio;
static struct termios oldtio2;

//volatile int STOP=FALSE;

/************************************************************************************/
static int set_interface_attribs (int fd, int speed, int parity)
{
	struct termios tty;
	memset (&tty, 0, sizeof tty);
	if (tcgetattr (fd, &tty) != 0)
	{
		myprintf1("tcgetattr error\0", errno);
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
	tty.c_cc[VMIN]  = 2;						  // read doesn't block
	tty.c_cc[VTIME] = 20;						  // 0.5 seconds read timeout

	tty.c_iflag &= ~(IXON | IXOFF | IXANY);		  // shut off xon/xoff ctrl

	tty.c_cflag |= (CLOCAL | CREAD);			  // ignore modem controls,
// enable reading
	tty.c_cflag &= ~(PARENB | PARODD);			  // shut off parity
	tty.c_cflag |= parity;
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~CRTSCTS;

	if (tcsetattr (fd, TCSANOW, &tty) != 0)
	{
		myprintf1("tcgetattr error\0", errno);
		perror(" ");
		return -1;
	}
	return 0;
}

/************************************************************************************/
static void set_blocking (int fd, int should_block)
{
	struct termios tty;
	memset (&tty, 0, sizeof tty);
	if (tcgetattr (fd, &tty) != 0)
	{
		myprintf1("tcgetattr error\0", errno);
		perror(" ");
		return;
	}

	tty.c_cc[VMIN]  = should_block ? 1 : 0;
	tty.c_cc[VTIME] = 10;						  // 0.5 seconds read timeout

	if (tcsetattr (fd, TCSANOW, &tty) != 0)
		myprintf1("term error\0", errno);
}

/************************************************************************************/
int init_serial(void)
{
	char buf[LEN];
	UCHAR test = 0x21;
	global_handle = -1;

	global_handle = open (MODEMDEVICE, O_RDWR | O_NOCTTY | O_SYNC);
	if (global_handle <0)
	{
		perror(MODEMDEVICE);
		exit(-1);
	}
//	printf("global_handle = %d\nse",global_handle);

	if(tcgetattr(global_handle,&oldtio) != 0)	  /* save current port settings */
	{
		myprintf1("tcgetattr error\0", errno);
		close(global_handle);
		exit(1);
	}

	set_interface_attribs (global_handle, BAUDRATE, 0);
// if set to blocking, the serial_task won't exit properly
// and then the sched program won't quit	
//	set_blocking (global_handle, 1);	 // blocking
	set_blocking (global_handle, 0);	// non-blocking
/*
	for(i = 0;i < LEN;i++)
	{
		buf[i] = test;
		if(++test > 0x7e)
			test = 0x21;
	}
	for(i = 0;i < LEN;i++)
		write(fd,&buf[i],1);
*/
	return global_handle;
}

/************************************************************************************/
int write_serial(UCHAR byte)
{
	int res;
//	printf("fd = %d\n",global_handle);
	res = write(global_handle,&byte,1);
	return res;
}

/************************************************************************************/
void printString(const char myString[])
{
	UCHAR i = 0;
	while(myString[i])
	{
		write_serial(myString[i]);
		i++;
	}
}

/************************************************************************************/
UCHAR read_serial(char *errmsg)
{
	int res;
	UCHAR byte;
	res = read(global_handle,&byte,1);
	if(res < 0)
//		printf("\nread error: %s\n",strerror(errno));
		strcpy(errmsg,strerror(errno));
	return byte;
}

/************************************************************************************/
void close_serial(void)
{
//	printf("closing serial port\n");
	tcsetattr(global_handle,TCSANOW,&oldtio);
	close(global_handle);
}

/************************************************************************************/
int init_serial2(void)
{
	char buf[LEN];
	UCHAR test = 0x21;
	global_handle2 = -1;

// undef this section if console disabled just so it can compile with the console enabled
	global_handle2 = open (MODEMDEVICE2, O_RDWR | O_NOCTTY | O_SYNC);
	if (global_handle2 <0)
	{
		perror(MODEMDEVICE2);
		exit(-1);
	}
	if(tcgetattr(global_handle2,&oldtio2) != 0)	  /* save current port settings */
	{
		myprintf1("tcgetattr (2) error\0", errno);
		close(global_handle2);
		exit(1);
	}
	set_interface_attribs (global_handle2, BAUDRATE, 0);
//	set_blocking (global_handle2, 1);	 // blocking
	set_blocking (global_handle2, 0);	// non-blocking

	return global_handle2;
}
/************************************************************************************/
int write_serial2(UCHAR byte)
{
	int res;
	res = write(global_handle2,&byte,1);
	return res;
}

/************************************************************************************/
void printString2(char *myString)
{
	UCHAR i = 0;
//	printf("%s",myString);
//	printf("\n");
	return;
	while(myString[i])
	{
		write_serial2(myString[i]);
		i++;
	}
	write_serial2(0xfe);
}

/************************************************************************************/
UCHAR read_serial2(char *errmsg)
{
	int res;
	UCHAR byte;
	res = read(global_handle2,&byte,1);
	if(res < 0)
//		printf("\nread error: %s\n",strerror(errno));
		strcpy(errmsg,strerror(errno));
	return byte;
}

/************************************************************************************/
void close_serial2(void)
{
//	printf("closing serial port\n");
	tcsetattr(global_handle2,TCSANOW,&oldtio2);
	close(global_handle2);
}

