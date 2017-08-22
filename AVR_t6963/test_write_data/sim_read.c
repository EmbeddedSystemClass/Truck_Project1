// sim_read.c

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
#include <ncurses.h>
#include "../sfr_helper.h"
//#include "../main.h"
#include "../avr_main.h"
#include "../t6963.h"

#define BAUDRATE B19200
#define MODEMDEVICE "/dev/ttyS0"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define LEN 200
// really cranking
#define TIME_DELAY 20000
// readable
//#define TIME_DELAY 300000

int set_interface_attribs (int fd, int speed, int parity);
void set_blocking (int fd, int should_block);

//******************************************************************************************//
//****************************************** main ******************************************//
//******************************************************************************************//
int main(int argc, char *argv[])
{
#if 1
	int fd;
	int iters, itr;
	int i,j;
	UCHAR ch;
	struct termios oldtio,newtio;
	WINDOW *menu_win;
	useconds_t tdelay = TIME_DELAY;
	useconds_t tdelay2 = TIME_DELAY;
    UCHAR data = 2;
	UINT data2 = 0;
	UCHAR key;
	UCHAR wkey;
	UCHAR req;
	int display_offset = 3;
	UCHAR temp_params[NUM_UCHAR_PARAMS];
	int res;

//	UCHAR read_buf[NUM_ENTRY_SIZE];

//	memset(sample_data,0,sizeof(sample_data));
	send_data = recv_data = 0;

/*
	memset(new_global_number,0,NUM_ENTRY_SIZE);
	memset(menu_labels,0,NUM_MENU_LABELS*MAX_LABEL_LEN);
	memset(rt_labels,0,NUM_RT_LABELS*MAX_LABEL_LEN);
	memset(menu_structs,0,NUM_MENU_STRUCTS*sizeof(MENU_FUNC_STRUCT));
	memset(rt_params,0,NUM_RT_PARAMS*sizeof(RT_PARAM));
*/
#endif
	burn_eeprom();
	// reserve an extra sample_data space for in case of 'escape'
	initscr();			/* Start curses mode 		*/
	clear();
	noecho();
//	nodelay(stdscr,TRUE);
	raw();				/* Line buffering disabled	*/
	cbreak();	/* Line buffering disabled. pass on everything */
	menu_win = newwin(60, 65, 0,0);
	keypad(menu_win, TRUE);
//	nodelay(menu_win, TRUE);
	box(menu_win,0,0);
	set_win(menu_win);

	display_offset = DISP_OFFSET;

#if 0
		if(argc > 2)
		{
			data2 = atoi(argv[3]);
//			printf("rpm starting at: %d\n",data2);
			mvwprintw(menu_win,display_offset+18,4,"rpm starting at: %d",data2);
		}
		if(argc > 3)
		{
			data = atoi(argv[4]);
//			printf("others starting at: %d\n",data);
			mvwprintw(menu_win,display_offset+19,4,"others starting at: %d",data);
		}
		if(argc > 4)
		{
//			printf("time delay: %s  ",argv[5]);
			tdelay = atoi(argv[5])*5000;
			tdelay2 = atoi(argv[5])*10000;
			mvwprintw(menu_win,display_offset+20,4,"time delays: %d  %d ",tdelay,tdelay2);
		}
#endif

	wrefresh(menu_win);

	memset(&newtio, 0, sizeof newtio);

	fd = open (MODEMDEVICE, O_RDWR | O_NOCTTY | O_SYNC);
	if (fd <0) {perror(MODEMDEVICE);
		exit(-1); }
	global_fd = fd;
	if(tcgetattr(fd,&oldtio) != 0) /* save current port settings */
	{
		printf("Error from tcgetattr: %d ",errno);
		close(fd);
		exit(1);
	}

	set_interface_attribs (fd, BAUDRATE, 0);
	set_blocking (fd, 1);	// block on read
//	set_blocking (fd, 0);	// non-blocking

// read	- simulate the AVR
	i = 0;
	j = 0;
	res = 0;
	init_list();
	while(j < 100)
	{
		read_get_key(temp_params);

		j++;
	}
	delwin(menu_win);
	clrtoeol();
	refresh();
	endwin();
	tcsetattr(fd,TCSANOW,&oldtio);
	close(fd);
	exit(0);
}

//******************************************************************************************//
//*********************************** set_interface_attribs ********************************//
//******************************************************************************************//
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
//        tty.c_cc[VTIME] = 10;			// 1 second read timeout
//        tty.c_cc[VTIME] = 0.5;
        tty.c_cc[VTIME] = 1;            // 0.1 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
                printf ("error %d setting term attributes", errno);
}

