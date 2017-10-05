#ifndef MAIN_C
#include "sfr_helper.h"
#ifdef TEST_WRITE_DATA
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <ncurses.h>
WINDOW *win;
#endif
#include "t6963.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "pic_main.h"

static UCHAR menu_change(UCHAR ch);
static UCHAR do_exec(UCHAR ch);
static UCHAR do_chkbox(UCHAR ch);
static UCHAR init_checkboxes(int index);
static UCHAR non_func(UCHAR ch);
static UCHAR do_numentry(UCHAR ch);
static UCHAR do_init(UCHAR ch);
static int checkboxes_reset(int index);
static void init_numentry(int menu_index);
static void cursor_forward_stuff(char x);
static void cursor_forward(char x);
static void cursor_backward(void);
static void _eclear(void);
static UCHAR backspace(UCHAR ch);
static UCHAR enter(UCHAR ch);
static UCHAR escape(UCHAR ch);
static void stuff_num(char num);
static UCHAR generic_menu_function(UCHAR ch);
static int toggle_checkboxes(int index);
static int scrollup_checkboxes(int index);
static int scrolldown_checkboxes(int index);
static int scrollup_execchoice(UCHAR ch);
static int scrolldown_execchoice(UCHAR ch);

static int first_menu = MAIN;
static int current_fptr;		// pointer into the menu_list[]
static int list_size;
static int curr_type;
static int scroll_ptr;
static int cur_alnum_col;
#ifdef TEST_WRITE_DATA
static void display_menus(int index);
#endif
static UCHAR cur_row, cur_col;	// used by the current menu/dialog function to keep track of the current row,col
static UCHAR alnum_array[NUM_ALNUM+1];
static UCHAR choose_alnum;
static int prev_list(void);
static int menu_list[LIST_SIZE];	// works like a stack for the how deep into the menus we go
static UCHAR set_list(int fptr);
static UCHAR get_fptr(void);
static void get_fptr_label(char *str);
static int get_curr_menu_index(void);
static UCHAR random_data;

static UCHAR (*fptr[NUM_FPTS])(UCHAR) = { menu_change, do_chkbox, do_exec, non_func, do_numentry, do_init };
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static int prev_list(void)
{
	int ret;

	if(current_fptr == 0)
		ret = -1;
	else
	{
		menu_list[current_fptr] = 0;
		current_fptr--;
		ret = 0;
		display_menus(get_curr_menu());
	}
	return ret;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static UCHAR set_list(int fptr)
{
	int i,k;
	UCHAR ret_char = 0;
	char tlabel[MAX_LABEL_LEN];

	if(current_fptr < list_size)
	{
		current_fptr++;
		menu_list[current_fptr] = fptr;

		mvwprintw(win, LAST_ROW_DISP-7,2, "set_list   %d %d           ",
			get_curr_menu(),
			menu_structs[get_curr_menu()].fptr);
		wrefresh(win);

		if(menu_structs[get_curr_menu()].fptr != _menu_change)
		{

			wrefresh(win);
			k = get_curr_menu_index();
			switch(k)
			{
				case MENU1C:
				case MENU1D:
					init_checkboxes(k);
					mvwprintw(win, LAST_ROW_DISP-6,2, "init_chk 1  %d  %d  ",
						k,
						ret_char);
				break;
				case MENU1E:
				case MENU2A:
				case MENU2B:
					init_checkboxes(k);
					mvwprintw(win, LAST_ROW_DISP-6,2, "init_chk 2  %d  %d  ",
						k,
						ret_char);
				break;
				case MENU2C:
				case MENU2D:
				case MENU2E:
				case MENU3A:
				case MENU3B:
					init_numentry(ret_char);
				break;
				default:
				break;
			}
		}
		else display_menus(get_curr_menu_index());


	}
	return ret_char;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static UCHAR menu_change(UCHAR ch)
{
	UCHAR ret_char = ch;
	int menu_index = 0;
	int curr_menu;

	curr_menu = get_curr_menu();
	switch(ch)
	{
		case KP_A:
		break;
		case KP_B:
		menu_index++;
		break;
		case KP_C:
		menu_index += 2;
		break;
		case KP_D:
		menu_index += 3;
		break;
		case KP_POUND:
		menu_index += 4;
		break;
		case KP_0:
		menu_index += 5;
		break;
		case KP_AST:
		prev_list();
		break;
		default:
		break;
	}
	if(ch != KP_AST)
		ret_char = set_list(menu_structs[curr_menu].menus[menu_index]);

#ifdef TEST_WRITE_DATA
	mvwprintw(win, DISP_OFFSET+1,2, "menu_change   %x    ",ch);
	wrefresh(win);
#endif
	return ret_char;
}
//******************************************************************************************//
//********************************* generic_menu_function **********************************//
//******************************************************************************************//
static UCHAR generic_menu_function(UCHAR ch)
{
	UCHAR ret_char;
	int i,j;
	UCHAR temp2;
	int temp3;
	char tlabel[MAX_LABEL_LEN];
	UCHAR low_byte, high_byte;
	UCHAR code = 0;
	int res;
	UCHAR prompt_size;
	int start_addr;

	// execute function pointer
	ret_char = (*fptr[menu_structs[get_curr_menu()].fptr])(ch);

#ifdef TEST_WRITE_DATA
	get_mlabel(get_curr_menu(),tlabel);
	mvwprintw(win, DISP_OFFSET+3, 2,"ch: %x label: %s curr menu %d func: %d            ",
			ch,
			tlabel,
			get_curr_menu(),
//			get_fptr_label(),
			menu_structs[get_curr_menu()].fptr);
	wrefresh(win);

//	write(global_fd,&ch,1);
	write(global_fd,&ret_char,1);
	usleep(tdelay);
	temp2 = get_fptr();
	write(global_fd,&temp2,1);
	usleep(tdelay);
	temp3 = get_curr_menu_index();

	start_addr = temp3 * 10;	// just for testing

	unpack(temp3,&low_byte, &high_byte);
	write(global_fd,&high_byte,1);
	usleep(tdelay);
	write(global_fd,&low_byte,1);
	usleep(tdelay);

	for(i = 0;i < 6;i++)
	{
		unpack(menu_structs[get_curr_menu()].menus[i],&low_byte,&high_byte);
		write(global_fd, &high_byte,1);
		usleep(tdelay);
		write(global_fd, &low_byte,1);
		usleep(tdelay);
//		mvwprintw(win, DISP_OFFSET+22+i, 2,"%d  ",
//		menu_structs[get_curr_menu()].menus[i]);
	}

//	mvwprintw(win, DISP_OFFSET+3, 1,"index: %d  no_prompts: %d       ",
//		get_curr_menu(),no_prompts);

	wrefresh(win);
	return ret_char;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
// pass in the keypress from the PIC24 keypad and pass out the string of UCHARs
UCHAR get_key(UCHAR ch, int size, int start_addr, UCHAR *str, int type)
{
	int i,j,k;
	UCHAR ret_char = ch;
	int res = 0;
	UCHAR low_byte, high_byte;
	void *vptr;
	int fp;
	char filename[20];

	switch(ret_char)
	{
		case INIT:		// 'i'
			init_list();
			break;

		case TEST1:		// 'g'
		case TEST2:		// 'h'
		case TEST3:		// 'j'
		case TEST4:		// 'k'
			write(global_fd,&ret_char,1);
			mvwprintw(win, LAST_ROW_DISP-3,1,"size: %d  start_addr: %d                   ",size,start_addr);
			break;

		case TEST5:		// 'l'  fill aux_string with random data and write to target
//			write(global_fd,&ret_char,1);
			for(i = 0;i < AUX_STRING_LEN;i++)
				aux_string[i] = ~i - random_data;
			random_data++;
			k = j = 0;
			for(i = 0;i < AUX_STRING_LEN;i++)
			{
				mvwprintw(win, LAST_ROW_DISP+j-45, 7+(k*3),"%2x ",aux_string[i]);
				if(++k > 15)
				{
					k = 0;
					++j;
					mvwprintw(win, LAST_ROW_DISP+j-45,1,"%2d: ",i+1);
				}
			}
			res = 0;
			write(global_fd,&ret_char,1);
			for(i = 0;i < AUX_STRING_LEN;i++)
			{
				res += write(global_fd,&aux_string[i],1);
				usleep(tdelay);
			}
			mvwprintw(win, LAST_ROW_DISP-1,1,"filled aux_string with random data and wrote to target %d    ",res);
			break;

		case TEST6:		// 'm' draw border
			write(global_fd,&ret_char,1);
			mvwprintw(win, LAST_ROW_DISP-3,1,"draw border                                   ");
			break;

		case TEST7:		// 'n' print cblabels
			mvwprintw(win, LAST_ROW_DISP-3,1,"print cblabels                               ");
			write(global_fd,&ret_char,1);
			break;

		case TEST8:		// 'o' restore eeprom form eeprom.bin
			res = 0;
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
						"reading part into eeprom_sim: %d %d %d  ",res,size,start_addr);
					j = k = 0;
				}
			}
			mvwprintw(win, LAST_ROW_DISP-3,1,"TEST8  - restoring eeprom from eeprom.bin res: %d          ",res);
/*
			k = j = 0;
			for(i = 0;i < EEPROM_SIZE;i++)
			{
				if(eeprom_sim[i] < 0x7e && eeprom_sim[i] > 0x21)
					mvwprintw(win, LAST_ROW_DISP+j-45, 24+k,"%c",eeprom_sim[i]);
				else if(eeprom_sim[i] == 0)	mvwprintw(win, LAST_ROW_DISP+j-45, 24+k," ");
				else mvwprintw(win, LAST_ROW_DISP+j-45, 24+k,"_");
				if(++k > 30)
				{
					k = 0;
					++j;
				}
			}
*/
			wrefresh(win);
			break;

		case TEST9:		// 'p' copy eeprom2.bin to eeprom_sim in host
			res = 0;
			strcpy(filename,"eeprom2.bin\0");
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
					res = read(fp,eeprom_sim,EEPROM_SIZE);
					close(fp);
					mvwprintw(win, LAST_ROW_DISP-1,2,
						"reading part into eeprom_sim: %d %d %d  ",res,size,start_addr);
					j = k = 0;
				}
			}
			mvwprintw(win, LAST_ROW_DISP-1,1,"TEST9  - copy eeprom2.bin to eeprom_simp[] res: %d          ",res);

//			write(global_fd,&ret_char,1);
			break;

		case TEST10:	// 'x' read aux_string from target
			write(global_fd,&ret_char,1);
			res = 0;
			for(i = 0;i < AUX_STRING_LEN;i++)
				res += read(global_fd,&aux_string[i],1);

			k = j = 0;
			for(i = 0;i < AUX_STRING_LEN;i++)
			{
				mvwprintw(win, LAST_ROW_DISP+j-45, 7+(k*3),"%2x ",aux_string[i]);
				if(++k > 15)
				{
					k = 0;
					++j;
					mvwprintw(win, LAST_ROW_DISP+j-45,1,"%2d: ",i+1);
				}
			}
			mvwprintw(win, LAST_ROW_DISP-1,1,"read aux_string: res %d                       ",res);
			break;

		case TEST11:	// 's' - tell AVR to display pattern on screen
			write(global_fd,&ret_char,1);
			break;

		case TEST12:	// 'A'
			write(global_fd,&ret_char,1);
			break;

		case TEST13:	// 'B'
			write(global_fd,&ret_char,1);
			k = j = 0;
//			for(i = start_addr;i < size+start_addr;i++)
			for(i = 0;i < EEPROM_SIZE;i++)
			{
				if(eeprom_sim[i] < 0x7e && eeprom_sim[i] > 0x21)
					mvwprintw(win, LAST_ROW_DISP+j-45, 2+k,"%c",eeprom_sim[i]);
				else if(eeprom_sim[i] == 0)	mvwprintw(win, LAST_ROW_DISP+j-45, 2+k," ");
				else mvwprintw(win, LAST_ROW_DISP+j-45, 2+k,"_");
				if(++k > 30)
				{
					k = 0;
					++j;
				}
			}

			break;

		case TEST14:	// 'C'
			mvwprintw(win, LAST_ROW_DISP-3,1,"size: %d  start_addr: %d                   ",size,start_addr);
			write(global_fd,&ret_char,1);
			break;

		case TEST15:	// 'D'
			mvwprintw(win, LAST_ROW_DISP-3,1,"display cblabels in ascii                       ");
			k = j = 0;
			for(i = 0;i < CBLABEL_SIZE;i++)
			{
				if(cblabels[i] < 0x7e && cblabels[i] > 0x21)
					mvwprintw(win, LAST_ROW_DISP+j-45, 2+k,"%c",cblabels[i]);
				else if(cblabels[i] == 0)	mvwprintw(win, LAST_ROW_DISP+j-45, 2+k,"*");
				else mvwprintw(win, LAST_ROW_DISP+j-45, 2+k,"_");
				if(++k > 30)
				{
					k = 0;
					++j;
				}
			}

			write(global_fd,&ret_char,1);
			break;

		case TEST16:	// 'E'
			mvwprintw(win, LAST_ROW_DISP-3,1,"display cblabels in hex                         ");
			k = j = 0;
			for(i = 0;i < CBLABEL_SIZE;i++)
			{
				mvwprintw(win, LAST_ROW_DISP+j-45, 2+(k*3),"%2x ",cblabels[i]);
				if(++k > 15)
				{
					k = 0;
					++j;
				}
			}
			write(global_fd,&ret_char,1);
			break;

		case TEST17:	// 'F'
			write(global_fd,&ret_char,1);
//			no_prompts = 10;
			break;

		case LOAD_RAM:  //  'r'
			i = 0;
			total_offset = 0;
			i = update_rtparams(i, 0, 0, SHOWN_SENT, 1, RT_RPM);	// first label is at offset 0
			i = update_rtparams(i, 1, 0, SHOWN_SENT, 0, RT_ENGT);
			i = update_rtparams(i, 2, 0, SHOWN_SENT, 0, RT_TRIP);	// first element of offset_array has offset of 2nd label
			i = update_rtparams(i, 3, 0, SHOWN_SENT, 0, RT_TIME);
			i = update_rtparams(i, 4, 0, SHOWN_SENT, 0, RT_AIRT);
			i = update_rtparams(i, 0, 18, SHOWN_SENT, 0, RT_MPH);
			i = update_rtparams(i, 1, 18, SHOWN_SENT, 0, RT_OILP);
			i = update_rtparams(i, 2, 18, SHOWN_SENT, 0, RT_MAP);
			i = update_rtparams(i, 3, 18, SHOWN_SENT, 0, RT_OILT);
			i = update_rtparams(i, 4, 18, SHOWN_SENT, 0, RT_O2);

			no_rtparams = i;

			write(global_fd,&ret_char,1);
			usleep(tdelay);
			res = 0;

			for(i = 0;i < CBLABEL_SIZE;i++)
			{
				res += write(global_fd,&cblabels[i],1);
				mvwprintw(win, LAST_ROW_DISP-2,2,"%2x %2d ",cblabels[i],res);
				wrefresh(win);
				usleep(tdelay);
			}

			wrefresh(win);
			res = 0;
			low_byte = 0;

// send over the check_boxes[i].index and .checked as samples
// later this will be sent by the PIC
			for(i = 0;i < NUM_CBLABELS;i++)
			{
				low_byte = ~low_byte;
				low_byte &= 1;
				res += write(global_fd,&low_byte,1);
				check_boxes[i].checked = prev_check_boxes[i].checked =  low_byte;
//				mvwprintw(win, LAST_ROW_DISP-1,2,"%2d ",res);
//				wrefresh(win);
				usleep(tdelay);
//				j = i%NUM_CHECKBOXES;
				res += write(global_fd,&i,1);
//				check_boxes[i].index = prev_check_boxes[i].index =  low_byte;
				check_boxes[i].index = prev_check_boxes[i].index =  i;
				mvwprintw(win, LAST_ROW_DISP-1,2,"%2d ",i);
				wrefresh(win);
				usleep(tdelay);
			}

			// if these are in eeprom then this is not necessary
			//
#ifndef TESTING_AVR
			for(i = 0;i < NUM_RT_PARAMS;i++)
			{
//				write(global_fd,&rt_params[0],sizeof(RT_PARAM));
				write(global_fd,&rt_params[i].row,1);
				usleep(tdelay);
				write(global_fd,&rt_params[i].col,1);
				usleep(tdelay);
				write(global_fd,&rt_params[i].shown,1);
				usleep(tdelay);
				write(global_fd,&rt_params[i].dtype,1);
				usleep(tdelay);
				write(global_fd,&rt_params[i].type,1);
				usleep(tdelay);
			}
#endif
// sample numbers
			for(i = 0;i < 5;i++)
			{
				unpack(sample_numbers[i],&low_byte,&high_byte);
				write(global_fd,&low_byte,1);
				usleep(tdelay);
				write(global_fd,&high_byte,1);
				usleep(tdelay);
			}
			mvwprintw(win, LAST_ROW_DISP-1,2,"LOAD_RAM done                         ");
			wrefresh(win);
			break;

		case BURN_PART:		// 'v'
			for(i = 1;i < 6;i++)
				mvwprintw(win, LAST_ROW_DISP-i,1,"                                                   ");

			j = k = 0;
			mvwprintw(win, LAST_ROW_DISP-2,2,
				"BURN_PART: size: %d  start_addr: %d    ",size,start_addr);
			wrefresh(win);

			res += write(global_fd,&ret_char,1);

			unpack(size,&low_byte,&high_byte);
			res += write(global_fd, &high_byte,1);
//			read(global_fd, &t1, 1);
			res += write(global_fd, &low_byte,1);
//			read(global_fd, &t2, 1);

//			mvwprintw(win, LAST_ROW_DISP-2,1,
//				"burn part size:       %x  %x  %x  %x",t1, t2,high_byte,low_byte);
//			wrefresh(win);

			unpack(start_addr,&low_byte,&high_byte);
			res += write(global_fd, &high_byte,1);
//			read(global_fd, &t1, 1);
			res += write(global_fd, &low_byte,1);
//			read(global_fd, &t2, 1);

			low_byte = 0;
			for(i = start_addr;i < size+start_addr;i++)
			{
				res += write(global_fd,&eeprom_sim[i],1);
				low_byte += eeprom_sim[i];
				usleep(tdelay);
			}
			mvwprintw(win, LAST_ROW_DISP-1,2,"res: %d chksum: %d     ",res,low_byte);
 			break;

		case READ_EEPROM1:  // 'e'
		case READ_EEPROM2:	// 'f'

			for(i = LAST_ROW_DISP-1;i > 0;i--)
				mvwprintw(win, i,1,"                                                             ");

			res += write(global_fd,&ret_char,1);

			unpack(size,&low_byte,&high_byte);
			res += write(global_fd, &high_byte,1);
			res += write(global_fd, &low_byte,1);

			unpack(start_addr,&low_byte,&high_byte);
			res += write(global_fd, &high_byte,1);
			res += write(global_fd, &low_byte,1);

			low_byte = 0;
			for(i = start_addr;i < size+start_addr;i++)
			{
				read(global_fd,&eeprom_sim[i],1);
				low_byte += eeprom_sim[i];
				mvwprintw(win, LAST_ROW_DISP-2,1,"%d   %x    ",i,eeprom_sim[i]);
				wrefresh(win);
			}
			get_mlabel_offsets();

			j = k = 0;
			mvwprintw(win, LAST_ROW_DISP-45, 23,"|");
			if(ret_char == READ_EEPROM1)
			{
				for(i = start_addr;i < size+start_addr;i++)
				{
					if(eeprom_sim[i] < 0x7e && eeprom_sim[i] > 0x21)
						mvwprintw(win, LAST_ROW_DISP+j-45, 2+k,"%c",eeprom_sim[i]);
					else if(eeprom_sim[i] == 0)	mvwprintw(win, LAST_ROW_DISP+j-45, 2+k," ");
					else mvwprintw(win, LAST_ROW_DISP+j-45, 2+k,"_");
					if(++k > 30)
					{
						k = 0;
						++j;
					}
				}
				k++;
				mvwprintw(win, LAST_ROW_DISP+j-45, 2+k,"|");
				mvwprintw(win, LAST_ROW_DISP-1,2,"read: size %d start %d chksum %d         ",size,start_addr,low_byte);

			}
			else
			{
				mvwprintw(win, LAST_ROW_DISP-45,1,"%d: ",i);
				for(i = start_addr;i < size+start_addr;i++)
				{
					mvwprintw(win, LAST_ROW_DISP+j-45, 7+(k*3),"%2x ",eeprom_sim[i]);
					if(++k > 15)
					{
						k = 0;
						++j;
						mvwprintw(win, LAST_ROW_DISP+j-45,1,"%2d: ",i+1);
					}
				}
				mvwprintw(win, LAST_ROW_DISP-1,2,"read: size %d start %d chksum %d         ",size,start_addr,low_byte);
			}
			wrefresh(win);
 			break;


#ifdef TEST_WRITE_DATA
 		case SPACE:
			write(global_fd,&ret_char,1);
 			break;
#endif
		default:
			ret_char = generic_menu_function(ret_char);
#ifdef TEST_WRITE_DATA
			wrefresh(win);
#endif
			break;
	}
	return ret_char;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static UCHAR do_numentry(UCHAR ch)
{
	UCHAR ret_char = ch;
	char temp;
	switch(ch)
	{
		case KP_0:
		cursor_forward_stuff(0);
		break;
		case KP_1:
		cursor_forward_stuff(1);
		break;
		case KP_2:
		cursor_forward_stuff(2);
		break;
		case KP_3:
		cursor_forward_stuff(3);
		break;
		case KP_4:
		cursor_forward_stuff(4);
		break;
		case KP_5:
		cursor_forward_stuff(5);
		break;
		case KP_6:
		cursor_forward_stuff(6);
		break;
		case KP_7:
		cursor_forward_stuff(7);
		break;
		case KP_8:
		cursor_forward_stuff(8);
		break;
		case KP_9:
		cursor_forward_stuff(9);
		break;
		case KP_A:
		cursor_forward(ch);
		break;
		case KP_B:
		backspace(ch);
		break;
		case KP_C:
		_eclear();
		break;
		case KP_D:
		enter(ch);
		prev_list();
		break;
		case KP_POUND:
		prev_list();
		break;
		case KP_AST:
			prev_list();
		break;
		default:
		break;
	}
#ifdef TEST_WRITE_DATA
//	mvwprintw(win, DISP_OFFSET+31,2,"cur: %s %d   ",cur_global_number,cur_col);
	mvwprintw(win, DISP_OFFSET+1,2, "*do_numentry  %x   ",ret_char);
	wrefresh(win);
#endif
	return ret_char;
}
//******************************************************************************************//
//******************************************* escape ***************************************//
//******************************************************************************************//
static UCHAR escape(UCHAR ch)
{
	memset((void*)cur_global_number,0,NUM_ENTRY_SIZE);
	cur_col = NUM_ENTRY_BEGIN_COL;
	return ch;
}
//******************************************************************************************//
//******************************************* enter ****************************************//
//******************************************************************************************//
static UCHAR enter(UCHAR ch)
{
	int limit;
	int index;
	if(1)
//	if(data_entry_mode)
	{
		limit = atoi(cur_global_number);
		if(limit > 32767)
			strcpy(cur_global_number,"32766\0");
		memcpy((void*)new_global_number,(void*)cur_global_number,NUM_ENTRY_SIZE);
		cur_col = NUM_ENTRY_BEGIN_COL;
		index = get_curr_menu_index() - MENU2C;
		sample_numbers[index] = atoi(new_global_number);
	}
#ifdef TEST_WRITE_DATA
	mvwprintw(win, DISP_OFFSET+32, 2,"new: %s  %d  ",new_global_number,sample_numbers[index]);
	wrefresh(win);
#endif
	memset((void*)new_global_number,0,NUM_ENTRY_SIZE);
	memset((void*)cur_global_number,0,NUM_ENTRY_SIZE);
	return ch;
}
//******************************************************************************************//
//*************************************** init_numentry ************************************//
//******************************************************************************************//
static void init_numentry(int menu_index)
{
	int temp_int;
	cur_row = NUM_ENTRY_ROW;
	cur_col = NUM_ENTRY_BEGIN_COL;
//	memset((void*)new_global_number,0,NUM_ENTRY_SIZE);
	memset((void*)cur_global_number,0,NUM_ENTRY_SIZE);
	temp_int = sample_numbers[menu_index-MENU2C];
	sprintf(cur_global_number,"%4d",temp_int);
#ifdef TEST_WRITE_DATA
	mvwprintw(win, LAST_ROW_DISP-12,2,"init_numentry: %s %d %d %d  ",cur_global_number,temp_int,cur_col,menu_index);
#endif
	aux_string[0] = (UCHAR)temp_int;
	temp_int >>= 8;
	aux_string[1] = (UCHAR)temp_int;
	cur_col = strlen(cur_global_number)+NUM_ENTRY_BEGIN_COL;
}
//******************************************************************************************//
//********************************** cursor_forward_stuff **********************************//
//******************************************************************************************//
static void cursor_forward_stuff(char x)
{
//	if(data_entry_mode)
	if(1)
	{
		stuff_num(x);
		cursor_forward(x);
#ifdef TEST_WRITE_DATA
		mvwprintw(win, DISP_OFFSET+29,2,"stuff: %d    ",x);
		wrefresh(win);
#endif
	}
}
//******************************************************************************************//
//*************************************** stuff_num ****************************************//
//******************************************************************************************//
static void stuff_num(char num)
{
	num += 0x30;
	cur_global_number[cur_col-NUM_ENTRY_BEGIN_COL] = num;
}
//******************************************************************************************//
//************************************* cursor_forward *************************************//
//******************************************************************************************//
static void cursor_forward(char ch)
{
	if(++cur_col > NUM_ENTRY_END_COL)
		cur_col = NUM_ENTRY_BEGIN_COL;
}
//******************************************************************************************//
//***************************************** backspace **************************************//
//******************************************************************************************//
static UCHAR backspace(UCHAR ch)
{
//	if(data_entry_mode)
	if(1)
	{
		cursor_backward();
		cur_global_number[cur_col-NUM_ENTRY_BEGIN_COL] = 0x20;
#ifdef TEST_WRITE_DATA
	mvwprintw(win, DISP_OFFSET+32,2,"bs: %s %d   ",cur_global_number,cur_col);
	wrefresh(win);
#endif
	}
	return ch;
}
//******************************************************************************************//
//**************************************** _eclear *******************************************//
//******************************************************************************************//
static void _eclear(void)
{
	cur_row = NUM_ENTRY_ROW;
	cur_col = NUM_ENTRY_BEGIN_COL;
	memset((void*)new_global_number,0,NUM_ENTRY_SIZE);
	memset((void*)cur_global_number,0,NUM_ENTRY_SIZE);
}
//******************************************************************************************//
//************************************* cursor_backward ************************************//
//******************************************************************************************//
static void cursor_backward(void)
{
	if(--cur_col < NUM_ENTRY_BEGIN_COL)
		cur_col = NUM_ENTRY_BEGIN_COL;
#if 0
#ifdef TEST_WRITE_DATA
	mvwprintw(win, DISP_OFFSET+33,2,"cb: %s %d   ",cur_global_number,cur_col);
	wrefresh(win);
#endif
#endif
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static UCHAR do_exec(UCHAR ch)
{
	UCHAR ret_char = ch;
	int i,j,k,l;
//	mvwprintw(win, DISP_OFFSET+17,2, "                    ");

	k = get_curr_menu_index() * NUM_CHECKBOXES;
	l = k;
	j = 0;

	wrefresh(win);

	switch(ch)
	{
		case KP_A:
		j = scrollup_checkboxes(k);
		break;
		case KP_B:
		j = scrolldown_checkboxes(k);
		break;
		case KP_C:
		break;
		case KP_D:			// enter
			prev_list();
		break;
		case KP_POUND:
		break;
		case KP_0:
		break;
		case KP_AST:		// same as enter ?
			prev_list();
		break;
		default:
		break;
	}
#ifdef TEST_WRITE_DATA
//	mvwprintw(win, DISP_OFFSET+1,2,"                                  ");
//	mvwprintw(win, DISP_OFFSET+2,2,"                                  ");
	mvwprintw(win, DISP_OFFSET+2,2," %d %d %d ",l,k,j);
	wrefresh(win);

	mvwprintw(win, DISP_OFFSET+1,2, "*do_exec  %x  %d   ",ret_char,curr_checkbox);
	/// k + current_checkbox is index into check_boxes[]
	wrefresh(win);
#endif
	return ret_char;
}

//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static UCHAR do_chkbox(UCHAR ch)
{
	UCHAR ret_char = ch;
	int i,j,k,l;
	k = get_curr_menu_index() * NUM_CHECKBOXES;

	l = k;
//	k =-MENU1C;
	j = 0;

	wrefresh(win);

	switch(ch)
	{
		case KP_A:
			j = scrollup_checkboxes(k);
		break;
		case KP_B:
			j = scrolldown_checkboxes(k);
		break;
		case KP_C:
			j = toggle_checkboxes(k);
		break;
		case KP_D:		// enter
			for(i = 0;i < NUM_CHECKBOXES;i++)
			{
				prev_check_boxes[k+i].checked = check_boxes[k+i].checked;
			}
			prev_list();
		break;
		case KP_POUND:		// esc
			for(i = 0;i < NUM_CHECKBOXES;i++)
			{
				check_boxes[k+i].checked = prev_check_boxes[k+i].checked;
			}
			prev_list();
		break;
		case KP_0:
			j = checkboxes_reset(k);
		break;
		case KP_AST:
			prev_list();
		break;
		default:
		break;
	}
#ifdef TEST_WRITE_DATA
//	mvwprintw(win, DISP_OFFSET+19,2,"                                           ");
//	mvwprintw(win, DISP_OFFSET+20,2,"                                           ");
//	mvwprintw(win, DISP_OFFSET+20,2," %d %d %d ",l,k,j);
//	wrefresh(win);
/*
	for(i = 0;i < NUM_CHECKBOXES;i++)
	{
		mvwprintw(win, DISP_OFFSET+21,2+(i*4),"%d  ",check_boxes[k+i].checked);
		wrefresh(win);
	}
*/
	mvwprintw(win, DISP_OFFSET+1,2, "*do_chkbox  %x   ",ret_char);

	wrefresh(win);
#endif
	return ret_char;
}
//******************************************************************************************//
//************************************* init_checkboxes ************************************//
//******************************************************************************************//
static UCHAR init_checkboxes(int menu_index)
{
	curr_checkbox = 0;
	curr_execchoice = 0;
	mvwprintw(win, LAST_ROW_DISP-3,2, "*init_checkbox  %x   ",menu_index);
	wrefresh(win);
	return menu_index;
}
//******************************************************************************************//
//********************************** scrollup_checkboxes ***********************************//
//******************************************************************************************//
static int checkboxes_reset(int index)
{
//	int k = index+curr_checkbox;
	int i,k;
	curr_checkbox = 0;
	k = (get_curr_menu_index()-MENU1C) * NUM_CHECKBOXES;

	for(i = 0;i < NUM_CHECKBOXES;i++)
	{
		check_boxes[k].checked = 0;
		curr_checkbox++;
		k++;
	}
//	mvwprintw(win, LAST_ROW_DISP-2,2,"reset: curr %d ckbox %d %d  ",curr_checkbox,check_boxes[k].index,k);
//	wrefresh(win);

	curr_checkbox = 0;
	return k;
}
//******************************************************************************************//
//********************************** scrollup_checkboxes ***********************************//
//******************************************************************************************//
static int scrollup_checkboxes(int index)
{
//	int k = index+curr_checkbox;
	int k;
	int i;

	if(--curr_checkbox < 0)
		curr_checkbox = 9;
	// move cursor to 3 + check_boxes[curr_checkbox]
	k = ((get_curr_menu_index()-MENU1C) * NUM_CHECKBOXES)+curr_checkbox;
//	dispCharAt(1+check_boxes[k].index,20,0x21);
#if 0
#ifdef TEST_WRITE_DATA
	mvwprintw(win, LAST_ROW_DISP-2,2,"up: curr %d ckbox %d %d             ",curr_checkbox,check_boxes[k].index,k);
	mvwprintw(win, LAST_ROW_DISP-3,2,"                                            ");
	mvwprintw(win, LAST_ROW_DISP-5,2,"                                            ");

	k = ((get_curr_menu_index()-MENU1C) * NUM_CHECKBOXES);
	for(i = 0;i < NUM_CHECKBOXES;i++)
	{
		mvwprintw(win, LAST_ROW_DISP-3,2+(i*4),"%d  ",check_boxes[k].checked);
		k++;
	}
	k = ((get_curr_menu_index()-MENU1C) * NUM_CHECKBOXES);
	for(i = 0;i < NUM_CHECKBOXES;i++)
	{
		mvwprintw(win, LAST_ROW_DISP-5,2+(i*4),"%d  ",check_boxes[k].index);
		k++;
	}
	wrefresh(win);
#endif
#endif
	return k;
}
//******************************************************************************************//
//********************************* scrolldown_checkboxes **********************************//
//******************************************************************************************//
static int scrolldown_checkboxes(int index)
{
	int k = ((get_curr_menu_index()-MENU1C) * NUM_CHECKBOXES)+curr_checkbox;
	int i;

	if(++curr_checkbox > 9)
		curr_checkbox = 0;

	k = ((get_curr_menu_index()-MENU1C) * NUM_CHECKBOXES)+curr_checkbox;
#if 0
#ifdef TEST_WRITE_DATA
	mvwprintw(win, LAST_ROW_DISP-2,2,"down: curr %d ckbox %d %d             ",curr_checkbox,check_boxes[k].index,k);
	mvwprintw(win, LAST_ROW_DISP-3,2,"                                            ");
	mvwprintw(win, LAST_ROW_DISP-5,2,"                                            ");

	k = ((get_curr_menu_index()-MENU1C) * NUM_CHECKBOXES);
	for(i = 0;i < NUM_CHECKBOXES;i++)
	{
		mvwprintw(win, LAST_ROW_DISP-3,2+(i*4),"%d  ",check_boxes[k].checked);
		k++;
	}
	k = ((get_curr_menu_index()-MENU1C) * NUM_CHECKBOXES);
	for(i = 0;i < NUM_CHECKBOXES;i++)
	{
		mvwprintw(win, LAST_ROW_DISP-5,2+(i*4),"%d  ",check_boxes[k].index);
		k++;
	}
	wrefresh(win);
#endif
#endif
	return k;
}
//******************************************************************************************//
//********************************** toggle_checkboxes *************************************//
//******************************************************************************************//
static int toggle_checkboxes(int index)
{
	int k = ((get_curr_menu_index()-MENU1C) * NUM_CHECKBOXES)+curr_checkbox;

	if(check_boxes[k].checked == 1)
	{
		check_boxes[k].checked = 0;
	}
	else
	{
		check_boxes[k].checked = 1;
	}
#ifdef TEST_WRITE_DATA
//	mvwprintw(win, LAST_ROW_DISP-2,2,"toggle: curr %d ckbox %d %d             ",curr_checkbox,check_boxes[k].index,k);
//	wrefresh(win);
#endif
	return k;
}
//******************************************************************************************//
//********************************** scrollup_execchoice ***********************************//
//******************************************************************************************//
static int scrollup_execchoice(UCHAR ch)
{
	if(--curr_execchoice < 0)
		curr_execchoice = 9;
	return ch;
}
//******************************************************************************************//
//********************************* scrolldown_execchoice **********************************//
//******************************************************************************************//
static int scrolldown_execchoice(UCHAR ch)
{
	if(++curr_execchoice > 9)
		curr_execchoice = 0;
	return ch;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static UCHAR non_func(UCHAR ch)
{
	UCHAR ret_char = ch;
	switch(ch)
	{
		case KP_A:
		ret_char = NF_1;
		break;
		case KP_B:
		ret_char = NF_2;
		break;
		case KP_C:
		ret_char = NF_3;
		break;
		case KP_D:
		ret_char = NF_4;
		break;
		case KP_POUND:
		ret_char = NF_5;
		break;
		case KP_0:
		ret_char = NF_6;
		break;
		case KP_AST:
		prev_list();
		break;
		default:
		break;
	}
#ifdef TEST_WRITE_DATA
	mvwprintw(win, DISP_OFFSET+1,2, "*non_func %x              ",ch);
	wrefresh(win);
#endif
	return ret_char;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
#ifdef TEST_WRITE_DATA
static void display_menus(int index)
{
	int i;
	char tlabel[MAX_LABEL_LEN];

	mvwprintw(win, DISP_OFFSET+10,2,"%d  ",index);
	for(i = 0;i < 6;i++)
	{
		get_mlabel(menu_structs[get_curr_menu()].menus[i],tlabel);
		if(strcmp(tlabel,"blank") != 0)
			mvwprintw(win, DISP_OFFSET+11+i, 2,"%s              ",tlabel);
		else
			mvwprintw(win, DISP_OFFSET+11+i, 2,"                ");
	}
}
#endif
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static UCHAR get_fptr(void)
{
	return menu_structs[get_curr_menu()].fptr;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static void get_fptr_label(char *str)
{
#ifdef TEST_WRITE_DATA
	char tlabel[MAX_LABEL_LEN];
//	return menu_labels[menu_structs[get_curr_menu()].fptr+total_no_menu_labels];
	get_mlabel(menu_structs[get_curr_menu()].fptr+total_no_menu_labels,tlabel);
#endif
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
int get_curr_menu(void)
{
	return menu_list[current_fptr];
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static int get_curr_menu_index(void)
{
	return menu_structs[get_curr_menu()].index;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
int curr_fptr_changed(void)
{
	return 0;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static UCHAR do_init(UCHAR ch)
{
	init_list();
	return ch;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
void init_list(void)
{
	int i = 0;
	UCHAR k;

	alnum_array[i++] = 33;		// first one is a '!'
	for(k = 34;k < 48;k++)		// '"' - '/'	14
		alnum_array[i++] = k;
	alnum_array[i++] = 32;		// 'space'		1
	for(k = 58;k < 65;k++)		// ':' - '@'	7
		alnum_array[i++] = k;
	for(k = 91;k < 97;k++)		// '[' - '`'	6
		alnum_array[i++] = k;
	for(k = 123;k < 127;k++)	// '{' - '~'	4
		alnum_array[i++] = k;
	for(k = 65;k < 91;k++)		// 'A' - 'Z'	26
		alnum_array[i++] = k;
	for(k = 97;k < 123;k++)		// 'a' - 'z'	26	total: 85 (NUM_ALNUM)
		alnum_array[i++] = k;
	choose_alnum = 0x30;
	scroll_ptr = 25;		// start showing at 'a' (skip '!' - '9')
	cur_alnum_col = NUM_ENTRY_BEGIN_COL;
	list_size = LIST_SIZE;
	current_fptr = first_menu;
	memset(menu_list,0,sizeof(menu_list));
	menu_list[0] = current_fptr;
	curr_checkbox = 0;
	last_checkbox = NUM_CHECKBOXES-1;
	last_execchoice = NUM_EXECCHOICES-1;
	curr_execchoice = 0;
	scale_type = 0;
	prev_scale_type = 1;
	cur_row = NUM_ENTRY_ROW;
	cur_col = NUM_ENTRY_BEGIN_COL;
}
#endif
#endif
