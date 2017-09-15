#ifndef MAIN_C
#include "sfr_helper.h"
#include "t6963.h"
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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "pic_main.h"

static UCHAR menu_change(UCHAR ch);
static UCHAR do_exec(UCHAR ch);
static UCHAR do_chkbox(UCHAR ch);
static UCHAR init_checkboxes(UCHAR ch);
static UCHAR init_execchoices(UCHAR ch);
static UCHAR non_func(UCHAR ch);
static UCHAR do_numentry(UCHAR ch);
static UCHAR do_init(UCHAR ch);
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
static UCHAR toggle_checkboxes(UCHAR ch);
static UCHAR scrollup_checkboxes(UCHAR ch);
static UCHAR scrolldown_checkboxes(UCHAR ch);
static UCHAR scrollup_execchoice(UCHAR ch);
static UCHAR scrolldown_execchoice(UCHAR ch);
static UCHAR do_exec_choice(void);
static UCHAR *ppic_data;

static int first_menu = MAIN;
static int current_fptr;		// pointer into the menu_list[]
static int prev_fptr;
static int list_size;
static int curr_type;
static int scroll_ptr;
static int cur_alnum_col;
static int dirty_flag;
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

UCHAR taux_string[AUX_STRING_LEN];

static UCHAR (*fptr[NUM_FPTS])(UCHAR) = { menu_change, do_exec, do_chkbox, non_func, do_numentry, do_init };
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
		dirty_flag = 1;
		ret = 0;
	}
	return ret;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
//#if 0
static UCHAR set_list(int fptr)
{
	int i;
	UCHAR ret_char = 0;
//	char tlabel[MAX_LABEL_LEN];

	if(current_fptr < list_size)
	{
		current_fptr++;
		menu_list[current_fptr] = fptr;
		dirty_flag = 1;

		ret_char = get_curr_menu_index();
		send_aux_data = 0;
		if(menu_structs[get_curr_menu()].fptr != _menu_change)
		{
#ifdef TEST_WRITE_DATA
//			get_fptr_label(tlabel);
			mvwprintw(win, DISP_OFFSET,2,"fptr: %d menu_index: %d     ",fptr,ret_char);
			wrefresh(win);
#endif
			switch(ret_char)
			{
				case MENU1C:	// 1st do_chkbox
				case MENU1D:	// 2nd do_chkbox
					init_checkboxes(ret_char);
				break;
				case MENU2A:	// 1st exec_choice
				case MENU2B:	// 2nd exec_choice
					init_execchoices(ret_char);
					curr_execchoice = 0;
				break;
				case MENU2C:	// do_numentry
				case MENU2D:
				case MENU2E:
				case MENU3A:
				case MENU3B:
					init_numentry(ret_char);
				break;
				default:
					send_aux_data = 0;
				break;
			}
		}

	}
	return ret_char;
}
//#endif
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
//******************************************************************************************//
//******************************************************************************************//
// pass in the keypress from the PIC24 keypad and pass out the string of UCHARs
UCHAR get_key(UCHAR ch, int size, int start_addr, UCHAR *str, int type)
{
	int i,j,k;
	UCHAR ret_char = ch;
	int res = 0;
	UCHAR low_byte, high_byte;
	UCHAR t1, t2;
// warning: str passed when not in simulator is pointing to memory in the PIC24
//	ppic_data = str;

	switch(ret_char)
	{
		case INIT:
			init_list();
			break;
		case READ_EEPROM:
			res += write(global_fd,&ret_char,1);

			unpack(size,&low_byte,&high_byte);
			res += write(global_fd, &high_byte,1);
			res += write(global_fd, &low_byte,1);

			unpack(start_addr,&low_byte,&high_byte);
			res += write(global_fd, &high_byte,1);
			res += write(global_fd, &low_byte,1);
// 			res += write(global_fd,&type,1);		// send the 'type' to tell AVR what to load
 			send_aux_data = 0;

			for(i = start_addr;i < size+start_addr;i++)
				read(global_fd,&str[i],1);

			goffset = 0;
			get_label_offsets();

			mvwprintw(win, LAST_ROW_DISP-2,1,
				"read e: ch: %x size: %d st addr: %d rtparams: %d ",ret_char,size,start_addr,no_rtparams);
 			break;

		case BURN_PART:
			memset(taux_string,0,AUX_STRING_LEN);
			j = k = 0;
			for(i = start_addr;i < size+start_addr;i++)
				aux_string[j++] = *(str + i);
			j = 0;

			for(i = 0;i < size;i++)
			{
				if(aux_string[i] < 0x7e && aux_string[i] > 0x21)
					mvwprintw(win, LAST_ROW_DISP+j-40, 24+k,"%c",aux_string[i]);
				else if(aux_string[i] == 0)	mvwprintw(win, LAST_ROW+j-40, 24+k," ");
				else mvwprintw(win, LAST_ROW_DISP+j-40, 24+k,"_");
				if(++k > 30)
				{
					k = 0;
					++j;
				}
			}
			wrefresh(win);

			mvwprintw(win, LAST_ROW_DISP-3,1,
				"size: %d  start_addr: %d  ",size,start_addr);
			wrefresh(win);

			res += write(global_fd,&ret_char,1);

			unpack(size,&low_byte,&high_byte);
			res += write(global_fd, &high_byte,1);
			read(global_fd, &t1, 1);
			res += write(global_fd, &low_byte,1);
			read(global_fd, &t2, 1);

//			mvwprintw(win, LAST_ROW_DISP-2,1,
//				"burn part size:       %x  %x  %x  %x",t1, t2,high_byte,low_byte);
//			wrefresh(win);

			unpack(start_addr,&low_byte,&high_byte);
			res += write(global_fd, &high_byte,1);
			read(global_fd, &t1, 1);
			res += write(global_fd, &low_byte,1);
			read(global_fd, &t2, 1);

//			mvwprintw(win, LAST_ROW_DISP-1,1,
//				"burn part start_addr: %x  %x  %x  %x",t1, t2,high_byte,low_byte);
//			wrefresh(win);

			for(i = 0;i < size;i++)
			{
//				write(global_fd,&str[i],1);
//				aux_string[i] = i;
				write(global_fd,&aux_string[i],1);
				read(global_fd,&taux_string[i],1);
//				usleep(10000);
			}

			j = k = 0;

			for(i = start_addr;i < size+start_addr;i++)
			{
				if(taux_string[i] < 0x7e && taux_string[i] > 0x21)
					mvwprintw(win, LAST_ROW_DISP+j-30, 24+k,"%c",taux_string[i]);
				else if(taux_string[i] == 0)	mvwprintw(win, LAST_ROW+j-30, 24+k," ");
				else mvwprintw(win, LAST_ROW_DISP+j-30, 24+k,"_");
				if(++k > 30)
				{
					k = 0;
					++j;
				}
			}
			wrefresh(win);

// 			res += write(global_fd,&type,1);		// send the 'type' to tell AVR what to load

			goffset = 0;
//			get_label_offsets();

 			break;
			case LOAD_MENU_STRUCT:
				i = 0;
				total_offset = 0;
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
				goffset = 0;
				no_menu_labels = blank;
				no_rt_labels = test - rpm;
				get_label_offsets();

#ifdef TEST_WRITE_DATA
			mvwprintw(win, LAST_ROW_DISP-2,1,"load menu_struct: %d %d   ",no_rt_labels,no_menu_labels);
#endif
				break;

#ifdef TEST_WRITE_DATA
 		case SPACE:
			write(global_fd,&ret_char,1);
 			break;
#endif
		default:
			ret_char = generic_menu_function(ret_char);
#ifdef TEST_WRITE_DATA
			if(curr_fptr_changed())
				display_menus(get_curr_menu());
			wrefresh(win);
#endif
			break;
	}
	return ret_char;
}
//******************************************************************************************//
//********************************* generic_menu_function **********************************//
//******************************************************************************************//
static UCHAR generic_menu_function(UCHAR ch)
{
	UCHAR ret_char;
	int i;
	UCHAR temp2;
	int temp3;
	char tlabel[MAX_LABEL_LEN];
	UCHAR low_byte, high_byte;
	UCHAR code = 0;

	// execute function pointer
	ret_char = (*fptr[menu_structs[get_curr_menu()].fptr])(ch);

#ifdef TEST_WRITE_DATA
	get_label(get_curr_menu(),tlabel);
	mvwprintw(win, DISP_OFFSET+2, 2,"%s %d func: %d            ",
			tlabel,
			get_curr_menu(),
//			get_fptr_label(),
			menu_structs[get_curr_menu()].fptr);
	wrefresh(win);
#endif
/*
	for(i = 0;i < 6;i++)
	{
		memset(tlabel,0,MAX_LABEL_LEN);
		strcpy(tlabel,menu_labels[menu_structs[get_curr_menu()].menus[i]]);
//		memcpy(cur_param_string+(i*MAX_LABEL_LEN),tlabel,MAX_LABEL_LEN);
	}
*/
// write 4 bytes: fptr, ch, menu_index & current_param_string
// then the size of the aux_data, then the aux_data (which can be up to AUX_STRING_LEN)
// and if the AUX_STRING_LEN is 48 then to total is 4+60+48 = 112
// so the PIC must allocate this much space for this module to write to so the pic can
// send it using its own serial interface.
#ifdef TEST_WRITE_DATA
	write(global_fd,&ch,1);
	temp2 = get_fptr();
	write(global_fd,&temp2,1);
//	write(global_fd,&ch,1);
	temp3 = get_curr_menu_index();
	unpack(temp3,&low_byte, &high_byte);
	write(global_fd,&high_byte,1);
	write(global_fd,&low_byte,1);

	for(i = 0;i < 6;i++)
	{
		unpack(menu_structs[get_curr_menu()].menus[i],&low_byte,&high_byte);
		write(global_fd, &high_byte,1);
		// for some reason when sending a 0x0c - 13 it get recieved as a 0x0a - 10
		// so I changed it in the pack/unpack functions
//		low_byte = ~low_byte;
		write(global_fd, &low_byte,1);
//		mvwprintw(win, DISP_OFFSET+22+i, 2,"%d  %d  %d  ",menu_structs[get_curr_menu()].menus[i],high_byte,low_byte);
	}
	unpack(send_aux_data,&low_byte,&high_byte);
	write(global_fd, &high_byte,1);
	write(global_fd, &low_byte,1);
	for(i = 0;i < send_aux_data;i++)
		write(global_fd,&aux_string[i],1);

	// if code gets read back as an 0xAA, then that means we are
	// running this simulator hooked up to the AVR
	// (show everthing is working if no LCD screen is attached)
	read(global_fd,&code,1);
	if(code == 0xAA)
	{
		for(i = 0;i < 6;i++)
		{
			read(global_fd,&code,1);
			mvwprintw(win, DISP_OFFSET+3,2+(i * 3),"%d ",code);
		}
	}

/*
	unpack(aux_data_offset,&low_byte,&high_byte);
	write(global_fd, &high_byte,1);
	write(global_fd, &low_byte,1);
*/

	for(i = 0;i < current_fptr;i++)
	{
		get_label(menu_list[i],tlabel);
		mvwprintw(win, DISP_OFFSET+18+i, 2,"%s  ",tlabel);
	}
	get_label(menu_list[i],tlabel);

	mvwprintw(win, DISP_OFFSET+18+i, 2,"%s  ",tlabel);

	mvwprintw(win, DISP_OFFSET+18+i, 2,"%s  ",tlabel);
	mvwprintw(win, DISP_OFFSET+18+i+1, 2,"                                  ");

//	mvwprintw(win, DISP_OFFSET+3,2,"curr_checkbox: %d     ",curr_checkbox);

//	for(i = 0;i < NUM_CHECKBOXES/4;i++)
//		mvwprintw(win, DISP_OFFSET+4,2+(i*2),"%d  ",check_boxes[i].checked);

//	mvwprintw(win, DISP_OFFSET+5,2+(i*7),"%s  ",check_boxes[i].string);
/*
	for(i = 0;i < NUM_CHECKBOXES;i++)
		mvwprintw(win, DISP_OFFSET+5,2+(i+2),"%d  ",prev_check_boxes[i]);
	mvwprintw(win, DISP_OFFSET+6,2,"exec choice: %d",curr_execchoice);
*/
	wrefresh(win);
#else
	temp2 = get_fptr();
	memcpy(ppic_data,&temp2,1);
	memcpy(ppic_data+1,&ch,1);
	temp2 = (UCHAR)get_curr_menu_index();
	memcpy(ppic_data+2,&temp2,1);
#endif
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
	mvwprintw(win, DISP_OFFSET+31,2,"cur: %s %d   ",cur_global_number,cur_col);
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
		mod_data_ready = 1;
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
	send_aux_data = 2;
	temp_int = sample_numbers[menu_index-MENU2C];
	sprintf(cur_global_number,"%4d",temp_int);
#ifdef TEST_WRITE_DATA
	mvwprintw(win, LAST_ROW_DISP-3,2,"init_numentry: %s %d %d %d  ",cur_global_number,temp_int,cur_col,menu_index);
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
	int i;
	int menu_index = 0;

	switch(ch)
	{
		case KP_A:
		scrollup_execchoice(ch);
		break;
		case KP_B:
		scrolldown_execchoice(ch);
		break;
		case KP_C:
		prev_list();
		break;
		case KP_D:
		break;
		case KP_POUND:
		break;
		case KP_0:
		break;
		case KP_AST:
		prev_list();
		break;
		default:
		break;
	}
//	if(ch != KP_AST)

#ifdef TEST_WRITE_DATA
	mvwprintw(win, DISP_OFFSET+1,2, "*do_exec: %x            ",ch);
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
	int i;

	switch(ch)
	{
		case KP_A:
		scrollup_checkboxes(ch);
		break;
		case KP_B:
		scrolldown_checkboxes(ch);
		break;
		case KP_C:
		toggle_checkboxes(ch);
		break;
		case KP_D:		// enter
		prev_list();
		break;
		case KP_POUND:		// esc
			for(i = 0;i < NUM_CHECKBOXES;i++)
				check_boxes[i].checked = prev_check_boxes[i];	// restore old

#ifdef TEST_WRITE_DATA
			for(i = 0;i < NUM_CHECKBOXES;i++)
				mvwprintw(win, DISP_OFFSET+6,2+(i+2),"%d  ",prev_check_boxes[i]);
			wrefresh(win);
#endif
		prev_list();
		break;
		case KP_0:
		break;
		case KP_AST:
		prev_list();
		break;
		default:
		break;
	}
#ifdef TEST_WRITE_DATA
	mvwprintw(win, DISP_OFFSET+1,2, "*do_chkbox %x	               ",ch);
	wrefresh(win);
#endif
	return ret_char;
}
//******************************************************************************************//
//************************************* init_checkboxes ************************************//
//******************************************************************************************//
static UCHAR init_checkboxes(UCHAR ch)
{
	curr_checkbox = 0;
	int i;
	int j = 0;
	int k = 0;

	for(i = 0;i < NUM_CHECKBOXES;i++)
	{
		memcpy(aux_string+(sizeof(CHECKBOXES)*i),&check_boxes[i+((ch-MENU1C)*NUM_CHECKBOXES)],sizeof(CHECKBOXES));
		prev_check_boxes[i] = check_boxes[i].checked;
//		mvwprintw(win, DISP_OFFSET+5+j,2+(k*8)," %s ",check_boxes[i+((ch-MENU1C)*NUM_CHECKBOXES)].string);
		if(++k > 5)
		{
			j++;
			k = 0;
		}
	}
	send_aux_data = sizeof(CHECKBOXES)*NUM_CHECKBOXES;
}
//******************************************************************************************//
//************************************* init_checkboxes ************************************//
//******************************************************************************************//
static UCHAR init_execchoices(UCHAR ch)
{
	curr_checkbox = 0;
	int i;
	int j = 0;
	int k = 0;

	for(i = 0;i < NUM_CHECKBOXES;i++)
	{
		memcpy(aux_string+(sizeof(CHECKBOXES)*i),&check_boxes[i+((ch-MENU2A+2)*NUM_CHECKBOXES)],sizeof(CHECKBOXES));
		prev_check_boxes[i] = check_boxes[i].checked;
//		mvwprintw(win, DISP_OFFSET+5+j,2+(k*8)," %s ",check_boxes[i+((ch-MENU2A+2)*NUM_CHECKBOXES)].string);
		if(++k > 5)
		{
			j++;
			k = 0;
		}
	}
	send_aux_data = sizeof(CHECKBOXES)*NUM_CHECKBOXES;
}
//******************************************************************************************//
//********************************** scrollup_checkboxes ***********************************//
//******************************************************************************************//
static UCHAR scrollup_checkboxes(UCHAR ch)
{
	if(--curr_checkbox < 0)
		curr_checkbox = last_checkbox;
	return ch;
}
//******************************************************************************************//
//********************************* scrolldown_checkboxes **********************************//
//******************************************************************************************//
static UCHAR scrolldown_checkboxes(UCHAR ch)
{
	if(++curr_checkbox > last_checkbox)
		curr_checkbox = 0;
	return ch;
}
//******************************************************************************************//
//********************************** scrollup_checkboxes ***********************************//
//******************************************************************************************//
static UCHAR scrollup_execchoice(UCHAR ch)
{
	if(--curr_execchoice < 0)
		curr_execchoice = last_execchoice;
	return ch;
}
//******************************************************************************************//
//********************************* scrolldown_checkboxes **********************************//
//******************************************************************************************//
static UCHAR scrolldown_execchoice(UCHAR ch)
{
	if(++curr_execchoice > last_execchoice)
		curr_execchoice = 0;
	return ch;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static UCHAR do_exec_choice(void)
{
	return 0;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static UCHAR toggle_checkboxes(UCHAR ch)
{
	if(check_boxes[curr_checkbox].checked == 1)
	{
		check_boxes[curr_checkbox].checked = 0;
	}
	else
	{
		check_boxes[curr_checkbox].checked = 1;
	}
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
		get_label(menu_structs[get_curr_menu()].menus[i],tlabel);
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
	get_label(menu_structs[get_curr_menu()].fptr+total_no_menu_labels,tlabel);
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
	int flag = dirty_flag;
	dirty_flag = 0;
	return flag;
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
	prev_fptr = first_menu;
	memset(menu_list,0,sizeof(menu_list));
	menu_list[0] = current_fptr;
	dirty_flag = 0;
	curr_checkbox = 0;
	last_checkbox = NUM_CHECKBOXES-1;
	last_execchoice = NUM_EXECCHOICES-1;
	curr_execchoice = 0;
	scale_type = 0;
	prev_scale_type = 1;
	new_data_ready = 0;
	data_entry_mode = 0;
	mod_data_ready = 0;
	cur_row = NUM_ENTRY_ROW;
	cur_col = NUM_ENTRY_BEGIN_COL;
	aux_index = 0;
	send_aux_data = 0;
}
#endif
