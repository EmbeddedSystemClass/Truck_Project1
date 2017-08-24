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
//#include "../main.h"
#include "../main.h"
#include "../pic_main.h"
#include "../t6963.h"

#define BAUDRATE B19200
#define MODEMDEVICE "/dev/ttyS0"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define LEN 200
// really cranking
#define TIME_DELAY 200000
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
	int fd;
	int type = 0;
	int iters, itr;
	int i,j;
	int res;
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
	UCHAR ret_key;
	int display_offset = 1;
//	UCHAR read_buf[NUM_ENTRY_SIZE];

//	memset(sample_data,0,sizeof(sample_data));
	iters = 100;
#if 0
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

#if 0
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
#endif
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
//	memset(cur_param_string,0,sizeof(cur_param_string));
	for(i = 0;i < NUM_UCHAR_PARAMS;i++)
		cur_param_string[i] = i;
	for(i = 0;i < AUX_STRING_LEN;i++)
		aux_string[i] = i;
	j = 0;
	res = 0;
// comment out the next #if 0/#endif to test
#if 0
	mvwprintw(menu_win, display_offset,2,"test  ");
	wrefresh(menu_win);
	getch();
	for(i = 0;i < no_rt_labels;i++)
		mvwprintw(menu_win, display_offset+i,2,"%d: %s  ",i,rt_labels[i]);
	wrefresh(menu_win);
	getch();
	for(i = 0;i < no_rt_labels;i++)
		mvwprintw(menu_win, display_offset+i,2,"                          ");
	for(i = no_menu_labels;i < no_menu_labels+no_func_labels; i++)
		mvwprintw(menu_win, display_offset+i-no_menu_labels,2,"%d: %s  ",i-no_menu_labels,menu_labels[i]);
	wrefresh(menu_win);
	getch();
	for(i = 0;i < no_menu_labels;i++)
		mvwprintw(menu_win, display_offset+i,2,"%d: %s  ",i,menu_labels[i]);
	wrefresh(menu_win);
	getch();
	for(i = 0;i < no_menu_labels;i++)
		mvwprintw(menu_win, display_offset+i,2,"                     ");
	for(i = 0;i < no_menu_structs;i++)
	{
		mvwprintw(menu_win, display_offset+i,2,"%d ",menu_structs[i].fptr);
		for(j = 0;j < 6;j++)
			mvwprintw(menu_win, display_offset+i,7+(j*12),"%s ", menu_labels[ menu_structs[i].menus[j]] );
	}
	wrefresh(menu_win);
	getch();
//	for(i = 0;i < no_menu_structs;i++)
	for(i = 0;i < 60;i++)
	{
		mvwprintw(menu_win, display_offset+i,2,"                                                             ");
	}
	wrefresh(menu_win);
	getch();
	delwin(menu_win);
	clrtoeol();
	refresh();
	endwin();
	tcsetattr(fd,TCSANOW,&oldtio);
	close(fd);
	exit(1);
#endif
	for(i = 0;i < NUM_CHECKBOXES;i++)
	{
		check_boxes[i].index = i;
		check_boxes[i].checked = 0;
		strcpy(check_boxes[i].string,"test\0");
	}
	for(i = 0;i < NUM_EXECCHOICES;i++)
	{
		exec_choices[i].index = i;
		strcpy(exec_choices[i].string,"test\0");
	}
	i = 0;
	j = 0;
	init_list();
	do {
		key = wgetch(menu_win);
		wkey = get_keypress(key,menu_win, display_offset);
		if(wkey != 0xff)
		{
			ret_key = get_key(wkey,0);
			res = 0;
			mvwprintw(menu_win, DISP_OFFSET+34,2,"non_func: %x %c  ",wkey, wkey-NF_1);
			wrefresh(menu_win);

			if(ret_key >= NF_1 && ret_key <= NF_10)
			{
				mvwprintw(menu_win, DISP_OFFSET+34,2,"non_func: %x %d  ",ret_key, ret_key-NF_1);
				wrefresh(menu_win);

				switch(ret_key)
				{
					case NF_1:
					break;
					case NF_2:
					break;
					case NF_3:
					break;
					case NF_4:
					break;
					case NF_5:
					break;
					case NF_6:
					break;
					case NF_7:
					break;
					case NF_8:
					break;
					case NF_9:
					break;
					case NF_10:
					break;
					default:
					break;
				}
			}
			else
			{
				mvwprintw(menu_win, DISP_OFFSET+34,2,"                 ");
				wrefresh(menu_win);
			}	
/*
			if(wkey == KP_SIM_DATA)
			{
				read(fd,&recv_data,sizeof(UINT));
				write(fd,&send_data,sizeof(UINT));
				send_data++;
				mvwprintw(menu_win, display_offset+2, 4, "recv_data: %d  ",recv_data);
			}
*/
			j++;
		}
//		wkey = key = 0;
		wrefresh(menu_win);
	}while(key != 'q');

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
				mvwprintw(win, display_offset,50, "zero  ");
				wkey = KP_0;
				break;
			case '1':
				mvwprintw(win, display_offset,50, "one   ");
				wkey = KP_1;
				break;
			case '2':
				mvwprintw(win, display_offset,50, "two   ");
				wkey = KP_2;
				break;
			case '3':
				mvwprintw(win,display_offset,50, "three ");
				wkey = KP_3;
				break;
			case '4':
				mvwprintw(win, display_offset,50, "four  ");
				wkey = KP_4;
				break;
			case '5':
				mvwprintw(win, display_offset,50, "five  ");
				wkey = KP_5;
				break;
			case '6':
				mvwprintw(win, display_offset,50, "six   ");
				wkey = KP_6;
				break;
			case '7':
				mvwprintw(win, display_offset,50, "seven ");
				wkey = KP_7;
				break;
			case '8':
				mvwprintw(win, display_offset,50, "eight ");
				wkey = KP_8;
				break;
			case '9':
				mvwprintw(win, display_offset,50, "nine  ");
				wkey = KP_9;
				break;
			case '*':
				mvwprintw(win, display_offset,50, "ast   ");
				wkey = KP_AST;
				break;
			case '#':
				mvwprintw(win, display_offset,50, "pound ");
				wkey = KP_POUND;
				break;
			case 'A':
			case 'a':
				mvwprintw(win, display_offset,50, "A     ");
				wkey = KP_A;
				break;
			case 'B':
			case 'b':
				mvwprintw(win, display_offset,50, "B     ");
				wkey = KP_B;
				break;
			case 'C':
			case 'c':
				mvwprintw(win, display_offset,50, "C     ");
				wkey = KP_C;
				break;
			case 'D':
			case 'd':
				mvwprintw(win, display_offset,50, "D     ");
				wkey = KP_D;
				break;
// use 'z' as a shortcut to '*' and 'y' as a shortcut to '#'
			case 'Y':
			case 'y':
				mvwprintw(win, display_offset,50, "pound ");
				wkey = KP_POUND;
				break;
			case 'Z':
			case 'z':
				mvwprintw(win, display_offset,50, "ast   ");
				wkey = KP_AST;
				break;
			case 'U':
			case 'u':
				mvwprintw(win, display_offset,50,"U    ");
				wkey = 0xff;
				break;
			case 'V':
			case 'v':
				mvwprintw(win, display_offset,50,"V     ");
				wkey = KP_SIM_DATA;
				break;
			case ' ':
				mvwprintw(win, display_offset,50,"space   ");
				wkey = SPACE;
				break;
			default:
				mvwprintw(win, display_offset,50, "?     ");
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

