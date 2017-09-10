// AVR_menus.c
//
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
//static char *show_keypress(UCHAR key, int offset, char *string);
void dispSetCursorX(UCHAR mode, UCHAR row, UCHAR col, UCHAR type)
{
//	cursor_row = row;
//	cursor_col = col;
}
WINDOW *win;
UCHAR eeprom_sim[EEPROM_SIZE];
#endif
#ifdef MAIN_C
extern char eepromString[EEPROM_SIZE] EEMEM;
#define dispSetCursorX(mode, row, col, type) dispSetCursor(mode, row, col, type)
#endif

static void clean_disp_num(void);
static void _eclear(void);
static UCHAR cursor_forward(UCHAR ch);
static void cursor_backward(void);
static void cursor_forward_stuff(char);
static void stuff_num(char);
static void blank_menu(void);
static void blank_display(void);
static void adv_menu_label(int index, UCHAR *row, UCHAR *col);
static void display_menus(void);
static void init_numentry(int);
static void scale_disp(int amt);
static void init_checkboxes(int menu_index);
static void init_execchoices(int);
static void blank_choices(void);
static void display_edit_value(void);
static UCHAR generic_menu_function(UCHAR ch);
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
static UCHAR do_exec(UCHAR ch);
static UCHAR do_chkbox(UCHAR ch);
static UCHAR non_func(UCHAR ch);
static UCHAR do_numentry(UCHAR ch);
static UCHAR do_init(UCHAR ch);
static void get_fptr_label(int index, char *str);

static int scroll_ptr;
static int cur_alnum_col;
static int dirty_flag;
static UCHAR cur_row, cur_col;	// used by the current menu/dialog function to keep track of the current row,col
static UCHAR alnum_array[NUM_ALNUM+1];
static UCHAR choose_alnum;
static UCHAR prev_menu_index;
//static UCHAR new_data_ch, new_data_ex;

static UCHAR (*fptr[NUM_FPTS])(UCHAR) = { menu_change, do_exec, do_chkbox, non_func, do_numentry, do_init };

//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static UCHAR menu_change(UCHAR ch)
{
	UCHAR ret_char = ch;
#ifdef TEST_WRITE_DATA
	mvwprintw(win, DISP_OFFSET+17,2, "                    ");
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
//	mvwprintw(win, DISP_OFFSET+17,2, "*menu_change  %x   ",show_keypress(ret_char,offset));
	mvwprintw(win, DISP_OFFSET+17,2, "*menu_change  %x   ",ret_char);
	mvwprintw(win, DISP_OFFSET+24, 2,"                           ");
	wrefresh(win);
#endif
	return ret_char;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
/*
	In real life, the main.c in the AVR will read the 1st keypress and then call this function:

		ret_char = receiveByte();
		read_get_key(ret_char);
		...
*/
UCHAR read_get_key(UCHAR key)
{
	int i,j,k;
	UCHAR ret_char = key;
	int size = 0;
	int start_addr = 0;
	int res = 0;
	int res2 = 0;
	int type = 0;
	UCHAR low_byte, high_byte, recByte;
	int choice_aux_offset;
	int exec_aux_offset;

	switch(ret_char)
	{
		case INIT:
			init_list();
			break;
		case SPACE:
#ifdef TEST_WRITE_DATA
			for(i = 1;i < LAST_ROW+1;i++)
				mvwprintw(win, i,2,"                                                            ");
			wrefresh(win);
#endif
			break;
		case READ_EEPROM:
#ifdef TEST_WRITE_DATA

			read(global_fd,&high_byte,1);
			read(global_fd,&low_byte,1);
			size = pack(low_byte,high_byte);

			read(global_fd,&high_byte,1);
			read(global_fd,&low_byte,1);
			start_addr = pack(low_byte,high_byte);

			for(i = start_addr;i < size+start_addr;i++)
				res2 += write(global_fd,&eeprom_sim[i],1);

			j = 0;
			k = 0;
			memcpy((void*)&no_rt_labels, (void*)(eeprom_sim+NO_RT_LABELS_EEPROM_LOCATION),sizeof(UINT));
			memcpy((void*)&no_rtparams, (void*)(eeprom_sim+NO_RTPARAMS_EEPROM_LOCATION),sizeof(UINT));
			memcpy((void*)&no_menu_labels, (void*)(eeprom_sim+NO_MENU_LABELS_EEPROM_LOCATION),sizeof(UINT));

			mvwprintw(win, LAST_ROW-4,2,
				"no_rt_labels: %d no_menu_labels: %d no_rtparams: %d  ",no_rt_labels,no_menu_labels,no_rtparams);

			for(i = 1;i < LAST_ROW+1;i++)
				mvwprintw(win, i,2,"                                                            ");

			for(i = 0;i < EEPROM_SIZE;i++)
			{
				if(eeprom_sim[i] < 0x7e && eeprom_sim[i] > 0x21)
					mvwprintw(win, LAST_ROW+j-40, 2+k,"%c",eeprom_sim[i]);
				else if(eeprom_sim[i] == 0)	mvwprintw(win, LAST_ROW+j-40, 24+k," ");
				else mvwprintw(win, LAST_ROW+j-40, 2+k,"_");
				if(++k > 30)
				{
					k = 0;
					++j;
				}
			}
			k = 0;
			for(i = NO_MENU_LABELS_EEPROM_LOCATION;i < NO_MENUS_EEPROM_LOCATION;i++,k++)
			{
				mvwprintw(win, LAST_ROW-3,2+(k*3),"%d ", eeprom_sim[i]);
			}
			mvwprintw(win, LAST_ROW-2,1,
				"read_eeprom - size: %d start: %d type: %d res: %d res2: %d ",size,start_addr,type,res,res2);
#else
			high_byte = receiveByte();
			low_byte = receiveByte();
			size = pack(low_byte,high_byte);

			high_byte = receiveByte();
			low_byte = receiveByte();
			start_addr = pack(low_byte,high_byte);

			for(i = start_addr;i < size+start_addr;i++)
			{
				low_byte = (UCHAR)eeprom_read_byte((const uint8_t *)eepromString+i);
				transmitByte(low_byte);
			}

/*
			memcpy((void*)&no_rt_labels, (void*)(eeprom_sim+NO_RT_LABELS_EEPROM_LOCATION),sizeof(UINT));
			memcpy((void*)&no_rtparams, (void*)(eeprom_sim+NO_RTPARAMS_EEPROM_LOCATION),sizeof(UINT));
			memcpy((void*)&no_menu_labels, (void*)(eeprom_sim+NO_MENU_LABELS_EEPROM_LOCATION),sizeof(UINT));
*/
#endif
			goffset = 0;
			get_label_offsets();
			break;
		case BURN_EEPROM:
#ifdef TEST_WRITE_DATA
			read(global_fd,&high_byte,1);
			read(global_fd,&low_byte,1);
			size = pack(low_byte,high_byte);

			read(global_fd,&high_byte,1);
			read(global_fd,&low_byte,1);
			start_addr = pack(low_byte,high_byte);

			for(i = start_addr;i < size+start_addr;i++)
				res2 += read(global_fd,&eeprom_sim[i],1);

			low_byte = 0x55;
			write(global_fd,&low_byte,1);

			mvwprintw(win, LAST_ROW-2,1,
				"burn eeprom - size: %d start: %d type: %d res: %d res2: %d ",size,start_addr,type,res,res2);

			k = j = 0;
			for(i = 1;i < LAST_ROW+1;i++)
				mvwprintw(win, i,2,"                                                            ");

			for(i = 0;i < EEPROM_SIZE;i++)
			{
				if(eeprom_sim[i] < 0x7e && eeprom_sim[i] > 0x21)
					mvwprintw(win, LAST_ROW+j-40, 2+k,"%c",eeprom_sim[i]);
				else if(eeprom_sim[i] == 0)	mvwprintw(win, LAST_ROW+j-40, 24+k," ");
				else mvwprintw(win, LAST_ROW+j-40, 2+k,"_");
				if(++k > 30)
				{
					k = 0;
					++j;
				}
			}
			k = 0;
			for(i = NO_MENU_LABELS_EEPROM_LOCATION;i < NO_MENUS_EEPROM_LOCATION;i++,k++)
			{
				mvwprintw(win, LAST_ROW-3,2+(k*3),"%d ", eeprom_sim[i]);
			}

			memcpy((void*)&no_rt_labels, (void*)(eeprom_sim+NO_RT_LABELS_EEPROM_LOCATION),sizeof(UINT));
			memcpy((void*)&no_rtparams, (void*)(eeprom_sim+NO_RTPARAMS_EEPROM_LOCATION),sizeof(UINT));
			memcpy((void*)&no_menu_labels, (void*)(eeprom_sim+NO_MENU_LABELS_EEPROM_LOCATION),sizeof(UINT));

#else
			high_byte = receiveByte();
			low_byte = receiveByte();
			size = pack(low_byte,high_byte);

			high_byte = receiveByte();
			low_byte = receiveByte();
			start_addr = pack(low_byte,high_byte);

			for(i = start_addr;i < size+start_addr;i++)
			{
				recByte = receiveByte();
				if(recByte != 0)
					eeprom_update_byte((UCHAR*)(eepromString+i),recByte);
//				transmitByte(eeprom_read_byte(eepromString+i));
			}
			low_byte = 0x55;
			transmitByte(low_byte);
#endif
			goffset = 0;
			get_label_offsets();

			break;
		case BURN_PART:
#ifdef TEST_WRITE_DATA
			read(global_fd,&high_byte,1);
			read(global_fd,&low_byte,1);
			size = pack(low_byte,high_byte);

			read(global_fd,&high_byte,1);
			read(global_fd,&low_byte,1);
			start_addr = pack(low_byte,high_byte);

			for(i = start_addr;i < size+start_addr;i++)
				res2 += read(global_fd,&eeprom_sim[i],1);

			low_byte = 0x55;
			write(global_fd,&low_byte,1);

			mvwprintw(win, LAST_ROW-2,1,
				"burn eeprom - size: %d start: %d type: %d res: %d res2: %d ",size,start_addr,type,res,res2);
#else

			high_byte = receiveByte();
			transmitByte(high_byte);
			low_byte = receiveByte();
			transmitByte(low_byte);
			size = pack(low_byte,high_byte);

			high_byte = receiveByte();
			transmitByte(high_byte);
			low_byte = receiveByte();
			transmitByte(low_byte);
			start_addr = pack(low_byte,high_byte);

//			for(i = start_addr;i < size+start_addr;i++)

			for(i = 0;i < size;i++)
			{
				aux_string[i] = receiveByte();
				transmitByte(aux_string[i]);
			}
//			for(i = 0;i < 240;i++)
//				transmitByte(aux_string[i]);
//			_delay_ms(1);

//			low_byte = 0xAA;
//			transmitByte(low_byte);
//			_delay_ms(1);

			eeprom_update_block((const void*)&aux_string[0],(void *)eepromString,(size_t)size);

#endif
			goffset = 0;
//			get_label_offsets();

			break;
		default:
			ret_char = generic_menu_function(ret_char);
			break;
	}
#ifdef TEST_WRITE_DATA
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
	int i,j,k;
	int aux_bytes_to_read;
	int aux_data_offset;
	UCHAR menu_index;
	UCHAR tfptr;
	int res,res2;
	UCHAR low_byte,high_byte;
#ifdef TTY_DISPLAY
	UCHAR tlow_byte,thigh_byte;
#endif
	char tlabel[MAX_LABEL_LEN];
//	UCHAR tlow;

	res = 0;
	res2 = 0;
#ifdef TEST_WRITE_DATA
	res += read(global_fd,&tfptr,1);
	res += 	read(global_fd,&high_byte,1);
	res += 	read(global_fd,&low_byte,1);
	menu_index = pack(low_byte,high_byte);

	for(i = 0;i < 6;i++)
	{
		res += read(global_fd,&high_byte,1);
		res += read(global_fd,&low_byte,1);
		// for some reason when sending a 13 it get recv'd as a 10
		// so I changed it in the pack/unpack functions
//		low_byte = ~low_byte;
		curr_menus[i] = pack(low_byte,high_byte);
//		mvwprintw(win, DISP_OFFSET+20, 2+(i*7),"%d    ",low_byte);
//		mvwprintw(win, DISP_OFFSET+21, 2+(i*7),"%d    ",curr_menus[i]);
		if(curr_menus[i] > blank)
			curr_menus[i] = 0;
	}
	res += read(global_fd,&high_byte,1);
	res += read(global_fd,&low_byte,1);
	aux_bytes_to_read = pack(low_byte,high_byte);
	mvwprintw(win, DISP_OFFSET+14, 2,"%x  %x   %d  ",low_byte, high_byte, res);
	mvwprintw(win, DISP_OFFSET+13,2,"                             ");
	mvwprintw(win, DISP_OFFSET+14, 2,
			"aux_string read:%d menu_index %d  prev index: %d  ",aux_bytes_to_read,menu_index,prev_menu_index);
	for(i = 0;i < aux_bytes_to_read;i++)
		res2 += read(global_fd,&aux_string[i],1);

	low_byte = 0;
	// write a code of 0 back to sim PIC
	write(global_fd,&low_byte,1);

	get_fptr_label(tfptr,tlabel);
	mvwprintw(win, DISP_OFFSET+15, 2,
	               "fptr: %s   ch:%x    res:%d    res2:%d   ",tlabel,ch,res,res2);
	mvwprintw(win, DISP_OFFSET+16, 2,"ch:%x  fptr:%x res:%d  res2:%d   ",ch,tfptr,res,res2);
	wrefresh(win);

#else
	tfptr = receiveByte();
	high_byte = receiveByte();
	low_byte = receiveByte();
	menu_index = pack(low_byte, high_byte);

#ifdef TTY_DISPLAY
	thigh_byte = high_byte;
	tlow_byte = low_byte;
#endif

	for(i = 0;i < 6;i++)
	{
		high_byte = receiveByte();
		low_byte = receiveByte();
		curr_menus[i] = pack(low_byte,high_byte);
	}
	high_byte = receiveByte();
	low_byte = receiveByte();
	aux_bytes_to_read = pack(low_byte, high_byte);

//	aux_bytes_to_read = receiveByte();
//	aux_data_offset = receiveByte();
	for(i = 0;i < aux_bytes_to_read;i++)
		aux_string[i] = receiveByte();

	// write a code of 0xAA back to sim PIC as a verification that everything is running
	// if not hooked up to LCD screen

	low_byte = 0xAA;
	transmitByte(low_byte);
	for(i = 0;i < 6;i++)
		transmitByte(curr_menus[i]);

#ifdef TTY_DISPLAY

#endif

#endif
	ret_char = (*fptr[tfptr])(ch);		// execute the function pointer from the PIC
//	mvwprintw(win, DISP_OFFSET+20, 2,"%x  %x  ",ret_char,ch);
	display_menus();

	if(prev_menu_index != menu_index)
	{
		blank_choices();
		switch (menu_index)
		{
			case MAIN:
			case MENU1A:
			case MENU1B:
				display_labels();
				break;
			case MENU1C:
			case MENU1D:
				init_checkboxes(menu_index);
			break;
			case MENU2A:
			case MENU2B:
				init_execchoices(menu_index);
			break;
			case MENU2C:
			case MENU2D:
			case MENU2E:
			case MENU3A:
			case MENU3B:
				init_numentry(menu_index);
			break;
		}
	}
	prev_menu_index = menu_index;
	// show the contents of aux_string
	j = k = 0;

#ifdef TEST_WRITE_DATA
#if 0
	for(i = 0;i < AUX_STRING_LEN/2;i++)
	{
		mvwprintw(win, LAST_ROW+j-30, 2+k,"%c",aux_string[i]);
		if(++k > 19)
		{
			k = 0;
			++j;
		}
	}
#endif
	if(ret_char != READ_EEPROM && ret_char != BURN_EEPROM)
	{
		for(i = 0;i < AUX_STRING_LEN/3;i++)
		{
			mvwprintw(win, LAST_ROW+j-30, 2+(k*3),"%x  ",aux_string[i]);
			if(++k > 18)
			{
				k = 0;
				++j;
			}
		}
	}

#endif
	return ret_char;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static void get_fptr_label(int fptr, char *str)
{
	switch(fptr)
	{
		case 0:
			strcpy(str,"change\0");
		break;
		case 1:
			strcpy(str,"exec\0");
		break;
		case 2:
			strcpy(str,"chckbox\0");
		break;
		case 3:
			strcpy(str,"non_func\0");
		break;
		case 4:
			strcpy(str,"numentry\0");
		break;
		case 5:
			strcpy(str,"init\0");
		break;
		default:
		break;
	}
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
		break;
		case KP_POUND:
		escape(ch);
		break;
		case KP_AST:
		break;
		default:
		break;
	}
#ifdef TEST_WRITE_DATA
	mvwprintw(win, DISP_OFFSET+17,2, "*do_numentry  %x   ",ret_char);
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
	clean_disp_num();
//	scale_disp(SCALE_DISP_ALL);
	data_entry_mode = 0;
	return ch;
}
//******************************************************************************************//
//******************************************* enter ****************************************//
//******************************************************************************************//
static UCHAR enter(UCHAR ch)
{
	int limit;
//	if(data_entry_mode)
	if(1)
	{
		limit = atoi(cur_global_number);
		if(limit < 0)
			strcpy(cur_global_number,"32766\0");
		memcpy((void*)new_global_number,(void*)cur_global_number,NUM_ENTRY_SIZE);
		cur_col = NUM_ENTRY_BEGIN_COL;
		clean_disp_num();
		mod_data_ready = 1;
	}
#ifdef TEST_WRITE_DATA
	mvwprintw(win, DISP_OFFSET+24, 2,"new: %s       %d    ",new_global_number,limit);
	wrefresh(win);
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
	int temp_int = 0;
	cur_row = NUM_ENTRY_ROW;
	cur_col = NUM_ENTRY_BEGIN_COL;
	memset((void*)new_global_number,0,NUM_ENTRY_SIZE);
	memset((void*)cur_global_number,0,NUM_ENTRY_SIZE);
	temp_int = (int)aux_string[1];
	temp_int >>= 8;
	temp_int |= (int)aux_string[0];
	sprintf(cur_global_number,"%4d",temp_int);
	clean_disp_num();
	dispCharAt(NUM_ENTRY_ROW,cur_col,'/');
	dispCharAt(NUM_ENTRY_ROW,cur_col+NUM_ENTRY_SIZE,'/');
	new_data_ready = 1;
	display_edit_value();
#ifdef TEST_WRITE_DATA
	mvwprintw(win, DISP_OFFSET+23, 2,"init_num_entry %s %d %d       ",cur_global_number,temp_int,cur_col);
	wrefresh(win);
#endif
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
	dispCharAt(NUM_ENTRY_ROW+1,cur_col,95);
}
//******************************************************************************************//
//********************************** cursor_forward_stuff **********************************//
//******************************************************************************************//
static void cursor_forward_stuff(char x)
{
	data_entry_mode = 1;
//	if(data_entry_mode)
	if(1)
	{
		stuff_num(x);
		cursor_forward(x);
#ifdef TEST_WRITE_DATA
		mvwprintw(win, DISP_OFFSET+23, 2,"                     ");
		mvwprintw(win, DISP_OFFSET+20, 2,"stuff: %d   %d  ",x,cur_col);
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
	dispCharAt(NUM_ENTRY_ROW,cur_col,num);
	cur_global_number[cur_col-NUM_ENTRY_BEGIN_COL] = num;
#ifdef TEST_WRITE_DATA
	mvwprintw(win, DISP_OFFSET+21, 2,"cur: %s cur_col: %d  num: %x ",cur_global_number,cur_col,num);
	wrefresh(win);
#endif
}
//******************************************************************************************//
//************************************* cursor_forward *************************************//
//******************************************************************************************//
static UCHAR cursor_forward(UCHAR ch)
{
	// these will be ifdef'd out because they just show where the cursor is
	dispCharAt(NUM_ENTRY_ROW+1,cur_col,95);
	if(++cur_col > NUM_ENTRY_END_COL)
		cur_col = NUM_ENTRY_BEGIN_COL;
    dispSetCursorX(TEXT_ON | CURSOR_BLINK_ON,NUM_ENTRY_ROW,cur_col,LINE_8_CURSOR);
	dispCharAt(NUM_ENTRY_ROW+1,cur_col,95);
//	mvwprintw(win, 45, 3,"cursor forward   ");
    return ch;
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
		dispCharAt(NUM_ENTRY_ROW,cur_col,0x20);
		cur_global_number[cur_col-NUM_ENTRY_BEGIN_COL] = 0x20;
#ifdef TEST_WRITE_DATA
		mvwprintw(win, DISP_OFFSET+24,2,"bs:%s %d  ",cur_global_number,cur_col);
#endif
	}
//	memset((void*)cur_global_number,0,NUM_ENTRY_SIZE);
	return ch;
}
//******************************************************************************************//
//************************************* cursor_backward ************************************//
//******************************************************************************************//
static void cursor_backward(void)
{
	dispCharAt(NUM_ENTRY_ROW+1,cur_col,0x20);
	if(--cur_col < NUM_ENTRY_BEGIN_COL)
		cur_col = NUM_ENTRY_BEGIN_COL;
	dispSetCursorX(TEXT_ON | CURSOR_BLINK_ON,NUM_ENTRY_ROW,cur_col,LINE_8_CURSOR);
	dispCharAt(NUM_ENTRY_ROW+1,cur_col,95);
}
//******************************************************************************************//
//**************************************** _eclear *******************************************//
//******************************************************************************************//
static void _eclear(void)
{
	clean_disp_num();
	cur_row = NUM_ENTRY_ROW;
	cur_col = NUM_ENTRY_BEGIN_COL;
	memset((void*)new_global_number,0,NUM_ENTRY_SIZE);
	memset((void*)cur_global_number,0,NUM_ENTRY_SIZE);
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
static UCHAR do_exec(UCHAR ch)
{

	UCHAR ret_char = ch;

//	mvwprintw(win, DISP_OFFSET+17,2, "                    ");

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
	mvwprintw(win, DISP_OFFSET+17,2, "*do_exec  %x   ",ret_char);
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
//	mvwprintw(win, DISP_OFFSET+17,2, "                    ");
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
//			for(i = 0;i < NUM_CHECKBOXES;i++)
//				check_boxes[i].checked = prev_check_boxes[i];	// restore old
		break;
		case KP_0:
		break;
		case KP_AST:
		break;
		default:
		break;
	}
#ifdef TEST_WRITE_DATA
	mvwprintw(win, DISP_OFFSET+17,2, "*do_chkbox  %x   ",ret_char);
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
	mvwprintw(win, DISP_OFFSET+17,2, "                    ");
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
	mvwprintw(win, DISP_OFFSET+17,2, "*non_func  %x   ",ret_char);
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

	blank_display();

	for(i = 0;i < NUM_CHECKBOXES;i++)
	{
//		prev_check_boxes[i] = check_boxes[i].checked;		// save previous in case of escape key to restore
		memcpy(&check_boxes[i],aux_string+(sizeof(CHECKBOXES)*i),sizeof(CHECKBOXES));
//		check_boxes[i].checked = aux_string[]; // load what's sent from PIC
	}

#ifdef TEST_WRITE_DATA
	mvwprintw(win, DISP_OFFSET+23, 2,"menu_index (init_checkboxes) %d           ",menu_index);
	wrefresh(win);
#endif

	for(i = 0;i < NUM_CHECKBOXES;i++)
	{
//		strcpy(check_boxes[i].string,"test \0");
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

	for(i = 0;i < NUM_CHECKBOXES;i++)
	{
//		prev_check_boxes[i] = check_boxes[i].checked;		// save previous in case of escape key to restore
		memcpy(&check_boxes[i],aux_string+(sizeof(CHECKBOXES)*i),sizeof(CHECKBOXES));
//		check_boxes[i].checked = aux_string[]; // load what's sent from PIC
	}

// setting the list to choice 'n' is all well and good but we need a way for the PIC to send
// the choices over in case there are more than 1 lists of choices

#ifdef TEST_WRITE_DATA
	mvwprintw(win, DISP_OFFSET+23, 2,"menu_index (init_execchoices) %d           ",menu_index);
	wrefresh(win);
#endif

	blank_display();
	for(i = 0;i < NUM_CHECKBOXES;i++)
	{
		check_boxes[i].index = i;
		GDispStringAt(row,col,check_boxes[i].string);
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
	dispCharAt(1+check_boxes[0].index,0,120);

}
//******************************************************************************************//
//********************************** scrollup_checkboxes ***********************************//
//******************************************************************************************//
static UCHAR scrollup_checkboxes(UCHAR ch)
{
	if(--curr_checkbox < 0)
		curr_checkbox = last_checkbox;
	// move cursor to 3 + check_boxes[curr_checkbox]
	dispCharAt(1+check_boxes[curr_checkbox].index,0,0x21);
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
	dispCharAt(1+check_boxes[curr_checkbox].index,0,0x21);
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

	dispCharAt(1+check_boxes[temp].index,0,0x20);
	dispCharAt(1+check_boxes[curr_execchoice].index,0,120);
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

	dispCharAt(1+check_boxes[temp].index,0,0x20);
	dispCharAt(1+check_boxes[curr_execchoice].index,0,120);
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

	get_label(curr_menus[index],temp);

#ifdef TEST_WRITE_DATA
//	mvwprintw(win, LAST_ROW-2, 2+(index*7),"%s      ",temp);
//	mvwprintw(win, LAST_ROW-1, 2+(index*3),"%d ",curr_menus[index]);
//	wrefresh(win);
#endif
	if(temp[0] != 0)
	{
		GDispStringAt(*row,*col,temp2);
		GDispStringAt(*row,*col+3,temp);
	}

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
	int i,j,k;
	UCHAR row,col;
	row = MENU_START_ROW;
	col = 0;
//	mvwprintw(win, 42, 3,"index: %d",index);
	k = j = 0;

	blank_menu();
	adv_menu_label(0,&row,&col);
	adv_menu_label(1,&row,&col);
	adv_menu_label(2,&row,&col);
	adv_menu_label(3,&row,&col);
	adv_menu_label(4,&row,&col);
	adv_menu_label(5,&row,&col);
//	mvwprintw(win, 45, 25,"index: %d ",index);
#ifdef TEST_WRITE_DATA
//	mvwprintw(win, LAST_ROW-5, 1, "display_menu            ");
	wrefresh(win);
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

	blank_display();
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
				if(rt_params[i].type == RT_O2)
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
				else if(rt_params[i].type == RT_O2)
					rt_params[i].shown = NOSHOWN_SENT;
				else
					rt_params[i].shown = NOSHOWN_NOSENT;

				GDispStringAt(rt_params[i].row,rt_params[i].col,blank);
			}
			break;
		case SCALE_DISP_NONE:
			for(i = 0;i < no_rtparams;i++)
			{
				if(rt_params[i].type == RT_O2)
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
static void blank_display(void)
{
	int row,col;
	for(row = 1;row < 12;row++)
		for(col = 0;col < COLUMN;col++)
			dispCharAt(row,col,0x20);
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static void blank_menu(void)
{
	int row,col;
	for(row = MENU_START_ROW;row < MENU_START_ROW+2;row++)
		for(col = 1;col < COLUMN+1;col++)
			dispCharAt(row,col,0x20);
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
//	display_menus();
//	new_data_ch = new_data_ex = 0;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
#if 0
int get_str_len(void)
{
	return cur_col-NUM_ENTRY_BEGIN_COL;
}
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

static char *show_keypress(UCHAR key, int offset, char *string)
{
	char string[10];
	switch(key)
	{
		case KP_POUND:
		strcpy(string,"#\0");
		break;
		case KP_AST:
		strcpy(string,"#\0");
		break;
		case KP_0:
		strcpy(string,"0\0");
		break;
		case KP_1:
		strcpy(string,"1\0");
		break;
		case KP_2:
		strcpy(string,"2\0");
		break;
		case KP_3:
		strcpy(string,"3\0");
		break;
		case KP_4:
		strcpy(string,"4\0");
		break;
		case KP_5:
		strcpy(string,"5\0");
		break;
		case KP_6:
		strcpy(string,"6\0");
		break;
		case KP_7:
		strcpy(string,"7\0");
		break;
		case KP_8:
		strcpy(string,"8\0");
		break;
		case KP_9:
		strcpy(string,"9\0");
		break;
		case KP_A:
		strcpy(string,"A\0");
		break;
		case KP_B:
		strcpy(string,"B\0");
		break;
		case KP_C:
		strcpy(string,"C\0");
		break;
		case KP_D:
		strcpy(string,"D\0");
		break;
		case SPACE:
		strcpy(string,"<space>\0");
		break;
		case SET_DATA1:
		strcpy(string,"SET_DATA1\0");
		break;
		case SET_DATA2:
		strcpy(string,"SET_DATA2\0");
		break;
		case PUSH_DATA:
		strcpy(string,"PUSH_DATA\0");
		break;
		default:
		strcpy(string,"<unknown>\0");
		break;
	}
	mvwprintw(win, offset, 1,"%s      ",string);
	wrefresh(win);
	return string;
}
#endif

