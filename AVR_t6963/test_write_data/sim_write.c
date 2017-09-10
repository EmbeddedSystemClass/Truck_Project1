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

UCHAR *peeprom_sim;

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
	peeprom_sim = (UCHAR *)&eeprom_sim;

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
/*
	for(i = 0;i < TOTAL_NUM_CHECKBOXES;i++)
	{
		printf("%d %d %s\n",check_boxes[i].index,check_boxes[i].checked,check_boxes[i].string);
	}
	exit(1);
*/
    memset(eeprom_sim,0,EEPROM_SIZE);

	memcpy((void*)(eeprom_sim+NO_RT_LABELS_EEPROM_LOCATION),(void*)&no_rt_labels,sizeof(UINT));
	memcpy((void*)(eeprom_sim+NO_MENU_LABELS_EEPROM_LOCATION),(void*)&no_menu_labels,sizeof(UINT));
	memcpy((void*)(eeprom_sim+NO_RTPARAMS_EEPROM_LOCATION),(void*)&no_rtparams,sizeof(UINT));	// reserve an extra sample_data space for in case of 'escape'
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
	init_list();
	size = 0;
	start_addr = 0;
//	char temp1[MAX_LABEL_LEN];
//	char temp2[MAX_LABEL_LEN];
	int choice_aux_offset, exec_aux_offset;
	print_menu(win);
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
				case INIT:
					mvwprintw(win, LAST_ROW_DISP-1,1,"init          ");
					wrefresh(win);
					memset(aux_string,0,AUX_STRING_LEN);
					size = sizeof(CHECKBOXES)*(NUM_CHECKBOXES*4);
					type = 0;
//					for(i = 0;i < NUM_CHECKBOXES*4;i++)
//						memcpy(aux_string+(sizeof(CHECKBOXES)*i),pcheckboxes+(sizeof(CHECKBOXES)*i),sizeof(CHECKBOXES));

					get_key(wkey,size,start_addr,aux_string,type);
					ret_key = 0xff;
					break;
				case READ_EEPROM:
#if 0
					strcpy(filename,"eeprom.bin\0");
					fp = open((const char *)filename, O_WRONLY | O_CREAT, S_IRUSR | S_IRUSR | S_IRGRP | S_IWGRP);
					if(fp < 0)
					{
						mvwprintw(win, LAST_ROW_DISP,2,"can't open file for writing");
						wrefresh(win);
						getch();
					}
#endif
					type = 3;
					size = EEPROM_SIZE;
					start_addr = 0;
//					start_addr = EEPROM_SIZE/2;
//					start_addr = menu_offset;
//					size = rt_params_offset - menu_offset;
					memset(eeprom_sim,0,EEPROM_SIZE);
					get_key(wkey,size,start_addr,eeprom_sim,type);
//					for(i = start_addr;i < size+start_addr;i++)
//						read(global_fd,&eeprom_sim[i],1);

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
					// write current eeprom image to file
#if 0
					if(fp > 0)
					{
						for(i = start_addr;i < size;i++)
							write(fp,&eeprom_sim[i],1);
						close(fp);
					}
#endif
					break;
				case BURN_EEPROM:
					size = EEPROM_SIZE;
					start_addr = 0;
//					start_addr = EEPROM_SIZE/2;
//					start_addr = menu_offset;
//					size = rt_params_offset - menu_offset;
					strcpy(filename,"eeprom.bin\0");
					if(access(filename,F_OK) != -1)
					{
						fp = open((const char *)filename, O_RDWR);
						if(fp < 0)
						{
							mvwprintw(win, LAST_ROW_DISP,2,"can't open file for writing");
							wrefresh(win);
							getch();
						}else
						{
							res = 0;
							lseek(fp,0,SEEK_SET);
//							for(i = start_addr;i < size;i++)
							res = read(fp,eeprom_sim,size);
							close(fp);
							mvwprintw(win, LAST_ROW_DISP,2,"reading file into eeprom_sim: %d   ",res);
							j = k = 0;
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
						}
					}else
					{
						mvwprintw(win, LAST_ROW_DISP,2,"creating new eeprom");
						burn_eeprom();
					}
					get_key(wkey,size,start_addr,peeprom_sim,type);
//					for(i = start_addr;i < size+start_addr;i++)
//						write(global_fd,&eeprom_sim[i],1);
					goffset = 0;
					get_label_offsets();

					break;
				case BURN_PART:
					size = 234;
					start_addr = 0;
					strcpy(filename,"eeprom.bin\0");
					mvwprintw(win, LAST_ROW_DISP,2,"starting burn part   ");
					if(access(filename,F_OK) != -1)
					{
						fp = open((const char *)filename, O_RDWR);
						if(fp < 0)
						{
							mvwprintw(win, LAST_ROW_DISP,2,"can't open file for writing");
							wrefresh(win);
							getch();
						}else
						{
							res = 0;
							lseek(fp,0,SEEK_SET);
//							for(i = start_addr;i < size;i++)
							res = read(fp,eeprom_sim,EEPROM_SIZE);
							close(fp);
							mvwprintw(win, LAST_ROW_DISP,2,"reading part into eeprom_sim: %d   ",res);
							j = k = 0;
						}
					}else
					{
						mvwprintw(win, LAST_ROW_DISP,2,"creating new eeprom");
						burn_eeprom();
					}
					get_key(wkey,size,start_addr,peeprom_sim,type);

//					for(i = start_addr;i < size+start_addr;i++)
//						write(global_fd,&eeprom_sim[i],1);
					goffset = 0;
					get_label_offsets();

					break;
				case SHOW_EEPROM:
					for(i = 1;i < LAST_ROW+1;i++)
						mvwprintw(win, i,2,"                                                            ");
					j = k = 0;
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
					print_menu(win);
					goffset = 0;
					get_label_offsets();
					break;
				case SHOW_MENU_STRUCT:
					for(i = 1;i < LAST_ROW+1;i++)
						mvwprintw(win, i,2,"                                                            ");

					k = 0;
					j = 1;
					for(i = 2;i < LAST_ROW_DISP+1;i++)
						mvwprintw(win, i,2,"                                                            ");
					for(i = 0;i < no_menu_structs;i++)
					{
						mvwprintw(win, display_offset+i,2,"%d ",menu_structs[i].fptr);
						for(j = 0;j < 6;j++)
						{
							get_label(menu_structs[i].menus[j],temp_label);
							mvwprintw(win, display_offset+i,6+(j*8),"%s ", temp_label);
//							mvwprintw(win, display_offset+i,6+(j*8),"%d ",menu_structs[i].menus[j]);
						}
					}
					k = 0;
					j = 17;
					for(i = 0;i < no_menu_structs;i++)
					{
						memcpy(&mf,&menu_structs[i],sizeof(MENU_FUNC_STRUCT));
						mvwprintw(win, display_offset+j,2,
							"%2d %2d %2d %2d %2d %2d %2d %2d",pmf->fptr,pmf->menus[0],pmf->menus[1],
								pmf->menus[2],pmf->menus[3],pmf->menus[4],pmf->menus[5],pmf->index);
						j++;
					}
					mvwprintw(win, LAST_ROW_DISP-1,2,
						"no_menu_structs: %d  ",no_menu_structs);
					wrefresh(win);
					print_menu(win);
					break;
				case LOAD_MENU_STRUCT:
					size = 0;
					start_addr = 0;
					type = 0;
					get_key(wkey,size,start_addr,peeprom_sim,type);
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
//***************************************** burn_eeprom ************************************//
//******************************************************************************************//
int burn_eeprom(void)
{
	int i;
	no_rt_labels = 0;
	no_menu_labels = 0;
	no_rtparams = 0;
	total_offset = 0;

    i = 0;
	i = update_labels(i,"home\0");
	i = update_labels(i,"MENU1a\0");
	i = update_labels(i,"MENU1b\0");
	i = update_labels(i,"MENU1c\0");
	i = update_labels(i,"MENU1d\0");
	i = update_labels(i,"MENU1e\0");
	i = update_labels(i,"MENU2a\0");
	i = update_labels(i,"MENU2B\0");
	i = update_labels(i,"MENU2c\0");
	i = update_labels(i,"MENU2d\0");
	i = update_labels(i,"MENU2e\0");
	i = update_labels(i,"MENU3a\0");
	i = update_labels(i,"MENU3b\0");

	i = update_labels(i,"enter\0");
	i = update_labels(i,"up\0");
	i = update_labels(i,"down\0");
	i = update_labels(i,"toggle\0");
	i = update_labels(i,"esc\0");

	i = update_labels(i,"enter\0");
	i = update_labels(i,"forward\0");
	i = update_labels(i,"back\0");
	i = update_labels(i,"clear\0");
	i = update_labels(i,"escape\0");

	i = update_labels(i,"caps\0");
	i = update_labels(i,"small\0");
	i = update_labels(i,"spec\0");
	i = update_labels(i,"next\0");

	i = update_labels(i,"\0");
	menu_offset = total_offset;
	no_menu_labels = i;

	i = update_labels(i,"RPM\0");
	i = update_labels(i,"ENG TEMP\0");
	i = update_labels(i,"TRIP\0");
	i = update_labels(i,"TIME\0");
	i = update_labels(i,"AIR TEMP\0");
	i = update_labels(i,"MPH\0");
	i = update_labels(i,"OIL PRES\0");
	i = update_labels(i,"MAP\0");
	i = update_labels(i,"OIL TEMP\0");
	i = update_labels(i,"O2\0");
	i = update_labels(i,"test\0");
	no_rt_labels = i - no_menu_labels;
//	choice_offset = i;
	rt_params_offset = total_offset;
	i = 0;

#ifndef TEST_WRITE_DATA
	eeprom_update_word((UINT *)NO_RT_LABELS_EEPROM_LOCATION,no_rt_labels);
	printString("\r\nno_rt_labels: ");
	printHexByte((UCHAR)no_rt_labels>>8);
	printHexByte((UCHAR)no_rt_labels);
	printString("\r\ndone writing no_rt_labels to eeprom\r\n");

	eeprom_update_word((UINT *)NO_MENU_LABELS_EEPROM_LOCATION,no_menu_labels);
	printString("\r\nno_menu_labels: ");
	printHexByte((UCHAR)no_menu_labels>>8);
	printHexByte((UCHAR)no_menu_labels);
	printString("\r\ndone writing no_menu_labels to eeprom\r\n");

/*
	eeprom_update_word((UINT *)RTPARAMS_OFFSET_EEPROM_LOCATION_LSB,total_offset);
	printString("\r\total_offset: ");
	printHexByte((UCHAR)total_offset>>8);
	printHexByte((UCHAR)total_offset);
	printString("\r\n");
*/
#else
//	memcpy((void*)(eeprom_sim+NO_RT_LABELS_EEPROM_LOCATION),(void*)&no_rt_labels,sizeof(UINT));
//	memcpy((void*)(eeprom_sim+NO_MENU_LABELS_EEPROM_LOCATION),(void*)&no_menu_labels,sizeof(UINT));
#endif
	total_offset = 0;

	i = update_rtparams(i, 1, 0, SHOWN_SENT, 1, RT_RPM);	// first label is at offset 0
	i = update_rtparams(i, 2, 0, SHOWN_SENT, 0, RT_ENGT);
	i = update_rtparams(i, 3, 0, SHOWN_SENT, 0, RT_TRIP);	// first element of offset_array has offset of 2nd label
	i = update_rtparams(i, 4, 0, SHOWN_SENT, 0, RT_TIME);
	i = update_rtparams(i, 5, 0, SHOWN_SENT, 0, RT_AIRT);
	i = update_rtparams(i, 1, 15, SHOWN_SENT, 0, RT_MPH);
	i = update_rtparams(i, 2, 15, SHOWN_SENT, 0, RT_OILP);
	i = update_rtparams(i, 3, 15, SHOWN_SENT, 0, RT_MAP);
	i = update_rtparams(i, 4, 15, SHOWN_SENT, 0, RT_OILT);
	i = update_rtparams(i, 5, 15, SHOWN_SENT, 0, RT_O2);

	no_rtparams = i;
// write to the number of rt_params location in eeprom the number of rt_params
#ifndef TEST_WRITE_DATA
	eeprom_update_word((UINT *)NO_RTPARAMS_EEPROM_LOCATION,no_rtparams);
	printString("\r\nno_rtparams: ");
	printHexByte((UCHAR)no_rtparams>>8);
	printHexByte((UCHAR)no_rtparams);
	printString("\r\ndone writing no_rtparams to eeprom\r\n");
/*
	eeprom_update_word((UINT *)MENUSTRUCT_OFFSET_EEPROM_LOCATION_LSB,total_offset);
	printString("\r\total_offset: ");
	printHexByte((UCHAR)total_offset>>8);
	printHexByte((UCHAR)total_offset);
	printString("\r\n");
*/
#else
//	memcpy((void*)(eeprom_sim+NO_RTPARAMS_EEPROM_LOCATION),(void*)&no_rtparams,sizeof(UINT));
#endif

	i = 0;
	total_offset = 0;
//												'A' 	'B'		'C'		'D'		'#'		'0'
	i = update_menu_structs(i, _menu_change, 	MENU1A, MENU1B, MENU1C, MENU1D, MENU2A, MENU2B,  MAIN);
// 1a
	i = update_menu_structs(i, _menu_change,	MENU2C, MENU2D, MENU2E, MENU3A, MENU3B, MENU1B, MENU1A);
// 1b
	i = update_menu_structs(i, _menu_change,	MAIN,   MENU2D, MENU1B, MENU1D, MENU2A, MENU2B, MENU1B);
// 1c
	i = update_menu_structs(i, _do_chkbox, 		ckup, ckdown, cktoggle, ckenter, ckesc, blank, MENU1C);
// 1d
	i = update_menu_structs(i, _do_chkbox, 		ckup, ckdown, cktoggle, ckenter, ckesc, blank, MENU1D);
// 1e
	i = update_menu_structs(i, _non_func,		test, blank, blank,   blank, blank, blank, MENU1E);
// 2a
	i = update_menu_structs(i, _exec_choice,	ckup, ckdown, ckenter, blank, blank, blank, MENU2A);
// 2b
	i = update_menu_structs(i, _exec_choice,	ckup, ckdown, ckenter, blank, blank, blank, MENU2B);
// 2c
	i = update_menu_structs(i, _do_numentry, 	forward, back, eclear, entr, esc, blank, MENU2C);
// 2d
	i = update_menu_structs(i, _do_numentry, 	forward, back, eclear, entr, esc, blank, MENU2D);
// 2e
	i = update_menu_structs(i, _do_numentry, 	forward, back, eclear, entr, esc, blank, MENU2E);
// 3a
	i = update_menu_structs(i, _do_numentry, 	forward, back, eclear, entr, esc, blank, MENU3A);
// 3b
	i = update_menu_structs(i, _do_numentry, 	forward, back, eclear, entr, esc, blank, MENU3B);

	no_menu_structs = i;
	return 0;
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
				mvwprintw(win, display_offset,50,"");
				wkey = 0xff;
				break;
			case 'V':
			case 'v':
				mvwprintw(win, display_offset,50,"BURN_PART ");
				wkey = BURN_PART;
				break;
			case 'R':
			case 'r':
				mvwprintw(win, display_offset,50,"SHOW_MENUs");
				wkey = SHOW_MENU_STRUCT;
				break;
			case 'S':
			case 's':
				mvwprintw(win, display_offset,50,"SHOW_EEPROM   ");
				wkey = SHOW_EEPROM;
				break;
			case 'L':
			case 'l':
				mvwprintw(win, display_offset,50,"LOAD_MENUs   ");
				wkey = LOAD_MENU_STRUCT;
				break;
			case 'T':
			case 't':
				mvwprintw(win, display_offset,50,"");
				wkey = 0xff;
				break;
			case 'E':
			case 'e':
				mvwprintw(win, display_offset,50,"READ_EEPROM   ");
				wkey = READ_EEPROM;
				break;
			case 'P':
			case 'p':
				mvwprintw(win, display_offset,50," ");
				wkey = 0xff;
				break;
			case 'X':
			case 'x':
				mvwprintw(win, display_offset,50,"BURN_EEPROM   ");
				wkey = BURN_EEPROM;
				break;
			case 'I':
			case 'i':
				mvwprintw(win, display_offset,50,"INIT        ");
				wkey = INIT;
				break;
			case ' ':
				mvwprintw(win, display_offset,50,"          ");
				wkey = SPACE;
				break;
			default:
				mvwprintw(win, display_offset,50, "?        ");
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
	mvwprintw(win, LAST_ROW-8,1,"I/i is a shortcut to INIT");
	mvwprintw(win, LAST_ROW-7,1,"E/e is a shortcut to READ_EEPROM");
	mvwprintw(win, LAST_ROW-6,1,"X/x is a shortcut to BURN_EEPROM");
	mvwprintw(win, LAST_ROW-5,1,"S/s is a shortcut to SHOW_EEPROM");
	mvwprintw(win, LAST_ROW-4,1,"R/r is a shortcut to SHOW_MENU_STRUCT");
	mvwprintw(win, LAST_ROW-3,1,"L/l is a shortcut to LOAD_MENU_STRUCT");
	mvwprintw(win, LAST_ROW-2,1,"V/v is a shortcut to BURN_PART");
	mvwprintw(win, LAST_ROW-1,1,"<space> = blank screen");
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

