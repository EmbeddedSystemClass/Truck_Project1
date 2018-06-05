// for some reason minicom doesn't write on this box (192.168.42.110)
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
#ifdef MAKE_TARGET
#define MODEMDEVICE "/dev/ttyAM1"
#warning "MAKE_TARGET defined......."
#else
#define MODEMDEVICE "/dev/ttyS0"
#endif
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define LEN 100
#define TIME_DELAY 5000
#define SEND_CHAR_CMD 0x81

#define OFF_CMD 0
#define SET_BRIGHTNESS_CMD			0x82
#define SET_CDECIMAL_CMD			0x83
#define SET_UPDATE_RATE_CMD			0x84
#define SET_FACTOR_CMD				0x85
#define SET_DISPLAY_UPDATE_RATE 	0x86
#define DTMF_TONE_ON				0x87	
#define DTMF_TONE_OFF				0x88
#define SPECIAL_TONE_ON				0x89
#define LCD_PWM						0x8A
//	- 	state mparam 0->1: 00 - off; 01 - all on; 10 - pwm using duty cycle
//				duty_cycle: param bit 2->4 (sets duty cycle)
#define LCD_PWM_OFF 0x00 // off
#define LCD_PWM_ON 0x01 // on
#define LCD_PWM_80PCT 0x1A // duty_cycle = 80%
#define LCD_PWM_75PCT 0x16 // duty_cycle = 75%
#define LCD_PWM_60PCT 0x12 // duty_cycle = 60%
#define LCD_PWM_50PCT 0x0E // duty_cycle = square wave
#define LCD_PWM_30PCT 0x0A // duty_cycle = 30%
#define LCD_PWM_25PCT 0x06 // duty_cycle = 25%
#define LCD_PWM_12PCT 0x02 // duty_cycle = 12%

typedef unsigned char UCHAR;
typedef unsigned int UINT;

void fpga_cmd(int fd,UCHAR cmd, UCHAR param);

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
	int fd,c, res, res2;
	int i,j,k,x;
	UCHAR ch;
	UCHAR ch2;
	struct termios oldtio,newtio;
	UCHAR buf[LEN];
	UCHAR dtmf_index;
	UCHAR dtmf_len;
	UCHAR my_number[11];
	memset(&newtio, 0, sizeof newtio);

	fd = open (MODEMDEVICE, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd <0) {perror(MODEMDEVICE);
		exit(-1); }

#ifdef MAKE_TARGET
	printf("\nopened ttyAM1\n");
#else
	printf("\nopened ttyS0\n");
#endif

	if(tcgetattr(fd,&oldtio) != 0) /* save current port settings */
	{
		printf("Error from tcgetattr: %d ",errno);
		close(fd);
		exit(1);
	}

	set_interface_attribs (fd, BAUDRATE, 0);
	set_blocking (fd, 1);
	ch = '*';

	res2 = 0;
	ch = 14;
	j = 0;
	i = 0;
	x = 0;
	dtmf_index = 0;
	ch = 0;

	my_number[0] = 3;
	my_number[1] = 0;
	my_number[2] = 7;
	my_number[3] = 2;
	my_number[4] = 5;
	my_number[5] = 7;
	my_number[6] = 9;
	my_number[7] = 6;
	my_number[8] = 8;
	my_number[9] = 4;
	my_number[10] = 0;

	fpga_cmd(fd,SPECIAL_TONE_ON,0);
	usleep(TIME_DELAY*100);

	fpga_cmd(fd,DTMF_TONE_OFF,0);
	usleep(TIME_DELAY*20);

	while(1)
	{
		dtmf_index = my_number[x];
		printf("%d: index: %2x\n",i++,dtmf_index);

		fpga_cmd(fd,DTMF_TONE_ON,dtmf_index);
		usleep(TIME_DELAY*7);

		if(++x > 10)
			x = 0;

		fpga_cmd(fd,DTMF_TONE_OFF,0);
		usleep(TIME_DELAY*5);

		if(++j > 10)
		{
			usleep(TIME_DELAY*300);
			j = 0;
			printf("\n");
		}else usleep(TIME_DELAY*5);

		if(i > 60)
		{
			for(k = 0;k < 2;k++)
			{
				fpga_cmd(fd,SPECIAL_TONE_ON,0);
				usleep(TIME_DELAY*200);

				fpga_cmd(fd,DTMF_TONE_OFF,0);
				usleep(TIME_DELAY*10);
			}
			i = 0;
		}

	}
	tcsetattr(fd,TCSANOW,&oldtio);
	close(fd);
	printf("\ndone\n");
	exit(0);
}

void fpga_cmd(int fd,UCHAR cmd, UCHAR param)
{
	write(fd,(void*)&param,1);
	usleep(TIME_DELAY);
	write(fd,(void*)&cmd,1);
	usleep(TIME_DELAY);
}
