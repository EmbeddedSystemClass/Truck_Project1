// sim_read.c
#if 1
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


typedef unsigned char UCHAR;
typedef unsigned int UINT;

#define BAUDRATE B19200
#define MODEMDEVICE "/dev/ttyS0"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define LEN 200
#define DEBUG_CHAR_AT 1
#define DEBUG_STRING_AT 2
#define DEBUG_SET_CURSOR 3
#define DEBUG_CLRSCR1 4
#define DEBUG_CLRSCR2 5
#define DEBUG_CLRSCR3 6
#define DEBUG_MSG1 7
#define COLUMN              40      //Set column number to be e.g. 32 for 8x8 fonts, 2 pages
#define ROWS                16
// really cranking
#define TIME_DELAY 20000
// readable
//#define TIME_DELAY 300000

int set_interface_attribs (int fd, int speed, int parity);
void set_blocking (int fd, int should_block);
#endif
//******************************************************************************************//
//****************************************** main ******************************************//
//******************************************************************************************//
int main(int argc, char *argv[])
{
#if 1
	int fd;
	int i,j,k;
	UCHAR ch;
	UCHAR key;
	struct termios oldtio,newtio;
	WINDOW *win;
	int res;
	int display_offset = 1;
	UCHAR buff[LEN];
	UINT row, col, mode, type;
	char str[30];
	UCHAR str_len;
	UINT debug_msg_ptr;

	// reserve an extra sample_data space for in case of 'escape'
	initscr();			/* Start curses mode 		*/
	clear();
	noecho();
//	nodelay(stdscr,TRUE);
	raw();				/* Line buffering disabled	*/
	cbreak();	/* Line buffering disabled. pass on everything */
	win = newwin(65, 70, 0,0);
	keypad(win, TRUE);
//	nodelay(win, TRUE);
	box(win,0,0);

	wrefresh(win);
//	mvwaddch(win,2,3,ACS_LTEE);

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
	set_blocking (fd, 1);	// block on read
//	set_blocking (fd, 0);	// non-blocking

	mvwhline(win, 1, 5, ACS_HLINE, 41);
	mvwhline(win, 18, 5, ACS_HLINE, 41);
	mvwvline(win, 2, 4, ACS_VLINE, 16);
	mvwvline(win, 2, 45, ACS_VLINE, 16);
	mvwaddch(win, 1, 4, ACS_ULCORNER);
	mvwaddch(win, 18, 4, ACS_LLCORNER);
	mvwaddch(win, 1, 45, ACS_URCORNER);
	mvwaddch(win, 18, 45, ACS_LRCORNER);

	wrefresh(win);
#endif
	j = 0;
	res = 0;
	i = 0;
	debug_msg_ptr = 20;
	while(1)
	{
		read(fd,&key,1);
		buff[i] = key;
		i++;
		if(key == 0xff)
		{
			switch(buff[0])
			{
				case DEBUG_CHAR_AT:
					ch = buff[1];
					col = buff[2];
					row = buff[3];
					mvwaddch(win,row-13,col-10,ch);
					wrefresh(win);
				break;
				case DEBUG_STRING_AT:
					row = buff[2];
					row <<= 8;
					row |= buff[1];
					col = buff[4];
					col <<= 8;
					col |= buff[3];
					str_len = buff[5];
					memset(str,0x20,sizeof(str));	
					memcpy(str,&buff[7],str_len);
					str[str_len] = 0;
					mvwprintw(win,row-13, col-10,"%s",str);
/*
					for(i = 1;i < 19;i++)
						mvwprintw(win,57,1+(i*3),"%x ",buff[i]);
					for(i = 1;i < 19;i++)
						mvwprintw(win,58,1+(i*3),"%x ",buff[i+19]);
*/
					wrefresh(win);
				break;
				case DEBUG_SET_CURSOR:
					mode = buff[1];
					row = buff[3];
					row <<= 8;
					row |= buff[2];
					col = buff[5];
					col <<= 8;
					col |= buff[4];
					type = buff[6];
					mvwprintw(win,50,1,"set cursor: mode %x row %x col %x type %x   ",
							mode, row,col,type);
//					for(i = 1;i < 19;i++)
//						mvwprintw(win,24,1+(i*3),"%x ",buff[i]);
//					mvwaddch(win,row+2,col+5,"_");
					wrefresh(win);
				break;
				case DEBUG_CLRSCR1:
					debug_msg_ptr = 20;
				
					for(i = 0;i < 30;i++)
						mvwprintw(win,1+i,1,
							"                                                                   ");
					wrefresh(win);
				break;	
				case DEBUG_CLRSCR2:
					debug_msg_ptr = 20;
					for(i = 30;i < 70;i++)
						mvwprintw(win,1+i,1,
							"                                                                   ");
					wrefresh(win);
				break;	
				case DEBUG_CLRSCR3:
					for(row = 2;row < ROWS+2;row++)
						mvwprintw(win,row,5,"                                        ");
					wrefresh(win);
				break;
				case DEBUG_MSG1:
					row = buff[2];
					row <<= 8;
					row |= buff[1];
					col = buff[4];
					col <<= 8;
					col |= buff[3];
					str_len = buff[5];
					memset(str,0x20,sizeof(str));	
					memcpy(str,&buff[7],str_len);
					str[str_len] = 0;
					mvwprintw(win,debug_msg_ptr+1,1,
						"                                                               ");
					
					mvwprintw(win,debug_msg_ptr-1, 2," ");
					mvwprintw(win,debug_msg_ptr, 2,"- %d %d %s          ",row,col,str);
					if(++debug_msg_ptr > 60)
						debug_msg_ptr = 20;
					wrefresh(win);
					usleep(1000);
				break;
				default:
				break;
			}
			i = 0;
//			memset(buff,0,sizeof(buff));
		}
	}
	delwin(win);
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

