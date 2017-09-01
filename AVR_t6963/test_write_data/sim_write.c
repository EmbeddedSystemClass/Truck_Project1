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
static void print_menu(WINDOW *win);

//******************************************************************************************//
//****************************************** main ******************************************//
//******************************************************************************************//
int main(int argc, char *argv[])
{
	int fd;
	int type = 0;
	int i,j,k;
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

	burn_eeprom();
	// reserve an extra sample_data space for in case of 'escape'
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

	goffset = 0;
	get_label_offsets();

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
	size = 0;
	start_addr = 0;
	char temp1[MAX_LABEL_LEN];
	char temp2[MAX_LABEL_LEN];
	int choice_aux_offset, exec_aux_offset;
	print_menu(win);
	strcpy(temp1,"hello   0\0");
	strcpy(temp2,"whatsup?0\0");
	for(i = 0;i < 5;i++)
		sample_numbers[i] = 100+(i*10);
//	memset(aux_string,0,AUX_STRING_LEN);
	for(i = 0;i < AUX_STRING_LEN;i++)
		aux_string[i] = i;

	do {
		key = wgetch(win);
		wkey = get_keypress(key,win, display_offset);
		if(wkey != 0xff)
		{
			switch(wkey)
			{
				case SET_DATA1:
//					memset(aux_string+NUM_CHECKBOXES,0,AUX_STRING_LEN-NUM_CHECKBOXES);
//					choice_aux_offset = NUM_CHECKBOXES+1;
					choice_aux_offset = 0;
//					mvwprintw(win, LAST_ROW-10,1,"choice_aux_offset       %d  ",choice_aux_offset);
					memcpy(aux_string+choice_aux_offset,temp1,MAX_LABEL_LEN);
					temp1[8] = '1';
					choice_aux_offset += MAX_LABEL_LEN;
					memcpy(aux_string+choice_aux_offset,temp1,MAX_LABEL_LEN);
					temp1[8] = '2';
					choice_aux_offset += MAX_LABEL_LEN;
					memcpy(aux_string+choice_aux_offset,temp1,MAX_LABEL_LEN);
					temp1[8] = '3';
					choice_aux_offset += MAX_LABEL_LEN;
					memcpy(aux_string+choice_aux_offset,temp1,MAX_LABEL_LEN);
					temp1[8] = '4';
					choice_aux_offset += MAX_LABEL_LEN;
					memcpy(aux_string+choice_aux_offset,temp1,MAX_LABEL_LEN);
					temp1[8] = '5';
					choice_aux_offset += MAX_LABEL_LEN;
					memcpy(aux_string+choice_aux_offset,temp1,MAX_LABEL_LEN);
					temp1[8] = '6';
					choice_aux_offset += MAX_LABEL_LEN;
					memcpy(aux_string+choice_aux_offset,temp1,MAX_LABEL_LEN);
					temp1[8] = '7';
					choice_aux_offset += MAX_LABEL_LEN;
					memcpy(aux_string+choice_aux_offset,temp1,MAX_LABEL_LEN);
					temp1[8] = '8';
					choice_aux_offset += MAX_LABEL_LEN;
					memcpy(aux_string+choice_aux_offset,temp1,MAX_LABEL_LEN);
					temp1[8] = '9';
					choice_aux_offset += MAX_LABEL_LEN;
					memcpy(aux_string+choice_aux_offset,temp1,MAX_LABEL_LEN);
					type = 1;
					size = MAX_LABEL_LEN*NUM_CHECKBOXES;
					mvwprintw(win, LAST_ROW_DISP-1,1,"data1 set       %d  ",choice_aux_offset);
					wrefresh(win);
					break;
				case SET_DATA2:
//					memset(aux_string+NUM_CHECKBOXES,0,AUX_STRING_LEN-NUM_CHECKBOXES);
//					exec_aux_offset = NUM_CHECKBOXES+1+(MAX_LABEL_LEN*NUM_EXECCHOICES);
					exec_aux_offset = 0;
					// here we set size to include both checkboxes and execchoices
					// to be more efficient, copy exec choices to beginning of aux_string
//					size = MAX_LABEL_LEN*NUM_EXECCHOICES+exec_aux_offset;
					size = MAX_LABEL_LEN*NUM_EXECCHOICES;
//					mvwprintw(win, LAST_ROW-10,1,"exec_aux_offset       %d  ",exec_aux_offset);
					memcpy(aux_string+exec_aux_offset,temp2,MAX_LABEL_LEN);
					temp2[8] = '1';
					exec_aux_offset += MAX_LABEL_LEN;
					memcpy(aux_string+exec_aux_offset,temp2,MAX_LABEL_LEN);
					temp2[8] = '2';
					exec_aux_offset += MAX_LABEL_LEN;
					memcpy(aux_string+exec_aux_offset,temp2,MAX_LABEL_LEN);
					temp2[8] = '3';
					exec_aux_offset += MAX_LABEL_LEN;
					memcpy(aux_string+exec_aux_offset,temp2,MAX_LABEL_LEN);
					temp2[8] = '4';
					exec_aux_offset += MAX_LABEL_LEN;
					memcpy(aux_string+exec_aux_offset,temp2,MAX_LABEL_LEN);
					temp2[8] = '5';
					exec_aux_offset += MAX_LABEL_LEN;
					memcpy(aux_string+exec_aux_offset,temp2,MAX_LABEL_LEN);
					temp2[8] = '6';
					exec_aux_offset += MAX_LABEL_LEN;
					memcpy(aux_string+exec_aux_offset,temp2,MAX_LABEL_LEN);
					temp2[8] = '7';
					exec_aux_offset += MAX_LABEL_LEN;
					memcpy(aux_string+exec_aux_offset,temp2,MAX_LABEL_LEN);
					temp2[8] = '8';
					exec_aux_offset += MAX_LABEL_LEN;
					memcpy(aux_string+exec_aux_offset,temp2,MAX_LABEL_LEN);
					temp2[8] = '9';
					exec_aux_offset += MAX_LABEL_LEN;
					memcpy(aux_string+exec_aux_offset,temp2,MAX_LABEL_LEN);
					type = 2;
					mvwprintw(win, LAST_ROW_DISP-1,1,"data2 set       %d  ",exec_aux_offset);
					wrefresh(win);
					break;
				case SET_DATA3:
					strcpy(temp1,"wtf? ch 0\0");
					for(i = 0;i < 11;i++)
						mvwprintw(win, LAST_ROW-30+i,2,"                                   ");
					wrefresh(win);
					break;
				case SET_DATA4:
					strcpy(temp2,"asdf ch 0\0");
					for(i = 0;i < 11;i++)
						mvwprintw(win, LAST_ROW-30+i,2,"                                   ");
					wrefresh(win);
					break;
				case SET_DATA5:
					for(i = 0;i < AUX_STRING_LEN;i++)
						aux_string[i] = i;
					size = AUX_STRING_LEN;
					for(i = 0;i < 11;i++)
						mvwprintw(win, LAST_ROW_DISP-1+i,2,"                                   ");
					wrefresh(win);
					type = 3;
					break;
				case PUSH_DATA:
					mvwprintw(win, LAST_ROW_DISP-1,1,"data pushed    ");
					wrefresh(win);
					get_key(wkey,size,start_addr, aux_string,type);
					ret_key = 0xff;
					k = j = 0;
					for(i = 0;i < 11;i++)
						mvwprintw(win, LAST_ROW_DISP-20+i,2,"                                   ");
					for(i = 0;i < AUX_STRING_LEN;i++)
					{
						mvwprintw(win, LAST_ROW_DISP+j-20, 2+k,"%c",aux_string[i]);
						if(++k > 30)
						{
							k = 0;
							++j;
						}
					}
					wrefresh(win);
					break;
				case INIT:
					mvwprintw(win, LAST_ROW_DISP-1,1,"init          ");
					wrefresh(win);
					memset(aux_string,0,AUX_STRING_LEN);
					size = AUX_STRING_LEN;
					type = 0;
					strcpy(temp1,"test ch 0\0");
					strcpy(temp2,"exec ch 0\0");
					get_key(wkey,size,start_addr,aux_string,type);
					ret_key = 0xff;
					break;
				case READ_MENUSTR:
					start_addr = start_menu_structs;
					size = no_menu_structs*sizeof(MENU_FUNC_STRUCT);
					get_key(wkey,size,start_addr,aux_string,type);
					j = 30;
					for(i = 0;i < no_menu_structs;i++)
					{
						read(global_fd,&mf,sizeof(MENU_FUNC_STRUCT));
						memcpy(&mf,eeprom_sim+(sizeof(MENU_FUNC_STRUCT)*i)+start_menu_structs,sizeof(MENU_FUNC_STRUCT));
						mvwprintw(win, display_offset+j,2,
							"%2d %2d %2d %2d %2d %2d %2d %2d",pmf->fptr,pmf->menus[0],pmf->menus[1],
								pmf->menus[2],pmf->menus[3],pmf->menus[4],pmf->menus[5],pmf->index);
						j++;
					}	
					break;
				case READ_EEPROM:
					type = 3;
					size = EEPROM_SIZE-2;
					start_addr = 0;
//					start_addr = EEPROM_SIZE/2;
//					start_addr = menu_offset;
//					size = rt_params_offset - menu_offset;
					get_key(wkey,size,start_addr,aux_string,type);
					for(i = start_addr;i < size+start_addr;i++)
//						read(global_fd,&aux_string[i],1);
						read(global_fd,&eeprom_sim[i],1);

					j = k = 0;
					for(i = 0;i < 22;i++)
						mvwprintw(win, LAST_ROW_DISP-35+i,24,"                                   ");
					for(i = start_addr;i < size+start_addr;i++)
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
					j++;
					k = 0;
					for(i = NO_MENU_LABELS_EEPROM_LOCATION;i < NO_MENUS_EEPROM_LOCATION;i++,k++)
					{
						mvwprintw(win, LAST_ROW_DISP-6,24+(k*3),"%d ", eeprom_sim[i]);
					}
					wrefresh(win);
					break;
				case BURN_EEPROM:
					size = EEPROM_SIZE/2;
					start_addr = 0;
					burn_eeprom2();
//					start_addr = EEPROM_SIZE/2;
//					start_addr = menu_offset;
//					size = rt_params_offset - menu_offset;
					get_key(wkey,size,start_addr,aux_string,type);
					for(i = start_addr;i < size+start_addr;i++)
//						read(global_fd,&aux_string[i],1);
						write(global_fd,&eeprom_sim[i],1);
//					burn_eeprom2();
					break;	
				case SPACE:
					size = 0;
					type = 0;
					get_key(wkey,size,start_addr,aux_string,type);
					for(i = 1;i < LAST_ROW+1;i++)
						mvwprintw(win, i,2,"                                                            ");
					print_menu(win);
					break;
				default:
					wrefresh(win);

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
	if(key != 0xff)
	{
		switch(key)
		{
			case '0':
				mvwprintw(win, display_offset,50, "zero    ");
				wkey = KP_0;
				break;
			case '1':
				mvwprintw(win, display_offset,50, "one     ");
				wkey = KP_1;
				break;
			case '2':
				mvwprintw(win, display_offset,50, "two     ");
				wkey = KP_2;
				break;
			case '3':
				mvwprintw(win,display_offset,50, "three    ");
				wkey = KP_3;
				break;
			case '4':
				mvwprintw(win, display_offset,50, "four    ");
				wkey = KP_4;
				break;
			case '5':
				mvwprintw(win, display_offset,50, "five    ");
				wkey = KP_5;
				break;
			case '6':
				mvwprintw(win, display_offset,50, "six     ");
				wkey = KP_6;
				break;
			case '7':
				mvwprintw(win, display_offset,50, "seven   ");
				wkey = KP_7;
				break;
			case '8':
				mvwprintw(win, display_offset,50, "eight   ");
				wkey = KP_8;
				break;
			case '9':
				mvwprintw(win, display_offset,50, "nine    ");
				wkey = KP_9;
				break;
			case '*':
				mvwprintw(win, display_offset,50, "ast     ");
				wkey = KP_AST;
				break;
			case '#':
				mvwprintw(win, display_offset,50, "pound   ");
				wkey = KP_POUND;
				break;
			case 'A':
			case 'a':
				mvwprintw(win, display_offset,50, "A       ");
				wkey = KP_A;
				break;
			case 'B':
			case 'b':
				mvwprintw(win, display_offset,50, "B       ");
				wkey = KP_B;
				break;
			case 'C':
			case 'c':
				mvwprintw(win, display_offset,50, "C       ");
				wkey = KP_C;
				break;
			case 'D':
			case 'd':
				mvwprintw(win, display_offset,50, "D       ");
				wkey = KP_D;
				break;
// use 'z' as a shortcut to '*' and 'y' as a shortcut to '#'
			case 'Y':
			case 'y':
				mvwprintw(win, display_offset,50, "pound   ");
				wkey = KP_POUND;
				break;
			case 'Z':
			case 'z':
				mvwprintw(win, display_offset,50, "ast     ");
				wkey = KP_AST;
				break;
			case 'U':
			case 'u':
				mvwprintw(win, display_offset,50,"SET_DATA3 ");
				wkey = SET_DATA3;
				break;
			case 'V':
			case 'v':
				mvwprintw(win, display_offset,50,"SET_DATA5  ");
				wkey = SET_DATA5;
				break;
			case 'R':
			case 'r':
				mvwprintw(win, display_offset,50,"SET_DATA1 ");
				wkey = SET_DATA1;
				break;
			case 'S':
			case 's':
				mvwprintw(win, display_offset,50,"SET_DATA2 ");
				wkey = SET_DATA2;
				break;
			case 'T':
			case 't':
				mvwprintw(win, display_offset,50,"PUSH_DATA ");
				wkey = PUSH_DATA;
				break;
			case 'E':
			case 'e':
				mvwprintw(win, display_offset,50,"READ_EEPROM ");
				wkey = READ_EEPROM;
				break;
			case 'P':
			case 'p':
				mvwprintw(win, display_offset,50,"READ_MENUSTR ");
				wkey = READ_MENUSTR;
				break;
			case 'X':
			case 'x':
				mvwprintw(win, display_offset,50,"BURN_EEPROM ");
				wkey = BURN_EEPROM;
				break;
			case 'I':
			case 'i':
				mvwprintw(win, display_offset,50,"INIT ");
				wkey = INIT;
				break;
			case ' ':
				mvwprintw(win, display_offset,50,"space     ");
				wkey = SPACE;
				break;
			case PUSH_DATA:
			default:
				mvwprintw(win, display_offset,50, "?     ");
				wkey = 0xff;
				break;
		}
	}
	return wkey;
}

static void print_menu(WINDOW *win)
{
//	mvwprintw(win, LAST_ROW-6,LAST_COL,"*");
	mvwprintw(win, LAST_ROW-11,1,"----------------------------- menu ----------------------------");
	mvwprintw(win, LAST_ROW-10,1,"0->9, A->D, # and * are the keys on the keypad");
	mvwprintw(win, LAST_ROW-9,1,"Z/z is a shortcut to '*', Y/y is a shortcut to '*'");
	mvwprintw(win, LAST_ROW-8,1,"R/r is a shortcut to SET_DATA1");
	mvwprintw(win, LAST_ROW-7,1,"S/s is a shortcut to SET_DATA2");
	mvwprintw(win, LAST_ROW-6,1,"T/t is a shortcut to PUSH_DATA");
	mvwprintw(win, LAST_ROW-5,1,"I/i is a shortcut to INIT");
	mvwprintw(win, LAST_ROW-4,1,"E/e is a shortcut to READ_EEPROM");
	mvwprintw(win, LAST_ROW-3,1,"X/x is a shortcut to BURN_EEPROM");
	mvwprintw(win, LAST_ROW-2,1,"P/p is a shortcut to READ_MENUSTR");
	mvwprintw(win, LAST_ROW-1,1,"<space> = blank screen");
	mvwprintw(win, LAST_ROW,1,"the '*' key always goes to the previous menu");
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

