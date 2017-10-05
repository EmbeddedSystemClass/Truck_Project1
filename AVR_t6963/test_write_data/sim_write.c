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
#include "../pic_main.h"
#include "../t6963.h"

#define BAUDRATE B19200
#define MODEMDEVICE "/dev/ttyS0"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

int set_interface_attribs (int fd, int speed, int parity);
void set_blocking (int fd, int should_block);
static UCHAR get_keypress(UCHAR ch,WINDOW *win, int display_offset);
static void print_menu(WINDOW *win);

//******************************************************************************************//
//****************************************** main ******************************************//
//******************************************************************************************//
int main(int argc, char *argv[])
{
	int fd;
	int type = 0;
	int i,j,k;
	int fp;
	int res;
	struct termios oldtio,newtio;
	WINDOW *win;
	MENU_FUNC_STRUCT mf;
	MENU_FUNC_STRUCT *pmf = &mf;
	UCHAR key;
	UCHAR wkey;
	int size;
	int start_addr;
	UCHAR ret_key;
	int display_offset = 1;
	char temp_label[MAX_LABEL_LEN];
	char tchbox[10];
	char filename[20];
	void *vptr;

//	syncup = 0;

/*
	for(i = 0;i < TOTAL_NUM_CHECKBOXES;i++)
	{
		check_boxes[i].index = (i % 10);
		check_boxes[i].checked = 0;
		if(i < 10)
			strcpy(check_boxes[i].string,"test  \0");
		else if(i < 20)
			strcpy(check_boxes[i].string,"hello \0");
		else if(i < 30)
			strcpy(check_boxes[i].string,"asdf  \0");
		else if(i < 40)
			strcpy(check_boxes[i].string,"1234  \0");

		sprintf(tchbox,"%2d",i);
		memcpy(&check_boxes[i].string[strlen(check_boxes[i].string)],tchbox,2);
	}

	for(i = 0;i < TOTAL_NUM_CHECKBOXES;i++)
	{
		printf("%d %d %s\n",check_boxes[i].index,check_boxes[i].checked,check_boxes[i].string);
	}
	exit(1);
*/
	initscr();			/* Start curses mode 		*/
	clear();
	noecho();
//	nodelay(stdscr,TRUE);
	raw();				/* Line buffering disabled	*/
	cbreak();	/* Line buffering disabled. pass on everything */
	win = newwin(60, 65, 0,0);
	keypad(win, TRUE);
//	nodelay(win, TRUE);
	box(win,0,0);
	set_win(win);

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

	j = 0;
	k = 0;
	i = 0;
	tdelay = 500;
	init_list();
	size = 300;
	start_addr = 0;
//	char temp1[MAX_LABEL_LEN];
//	char temp2[MAX_LABEL_LEN];
	print_menu(win);
	for(i = 0;i < 5;i++)
		sample_numbers[i] = 10+(i*10)+i;
	memset(aux_string,0,AUX_STRING_LEN);

	strcpy(filename,"eeprom.bin\0");
	if(access(filename,F_OK) != -1)
	{
		fp = open((const char *)filename, O_RDWR);
		if(fp < 0)
		{
			mvwprintw(win, LAST_ROW_DISP-1,2,"can't open file for writing");
			wrefresh(win);
			getch();
		}else
		{
			res = 0;
			lseek(fp,0,SEEK_SET);
//							for(i = start_addr;i < size;i++)
			res = read(fp,eeprom_sim,EEPROM_SIZE);
			close(fp);
			mvwprintw(win, LAST_ROW_DISP-1,2,
				"reading part into eeprom_sim: %d            ",res);
			j = k = 0;
		}
	}else
	{
		mvwprintw(win, LAST_ROW_DISP-1,2,"do a burn_eeprom here?");
	}
/*
	memcpy((void*)(eeprom_sim+NO_RT_LABELS_EEPROM_LOCATION),(void*)&no_rt_labels,sizeof(UINT));
	memcpy((void*)(eeprom_sim+NO_MENU_LABELS_EEPROM_LOCATION),(void*)&no_menu_labels,sizeof(UINT));
	memcpy((void*)(eeprom_sim+NO_RTPARAMS_EEPROM_LOCATION),(void*)&no_rtparams,sizeof(UINT));	// reserve an extra sample_data space for in case of 'escape'
*/
	res = 0;
#ifndef TESTING_AVR
	for(i = 0;i < EEPROM_SIZE;i++)
	{
		res += write(global_fd,&eeprom_sim[i],1);
		mvwprintw(win, LAST_ROW_DISP-1,2,"%2x %2d ",eeprom_sim[i],res);
		wrefresh(win);
		usleep(tdelay);
	}

	mvwprintw(win, LAST_ROW_DISP-1,2,"res: %2d ",res);
	wrefresh(win);
#endif
	res = 0;
/*
	for(i = 0;i < TOTAL_NUM_CHECKBOXES;i++)
	{
		res += write(global_fd,&check_boxes[i].index,1);
		usleep(tdelay);
		res += write(global_fd,&check_boxes[i].checked,1);
		usleep(tdelay);
		res += write(global_fd,&check_boxes[i].string[0],CHECKBOX_STRING_LEN);
		usleep(tdelay);
		mvwprintw(win, LAST_ROW_DISP-1,2,"%d   %d  ",res,i);
		wrefresh(win);


		for(j = 0;j < CHECKBOX_STRING_LEN;j++)
		{
			res += write(global_fd,&check_boxes[i].string[j],1);
			usleep(tdelay);
		}
	}
*/
	mvwprintw(win, LAST_ROW_DISP-2,2,"%d   ",res);
	wrefresh(win);

	i = 0;
	total_offset = 0;

	no_menu_structs = i;
	goffset = 0;
	no_menu_labels = blank;
	no_rt_labels = test - rpm;

	memset(cblabels,255,CBLABEL_SIZE);
	update_ram();


//		test the new prompt stuff
#if 0
	j = k = 0;
	for(i = 0;i < PROMPT_DATA_SIZE;i++)
	{
		mvwprintw(win, LAST_ROW_DISP-44+k, 2+j,"%c",prompt_data[i]);
		if(++j > 35)
		{
			j = 0;
			k++;
		}
	}
	mvwprintw(win,LAST_ROW_DISP-44+k, 2+j,"|");
		

	mvwprintw(win, LAST_ROW_DISP-31,2,"no_prompts: %d   ",no_prompts);
	for(i = 0;i < no_prompts;i++)
	{
		get_prompt_label(i,temp_label);
		mvwprintw(win, LAST_ROW_DISP-29+i, 2,"%s   ",temp_label);
	}
	wrefresh(win);
#endif
	no_cblabels = get_cblabel_offsets();
	no_menu_labels = get_mlabel_offsets();

//	mvwprintw(win, LAST_ROW_DISP-2,2,"no_menu_labels: %d no_rt_labels %d %d ",no_menu_labels,no_rt_labels, blank);


	j = k = 0;
//	for(i = 0;i < 22;i++)
//		mvwprintw(win, LAST_ROW_DISP-35+i,24,"                                   ");
/*
	for(i = 0;i < EEPROM_SIZE;i++)
	{
		if(eeprom_sim[i] < 0x7e && eeprom_sim[i] > 0x21)
			mvwprintw(win, LAST_ROW_DISP+j-40, 24+k,"%c",eeprom_sim[i]);
		else if(eeprom_sim[i] == 0)	mvwprintw(win, LAST_ROW+j-50, 24+k," ");
		else mvwprintw(win, LAST_ROW_DISP+j-40, 24+k,"_");
		if(++k > 30)
		{
			k = 0;
			++j;
		}
	}
*/
/*
	for(i = NO_RT_LABELS_EEPROM_LOCATION;i < EEPROM_SIZE;i++)
	{
		mvwprintw(win, LAST_ROW_DISP+j-40, 2+(k*3),"%2x ",eeprom_sim[i]);
		if(++k > 15)
		{
			k = 0;
			++j;
		}
	}

	wrefresh(win);
*/

	do {
		key = wgetch(win);
		wkey = get_keypress(key,win, display_offset);
//		mvwprintw(win, LAST_ROW_DISP-1,1,"key: %x  ",wkey);
		if(wkey != 0xff)
		{
			switch(wkey)
			{
				case TEST1:		// 'g'
					size = 300;
					start_addr = 0;
					get_key(wkey,size,start_addr,eeprom_sim,type);
					break;

				case TEST2:		// 'h'
					size = 300;
					start_addr = 300;
					get_key(wkey,size,start_addr,eeprom_sim,type);
					break;

				case TEST3:		// 'j'
					size = 300;
					start_addr = 600;
					get_key(wkey,size,start_addr,eeprom_sim,type);
					break;

				case TEST4:		// 'k'
					size = 123;
					start_addr = 900;
					get_key(wkey,size,start_addr,eeprom_sim,type);
					break;

				case TEST14:	// 'C'
					size = 0x210;
					start_addr = 0x120;
					get_key(wkey,size,start_addr,eeprom_sim,type);
					break;

				case INIT:
					mvwprintw(win, LAST_ROW_DISP-1,1,"init          ");
					wrefresh(win);
					memset(aux_string,0,AUX_STRING_LEN);
					ret_key = 0xff;
					break;
				case READ_EEPROM1:		// 'e'
					memset(eeprom_sim,0,EEPROM_SIZE);
					get_key(wkey,size,start_addr,eeprom_sim,type);
					break;

				case READ_EEPROM2:		// 'f'
					memset(eeprom_sim,0,EEPROM_SIZE);
					get_key(wkey,size,start_addr,eeprom_sim,type);
					break;

				case BURN_PART:		// 'v'
#if 0
					strcpy(filename,"eeprom.bin\0");
					mvwprintw(win, LAST_ROW_DISP-1,2,"starting burn part   ");
					if(access(filename,F_OK) != -1)
					{
						fp = open((const char *)filename, O_RDWR);
						if(fp < 0)
						{
							mvwprintw(win, LAST_ROW_DISP-1,2,"can't open file for writing");
							wrefresh(win);
						}else
						{
							res = 0;
							lseek(fp,0,SEEK_SET);
//							for(i = start_addr;i < size;i++)
							res = read(fp,eeprom_sim,EEPROM_SIZE);
							close(fp);
							mvwprintw(win, LAST_ROW_DISP-1,2,
								"reading part into eeprom_sim: %d %d %d  ",res,size,start_addr);
							get_key(key,size,start_addr,eeprom_sim,type);
							goffset = 0;
							get_mlabel_offsets();
						}
					}else
					{
						mvwprintw(win, LAST_ROW_DISP-1,2,"eeprom.bin not found!");
					}
#endif
					get_key(wkey,size,start_addr,eeprom_sim,type);
					break;

				case SPACE:		// (space bar)
					get_key(wkey,size,start_addr,aux_string,type);
					for(i = 1;i < LAST_ROW;i++)
						mvwprintw(win, i,1,"                                                             ");
					print_menu(win);

					break;

				default:
					wrefresh(win);
					start_addr = 500;
					ret_key = get_key(wkey,size,start_addr,aux_string,0);
	//				mvwprintw(win, LAST_ROW-9,1,"non_func: %x %c  ",wkey, wkey-NF_1);
	//				wrefresh(win);

					if(ret_key >= NF_1 && ret_key <= NF_10)
					{
						mvwprintw(win, LAST_ROW_DISP-1,1,"non_func: %x %d  ",ret_key, ret_key-NF_1);
						wrefresh(win);

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
						mvwprintw(win, LAST_ROW_DISP-1,1,"                 ");
						wrefresh(win);
					}
					j++;

					break;

			}
		}
//		wkey = key = 0;
		wrefresh(win);
	}while(key != 'q');

	write(global_fd,&key,1);

	delwin(win);
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

	switch(key)
	{
		case '0':
			wkey = KP_0;
			break;
		case '1':
			wkey = KP_1;
			break;
		case '2':
			wkey = KP_2;
			break;
		case '3':
			wkey = KP_3;
			break;
		case '4':
			wkey = KP_4;
			break;
		case '5':
			wkey = KP_5;
			break;
		case '6':
			wkey = KP_6;
			break;
		case '7':
			wkey = KP_7;
			break;
		case '8':
			wkey = KP_8;
			break;
		case '9':
			wkey = KP_9;
			break;
		case '*':
			wkey = KP_AST;
			break;
		case '#':
			wkey = KP_POUND;
			break;
		case 'a':
			wkey = KP_A;
			break;
		case 'b':
			wkey = KP_B;
			break;
		case 'c':
			wkey = KP_C;
			break;
		case 'd':
			wkey = KP_D;
			break;
// use 'z' as a shortcut to '*' and 'y' as a shortcut to '#'
		case 'y':
			wkey = KP_POUND;
			break;
		case 'z':
			wkey = KP_AST;
			break;
		case 'g':
			wkey = TEST1;		// set size = 300, start_addr = 0
			break;
		case 'h':
			wkey = TEST2;		// set size = 300, start_addr = 300
			break;
		case 'j':
			wkey = TEST3;		// set size = 300, start_addr = 600
			break;
		case 'k':
			wkey = TEST4;		// set size = 129, start_addr = 900
			break;
		case 'l':
			wkey = TEST5;		// fill aux_string with random data
			break;
		case 'm':
			wkey = TEST6;		// draw border
			break;
		case 'n':
			wkey = TEST7;		// print cblabels
			break;
		case 'o':
			wkey = TEST8;		// restore eeprom.bin
			break;
		case 'p':
			wkey = TEST9;		// copy eeprom2.bin to eeprom.bin in sim_write
			break;
		case 'x':
			wkey = TEST10;		// read aux_string
			break;
		case 's':
			wkey = TEST11;		// tell AVR to print pattern
			break;
		case 'r':
			wkey = LOAD_RAM;	// load all stuff in ram
			break;
		case 'i':
			wkey = INIT;
			break;
		case ' ':
			wkey = SPACE;
			break;
		case 'v':
			wkey = BURN_PART;
			break;
		case 'e':
			wkey = READ_EEPROM1;
			break;
		case 'f':
			wkey = READ_EEPROM2;
			break;
		case 'A':
			wkey = TEST12;
			break;
		case 'B':
			wkey = TEST13;
			break;
		case 'C':
			wkey = TEST14;
			break;
		case 'D':
			wkey = TEST15;
			break;
		case 'E':
			wkey = TEST16;
			break;
		case 'F':
			wkey = TEST17;
			break;
		default:
			wkey = 0xff;
			break;
	}
	return wkey;
}
//******************************************************************************************//
//**************************************** print_menu **************************************//
//******************************************************************************************//
static void print_menu(WINDOW *win)
{
//	mvwprintw(win, LAST_ROW-6,LAST_COL,"*");
	mvwprintw(win, LAST_ROW-13,1,"----------------------------- menu ----------------------------");
	mvwprintw(win, LAST_ROW-12,1,"G - set start_addr to 0");
	mvwprintw(win, LAST_ROW-11,1,"H - set start_addr to 300");
	mvwprintw(win, LAST_ROW-10,1,"J - set start_addr to 600");
	mvwprintw(win, LAST_ROW-9,1,"K - set start_addr to 900 and size to 123");
	mvwprintw(win, LAST_ROW-8,1,"M - draw border");
	mvwprintw(win, LAST_ROW-7,1,"N - print cblabels");
	mvwprintw(win, LAST_ROW-6,1,"E - READ_EEPROM and display in ascii (current size/start_addr)");
	mvwprintw(win, LAST_ROW-5,1,"F - READ_EEPROM and display in hex (current size/start_addr)");
	mvwprintw(win, LAST_ROW-4,1,"V - BURN_PART (current size/start_addr)");
	mvwprintw(win, LAST_ROW-3,1,"R - LOAD_RAM");
	mvwprintw(win, LAST_ROW-2,1,"X - read aux_string");
	mvwprintw(win, LAST_ROW-1,1,"<space> = blank screen and 'Q - quit");
	wrefresh(win);

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

