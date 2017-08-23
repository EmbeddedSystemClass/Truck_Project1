// menus.c
#ifdef MAIN_C
#warning "MAIN_C defined"
#else
#warning "MAIN_C not defined in menus.c"
#endif
#include "sfr_helper.h"
#include <stdlib.h>
#include "t6963.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#ifndef TEST_WRITE_DATA
#include <avr/io.h>
#include "avr8-gnu-toolchain-linux_x86/avr/include/util/delay.h"
#include <avr/eeprom.h>
#include "macros.h"
#include "USART.h"
#else
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <ncurses.h>
#endif
#include "main.h"
#include "avr_main.h"

#ifdef TEST_WRITE_DATA
void dispSetCursorX(UCHAR mode, UCHAR row, UCHAR col, UCHAR type)
{
//	cursor_row = row;
//	cursor_col = col;
}
WINDOW *win;
#else
extern char eepromString[STRING_LEN] EEMEM;
#define dispSetCursorX(mode, row, col, type) dispSetCursor(mode, row, col, type)
#endif

#if 1
static void clean_disp_num(void);
static UCHAR cursor_forward(UCHAR ch);
static void cursor_backward(void);
static void cursor_forward_stuff(char);
static void stuff_num(char);
static void blank_menu(void);
static void adv_menu_label(int index, UCHAR *row, UCHAR *col);
static void display_menus(void);
static void init_numentry(int);
static void scale_disp(int amt);
static void init_checkboxes(int menu_index);
static void init_execchoices(int);
static void blank_choices(void);
static void display_edit_value(void);
static UCHAR generic_menu_function(void);
static UCHAR backspace(UCHAR ch);
static UCHAR enter(UCHAR ch);
static UCHAR escape(UCHAR ch);
#if 0
static UCHAR scr_alnum0(UCHAR ch);
static UCHAR scr_alnum1(UCHAR ch);
static UCHAR scr_alnum2(UCHAR ch);
static UCHAR scr_alnum3(UCHAR ch);
static UCHAR alnum_enter(UCHAR ch);
static void scroll_alnum_list(int dir);
#endif
static UCHAR scrollup_checkboxes(UCHAR ch);
static UCHAR scrolldown_checkboxes(UCHAR ch);
static UCHAR toggle_checkboxes(UCHAR ch);
static UCHAR scrolldown_execchoice(UCHAR ch);
static UCHAR scrollup_execchoice(UCHAR ch);
/*
static UCHAR enter_checkboxes(UCHAR ch);
static UCHAR escape_checkboxes(UCHAR ch);
*/
static UCHAR menu_change(UCHAR ch);
static UCHAR exec_choice(UCHAR ch);
static UCHAR do_chkbox(UCHAR ch);
static UCHAR non_func(UCHAR ch);
static UCHAR do_numentry(UCHAR ch);

static char* get_fptr_label(UCHAR);

#endif
static int scroll_ptr;
static int cur_alnum_col;
static int dirty_flag;
static UCHAR cur_row, cur_col;	// used by the current menu/dialog function to keep track of the current row,col
static UCHAR alnum_array[NUM_ALNUM+1];
static UCHAR choose_alnum;
static UCHAR prev_menu_index;

static UCHAR (*fptr[NUM_FPTS])(UCHAR) = { menu_change, exec_choice, do_chkbox, non_func, do_numentry };

//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static UCHAR menu_change(UCHAR ch)
{
	UCHAR ret_char = ch;
#ifdef TEST_WRITE_DATA
	mvwprintw(win, DISP_OFFSET+20,2, "                    ");
#endif
	switch(ch)
	{
		case KP_A:
		break;
		case KP_B:
		break;
		case KP_C:
		break;
		case KP_D:
		break;
		case KP_POUND:
		break;
		case KP_0:
		break;
		case KP_AST:
		break;
		default:
		break;
	}
#ifdef TEST_WRITE_DATA
	mvwprintw(win, DISP_OFFSET+20,2, "*menu_change  %x   ",ret_char);
	wrefresh(win);
#endif
	return ret_char;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
UCHAR read_get_key(UCHAR *str)
{
	int i;
	UCHAR ret_char;
	memcpy(cur_param_string,str,NUM_UCHAR_PARAMS);

	ret_char = generic_menu_function();
/*
	if(new_data_ready == 1)
	{
		data_entry_mode = 1;
		display_edit_value();
		new_data_ready = 0;
	}
*/
	return ret_char;
}
//******************************************************************************************//
//********************************* generic_menu_function **********************************//
//******************************************************************************************//
static UCHAR generic_menu_function(void)
{
	UCHAR ret_char;
	int i,j;
	UCHAR temp;
	UCHAR menu_index;
	UCHAR ch = 0;
	UCHAR tfptr;
	int res,res2;

//	for(i = 0;i < NUM_UCHAR_PARAMS;i++)
//		cur_param_string[i]++;

	memset(aux_string,0,AUX_STRING_LEN);
	res = 0;
	res2 = 0;
#ifdef TEST_WRITE_DATA
	read(global_fd,&tfptr,1);
	read(global_fd,&ch,1);
	read(global_fd,&menu_index,1);
//	read(global_fd,cur_param_string,NUM_UCHAR_PARAMS);
	for(i = 0;i < NUM_UCHAR_PARAMS;i++)
		res += read(global_fd,&cur_param_string[i],1);

	read(global_fd,&temp,1);

	mvwprintw(win, DISP_OFFSET+25, 2,
			"aux_string read:%d menu_index %d  prev index: %d     ",temp,menu_index,prev_menu_index);
	for(i = 0;i < temp;i++)
		res2 += read(global_fd,&aux_string[i],1);
//	mvwprintw(win, DISP_OFFSET+30, 2,"fptr: %s   ch:%x    res:%d    res2:%d   ",get_fptr_label(tfptr),ch,res,res2);
	mvwprintw(win, DISP_OFFSET+26, 2,"ch:%x  fptr:%x  res:%d    res2:%d   ",ch,tfptr,res,res2);
#else
	tfptr = receiveByte();
	ch = receiveByte();
	menu_index = receiveByte();
	for(i = 0;i < NUM_UCHAR_PARAMS;i++)
	cur_param_string[i] = receiveByte();

	temp = receiveByte();
#endif
	ret_char = (*fptr[tfptr])(ch);		// execute the function pointer from the PIC
	display_menus();

	if(prev_menu_index != menu_index)
	{
		blank_choices();
		switch (menu_index)
		{
			case MAIN:
				break;
			case MENU1A:
				break;
			case MENU1B:
				break;
			case MENU1C:
#ifdef TEST_WRITE_DATA
				mvwprintw(win, DISP_OFFSET+27,2, "MENU1C: init_choices  %d          ",menu_index);
#endif
				init_execchoices(menu_index);
			break;
			case MENU1D:
#ifdef TEST_WRITE_DATA
				mvwprintw(win, DISP_OFFSET+27,2, "MENU1D: init_checkboxes           ");
#endif
				init_checkboxes(menu_index);
			case MENU1E:
#ifdef TEST_WRITE_DATA
				mvwprintw(win, DISP_OFFSET+27,2, "MENU1E: init_checkboxes           ");
#endif
				init_checkboxes(menu_index);
			break;
			case MENU2A:
#ifdef TEST_WRITE_DATA
				mvwprintw(win, DISP_OFFSET+27,2, "MENU2A:                           ");
#endif
			break;
			case MENU2B:
#ifdef TEST_WRITE_DATA
				mvwprintw(win, DISP_OFFSET+27,2, "MENU2B: init_choices  %d          ",menu_index);
#endif
				init_execchoices(menu_index);
			break;
			case MENU2C:
				init_numentry(menu_index);
#ifdef TEST_WRITE_DATA
				mvwprintw(win, DISP_OFFSET+27,2, "MENU2C: init_numentry             ");
#endif
			break;
		}
	}
	prev_menu_index = menu_index;
//#if 0
#ifdef TEST_WRITE_DATA
//	mvwprintw(win, DISP_OFFSET+2, 2,"fptr:   %s   %x   ",get_fptr_label(tfptr),ch);
	for(i = 0;i < AUX_STRING_LEN/3;i++)
		mvwprintw(win, DISP_OFFSET+31, 2+(i*3),"%x  ",aux_string[i]);
	for(i = AUX_STRING_LEN/3,j=0;i < (AUX_STRING_LEN/3)*2;i++,j++)
		mvwprintw(win, DISP_OFFSET+32, 2+(j*3),"%x  ",aux_string[i]);
	for(i = (AUX_STRING_LEN/3)*2,j=0;i < AUX_STRING_LEN;i++,j++)
		mvwprintw(win, DISP_OFFSET+33, 2+(j*3),"%x  ",aux_string[i]);
//	for(i = 0;i < AUX_STRING_LEN/2;i++)
//		mvwprintw(win, DISP_OFFSET+32, 2+(i*3),"%d  ",i);
	wrefresh(win);
#endif
//#endif
	return ret_char;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static char* get_fptr_label(UCHAR fptr)
{
//s	return menu_labels[fptr+18];
	return (char*)0;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static UCHAR do_numentry(UCHAR ch)
{
	UCHAR ret_char = ch;
	switch(ch)
	{
		case KP_0:
		cursor_forward_stuff(0);
		case KP_1:
		cursor_forward_stuff(1);
		case KP_2:
		cursor_forward_stuff(2);
		case KP_3:
		cursor_forward_stuff(3);
		case KP_4:
		cursor_forward_stuff(4);
		case KP_5:
		cursor_forward_stuff(5);
		case KP_6:
		cursor_forward_stuff(6);
		case KP_7:
		cursor_forward_stuff(7);
		case KP_8:
		cursor_forward_stuff(8);
		case KP_9:
		cursor_forward_stuff(9);
		break;
		case KP_A:
		cursor_forward(ch);
#ifdef TEST_WRITE_DATA
		mvwprintw(win, DISP_OFFSET+21,2, "A  ");
#endif
		break;
		case KP_B:
		backspace(ch);
#ifdef TEST_WRITE_DATA
		mvwprintw(win, DISP_OFFSET+21,2, "B  ");
#endif
		break;
		case KP_C:
		enter(ch);
#ifdef TEST_WRITE_DATA
		mvwprintw(win, DISP_OFFSET+21,2, "C  ");
#endif
		break;
		case KP_D:
#ifdef TEST_WRITE_DATA
		mvwprintw(win, DISP_OFFSET+21,2, "D  ");
#endif
		break;
		case KP_POUND:
#ifdef TEST_WRITE_DATA
		mvwprintw(win, DISP_OFFSET+21,2, "#  ");
#endif
		break;
		case KP_AST:
#ifdef TEST_WRITE_DATA
		mvwprintw(win, DISP_OFFSET+21,2, "*  ");
#endif
		break;
		default:
		break;
	}
#ifdef TEST_WRITE_DATA
	mvwprintw(win, DISP_OFFSET+20,2, "*do_numentry  %x   ",ret_char);
	wrefresh(win);
#endif
	return ret_char;
}
//******************************************************************************************//
//******************************************* escape ***************************************//
//******************************************************************************************//
static UCHAR escape(UCHAR ch)
{
//	memcpy((void*)new_global_number,(void*)cur_global_number,NUM_ENTRY_SIZE);
	memset((void*)cur_global_number,0,NUM_ENTRY_SIZE);
	cur_col = NUM_ENTRY_BEGIN_COL;
//	prev_list();
	clean_disp_num();
//	scale_disp(SCALE_DISP_ALL);
	data_entry_mode = 0;
//	mod_data_ready = 2;
	return ch;
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
		if(limit < 0)
			strcpy(cur_global_number,"32766\0");
		memcpy((void*)new_global_number,(void*)cur_global_number,NUM_ENTRY_SIZE);
		cur_col = NUM_ENTRY_BEGIN_COL;
//		prev_list();
		clean_disp_num();
		mod_data_ready = 1;
//		sample_data[aux_index-1] = atol(new_global_number);
	}
#ifdef TEST_WRITE_DATA
	mvwprintw(win, DISP_OFFSET+37, 2,"new: %s       %d    ",new_global_number,limit);
#endif
//	scale_disp(SCALE_DISP_ALL);
	return ch;
}
//******************************************************************************************//
//************************************* init_numentry ************************************//
//******************************************************************************************//
static void init_numentry(int menu_index)
{
#ifdef TEST_WRITE_DATA
//	mvwprintw(win, 41, 3,"                                   ");
#endif
//	scale_disp(SCALE_DISP_SOME);
	cur_row = NUM_ENTRY_ROW;
	cur_col = NUM_ENTRY_BEGIN_COL;
	memset((void*)new_global_number,0,NUM_ENTRY_SIZE);
	memset((void*)cur_global_number,0,NUM_ENTRY_SIZE);
	clean_disp_num();
	dispCharAt(NUM_ENTRY_ROW,cur_col,'/');
	dispCharAt(NUM_ENTRY_ROW,cur_col+NUM_ENTRY_SIZE,'/');
	new_data_ready = 1;
}
//******************************************************************************************//
//********************************* display_edit_value *************************************//
//******************************************************************************************//
static void display_edit_value(void)
{
	GDispStringAt(NUM_ENTRY_ROW,NUM_ENTRY_BEGIN_COL,cur_global_number);
	cur_col = strlen(cur_global_number)+NUM_ENTRY_BEGIN_COL;
//	dispCharAt(NUM_ENTRY_ROW,cur_col,'x');
    dispSetCursorX(TEXT_ON | CURSOR_BLINK_ON,NUM_ENTRY_ROW,cur_col,LINE_8_CURSOR);
#ifdef TEST_WRITE_DATA
		dispCharAt(NUM_ENTRY_ROW+1,cur_col,95);
#endif
}
//******************************************************************************************//
//************************************* cursor_forward *************************************//
//******************************************************************************************//
static UCHAR cursor_forward(UCHAR ch)
{
#ifdef TEST_WRITE_DATA
		dispCharAt(NUM_ENTRY_ROW+1,cur_col,95);
#endif
	if(++cur_col > NUM_ENTRY_END_COL)
		cur_col = NUM_ENTRY_BEGIN_COL;
    dispSetCursorX(TEXT_ON | CURSOR_BLINK_ON,NUM_ENTRY_ROW,cur_col,LINE_8_CURSOR);
#ifdef TEST_WRITE_DATA
		dispCharAt(NUM_ENTRY_ROW+1,cur_col,95);
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
//		mvwprintw(win, DISP_OFFSET+39,2,"bs:%s      %d  ",cur_global_number,cur_col);
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
		dispCharAt(NUM_ENTRY_ROW+1,cur_col,0x20);
#endif
	if(--cur_col < NUM_ENTRY_BEGIN_COL)
		cur_col = NUM_ENTRY_BEGIN_COL;
	dispSetCursorX(TEXT_ON | CURSOR_BLINK_ON,NUM_ENTRY_ROW,cur_col,LINE_8_CURSOR);
#ifdef TEST_WRITE_DATA
		dispCharAt(NUM_ENTRY_ROW+1,cur_col,95);
#endif
}
//******************************************************************************************//
//********************************** cursor_forward_stuff **********************************//
//******************************************************************************************//
static void cursor_forward_stuff(char x)
{
	if(data_entry_mode)
	{
		stuff_num(x);
		cursor_forward(x);
#ifdef TEST_WRITE_DATA
		mvwprintw(win, DISP_OFFSET+40, 2,"stuff: %d   %d  ",x,cur_col);
#endif
	}
}
//******************************************************************************************//
//*************************************** stuff_num ****************************************//
//******************************************************************************************//
static void stuff_num(char num)
{
	num += 0x30;
	dispCharAt(NUM_ENTRY_ROW,cur_col,num);
	cur_global_number[cur_col-NUM_ENTRY_BEGIN_COL] = num;
#ifdef TEST_WRITE_DATA
	mvwprintw(win, DISP_OFFSET+38, 2,"cur: %s           %d   ",cur_global_number,cur_col);
#endif
}
//******************************************************************************************//
//************************************ clean_disp_num **************************************//
//******************************************************************************************//
static void clean_disp_num(void)
{
	int i;
	for(i = 0;i < NUM_ENTRY_SIZE+1;i++)
	{
		dispCharAt(NUM_ENTRY_ROW,i+NUM_ENTRY_BEGIN_COL,0x20);
		dispCharAt(NUM_ENTRY_ROW,i+NUM_ENTRY_BEGIN_COL+1,0x20);
	}
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static UCHAR exec_choice(UCHAR ch)
{

	UCHAR ret_char = ch;

//	mvwprintw(win, DISP_OFFSET+20,2, "                    ");

	switch(ch)
	{
		case KP_A:
		scrollup_execchoice(ch);
		break;
		case KP_B:
		scrolldown_execchoice(ch);
		break;
		case KP_C:
		break;
		case KP_D:
		break;
		case KP_POUND:
		break;
		case KP_0:
		break;
		case KP_AST:
		break;
		default:
		break;
	}
#ifdef TEST_WRITE_DATA
	mvwprintw(win, DISP_OFFSET+20,2, "*exec_choice  %x   ",ret_char);
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
//	mvwprintw(win, DISP_OFFSET+20,2, "                    ");
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
		break;
		case KP_POUND:		// esc
			for(i = 0;i < NUM_CHECKBOXES;i++)
				check_boxes[i].checked = prev_check_boxes[i];	// restore old
		break;
		case KP_0:
		break;
		case KP_AST:
		break;
		default:
		break;
	}
#ifdef TEST_WRITE_DATA
	mvwprintw(win, DISP_OFFSET+20,2, "*do_chkbox  %x   ",ret_char);
	wrefresh(win);
#endif
	return ret_char;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static UCHAR non_func(UCHAR ch)
{
	UCHAR ret_char = ch;

#ifdef TEST_WRITE_DATA
	mvwprintw(win, DISP_OFFSET+20,2, "                    ");
#endif

	switch(ch)
	{
		case KP_A:
		break;
		case KP_B:
		break;
		case KP_C:
		break;
		case KP_D:
		break;
		case KP_POUND:
		break;
		case KP_0:
		break;
		case KP_AST:
		break;
		default:
		break;
	}
#ifdef TEST_WRITE_DATA
	mvwprintw(win, DISP_OFFSET+20,2, "*non_func  %x   ",ret_char);
	wrefresh(win);
#endif
	return ret_char;
}
//******************************************************************************************//
//************************************ init_checkboxes *************************************//
//******************************************************************************************//
static void init_checkboxes(int menu_index)
{
	int i;
	UCHAR row, col;
//	scale_disp(SCALE_DISP_SOME);
	row = 1;
	col = 3;
	curr_checkbox = 0;
	for(i = 0;i < NUM_CHECKBOXES;i++)
	{
		prev_check_boxes[i] = check_boxes[i].checked;		// save previous in case of escape key to restore
		check_boxes[i].checked = aux_string[i];				// load what's sent from PIC
	}

// setting the list to choice 'n' is all well and good but we need a way for the PIC to send
// the choices over in case there are more than 1 lists of choices
	strcpy(check_boxes[0].string,"choice 1\0");
	strcpy(check_boxes[1].string,"choice 2\0");
	strcpy(check_boxes[2].string,"choice 3\0");
	strcpy(check_boxes[3].string,"choice 4\0");
	strcpy(check_boxes[4].string,"choice 5\0");
	strcpy(check_boxes[5].string,"choice 6\0");
	strcpy(check_boxes[6].string,"choice 7\0");
	strcpy(check_boxes[7].string,"choice 8\0");
	strcpy(check_boxes[8].string,"choice 9\0");
	strcpy(check_boxes[9].string,"choice 10\0");

	for(i = 0;i < NUM_CHECKBOXES;i++)
	{
		check_boxes[i].index = i;
		GDispStringAt(row,col,check_boxes[i].string);
		row++;
		if(check_boxes[i].checked == 1)
		{
			dispCharAt(1+check_boxes[i].index,0,120);
		}
		else
		{
			dispCharAt(1+check_boxes[i].index,0,0x20);
		}
	}
}
//******************************************************************************************//
//************************************ init_execchoice *************************************//
//******************************************************************************************//
static void init_execchoices(int menu_index)
{
	int i;
	UCHAR row, col;
//	scale_disp(SCALE_DISP_SOME);
	row = 1;
	col = 3;
	curr_execchoice = 0;
//	for(i = 0;i < NUM_EXECCHOICES;i++)
//		check_boxes[i].checked = aux_string[i];

// setting the list to choice 'n' is all well and good but we need a way for the PIC to send
// the choices over in case there are more than 1 lists of choices
	strcpy(exec_choices[0].string,"exec 1\0");
	strcpy(exec_choices[1].string,"exec 2\0");
	strcpy(exec_choices[2].string,"exec 3\0");
	strcpy(exec_choices[3].string,"exec 4\0");
	strcpy(exec_choices[4].string,"exec 5\0");
	strcpy(exec_choices[5].string,"exec 6\0");
	strcpy(exec_choices[6].string,"exec 7\0");
	strcpy(exec_choices[7].string,"exec 8\0");
	strcpy(exec_choices[8].string,"exec 9\0");
	strcpy(exec_choices[9].string,"exec 10\0");


	for(i = 0;i < NUM_EXECCHOICES;i++)
	{
		exec_choices[i].index = i;
		GDispStringAt(row,col,exec_choices[i].string);
		row++;
/*
		if(exec_choices[i].checked == 1)
		{
			dispCharAt(1+exec_choices[i].index,0,120);
		}
		else
		{
			dispCharAt(1+exec_choices[i].index,0,0x20);
		}
*/
	}
	dispCharAt(1+exec_choices[0].index,0,120);

}
//******************************************************************************************//
//********************************** scrollup_checkboxes ***********************************//
//******************************************************************************************//
static UCHAR scrollup_checkboxes(UCHAR ch)
{
	if(--curr_checkbox < 0)
		curr_checkbox = last_checkbox;
	// move cursor to 3 + check_boxes[curr_checkbox]
//	dispCharAt(1+check_boxes[curr_checkbox].index,0,0x21);
	return ch;
}
//******************************************************************************************//
//********************************* scrolldown_checkboxes **********************************//
//******************************************************************************************//
static UCHAR scrolldown_checkboxes(UCHAR ch)
{
	if(++curr_checkbox > last_checkbox)
		curr_checkbox = 0;
	// move cursor
//	dispCharAt(1+check_boxes[curr_checkbox].index,0,0x21);
	return ch;
}
//******************************************************************************************//
//********************************** toggle_checkboxes *************************************//
//******************************************************************************************//
static UCHAR toggle_checkboxes(UCHAR ch)
{
	if(check_boxes[curr_checkbox].checked == 1)
	{
		check_boxes[curr_checkbox].checked = 0;
		dispCharAt(1+check_boxes[curr_checkbox].index,0,0x20);	// display 'blank'
	}
	else
	{
		check_boxes[curr_checkbox].checked = 1;
		dispCharAt(1+check_boxes[curr_checkbox].index,0,120);	// display 'x'
	}
	return ch;
}
//******************************************************************************************//
//********************************** scrollup_checkboxes ***********************************//
//******************************************************************************************//
static UCHAR scrollup_execchoice(UCHAR ch)
{
	UCHAR temp = curr_execchoice;
	if(--curr_execchoice < 0)
		curr_execchoice = last_execchoice;

//	mvwprintw(win, DISP_OFFSET+22,2,"%d %d   ",temp,curr_execchoice);
//	wrefresh(win);

	dispCharAt(1+exec_choices[temp].index,0,0x20);
	dispCharAt(1+exec_choices[curr_execchoice].index,0,120);
	return ch;
}
//******************************************************************************************//
//********************************* scrolldown_checkboxes **********************************//
//******************************************************************************************//
static UCHAR scrolldown_execchoice(UCHAR ch)
{
	UCHAR temp = curr_execchoice;
	if(++curr_execchoice > last_execchoice)
		curr_execchoice = 0;

//	mvwprintw(win, DISP_OFFSET+22,2,"%d %d   ",temp,curr_execchoice);
//	wrefresh(win);

	dispCharAt(1+exec_choices[temp].index,0,0x20);
	dispCharAt(1+exec_choices[curr_execchoice].index,0,120);
	return ch;
}
//******************************************************************************************//
//*********************************** enter_checkboxes *************************************//
//******************************************************************************************//
#if 0
static UCHAR enter_checkboxes(UCHAR ch)
{
	blank_choices();
//	scale_disp(SCALE_DISP_ALL);
//	prev_list();
	return ch;
}
//******************************************************************************************//
//********************************** escape_checkboxes *************************************//
//******************************************************************************************//
static UCHAR escape_checkboxes(UCHAR ch)
{
	blank_choices();
//	scale_disp(SCALE_DISP_ALL);
//	prev_list();
	return ch;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
#endif
static void blank_choices(void)
{
	int row,col,i;
	row = 1;
	col = 0;
	char blank[] = "                     ";
	for(i = 0;i < NUM_CHECKBOXES;i++)
	{
		GDispStringAt(row,col,blank);
		row++;
	}
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
	return 0;
}
UCHAR get_col(int index)
{
	return 0;
}
//******************************************************************************************//
//************************************ adv_menu_label **************************************//
//******************************************************************************************//
void adv_menu_label(int index, UCHAR *row, UCHAR *col)
{
//	int menu_index = index * 6;
	char temp[MAX_LABEL_LEN];
	char temp2[4];
	switch (index % 6)
	{
		case 0: strncpy(temp2," A:\0",4);break;
		case 1: strncpy(temp2," B:\0",4);break;
		case 2: strncpy(temp2," C:\0",4);break;
		case 3: strncpy(temp2," D:\0",4);break;
		case 4: strncpy(temp2," #:\0",4);break;
		case 5: strncpy(temp2," 0:\0",4);break;
		default:strncpy(temp2,"\0",4);break;
	}

	strncpy(temp,cur_param_string+(index*MAX_LABEL_LEN),MAX_LABEL_LEN);
	if(strcmp(temp,"blank") != 0)
	{
		GDispStringAt(*row,*col,temp2);
		GDispStringAt(*row,*col+3,temp);
	}

//printf("%d %d %d %s\n",index,*row,*col,temp);

	if(*col > MAX_LABEL_LEN*2-1)
	{
		*col = 0;
		(*row)++;
	}
	else
		*col += MAX_LABEL_LEN;
}
//******************************************************************************************//
//************************************* display_menus **************************************//
//******************************************************************************************//
// display a different menu
// should be able to get the label by indexing into the labels area by:
// (no_rtparams*sizeof(int) + get_type()*sizeof(int)) * menu_struct[i].ch_type
static void display_menus(void)
{
	UCHAR row,col;
	row = MENU_START_ROW;
	col = 0;
//	mvwprintw(win, 42, 3,"index: %d",index);
	blank_menu();
	adv_menu_label(0,&row,&col);
	adv_menu_label(1,&row,&col);
	adv_menu_label(2,&row,&col);
	adv_menu_label(3,&row,&col);
	adv_menu_label(4,&row,&col);
	adv_menu_label(5,&row,&col);
//	mvwprintw(win, 45, 25,"index: %d ",index);
#ifdef TEST_WRITE_DATA
//	mvwprintw(win, 45, 3, "current menu: %s  ",labels[get_curr_menu()+no_rtparams]);
#endif
}
//******************************************************************************************//
//**************************************** display_labels **********************************//
//******************************************************************************************//
// displays the rt_labels if shown is set
void display_labels(void)
{
	int i;
	char temp[MAX_LABEL_LEN];
	char blank[] = "               ";

	for(i = 0;i < no_rtparams;i++)
	{
		GDispStringAt(rt_params[i].row,rt_params[i].col,blank);
	}

	for(i = 0;i < no_rtparams;i++)
	{
		if(rt_params[i].shown == SHOWN_SENT)
		{
			strcpy(temp,get_rt_label(i));
			GDispStringAt(rt_params[i].row,rt_params[i].col,temp);
		}
	}
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
// reduce output to show just rpm, mph, engine temp and oil pressure at the top
// amt = SCALE_DISP_ALL - display all rt_params
// amt = SCALE_DISP_SOME - display limited rt_params
// amt = SCALE_DISP_NONE - don't display any rt_params
static void scale_disp(int amt)
{
	int i;
	char blank[] = "               ";

	scale_type = amt;
	switch (amt)
	{
		case SCALE_DISP_ALL:
			for(i = 0;i < no_rtparams;i++)
				if(rt_params[i].type == RT_AUX1 || rt_params[i].type == RT_AUX2)
					rt_params[i].shown = NOSHOWN_SENT;
				else
					rt_params[i].shown = SHOWN_SENT;
			break;
		case SCALE_DISP_SOME:
			for(i = 0;i < no_rtparams;i++)
			{
				if(rt_params[i].type == RT_RPM || rt_params[i].type == RT_ENGT || \
						rt_params[i].type == RT_MPH || rt_params[i].type == RT_OILP)
					rt_params[i].shown = SHOWN_SENT;
				else if(rt_params[i].type == RT_AUX1 || rt_params[i].type == RT_AUX2)
					rt_params[i].shown = NOSHOWN_SENT;
				else
					rt_params[i].shown = NOSHOWN_NOSENT;

				GDispStringAt(rt_params[i].row,rt_params[i].col,blank);
			}
			break;
		case SCALE_DISP_NONE:
			for(i = 0;i < no_rtparams;i++)
			{
				if(rt_params[i].type == RT_AUX1 || rt_params[i].type == RT_AUX2)
					rt_params[i].shown = NOSHOWN_SENT;
				else
					rt_params[i].shown = NOSHOWN_NOSENT;
				GDispStringAt(rt_params[i].row,rt_params[i].col,blank);
			}
			break;
		default:
			break;
	}
	display_labels();
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static void blank_menu(void)
{
	int row,col;
	for(row = MENU_START_ROW-1;row < 15;row++)
		for(col = 0;col < COLUMN+1;col++)
			dispCharAt(row,col,0x20);
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
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
int get_str_len(void)
{
	return cur_col-NUM_ENTRY_BEGIN_COL;
}
#if 0
//******************************************************************************************//
//************************************** scr_alnum0 ****************************************//
//******************************************************************************************//
static UCHAR scr_alnum0(UCHAR ch)
{
	scroll_alnum_list(0);		// CAPS
	return ch;
}
//******************************************************************************************//
//*************************************** scr_alnum1 ***************************************//
//******************************************************************************************//
static UCHAR scr_alnum1(UCHAR ch)
{
	scroll_alnum_list(1);		// small
	return ch;
}
//******************************************************************************************//
//*************************************** scr_alnum2 ***************************************//
//******************************************************************************************//
static UCHAR scr_alnum2(UCHAR ch)
{
	scroll_alnum_list(2);		// special
	return ch;
}
//******************************************************************************************//
//*************************************** scr_alnum3 ***************************************//
//******************************************************************************************//
static UCHAR scr_alnum3(UCHAR ch)
{
	scroll_alnum_list(3);		// next char in list
	return ch;
}
//******************************************************************************************//
//************************************* alnum_enter ****************************************//
//******************************************************************************************//
static UCHAR alnum_enter(UCHAR ch)
{
	cur_global_number[cur_col-NUM_ENTRY_BEGIN_COL] = choose_alnum;
	cursor_forward(ch);
//	prev_list();
	return ch;
}
//******************************************************************************************//
//********************************** scroll_alnum_list *************************************//
//******************************************************************************************//
static void scroll_alnum_list(int dir)
{
	switch (dir)
	{
		case 0:		// start at 'A'
			scroll_ptr = 0;
			while(alnum_array[++scroll_ptr] != 'A');
			choose_alnum = alnum_array[scroll_ptr];
			dispCharAt(NUM_ENTRY_ROW,cur_col,choose_alnum);
		break;
		case 1:		// start at 'a'
			scroll_ptr = 0;
			while(alnum_array[++scroll_ptr] != 'a');
			choose_alnum = alnum_array[scroll_ptr];
			dispCharAt(NUM_ENTRY_ROW,cur_col,choose_alnum);
		break;
		case 2:		// start at '!' (very first one)
			scroll_ptr = 0;
//			while(alnum_array[++scroll_ptr] != '!');	// this will crash it
			choose_alnum = alnum_array[scroll_ptr];
			dispCharAt(NUM_ENTRY_ROW,cur_col,choose_alnum);
		break;
		case 3:		// advance the chosen character to next one
			if(++scroll_ptr > NUM_ALNUM-1)
				scroll_ptr = 0;
			choose_alnum = alnum_array[scroll_ptr];
			dispCharAt(NUM_ENTRY_ROW,cur_col,choose_alnum);
//			mvwprintw(win, 45, 3,"next char in list");
		break;
		default:
		break;
	}
	cur_global_number[cur_col-NUM_ENTRY_BEGIN_COL] = choose_alnum;
}
#endif

