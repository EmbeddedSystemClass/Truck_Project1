#ifdef TEST_WRITE_DATA
#include "sfr_helper.h"
#include "t6963.h"
#ifdef TEST_WRITE_DATA
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <ncurses.h>
WINDOW *win;
#endif
#include "main.h"
#include "pic_main.h"

static UCHAR menu_change(UCHAR ch);
static UCHAR exec_choice(UCHAR ch);
static UCHAR do_chkbox(UCHAR ch);
static UCHAR non_func(UCHAR ch);
static UCHAR do_numentry(UCHAR ch);
static void init_numentry(int menu_index);
static void cursor_forward_stuff(char x);
static void cursor_forward(char x);
static void cursor_backward(void);
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

static int first_menu = MAIN;
static int last_menu = testnum8;
static int current_fptr;		// pointer into the menu_list[]
static int prev_fptr;
static int list_size;
static int curr_type;
static int scroll_ptr;
static int cur_alnum_col;
static int dirty_flag;
static int next_prev_list;
#ifdef TEST_WRITE_DATA
static void display_menus(int index);
#endif
static UCHAR cur_row, cur_col;	// used by the current menu/dialog function to keep track of the current row,col
static UCHAR alnum_array[NUM_ALNUM+1];
static UCHAR choose_alnum;
static void prev_list(void);
static int menu_list[LIST_SIZE];	// works like a stack for the how deep into the menus we go
static void set_list(int fptr);
static UCHAR get_fptr(void);
static char* get_fptr_label(void);
static int get_curr_menu_index(void);

static UCHAR (*fptr[NUM_FPTS])(UCHAR) = { menu_change, exec_choice, do_chkbox, non_func, do_numentry };
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static void prev_list(void)
{
//	UCHAR temp = menu_list[current_fptr];
//	UCHAR temp2 = current_fptr;

	if(current_fptr == 0)
		return;
	else
	{
		menu_list[current_fptr] = 0;
		current_fptr--;
		dirty_flag = 1;
	}
//	return menu_list[current_fptr];
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
//#if 0
static void set_list(int fptr)
{
	int i;
	if(current_fptr < list_size)
	{
		current_fptr++;
		menu_list[current_fptr] = fptr;
		dirty_flag = 1;

		send_aux_data = 1;
		if(menu_structs[get_curr_menu()].fptr != _menu_change)
		{
			mvwprintw(win, DISP_OFFSET,2,"set_list: %s   %d     ",get_fptr_label(),get_curr_menu_index());
			wrefresh(win);
			switch(get_curr_menu_index())
			{
				case MENU1C:	// 1st exec_choice
					curr_execchoice = 0;
//					for(i = 0;i < NUM_EXECCHOICES;i++)
//						aux_string[i] = exec_choices[i].checked;
//					send_aux_data = NUM_EXECCHOICES;
					send_aux_data = 1;
				break;
				case MENU1D:	// do_chkbox
					curr_checkbox = 0;
					for(i = 0;i < NUM_CHECKBOXES;i++)
					{
						// store the what's sent in case they hit 'cancel'
						aux_string[i] = check_boxes[i].checked;
						prev_check_boxes[i] = check_boxes[i].checked;
					}
					send_aux_data = NUM_CHECKBOXES;
				break;
				case MENU1E:
					curr_checkbox = 0;
					for(i = 0;i < NUM_CHECKBOXES;i++)
					{
						// store the what's sent in case they hit 'cancel'
						aux_string[i] = check_boxes[i].checked;
						prev_check_boxes[i] = check_boxes[i].checked;
					}
					send_aux_data = NUM_CHECKBOXES;
				break;
				case MENU2A:	// non_func
					send_aux_data = 24;
//					for(i = 0;i < send_aux_data;i++)
//						aux_string[i] = i;
				break;
				case MENU2B:	// 2nd exec_choice
					curr_execchoice = 0;
//					for(i = 0;i < NUM_EXECCHOICES;i++)
//						aux_string[i] = exec_choices[i].checked;
//					send_aux_data = NUM_EXECCHOICES;
					send_aux_data = 1;
				break;
				case MENU2C:	// do_numentry
					init_numentry(MENU2C);
					send_aux_data = 16;
					for(i = 0;i < send_aux_data;i++)
						aux_string[i] = i;
				break;
				default:
					send_aux_data = 1;
				break;
			}
		}

	}
}
//#endif
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static UCHAR menu_change(UCHAR ch)
{
	UCHAR ret_char = ch;
	int i;
	int temp;
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
		set_list(menu_structs[curr_menu].menus[menu_index]);

#ifdef TEST_WRITE_DATA
//	mvwprintw(win, DISP_OFFSET+2,2,"%d  %d  %d  %s ",menu_index,temp,ret_char,menu_labels[menu_structs[ret_char].label]);
	mvwprintw(win, DISP_OFFSET+1,2, "menu_change   %x    ",ch);
//	mvwprintw(win, DISP_OFFSET+1,2, "*menu_change                           ");
	wrefresh(win);
#endif
	return ret_char;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
// pass in the keypress from the PIC24 keypad and pass out the string of UCHARs
UCHAR get_key(UCHAR ch, UCHAR *str)
{
	int i;
	memcpy(cur_param_string,str,NUM_UCHAR_PARAMS);
	UCHAR ret_char = generic_menu_function(ch);


	if(curr_fptr_changed())
	{
//#if 0
#ifdef TEST_WRITE_DATA
		display_menus(get_curr_menu());
#endif
//#endif
	}
	return ret_char;

	if(new_data_ready == 1)
	{
		data_entry_mode = 1;
//		display_edit_value();
		new_data_ready = 0;
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

//	mvwprintw(win, DISP_OFFSET+5,2,"                       ");


	if(menu_structs[get_curr_menu()].enabled == 1)
	{
		// execute function pointer
		ret_char = (*fptr[menu_structs[get_curr_menu()].fptr])(ch);
//		cur_param_string[0] = ret_char;
#ifdef TEST_WRITE_DATA
		mvwprintw(win, DISP_OFFSET+2, 2,"%s -> %d func: %s -> %d            ",
				menu_labels[get_curr_menu()],
				get_curr_menu(),
				get_fptr_label(),
				menu_structs[get_curr_menu()].fptr);
		wrefresh(win);
#endif
		for(i = 0;i < 6;i++)
		{
			memset(tlabel,0,MAX_LABEL_LEN);
			strcpy(tlabel,menu_labels[menu_structs[get_curr_menu()].menus[i]]);
			memcpy(cur_param_string+(i*MAX_LABEL_LEN),tlabel,MAX_LABEL_LEN);
		}

		temp2 = get_fptr();
		write(global_fd,&temp2,1);
		write(global_fd,&ch,1);
		temp2 = (UCHAR)get_curr_menu_index();
		write(global_fd,&temp2,1);
		write(global_fd,cur_param_string,NUM_UCHAR_PARAMS);

		write(global_fd,&send_aux_data,1);
		if(send_aux_data != 0)
			write(global_fd,aux_string,send_aux_data);
/*
		if(next_prev_list == 1)
		{
			prev_list();
			next_prev_list = 0;
		}
*/
	}
//#if 0
#ifdef TEST_WRITE_DATA
		for(i = 0;i < current_fptr;i++)
			mvwprintw(win, DISP_OFFSET+18+i, 2,"%d -> %s  ",menu_list[i],menu_labels[menu_list[i]]);

		mvwprintw(win, DISP_OFFSET+18+i, 2,"%d -> %s  ",menu_list[i],menu_labels[get_curr_menu()]);
		mvwprintw(win, DISP_OFFSET+18+i+1, 2,"                                  ");
		wrefresh(win);
		mvwprintw(win, DISP_OFFSET+3,2,"curr_checkbox: %d     ",curr_checkbox);
		for(i = 0;i < NUM_CHECKBOXES;i++)
			mvwprintw(win, DISP_OFFSET+4,2+(i+2),"%d  ",check_boxes[i].checked);

		for(i = 0;i < NUM_CHECKBOXES;i++)
			mvwprintw(win, DISP_OFFSET+5,2+(i+2),"%d  ",prev_check_boxes[i]);
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
		enter(ch);
		prev_list();
		break;
		case KP_D:
		break;
		case KP_POUND:
		break;
		case KP_AST:
			prev_list();
		break;
		default:
		break;
	}
#ifdef TEST_WRITE_DATA
	mvwprintw(win, DISP_OFFSET+1,2, "*do_numentry  %x   ",ret_char);
	wrefresh(win);
#endif
	return ret_char;
}
//******************************************************************************************//
//************************************* cursor_forward *************************************//
//******************************************************************************************//
static void cursor_forward(char ch)
{
#ifdef TEST_WRITE_DATA
//		dispCharAt(NUM_ENTRY_ROW+1,cur_col,95);
#endif
	if(++cur_col > NUM_ENTRY_END_COL)
		cur_col = NUM_ENTRY_BEGIN_COL;
//    dispSetCursorX(TEXT_ON | CURSOR_BLINK_ON,NUM_ENTRY_ROW,cur_col,LINE_8_CURSOR);
#ifdef TEST_WRITE_DATA
//		dispCharAt(NUM_ENTRY_ROW+1,cur_col,95);
#endif
//	mvwprintw(win, 45, 3,"cursor forward   ");
    return ch;
}
//******************************************************************************************//
//***************************************** backspace **************************************//
//******************************************************************************************//
static UCHAR backspace(UCHAR ch)
{
	if(data_entry_mode)
	{
		cursor_backward();
		dispCharAt(NUM_ENTRY_ROW,cur_col,0x20);
		cur_global_number[cur_col-NUM_ENTRY_BEGIN_COL] = 0x20;
//		mvwprintw(win, DISP_OFFSET+19,2,"bs:%s      %d  ",cur_global_number,cur_col);
	}
//	memset((void*)cur_global_number,0,NUM_ENTRY_SIZE);
	return ch;
}
//******************************************************************************************//
//************************************* cursor_backward ************************************//
//******************************************************************************************//
static void cursor_backward(void)
{
#ifdef TEST_WRITE_DATA
//		dispCharAt(NUM_ENTRY_ROW+1,cur_col,0x20);
#endif
	if(--cur_col < NUM_ENTRY_BEGIN_COL)
		cur_col = NUM_ENTRY_BEGIN_COL;
//	dispSetCursorX(TEXT_ON | CURSOR_BLINK_ON,NUM_ENTRY_ROW,cur_col,LINE_8_CURSOR);
#ifdef TEST_WRITE_DATA
//		dispCharAt(NUM_ENTRY_ROW+1,cur_col,95);
#endif
}
//******************************************************************************************//
//********************************** cursor_forward_stuff **********************************//
//******************************************************************************************//
static void cursor_forward_stuff(char x)
{
	data_entry_mode = 1;
	if(data_entry_mode)
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
//	dispCharAt(NUM_ENTRY_ROW,cur_col,num);
	cur_global_number[cur_col-NUM_ENTRY_BEGIN_COL] = num;
#ifdef TEST_WRITE_DATA
	mvwprintw(win, DISP_OFFSET+8,2,"test   ");
	mvwprintw(win, DISP_OFFSET+31,2,"cur: %s           %d   ",cur_global_number,cur_col);
	wrefresh(win);
#endif
}
//******************************************************************************************//
//******************************************* enter ****************************************//
//******************************************************************************************//
static UCHAR enter(UCHAR ch)
{
	int limit;
	if(data_entry_mode)
	{
		limit = atoi(cur_global_number);
		if(limit > 32767)
			strcpy(cur_global_number,"32766\0");
		memcpy((void*)new_global_number,(void*)cur_global_number,NUM_ENTRY_SIZE);
		cur_col = NUM_ENTRY_BEGIN_COL;
		mod_data_ready = 1;
	}
#ifdef TEST_WRITE_DATA
	mvwprintw(win, DISP_OFFSET+32, 2,"new: %s       %d    ",new_global_number,limit);
	wrefresh(win);
#endif
	return ch;
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
//************************************* init_numentry ************************************//
//******************************************************************************************//
static void init_numentry(int menu_index)
{
	cur_row = NUM_ENTRY_ROW;
	cur_col = NUM_ENTRY_BEGIN_COL;
//	memset((void*)new_global_number,0,NUM_ENTRY_SIZE);
	memset((void*)cur_global_number,0,NUM_ENTRY_SIZE);
}
//******************************************************************************************//
//************************************ clean_disp_num **************************************//
//******************************************************************************************//
#if 0
static void clean_disp_num(void)
{
	int i;
	for(i = 0;i < NUM_ENTRY_SIZE+1;i++)
	{
//		dispCharAt(NUM_ENTRY_ROW,i+NUM_ENTRY_BEGIN_COL,0x20);
//		dispCharAt(NUM_ENTRY_ROW,i+NUM_ENTRY_BEGIN_COL+1,0x20);
	}
}
#endif
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static UCHAR exec_choice(UCHAR ch)
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
		do_exec_choice();
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
//	mvwprintw(win, DISP_OFFSET+2,2,"%d  %d  %d  %s ",menu_index,temp,ret_char,menu_labels[menu_structs[ret_char].label]);
//	mvwprintw(win, DISP_OFFSET+1,2, "exec_choice %d %d  %d  %d  ",temp1,temp,menu_index,menu_structs[menu_index].menu);
	mvwprintw(win, DISP_OFFSET+1,2, "*exec_choice: %x            ",ch);
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
	next_prev_list = 0;

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
		next_prev_list = 1;
		break;
		case KP_POUND:		// esc
			for(i = 0;i < NUM_CHECKBOXES;i++)
				check_boxes[i].checked = prev_check_boxes[i];	// restore old

			for(i = 0;i < NUM_CHECKBOXES;i++)
				mvwprintw(win, DISP_OFFSET+6,2+(i+2),"%d  ",prev_check_boxes[i]);
			wrefresh(win);
		next_prev_list = 1;
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
//	mvwprintw(win, DISP_OFFSET+2,2,"%d  %d  %d  %s ",menu_index,temp,ret_char,menu_labels[menu_structs[ret_char].label]);
//	mvwprintw(win, DISP_OFFSET+1,2, "do_chkbox %d %d  %d  %d  ",temp1,temp,menu_index,menu_structs[menu_index].menu);
	mvwprintw(win, DISP_OFFSET+1,2, "*do_chkbox %x	               ",ch);
	wrefresh(win);
#endif
	return ret_char;
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
//	mvwprintw(win, DISP_OFFSET+9,2, "*do_exec_choice %x	               ",curr_execchoice);
	wrefresh(win);
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
//	mvwprintw(win, DISP_OFFSET+2,2,"%d  %d  %d  %s ",menu_index,temp,ret_char,menu_labels[menu_structs[ret_char].label]);
//	mvwprintw(win, DISP_OFFSET+1,2, "non_func %d %d  %d  %d  ",temp1,temp,menu_index,menu_structs[menu_index].menu);
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
	char temp[MAX_LABEL_LEN];

	mvwprintw(win, DISP_OFFSET+10,2,"%d  ",index);
	for(i = 0;i < 6;i++)
	{
		strcpy(temp, menu_labels[menu_structs[get_curr_menu()].menus[i]]);
		if(strcmp(temp,"blank") != 0)
			mvwprintw(win, DISP_OFFSET+11+i, 2,"%s              ",temp);
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
static char* get_fptr_label(void)
{
	return menu_labels[menu_structs[get_curr_menu()].fptr+no_menu_labels];
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
int get_str_len(void)
{
	return cur_col-NUM_ENTRY_BEGIN_COL;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
char *get_rt_label(int index)
{
	return rt_labels[index];
}
/*
char *get_menu_label(int index)
{
	return menu_labels[index];
}
*/
UCHAR get_row(int index)
{
//	return menu_structs[index].row;
	return 0;
}
UCHAR get_col(int index)
{
//	return menu_structs[index].col;
	return 0;
}
//#endif

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
	curr_type = MAIN;
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
	next_prev_list = 0;
}
#endif
#if 0
void dispSetCursorX(UCHAR mode, UCHAR row, UCHAR col, UCHAR type)
{
	cursor_row = row;
	cursor_col = col;
}
#endif
