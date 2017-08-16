// test_write_data.c - used to test the data protocol between the AVR and PIC24 where the
// data string is sent with FF,FE,FD.. and the next 3 bytes are shifted so they don't
// have the high bits set - displays in ncurses window
// when this is called with write params - 'w'... it simulates the PIC24
// when called with just the 'r' param, it simulates the AVR (calls do_read below)
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
#include "../main.h"
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
static UCHAR get_keypress(UCHAR ch,WINDOW *win, int display_offset);
//******************************************************************************************//
//****************************************** main ******************************************//
//******************************************************************************************//
int main(int argc, char *argv[])
{
#if 1
	int fd;
	int type = 0;
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
//	UCHAR read_buf[NUM_ENTRY_SIZE];

//	memset(sample_data,0,sizeof(sample_data));
	send_data = recv_data = 0;
	
	if(argv[1][0] == 'w')
		type = 1;
	else if(argv[1][0] == 'r')
		type = 0;
	else
	{
		printf("usage: test_data w [no iters][starting rpm][others][delay]\n");
		printf("or test_data r\n");
		return 1;
	}
	memset(new_global_number,0,NUM_ENTRY_SIZE);
	memset(menu_labels,0,NUM_MENU_LABELS*MAX_LABEL_LEN);
	memset(rt_labels,0,NUM_RT_LABELS*MAX_LABEL_LEN);
	memset(menu_structs,0,NUM_MENU_STRUCTS*sizeof(MENU_FUNC_STRUCT));
	memset(rt_params,0,NUM_RT_PARAMS*sizeof(RT_PARAM));
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

	if(type == 0)
		display_offset = DISP_OFFSET;

	else if(type == 1)
	{
//		printf("write: ");
		iters = atoi(argv[2])*10;
//		printf("iters: %d\n",iters);
		mvwprintw(menu_win,display_offset+17,4,"interations: %d",iters);
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
	}
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
	if(type == 0)
	{
		do_read(menu_win, fd,display_offset);
//		printf("do_read: %d\n",ret_code);
	}	// end of else

// write - simulate the PIC24
	else if(type == 1)
	{
		i = 0;
		for(itr = 0;itr < iters;itr++)
		{
//			usleep(tdelay);
			mvwprintw(menu_win, display_offset+17, 4, "iterations left: %d   ",iters-itr);

	// see if one of the keys from the "keypad" is pressed
//#if 0	//comment this out and set blocking to non-blocking
			key = wgetch(menu_win);
//			mvwprintw(menu_win, display_offset, 4, "key: %x  ",key);
			wrefresh(menu_win);
//			if(++i > 15)
//				i = 0;
			wkey = get_keypress(key,menu_win, display_offset);
			if(key != 0xff)
			{

				mvwprintw(menu_win, display_offset+1, 4, "wkey: %x  ",wkey);
				write(fd,&wkey,1);	// simulates the PIC24 sending a keypress
//				mvwprintw(menu_win, display_offset+2, 4, "         ");
//				read(fd,sample_data,sizeof(int)*10);
//				read(fd,&req,1);
//				mvwprintw(menu_win, display_offset+2, 4, "req: %d  ",req);
//				for(i = 0;i < 10;i++)
//					mvwprintw(menu_win, display_offset+4+i, 4, "%d              ",sample_data[i]);
				// simulates the AVR timer or int sending/receiving data
				// AVR uses 16-bit counter-timer
				if(wkey == KP_SIM_DATA)
				{
					read(fd,&recv_data,sizeof(UINT));
					write(fd,&send_data,sizeof(UINT));
					send_data++;
					mvwprintw(menu_win, display_offset+2, 4, "recv_data: %d  ",recv_data);
				}
			}

//			read(fd,&req,1);

			wkey = key = 0;
#if 0
			if(P24_rt_params[code2].dtype > 0)
			{
				data2 = rtdata[code2];
				if(data2 & 0x8000)
				{
					ch = RT_HIGH3;
					write(fd,&ch,1);
					usleep(tdelay);
					data1 = (UCHAR)(data2);
					write(fd,&data1,1);
					usleep(tdelay);
					data1 = (UCHAR)(data2>>8);
					data1 &= 0x7f;
					ch = data1;
					write(fd,&ch,1);
					usleep(tdelay);
				}
				else if(data2 & 0x0080)
				{
					ch = RT_HIGH2;
					write(fd,&ch,1);
					usleep(tdelay);
					data1 = (UCHAR)data2;
					data1 &= 0x7f;
					ch = data1;
					write(fd,&ch,1);
					usleep(tdelay);
					data1 = (UCHAR)(data2>>8);
					ch = data1;
					write(fd,&ch,1);
					usleep(tdelay);
				}
				else
				{
					ch = RT_HIGH1;
					write(fd,&ch,1);
					usleep(tdelay);
					data1 = (UCHAR)(data2);
					ch = data1;
					write(fd,&ch,1);
					usleep(tdelay);
					data1 = (UCHAR)(data2>>8);
					ch = data1;
					write(fd,&ch,1);
					usleep(tdelay);
				}

				sprintf(param_string,"%4u",data2);
				mvwprintw(menu_win, display_offset+code2, 10,"              ");
				mvwprintw(menu_win, display_offset+code2, 10, param_string);
				wrefresh(menu_win);
				if(code2 != RT_AUX1-RT_OFFSET && code2 != RT_AUX2-RT_OFFSET)
				{
					mvwprintw(menu_win, display_offset+22,2,"%d %d   ",code2,data2);
					data2 += 10;	// rpm get inc'd by 10
				}
				else mvwprintw(menu_win, display_offset+23,2,"%d %d   ",code2,data2);
				rtdata[code2] = data2;
			}
			// end of if RT_RPM
			else if(P24_rt_params[code2].dtype == 0)
			{
				data = (UCHAR)rtdata[code2];
				if(data > 0x7f)
				{
					ch = RT_HIGH0;
					write(fd,&ch,1);
					usleep(tdelay);
					data1 = data & 0x7f;
					ch = data1;
					write(fd,&ch,1);
					usleep(tdelay);
				}
				else
				{
					ch = RT_LOW;
					write(fd,&ch,1);
					usleep(tdelay);
					ch = data;
					write(fd,&ch,1);
					usleep(tdelay);
				}
				data++;
				rtdata[code2] = (UINT)data;
			}
#endif
			wrefresh(menu_win);
		}
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
//*************************************** get_keypress *************************************//
//******************************************************************************************//
static UCHAR get_keypress(UCHAR key,WINDOW *win, int display_offset)
{
	UCHAR wkey;
	if(key != 0xff)
	{
		switch(key)
		{
			case '0':
				mvwprintw(win, display_offset+23, 8, "zero  ");
				wkey = KP_0;
				break;
			case '1':
				mvwprintw(win, display_offset+23, 8, "one   ");
				wkey = KP_1;
				break;
			case '2':
				mvwprintw(win, display_offset+23, 8, "two   ");
				wkey = KP_2;
				break;
			case '3':
				mvwprintw(win,display_offset+23, 8, "three ");
				wkey = KP_3;
				break;
			case '4':
				mvwprintw(win, display_offset+23, 8, "four  ");
				wkey = KP_4;
				break;
			case '5':
				mvwprintw(win, display_offset+23, 8, "five  ");
				wkey = KP_5;
				break;
			case '6':
				mvwprintw(win, display_offset+23, 8, "six   ");
				wkey = KP_6;
				break;
			case '7':
				mvwprintw(win, display_offset+23, 8, "seven ");
				wkey = KP_7;
				break;
			case '8':
				mvwprintw(win, display_offset+23, 8, "eight ");
				wkey = KP_8;
				break;
			case '9':
				mvwprintw(win, display_offset+23, 8, "nine  ");
				wkey = KP_9;
				break;
			case '*':
				mvwprintw(win, display_offset+23, 8, "ast   ");
				wkey = KP_AST;
				break;
			case '#':
				mvwprintw(win, display_offset+23, 8, "pound ");
				wkey = KP_POUND;
				break;
			case 'A':
			case 'a':
				mvwprintw(win, display_offset+23, 8, "A     ");
				wkey = KP_A;
				break;
			case 'B':
			case 'b':
				mvwprintw(win, display_offset+23, 8, "B     ");
				wkey = KP_B;
				break;
			case 'C':
			case 'c':
				mvwprintw(win, display_offset+23, 8, "C     ");
				wkey = KP_C;
				break;
			case 'D':
			case 'd':
				mvwprintw(win, display_offset+23, 8, "D     ");
				wkey = KP_D;
				break;
// use 'z' as a shortcut to '*' and 'y' as a shortcut to '#'
			case 'Y':
			case 'y':
				mvwprintw(win, display_offset+23, 8, "pound ");
				wkey = KP_POUND;
				break;
			case 'Z':
			case 'z':
				mvwprintw(win, display_offset+23, 8, "ast   ");
				wkey = KP_AST;
				break;
			case 'U':
			case 'u':
				mvwprintw(win, display_offset+23, 8,"U    ");
				wkey = 0xff;
				break;
			case 'V':
			case 'v':
				mvwprintw(win, display_offset+23, 8,"V     ");
				wkey = KP_SIM_DATA;
				break;
			default:
				mvwprintw(win, display_offset+23, 8, "?     ");
				wkey = 0xff;
				break;
		}
	}
	return wkey;
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

