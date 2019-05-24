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
#include <unistd.h>

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
#define LEN 30
#define TIME_DELAY 50000

enum cmd_types
{
	NON_CMD,
	ENABLE_START,		// 1
	STARTER_OFF,		// 2
	ON_ACC,				// 3
	OFF_ACC,			// 4
	ON_FUEL_PUMP,		// 5
	OFF_FUEL_PUMP,		// 6
	ON_FAN,				// 7
	OFF_FAN,			// 8
	ON_LIGHTS,			// 9
	OFF_LIGHTS,			// 10
	ON_BRIGHTS,			// 11
	OFF_BRIGHTS,		// 12
	ON_BRAKES,
	OFF_BRAKES,
	ON_RUNNING_LIGHTS,
	OFF_RUNNING_LIGHTS,
	SPECIAL_CMD,
	START_SEQ,
	SHUTDOWN,
	ON_LLIGHTS,
	OFF_LLIGHTS,
	ON_LBRIGHTS,
	OFF_LBRIGHTS,
	ON_RLIGHTS,
	OFF_RLIGHTS,
	ON_RBRIGHTS,
	OFF_RBRIGHTS,
	SHUTDOWN_IOBOX,
	REBOOT_IOBOX,
	TEST_ALL_IO,
	SEND_ODATA,
	SAVE_TO_DISK,
	BLOWER_OFF,
	BLOWER1,
	BLOWER2,
	BLOWER3,
	WIPER1,
	WIPER2,
	WIPER_OFF,
	GET_DIR,
	LCD_SHIFT_RIGHT,
	LCD_SHIFT_LEFT,
	SCROLL_UP,
	SCROLL_DOWN,
	ENABLE_LCD,
	SET_TIME,
	GET_TIME,
	UPLOAD_NEW,
	NEW_PASSWORD1,
	SET_SERIAL_RECV_ON,
	SET_SERIAL_RECV_OFF,
	TEST_LEFT_BLINKER,
	TEST_RIGHT_BLINKER,
	RE_ENTER_PASSWORD,
	DISCONNECT,
	STOP_MBOX_RECV,
	CLOSE_DB,
	OPEN_DB,
	BAD_MSG,
	CURRENT_TIME,
	SET_PARAMS,
	EXIT_PROGRAM
}CMD_TYPES;


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
        tty.c_cc[VMIN]  = 1;
        tty.c_cc[VTIME] = 20;

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
	int i,j;
	unsigned char ch;
	struct termios oldtio,newtio;
	unsigned char buf[LEN];
	memset(&newtio, 0, sizeof newtio);

	fd = open (MODEMDEVICE, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd <0) {perror(MODEMDEVICE);
		exit(-1); }

#ifdef MAKE_TARGET
	printf("\nopened ttyAM1\n");
#else
//	printf("\nopened ttyS0\n");
	printf("\n");
#endif

	if(tcgetattr(fd,&oldtio) != 0) /* save current port settings */
	{
		printf("Error from tcgetattr: %d ",errno);
		close(fd);
		exit(1);
	}

	set_interface_attribs (fd, BAUDRATE, 0);
	set_blocking (fd, 1);

	res2 = 0;
	ch = '*';
	j = 1;

// serial port has to have it's line pulled up inorder to work on a linux box
// windows terminal doesn't seem to care ???

	buf[0] = START_SEQ;
	buf[1] = SHUTDOWN;
	buf[2] = ON_FAN;
	buf[3] = OFF_FAN;
	buf[4] = ON_LIGHTS;
	buf[5] = OFF_LIGHTS;
	buf[6] = ON_BRIGHTS;
	buf[7] = OFF_BRIGHTS;
	buf[8] = ON_BRAKES;
	buf[9] = OFF_BRAKES;
	buf[10] = ON_RUNNING_LIGHTS;
	buf[11] = OFF_RUNNING_LIGHTS;
	buf[12] = BLOWER1;
	buf[13] = BLOWER2;
	buf[14] = BLOWER3;
	buf[15] = BLOWER_OFF;


	while(1)
	{
		for(i = 0;i < 16;i++)
		{
			printf("%2x\n",buf[i]);
			write(fd,&buf[i],1);
			usleep(100000);
		}
	}

	ch = 0xff;
	write(fd,(void *)&ch,1);
	tcsetattr(fd,TCSANOW,&oldtio);
	close(fd);
	printf("\ndone\n");
	exit(0);
}

