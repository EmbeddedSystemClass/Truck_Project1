// AVR_menus.c
//
#ifdef MAIN_C
#warning "MAIN_C defined"
#else
#warning "MAIN_C not defined in menus.c"
#endif
#include "sfr_helper.h"
#include <stdlib.h>
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
#include "t6963.h"
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
static void init_execchoices(int menu_index);
static void blank_choices(void);
static void display_edit_value(void);
static UCHAR generic_menu_function(UCHAR ch);
static UCHAR backspace(UCHAR ch);
static UCHAR enter(UCHAR ch);
static UCHAR escape(UCHAR ch);
static int curr_menu_index;
static int curr_chkbox_index;
static UCHAR prev_ret_char;
static UCHAR prev_fptr;

#if 0
static UCHAR scr_alnum0(UCHAR ch);
static UCHAR scr_alnum1(UCHAR ch);
static UCHAR scr_alnum2(UCHAR ch);
static UCHAR scr_alnum3(UCHAR ch);
static UCHAR alnum_enter(UCHAR ch);
static void scroll_alnum_list(int dir);
#endif
static UCHAR scrollup_checkboxes(int index);
static UCHAR scrolldown_checkboxes(int index);
static UCHAR toggle_checkboxes(int index);
static UCHAR scrolldown_execchoice(UCHAR ch);
static UCHAR scrollup_execchoice(UCHAR ch);
static UCHAR checkboxes_reset(int index);
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
static int curr_checkbox;
static int last_checkbox;

//static UCHAR new_data_ch, new_data_ex;

static UCHAR (*fptr[NUM_FPTS])(UCHAR) = { menu_change, do_chkbox, do_exec, non_func, do_numentry, do_init };

//******************************************************************************************//
//********************************* generic_menu_function **********************************//
//******************************************************************************************//
static UCHAR generic_menu_function(UCHAR ch)
{
	UCHAR ret_char = ch;
	int i,j,k;
	int aux_bytes_to_read;
	int aux_data_offset;
	UCHAR tfptr;
	int res,res2;
	UCHAR low_byte,high_byte;
	char tlabel[MAX_LABEL_LEN];
	res = 0;
	res2 = 0;
	UCHAR prompt_size;

#ifdef TEST_WRITE_DATA
	res += read(global_fd,&tfptr,1);
	res += 	read(global_fd,&high_byte,1);
	res += 	read(global_fd,&low_byte,1);
	curr_menu_index = pack(low_byte,high_byte);

	for(i = 0;i < 6;i++)
	{
		res += read(global_fd,&high_byte,1);
		res += read(global_fd,&low_byte,1);

		curr_menus[i] = pack(low_byte,high_byte);
		if(curr_menus[i] > blank)
			curr_menus[i] = 0;
	}

#else
	tfptr = receiveByte();
	high_byte = receiveByte();
	low_byte = receiveByte();
	curr_menu_index = pack(low_byte, high_byte);

	for(i = 0;i < 6;i++)
	{
		high_byte = receiveByte();
		low_byte = receiveByte();
		curr_menus[i] = pack(low_byte,high_byte);
	}

#endif
#ifdef TEST_WRITE_DATA
	mvwprintw(win, LAST_ROW-4, 1,"fptrs: %d %d                               ",tfptr,prev_fptr);
#endif
	if(tfptr <= NUM_FPTS && tfptr >= 0)
	{
//		if(prev_menu_index != curr_menu_index && ret_char != KP_AST)

		if(ret_char != KP_AST && tfptr != prev_fptr && tfptr > _menu_change)
		{
#ifdef TEST_WRITE_DATA
			mvwprintw(win, LAST_ROW-3, 1,"menu_index: curr %d prev %d ret %2x %2x         ",
					curr_menu_index,
					prev_menu_index,
					ret_char,
					prev_ret_char);
#endif
			switch (curr_menu_index)
			{
				case MAIN:
				case MENU1A:
				case MENU1B:
				break;
				case MENU1C:
				case MENU1D:
				curr_chkbox_index = curr_menu_index-MENU1C;
				init_checkboxes(curr_menu_index);
				break;
				case MENU1E:
				case MENU2A:
				case MENU2B:
				curr_chkbox_index = curr_menu_index-MENU1C;
//				init_checkboxes(curr_menu_index);
				init_execchoices(curr_menu_index);
				break;
				case MENU2C:
				case MENU2D:
				case MENU2E:
				case MENU3A:
				case MENU3B:
				curr_chkbox_index = curr_menu_index-MENU1C;
				init_numentry(curr_menu_index);
				do_numentry(ret_char);
				break;
				default:
				break;
			}
		}

		prev_fptr = tfptr;

		ret_char = (*fptr[tfptr])(ret_char);		// execute the function pointer from the PIC
#ifdef TEST_WRITE_DATA
		mvwprintw(win, LAST_ROW-1, 1,"ch:%x  fptr:%x index:%d   ",ch,tfptr,curr_menu_index);
#endif
		display_menus();

		prev_menu_index = curr_menu_index;
		prev_ret_char = ret_char;
	}
#ifdef TEST_WRITE_DATA
	wrefresh(win);
#endif
	return ret_char;
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

	get_mlabel(curr_menus[index],temp);

	if(temp[0] != 0)
	{
		GDispStringAt(*row,*col,temp2);
		GDispStringAt(*row,*col+3,temp);
	}

	// if MAX_LABEL_LEN > 10 then menus will be too wide
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
static void display_menus(void)
{
	int i,j,k;
	UCHAR row,col;
	row = MENU_START_ROW;
	col = MENU_START_COL;
	col = 0;
	k = j = 0;

	blank_menu();
	adv_menu_label(0,&row,&col);
	adv_menu_label(1,&row,&col);
	adv_menu_label(2,&row,&col);
	adv_menu_label(3,&row,&col);
	adv_menu_label(4,&row,&col);
	adv_menu_label(5,&row,&col);
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static UCHAR menu_change(UCHAR ch)
{
	UCHAR ret_char = ch;

	if(prev_menu_index != curr_menu_index)
	{
		display_labels();
#ifdef TEST_WRITE_DATA
		mvwprintw(win, LAST_ROW-4,1, "*menu_change (init)             ");
		wrefresh(win);
#endif
		return ch;
	}

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
	mvwprintw(win, LAST_ROW-3,1, "*menu_change  %x                      ",ret_char);
//	mvwprintw(win, DISP_OFFSET+24, 2,"                           ");
	wrefresh(win);
#endif
	return ret_char;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
UCHAR read_get_key(UCHAR key)
{
	int i,j,k,done;
	UCHAR ret_char = key;
	int res = 0;
	int res2 = 0;
	int type = 0;
	UCHAR low_byte, high_byte, recByte;
	int choice_aux_offset;
	int exec_aux_offset;
	char temp[MAX_LABEL_LEN];
	char *vptr;

	switch(ret_char)
	{
		case 0xFC:
#ifdef TEST_WRITE_DATA
			mvwprintw(win, LAST_ROW-2,1,
				"stupid 0xFC                                                ");
			wrefresh(win);
#endif
			break;
		case INIT:
			init_list();
			break;
		case SPACE:
#ifdef TEST_WRITE_DATA
			for(i = 1;i < LAST_ROW;i++)
				mvwprintw(win, i,1,"                                                            ");
			wrefresh(win);
#else
			GDispClrTxt();
#endif
			break;

		case TEST1:				// 'g' set start_addr to 0
			size = AUX_STRING_LEN;
			start_addr = 0;
			break;

		case TEST2:				// 'h' start_addr to 300
			size = AUX_STRING_LEN;
			start_addr = AUX_STRING_LEN;
			break;

		case TEST3:				// 'j' 600
			size = AUX_STRING_LEN;
			start_addr = AUX_STRING_LEN*2;
			break;

		case TEST4:				// 'k' 900 (size 123)
			size = 123;
			start_addr = AUX_STRING_LEN*3;
			break;

		case TEST5:				// 'l' aux_string w/ random data
			res = 0;
			for(i = 0;i < AUX_STRING_LEN;i++)
			{
#ifdef TEST_WRITE_DATA
				res += read(global_fd,&aux_string[i],1);
#else
				aux_string[i] = receiveByte();
#endif
			}
#ifdef TEST_WRITE_DATA
			k = j = 0;
			mvwprintw(win, LAST_ROW-2,1,"get random data for aux_string   %d      ",res);
			for(i = 0;i < AUX_STRING_LEN;i++)
			{
				mvwprintw(win, LAST_ROW+j-58, 2+(k*3),"%2x ",aux_string[i]);
				if(++k > 15)
				{
					k = 0;
					++j;
				}
			}
			wrefresh(win);
#endif
			break;

		case TEST6:		// 'm' draw border
#ifdef TEST_WRITE_DATA
			mvwhline(win, 1, 5, ACS_HLINE, 41);
			mvwhline(win, 18, 5, ACS_HLINE, 41);
			mvwvline(win, 2, 4, ACS_VLINE, 16);
			mvwvline(win, 2, 45, ACS_VLINE, 16);
			mvwaddch(win, 1, 4, ACS_ULCORNER);
			mvwaddch(win, 18, 4, ACS_LLCORNER);
			mvwaddch(win, 1, 45, ACS_URCORNER);
			mvwaddch(win, 18, 45, ACS_LRCORNER);
			mvwprintw(win, LAST_ROW-2,1,"draw box                                        ");
			wrefresh(win);
#else
			GDispClrTxt();
			GDispStringAt(0,0,"TEST15");
#endif
			break;

		case TEST7:		// 'n' print cblabels
#ifdef TEST_WRITE_DATA
			mvwprintw(win, LAST_ROW-1, 2,"print out current cblabels                          ");

			k = j = 0;
			for(i = 0;i < LAST_ROW/2;i++)
			{
				mvwprintw(win, DISP_OFFSET+i,2,"                                     ");
			}

			mvwprintw(win, LAST_ROW-1, 22,"no_cblabels: %d  ",no_cblabels);
			for(i = 0;i < no_cblabels/2;i++)
			{
				get_cblabel(i,temp);
				mvwprintw(win, LAST_ROW+i-58, 2,"%d: %s             ",i,temp);
			}
			j = 0;
			for(i = no_cblabels/2;i < no_cblabels;i++)
			{
				get_cblabel(i,temp);
				mvwprintw(win, LAST_ROW+j-58, 22,"%d: %s             ",i,temp);
				j++;
			}

			wrefresh(win);
#else
			j = k = 0;
			for(i = 0;i < no_cblabels;i++)
			{
				get_cblabel(i,temp);
				GDispStringAt(j,k,temp);
				if(++j > 16)
				{
					j = 0;
					k += 10;
				}
			}
#endif
			break;

		case TEST9:			// 'p' copy eeprom2.bin to eeprom_sim
#ifdef TEST_WRITE_DATA
			k = j = 0;
			for(i = 0;i < EEPROM_SIZE;i++)
			{
				if(eeprom_sim[i] < 0x7e && eeprom_sim[i] > 0x21)
					mvwprintw(win, LAST_ROW+j-50, 2+k,"%c",eeprom_sim[i]);
				else if(eeprom_sim[i] == 0)	mvwprintw(win, LAST_ROW+j-50, 24+k," ");
				else mvwprintw(win, LAST_ROW+j-50, 2+k,"_");
				if(++k > 40)
				{
					k = 0;
					++j;
				}
			}
			k++;
			mvwprintw(win, LAST_ROW+j-50, 2+k,"|");
#endif
			break;

		case TEST10:		// 'x' read aux_string from here
			res = 0;
			for(i = 0;i < AUX_STRING_LEN;i++)
			{
#ifdef TEST_WRITE_DATA
				res += write(global_fd,&aux_string[i],1);
				usleep(tdelay);
#else
				transmitByte(aux_string[i]);
				_delay_ms(1);
#endif
			}
#ifdef TEST_WRITE_DATA
			mvwprintw(win, LAST_ROW-1,1,"writing aux_string: %d                               ",res);
#endif
			break;

		case TEST11:		// 's' display pattern on LCD
			i = j = 0;
			low_byte = 0x21;
#ifdef TEST_WRITE_DATA
			GDispClrTxt();
#endif
			for(k = 0;k < 5;k++)
				for(i = 0;i < ROWS;i++)
				{
					for(j = 0;j < COLUMN;j++)
					{
#ifdef TEST_WRITE_DATA
						usleep(tdelay/5);
#else
						_delay_ms(2);
#endif
						GDispCharAt(i,j,low_byte);
						if(++low_byte > 0x7e)
						{
							low_byte = 0x21;
						}
#ifdef TEST_WRITE_DATA
						wrefresh(win);
#endif
					}
				}
			break;


		case TEST12:	// print to screen debug info  'A'
//			memcpy((void*)&no_rt_labels, (void*)(eeprom_sim+NO_RT_LABELS_EEPROM_LOCATION),sizeof(UINT));
//			memcpy((void*)&no_rtparams, (void*)(eeprom_sim+NO_RTPARAMS_EEPROM_LOCATION),sizeof(UINT));
//			memcpy((void*)&no_menu_labels, (void*)(eeprom_sim+NO_MENU_LABELS_EEPROM_LOCATION),sizeof(UINT));

#ifndef TEST_WRITE_DATA
		GDispClrTxt();
		GDispStringAt(0,0,"no_rt_labels");
		GDispWordAt(0,15,(UINT)no_rt_labels);

		GDispStringAt(1,0,"no_rtparams");
		GDispWordAt(1,15,(UINT)no_rtparams);

		GDispStringAt(2,0,"no_menu_labels");
		GDispWordAt(2,15,(UINT)no_menu_labels);
/*
		sprintf(temp,"%2x",no_rt_labels);		// no sprintf in AVR library?
		GDispStringAt(0,0,temp);
		sprintf(temp,"%2x",no_rtparams);
		GDispStringAt(1,0,temp);
		sprintf(temp,"%2x",no_menu_labels);
		GDispStringAt(2,0,temp);
*/
#endif
			break;

		case TEST13:  // 'B'
#ifndef TEST_WRITE_DATA
			GDispClrTxt();
			GDispStringAt(0,0,"TEST13");
#else
			mvwprintw(win, LAST_ROW-1,1,"TEST13");
#endif
			break;

		case TEST14:  // 'C'
#ifndef TEST_WRITE_DATA
//			size = 0x210
			start_addr = 0x120;
			for(i = 0;i < AUX_STRING_LEN;i++)
				aux_string[i] = (UCHAR)(i*2);
			size = AUX_STRING_LEN;
			eeprom_update_block((const void*)aux_string,(void *)(eepromString+start_addr),(size_t)size);
			for(i = 0;i < AUX_STRING_LEN;i++)
				aux_string[i] = (UCHAR)(~i);
			size = 0x210 - AUX_STRING_LEN;
			start_addr = AUX_STRING_LEN;
			eeprom_update_block((const void*)aux_string,(void *)(eepromString+start_addr),(size_t)size);
			GDispClrTxt();
			GDispStringAt(0,0,"TEST14");
#else
			mvwprintw(win, LAST_ROW-1,1,"TEST14");
#endif
			break;

		case TEST15:  // 'D'
#ifndef TEST_WRITE_DATA
			GDispClrTxt();
			GDispStringAt(0,0,"TEST15");
#else
			mvwprintw(win, LAST_ROW-1,1,"TEST15");
#endif
			break;

		case TEST16:  // 'E'
#ifndef TEST_WRITE_DATA
			GDispClrTxt();
			GDispStringAt(0,0,"TEST16");
#else
			mvwprintw(win, LAST_ROW-1,1,"TEST16");
#endif
			break;

		case TEST17:  // 'F'
#ifndef TEST_WRITE_DATA
			GDispClrTxt();
			GDispStringAt(0,0,"TEST17");
#else
			mvwprintw(win, LAST_ROW-1,1,"TEST17");
#endif
			break;

		case LOAD_RAM:  // 'r'
#ifdef TEST_WRITE_DATA
			prev_menu_index = curr_menu_index = curr_chkbox_index = prev_fptr = 0;
			res = 0;
			mvwprintw(win, LAST_ROW-2,1,"                                       ");

			mvwprintw(win, LAST_ROW-2,1,"loading cblabels: ");

			for(i = 0;i < CBLABEL_SIZE;i++)
			{
				res += read(global_fd,&cblabels[i],1);
				mvwprintw(win, LAST_ROW-2,20,"%2d  %2d  ",cblabels[i],res);
				wrefresh(win);
			}

			mvwprintw(win, LAST_ROW-2,30,"%2d      ",res);
			wrefresh(win);

			last_checkbox = get_cblabel_offsets();

			res = 0;
			mvwprintw(win, LAST_ROW-3,1,"                                       ");

			mvwprintw(win, LAST_ROW-3,1,"loading checkboxes checked: ");

			k = j = 0;

			for(i = 0;i < NUM_CBLABELS;i++)
			{
				res += read(global_fd,&check_boxes[i].checked,1);
				res += read(global_fd,&check_boxes[i].index,1);
				memcpy(&prev_check_boxes[i],&check_boxes[i],sizeof(CHECKBOXES));
//				mvwprintw(win, LAST_ROW-3,25,"%2d  %2d  ",check_boxes[i].checked,res);
/*
				mvwprintw(win, LAST_ROW+j-50, 2+(k*2),"%2d ",check_boxes[i].checked);
				if(++k > 15)
				{
					k = 0;
					++j;
				}
				wrefresh(win);
*/
			}

			mvwprintw(win, LAST_ROW-4,2,"cblabels: %2d ",res);
			wrefresh(win);
			get_cblabel_offsets();
			res = 0;

			for(i = 0;i < NUM_RT_PARAMS;i++)
			{
				res += read(global_fd,&rt_params[i].row,1);
				res += read(global_fd,&rt_params[i].col,1);
				res += read(global_fd,&rt_params[i].shown,1);
				res += read(global_fd,&rt_params[i].dtype,1);
				res += read(global_fd,&rt_params[i].type,1);
			}
			res = 0;
			mvwprintw(win, LAST_ROW-5,2,"rt_params: %2d ",res);
			for(i = 0;i < 5;i++)
			{
				res += read(global_fd,&low_byte,1);
				res += read(global_fd,&high_byte,1);
				sample_numbers[i] = pack(low_byte,high_byte);
//				mvwprintw(win, LAST_ROW-11,2+(i*6),"%3d ",sample_numbers[i]);
//				mvwprintw(win, LAST_ROW-10+i,2,"res: %d  %x  %x  ",res,low_byte,high_byte);
			}
//			mvwprintw(win, LAST_ROW-12,2,"res: %d    ",res);
//			wrefresh(win);
			no_rtparams = NUM_RT_PARAMS;

			wrefresh(win);
#else
/*
#define NO_MENU_LABELS_EEPROM_LOCATION 0x03e0
#define NO_RT_LABELS_EEPROM_LOCATION 0x03e4
#define NO_RTPARAMS_EEPROM_LOCATION 0x03e8
#define NO_MENUS_EEPROM_LOCATION 0x3ea
*/

			size = NO_MENUS_EEPROM_LOCATION - NO_MENU_LABELS_EEPROM_LOCATION;
			eeprom_read_block((void *)aux_string,(const void *)(eepromString+NO_MENU_LABELS_EEPROM_LOCATION),(size_t)size);

			memcpy((void*)&no_menu_labels, (void*)(aux_string),sizeof(UINT));
			memcpy((void*)&no_rt_labels, (void*)(aux_string+
					(NO_RT_LABELS_EEPROM_LOCATION - NO_MENU_LABELS_EEPROM_LOCATION)),sizeof(UINT));
			memcpy((void*)&no_rtparams, (void*)(aux_string+
					(NO_RTPARAMS_EEPROM_LOCATION - NO_MENU_LABELS_EEPROM_LOCATION)),sizeof(UINT));

			no_menu_structs = 13;
			rt_params_offset = 243;
//			no_rtparams = NUM_RT_PARAMS;
//			no_menu_labels = 29;
//			no_rt_labels = 12;

			res = 0;
			for(i = 0;i < CBLABEL_SIZE;i++)
			{
				 cblabels[i] = receiveByte();
			}

			for(i = 0;i < NUM_CBLABELS;i++)
			{
				check_boxes[i].checked = receiveByte();
				check_boxes[i].index = receiveByte();
				memcpy(&prev_check_boxes[i],&check_boxes[i],sizeof(CHECKBOXES));
			}
			get_cblabel_offsets();
#if 0
			for(i = 0;i < NUM_RT_PARAMS;i++)
			{
				rt_params[i].row = receiveByte();
				rt_params[i].col = receiveByte();
				rt_params[i].shown = receiveByte();
				rt_params[i].dtype = receiveByte();
				rt_params[i].type = receiveByte();
			}
#endif
			for(i = 0;i < 5;i++)
			{
				low_byte = receiveByte();
				high_byte = receiveByte();
				sample_numbers[i] = pack(low_byte,high_byte);
			}
#endif

		break;

		case BURN_PART:
#ifdef TEST_WRITE_DATA
			mvwprintw(win, LAST_ROW-2,1,
				"burn eeprom start"                               );
			wrefresh(win);
			read(global_fd,&high_byte,1);
			read(global_fd,&low_byte,1);
			size = pack(low_byte,high_byte);

			read(global_fd,&high_byte,1);
			read(global_fd,&low_byte,1);
			start_addr = pack(low_byte,high_byte);

			mvwprintw(win, LAST_ROW-2,1,
				"burn eeprom - size: %d start: %d ",size,start_addr);
			wrefresh(win);
			for(i = start_addr;i < size+start_addr;i++)
//			for(i = 0;i < size;i++)
			{
				res2 += read(global_fd,&eeprom_sim[i],1);
//				usleep(100);
				mvwprintw(win, LAST_ROW-3,1,"%2x  %2d %2d",eeprom_sim[i],i,res2);
				wrefresh(win);
			}

//			low_byte = 0x55;
//			write(global_fd,&low_byte,1);
			mvwprintw(win, LAST_ROW-2,20,"done %d  ",res2);
#else
			high_byte = receiveByte();
			low_byte = receiveByte();
			size = pack(low_byte,high_byte);

			high_byte = receiveByte();
			low_byte = receiveByte();
			start_addr = pack(low_byte,high_byte);

			GDispClrTxt();
			GDispStringAt(0,0,"eeprom update");
			GDispStringAt(1,0,"size: ");
			GDispWordAt(1,15,(UINT)size);

			GDispStringAt(2,0,"start_addr");
			GDispWordAt(2,15,(UINT)start_addr);

//			for(i = start_addr;i < size+start_addr;i++)
			low_byte = 0;
			size = (size <= AUX_STRING_LEN ? size : AUX_STRING_LEN);
			for(i = 0;i < size;i++);
			{
				aux_string[i] = receiveByte();
			}
			for(i = 0;i < size;i++);
			{
				low_byte += aux_string[i];
			}
			GDispStringAt(3,0,"checksum");
			GDispByteAt(3,15,low_byte);
			_delay_ms(1);
//			for(i = 0;i < size;i++)
//				eeprom_update_byte((uint8_t *)&eepromString[start_addr + i], (uint8_t )aux_string[start_addr+i]);
			eeprom_update_block((const void*)aux_string,(void *)(eepromString+start_addr),(size_t)size);
//			eeprom_update_block(src, dest, size)
#endif
			get_mlabel_offsets();
			break;

		case READ_EEPROM1:
		case READ_EEPROM2:
#ifdef TEST_WRITE_DATA

			for(i = LAST_ROW-1;i > 0;i--)
				mvwprintw(win, i,1,"                                                             ");

			read(global_fd,&high_byte,1);
			read(global_fd,&low_byte,1);
			size = pack(low_byte,high_byte);

			read(global_fd,&high_byte,1);
			read(global_fd,&low_byte,1);
			start_addr = pack(low_byte,high_byte);

			if(size+start_addr > EEPROM_SIZE)
			{
				size = (size < EEPROM_SIZE?EEPROM_SIZE:size);
				start_addr = 0;
				mvwprintw(win, LAST_ROW-2,1,"reset size: %d start: %d    ",size,start_addr);
				wrefresh(win);
			}else
			{
				mvwprintw(win, LAST_ROW-2,1,"read_eeprom - size: %d start: %d    ",size,start_addr);
				wrefresh(win);
			}

			for(i = start_addr;i < size+start_addr;i++)
			{
				if(eeprom_sim[i] != 0)
					usleep(tdelay*2);
				else usleep(tdelay);
				write(global_fd,&eeprom_sim[i],1);

				mvwprintw(win, LAST_ROW-3,1,"%2x   %d                               ",eeprom_sim[i],i);
				wrefresh(win);

//				low_byte = ~i;
//				res2 += write(global_fd,&low_byte,1);
			}

			j = 0;
			k = 0;

			if(ret_char == READ_EEPROM1)
			{
				for(i = start_addr;i < size+start_addr;i++)
				{
					if(eeprom_sim[i] < 0x7e && eeprom_sim[i] > 0x21)
						mvwprintw(win, LAST_ROW+j-58, 2+k,"%c",eeprom_sim[i]);
					else if(eeprom_sim[i] == 0)	mvwprintw(win, LAST_ROW+j-58, 2+k," ");
					else mvwprintw(win, LAST_ROW+j-58, 2+k,"_");
					if(++k > 30)
					{
						k = 0;
						++j;
					}
				}
				k++;
				mvwprintw(win, LAST_ROW+j-58, 2+k,"|");
				mvwprintw(win, LAST_ROW-1,2,"display eeprom: size: start_addr         ",size,start_addr);

			}
			else
			{
				mvwprintw(win, LAST_ROW-58,1,"0: ");
				for(i = start_addr;i < size+start_addr;i++)
				{
					mvwprintw(win, LAST_ROW+j-58, 7+(k*3),"%2x ",eeprom_sim[i]);
					if(++k > 15)
					{
						k = 0;
						++j;
						mvwprintw(win, LAST_ROW+j-58,1,"%2d: ",i+1);
					}
				}
				mvwprintw(win, LAST_ROW-1,2,"display eeprom: size: start_addr         ",size,start_addr);
			}
			wrefresh(win);

//			memcpy((void*)&no_rt_labels, (void*)(eeprom_sim+NO_RT_LABELS_EEPROM_LOCATION),sizeof(UINT));
//			memcpy((void*)&no_rtparams, (void*)(eeprom_sim+NO_RTPARAMS_EEPROM_LOCATION),sizeof(UINT));
//			memcpy((void*)&no_menu_labels, (void*)(eeprom_sim+NO_MENU_LABELS_EEPROM_LOCATION),sizeof(UINT));

//			mvwprintw(win, LAST_ROW-4,2,
//				"no_rt_labels: %d no_menu_labels: %d no_rtparams: %d  ",no_rt_labels,no_menu_labels,no_rtparams);

//			for(i = 1;i < LAST_ROW+1;i++)
//				mvwprintw(win, i,2,"                                                      ");

/*
			for(i = NO_MENU_LABELS_EEPROM_LOCATION;i < NO_MENUS_EEPROM_LOCATION;i++,k++)
			{
				mvwprintw(win, LAST_ROW-3,2+(k*3),"%d ", eeprom_sim[i]);
			}
*/
			mvwprintw(win, LAST_ROW-2,1,
				"read_eeprom - size: %d start: %d    ",size,start_addr);
			wrefresh(win);
#else
			high_byte = receiveByte();
			low_byte = receiveByte();
			size = pack(low_byte,high_byte);

			high_byte = receiveByte();
			low_byte = receiveByte();
			start_addr = pack(low_byte,high_byte);

			GDispClrTxt();
			GDispStringAt(0,0,"eeprom read");
			GDispStringAt(1,0,"size: ");
			GDispWordAt(1,15,(UINT)size);

			GDispStringAt(2,0,"start_addr");
			GDispWordAt(2,15,(UINT)start_addr);

			memset(aux_string,0,AUX_STRING_LEN);
			size = (size <= AUX_STRING_LEN ? size : AUX_STRING_LEN);
//			for(i = 0;i < size;i++)
//				aux_string[i] = eeprom_read_byte((const uint8_t *)&eepromString[start_addr+i]);
			eeprom_read_block((void *)aux_string,(const void *)(eepromString+start_addr),(size_t)size);
			_delay_ms(1);
			low_byte = 0;
			for(i = 0;i < size;i++)
			{
				transmitByte(aux_string[i]);
			}
			for(i = 0;i < size;i++)
			{
				low_byte += aux_string[i];
			}
			GDispStringAt(3,0,"checksum");
			GDispByteAt(3,15,low_byte);
#endif
			get_mlabel_offsets();
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

#if 0
	if((prev_menu_index != curr_menu_index) && (ch != KP_D && ch != KP_AST && ch != KP_POUND))
	{
//		blank_choices();
		init_numentry(curr_menu_index);
#ifdef TEST_WRITE_DATA
		mvwprintw(win, LAST_ROW-6,2, "*init_numentry (do_numentry)");
		wrefresh(win);
#endif
		return ch;
	}
#endif

//	mvwprintw(win, LAST_ROW-6,2, "                                ");
//	wrefresh(win);
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
#ifdef TEST_WRITE_DATA
		mvwprintw(win, LAST_ROW-6,2, "*do_numentry (enter) %x          ",ret_char);
	wrefresh(win);
#endif
		enter(ch);
		break;
		case KP_POUND:
#ifdef TEST_WRITE_DATA
		mvwprintw(win, LAST_ROW-6,2, "*do_numentry (esc) %x          ",ret_char);
	wrefresh(win);
#endif
		escape(ch);
		break;
		case KP_AST:
#ifdef TEST_WRITE_DATA
		mvwprintw(win, LAST_ROW-6,2, "*do_numentry (ast) %x          ",ret_char);
	wrefresh(win);
#endif
		escape(ch);
		break;
		default:
		break;
	}
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
	return ch;
}
//******************************************************************************************//
//******************************************* enter ****************************************//
//******************************************************************************************//
static UCHAR enter(UCHAR ch)
{
	int limit;
	limit = atoi(cur_global_number);
	if(limit < 0)
		strcpy(cur_global_number,"32766\0");
	memcpy((void*)new_global_number,(void*)cur_global_number,NUM_ENTRY_SIZE);
	cur_col = NUM_ENTRY_BEGIN_COL;
	clean_disp_num();
	sample_numbers[prev_menu_index-MENU2C] = atoi((const char *)new_global_number);
#ifdef TEST_WRITE_DATA
	mvwprintw(win, LAST_ROW-5, 2,"new: %x %d %x   ",ch,prev_menu_index,prev_ret_char);
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
	int temp_int = 0;
	cur_row = NUM_ENTRY_ROW;
	cur_col = NUM_ENTRY_BEGIN_COL;
	memset((void*)new_global_number,0,NUM_ENTRY_SIZE);
	memset((void*)cur_global_number,0,NUM_ENTRY_SIZE);
	if(curr_menu_index-MENU2C < 6)
	{
		temp_int = sample_numbers[curr_menu_index-MENU2C];
//		sprintf(cur_global_number,"%4d",temp_int);
		clean_disp_num();
		dispCharAt(NUM_ENTRY_ROW,cur_col,'/');
		dispCharAt(NUM_ENTRY_ROW,cur_col+NUM_ENTRY_SIZE,'/');
		display_edit_value();
#ifdef TEST_WRITE_DATA
		mvwprintw(win, DISP_OFFSET+23, 2,"init_num_entry %s %d       ",cur_global_number,temp_int);
#endif

	}else{
#ifdef TEST_WRITE_DATA
		mvwprintw(win, DISP_OFFSET+23, 2,"init_num_entry (opps)           ");
#endif
	}
#ifdef TEST_WRITE_DATA
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
	stuff_num(x);
	cursor_forward(x);
#ifdef TEST_WRITE_DATA
	mvwprintw(win, DISP_OFFSET+23, 2,"                     ");
	mvwprintw(win, DISP_OFFSET+20, 2,"stuff: %d   %d  ",x,cur_col);
	wrefresh(win);
#endif
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
	cursor_backward();
	dispCharAt(NUM_ENTRY_ROW,cur_col,0x20);
	cur_global_number[cur_col-NUM_ENTRY_BEGIN_COL] = 0x20;
#ifdef TEST_WRITE_DATA
	mvwprintw(win, DISP_OFFSET+24,2,"bs:%s %d  ",cur_global_number,cur_col);
#endif
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
//		dispCharAt(NUM_ENTRY_ROW,i+NUM_ENTRY_BEGIN_COL+1,0x20);
	}
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static UCHAR do_exec(UCHAR ch)
{
	UCHAR ret_char = ch;
	int i,j,k,l;

	k = (curr_chkbox_index * NUM_CHECKBOXES);
	l = k;

#ifdef TEST_WRITE_DATA
//	mvwprintw(win, DISP_OFFSET+19,2,"                                       ");
//	mvwprintw(win, DISP_OFFSET+20,2,"                                       ");
#endif
	switch(ch)
	{
		case KP_A:
//		j = scrollup_checkboxes(k);
		j = scrollup_execchoice(k);
		break;
		case KP_B:
		j = scrolldown_execchoice(k);
		break;
		case KP_C:
		break;
		case KP_D:
		blank_choices();
		break;
		case KP_POUND:
		break;
		case KP_0:
		break;
		case KP_AST:
		blank_choices();
		break;
		default:
		break;
	}
#ifdef TEST_WRITE_DATA
//	mvwprintw(win, DISP_OFFSET+20,2," %d %d %d ",l,k,j);
	mvwprintw(win, LAST_ROW-6,2,"*do_exec  %x   %d               ",ret_char,k);
//	mvwprintw(win, DISP_OFFSET+17,2, "*do_exec  %x   %d               ",ret_char,k);
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

	k = (curr_chkbox_index * NUM_CHECKBOXES);
	l = k;

#ifdef TEST_WRITE_DATA
//	mvwprintw(win, DISP_OFFSET+19,2,"                                  ");
//	mvwprintw(win, DISP_OFFSET+20,2,"                                  ");
#endif

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
			blank_choices();
#ifdef TEST_WRITE_DATA
			mvwprintw(win, LAST_ROW-6,2, "*do_chkbox (enter) %x          ",ret_char);
			wrefresh(win);
#endif
		break;
		case KP_POUND:		// esc
			for(i = 0;i < NUM_CHECKBOXES;i++)
			{
				check_boxes[k+i].checked = prev_check_boxes[k+i].checked;
			}
			blank_choices();
#ifdef TEST_WRITE_DATA
			mvwprintw(win, LAST_ROW-6,2, "*do_chkbox (esc) %x          ",ret_char);
			wrefresh(win);
#endif
		break;
		case KP_0:
			checkboxes_reset(k);
		break;
		case KP_AST:
#ifdef TEST_WRITE_DATA
			mvwprintw(win, LAST_ROW-6,2, "*do_chkbox (ast) %x          ",ret_char);
			wrefresh(win);
#endif
		break;
		default:
		break;
	}
#ifdef TEST_WRITE_DATA
//	mvwprintw(win, DISP_OFFSET+20,2," %d %d %d ",l,k,j);

//	mvwprintw(win, DISP_OFFSET+17,2, "*do_chkbox  %x %d               ",ret_char,k);
	wrefresh(win);
#endif
	return ret_char;
}
//******************************************************************************************//
//************************************ init_checkboxes *************************************//
//******************************************************************************************//
static void init_checkboxes(int menu_index)
{
	int i,j,k;
	UCHAR row, col;
	char tlabel[MAX_LABEL_LEN];
//	scale_disp(SCALE_DISP_SOME);
	row = 0;
	col = 3;
	curr_checkbox = 0;

	blank_display();


	j = curr_chkbox_index * NUM_CHECKBOXES;

#ifdef TEST_WRITE_DATA
	mvwprintw(win, DISP_OFFSET+30, 2,"init_checkboxes  %d     ",j);
	wrefresh(win);
#endif

	for(i = 0;i < NUM_CHECKBOXES;i++)
	{
		get_cblabel(j+i,tlabel);
		GDispStringAt(row,col,tlabel);
#ifdef TEST_WRITE_DATA
//		mvwprintw(win, DISP_OFFSET+21+i, 2,"%d %d %s  ",k,j,tlabel);
#endif
		if(check_boxes[j+i].checked > 0)
		{
			dispCharAt(row,0,0x78);
			check_boxes[j+i].checked = 1;
		}
		else
		{
			dispCharAt(row,0,0x5F);	// underscore
			check_boxes[j+i].checked = 0;
		}
		row++;
	}
}
//******************************************************************************************//
//*********************************** init_execchoices *************************************//
//******************************************************************************************//
static void init_execchoices(int menu_index)
{
	int i,j,k;
	UCHAR row, col;
	char tlabel[MAX_LABEL_LEN];
//	scale_disp(SCALE_DISP_SOME);
	row = 0;
	col = 3;
	curr_checkbox = 0;

	blank_display();


	j = curr_chkbox_index * NUM_CHECKBOXES;

#ifdef TEST_WRITE_DATA
	mvwprintw(win, DISP_OFFSET+30, 2,"init_execchoices   %d    ",j);
	wrefresh(win);
#endif

	for(i = 0;i < NUM_CHECKBOXES;i++)
	{
		get_cblabel(j+i,tlabel);
		GDispStringAt(row,col,tlabel);
		row++;
	}
}
//******************************************************************************************//
//*********************************** checkboxes_reset *************************************//
//******************************************************************************************//
static UCHAR checkboxes_reset(int index)
{
//	int k = index+curr_checkbox;
	int i,k;
	curr_checkbox = 0;
	k = curr_chkbox_index * NUM_CHECKBOXES;

	for(i = 0;i < NUM_CHECKBOXES;i++)
	{
		check_boxes[k+i].checked = 0;
		curr_checkbox++;
		k++;
		dispCharAt(1+curr_checkbox,20,0x5F);
	}
#ifdef TEST_WRITE_DATA
	mvwprintw(win, DISP_OFFSET+20,2,"reset: curr %d ckbox %d     ",curr_checkbox,check_boxes[k+i].index);
	wrefresh(win);
#endif
	curr_checkbox = 0;
	return k;
}
//******************************************************************************************//
//********************************** scrollup_checkboxes ***********************************//
//******************************************************************************************//
static UCHAR scrollup_checkboxes(int index)
{
	int k;
	char tlabel[MAX_LABEL_LEN];
	char blank[] = "             ";

//	dispCharAt(curr_checkbox,20,0x5F);
	k = (curr_chkbox_index * NUM_CHECKBOXES)+curr_checkbox;
	get_cblabel(k,tlabel);
	GDispSetMode(ATTR_REVERSE);
	GDispStringAt(curr_checkbox,3,blank);
	GDispStringAt(curr_checkbox,3,tlabel);
	GDispSetMode(ATTR_REVERSE);

	if(--curr_checkbox < 0)
		curr_checkbox = 9;
	// move cursor to 3 + check_boxes[curr_checkbox]
	k = (curr_chkbox_index * NUM_CHECKBOXES)+curr_checkbox;

	get_cblabel(k,tlabel);
	GDispSetMode(ATTR_REVERSE);
	GDispStringAt(curr_checkbox,3,blank);
	GDispStringAt(curr_checkbox,4,tlabel);
	GDispSetMode(ATTR_REVERSE);

//	dispCharAt(1+check_boxes[k].index,20,0x21);
#ifdef TEST_WRITE_DATA
//	dispCharAt(curr_checkbox,20,0x21);
	mvwprintw(win, DISP_OFFSET+20,2,"chkbox up: ind %d curr %d ckbox %d  k: %d   ",
									curr_chkbox_index, curr_checkbox,check_boxes[k].index,k);
	wrefresh(win);
#endif
	return k;
}
//******************************************************************************************//
//********************************* scrolldown_checkboxes **********************************//
//******************************************************************************************//
static UCHAR scrolldown_checkboxes(int index)
{
	int i,k;
	char tlabel[MAX_LABEL_LEN];
	char blank[] = "             ";

//	dispCharAt(1+curr_checkbox,20,0x20);

	k = (curr_chkbox_index * NUM_CHECKBOXES)+curr_checkbox;
	get_cblabel(k,tlabel);
	GDispSetMode(ATTR_REVERSE);
	GDispStringAt(curr_checkbox,3,blank);
	GDispStringAt(curr_checkbox,3,tlabel);
	GDispSetMode(ATTR_REVERSE);

	if(++curr_checkbox > 9)
		curr_checkbox = 0;

	k = (curr_chkbox_index * NUM_CHECKBOXES)+curr_checkbox;
	get_cblabel(k,tlabel);
	GDispSetMode(ATTR_REVERSE);
	GDispStringAt(curr_checkbox,3,blank);
	GDispStringAt(curr_checkbox,4,tlabel);
	GDispSetMode(ATTR_REVERSE);

#ifdef TEST_WRITE_DATA
//	dispCharAt(curr_checkbox,20,0x21);
	mvwprintw(win, DISP_OFFSET+20,2,"chkbox down: ind %d curr %d ckbox %d  k: %d   ",
								curr_chkbox_index, curr_checkbox,check_boxes[k].index,k);
	wrefresh(win);
#endif
	return k;
}
//******************************************************************************************//
//********************************** toggle_checkboxes *************************************//
//******************************************************************************************//
static UCHAR toggle_checkboxes(int index)
{
	int k = (curr_chkbox_index * NUM_CHECKBOXES)+curr_checkbox;
	dispCharAt(1+curr_checkbox,20,0x20);

	if(check_boxes[k].checked == 1)
	{
		check_boxes[k].checked = 0;
		dispCharAt(curr_checkbox,0,0x5f);	// display '_'
	}
	else
	{
		check_boxes[k].checked = 1;
		dispCharAt(curr_checkbox,0,120);	// display 'x'
	}
#ifdef TEST_WRITE_DATA
	mvwprintw(win, DISP_OFFSET+20,2,"toggle: ind %d curr %d ckbox %d  k: %d   ",
						curr_chkbox_index, curr_checkbox,check_boxes[k].index,k);
	wrefresh(win);
#endif
	return k;
}
//******************************************************************************************//
//********************************** scrollup_execchoice ***********************************//
//******************************************************************************************//
static UCHAR scrollup_execchoice(UCHAR ch)
{
	int k;
	char tlabel[MAX_LABEL_LEN];
	char blank[] = "             ";

//	dispCharAt(curr_checkbox,20,0x5F);
	k = (curr_chkbox_index * NUM_CHECKBOXES)+curr_checkbox;
	get_cblabel(k,tlabel);
//	GDispSetMode(ATTR_REVERSE);
	GDispStringAt(curr_checkbox,3,blank);
	GDispStringAt(curr_checkbox,3,tlabel);
//	GDispSetMode(ATTR_REVERSE);

	if(--curr_checkbox < 0)
		curr_checkbox = 9;
	// move cursor to 3 + check_boxes[curr_checkbox]
	k = (curr_chkbox_index * NUM_CHECKBOXES)+curr_checkbox;

	get_cblabel(k,tlabel);
//	GDispSetMode(ATTR_REVERSE);
	GDispStringAt(curr_checkbox,3,blank);
	GDispStringAt(curr_checkbox,4,tlabel);
//	GDispSetMode(ATTR_REVERSE);

//	dispCharAt(1+check_boxes[k].index,20,0x21);
#ifdef TEST_WRITE_DATA
//	dispCharAt(curr_checkbox,20,0x21);
	mvwprintw(win, DISP_OFFSET+20,2,"exec up: ind %d curr %d ckbox %d  k: %d   ",
									curr_chkbox_index, curr_checkbox,check_boxes[k].index,k);
	wrefresh(win);
#endif
	return k;

/*
	UCHAR temp = curr_execchoice;
	if(--curr_execchoice < 0)
		curr_execchoice = last_execchoice;

//	mvwprintw(win, DISP_OFFSET+22,2,"%d %d   ",temp,curr_execchoice);
//	wrefresh(win);

	dispCharAt(1+check_boxes[temp].index,0,0x20);
	dispCharAt(1+check_boxes[curr_execchoice].index,0,120);
	return ch;
*/
}
//******************************************************************************************//
//********************************* scrolldown_execchoice **********************************//
//******************************************************************************************//
static UCHAR scrolldown_execchoice(UCHAR ch)
{
	int i,k;
	char tlabel[MAX_LABEL_LEN];
	char blank[] = "             ";

//	dispCharAt(1+curr_checkbox,20,0x20);

	k = (curr_chkbox_index * NUM_CHECKBOXES)+curr_checkbox;
	get_cblabel(k,tlabel);
//	GDispSetMode(ATTR_REVERSE);
	GDispStringAt(curr_checkbox,3,blank);
	GDispStringAt(curr_checkbox,3,tlabel);
//	GDispSetMode(ATTR_REVERSE);

	if(++curr_checkbox > 9)
		curr_checkbox = 0;

	k = (curr_chkbox_index * NUM_CHECKBOXES)+curr_checkbox;
	get_cblabel(k,tlabel);
//	GDispSetMode(ATTR_REVERSE);
	GDispStringAt(curr_checkbox,3,blank);
	GDispStringAt(curr_checkbox,4,tlabel);
//	GDispSetMode(ATTR_REVERSE);

#ifdef TEST_WRITE_DATA
//	dispCharAt(curr_checkbox,20,0x21);
	mvwprintw(win, DISP_OFFSET+20,2,"exec down: ind %d curr %d ckbox %d  k: %d   ",
								curr_chkbox_index, curr_checkbox,check_boxes[k].index,k);
	wrefresh(win);
#endif
	return k;
/*

	UCHAR temp = curr_execchoice;
	if(++curr_execchoice > last_execchoice)
		curr_execchoice = 0;

//	mvwprintw(win, DISP_OFFSET+22,2,"%d %d   ",temp,curr_execchoice);
//	wrefresh(win);

	dispCharAt(1+check_boxes[temp].index,0,0x20);
	dispCharAt(1+check_boxes[curr_execchoice].index,0,120);
	return ch;
*/
}
//******************************************************************************************//
//**************************************** display_labels **********************************//
//******************************************************************************************//
// displays the rt_labels if shown is set
void display_labels(void)
{
	int i,j;
	char temp[MAX_LABEL_LEN];

	blank_display();

#ifdef TEST_WRITE_DATA
//			mvwprintw(win, DISP_OFFSET+23, 2,"display labels: index %d rt_params %d                     ",curr_menu_index,no_rtparams);
//			wrefresh(win);
#endif

	for(i = 0;i < no_rtparams;i++)
	{
		if(rt_params[i].shown == SHOWN_SENT)
		{
//			strcpy(temp,get_rt_label(i));
			j = i+no_menu_labels;
			get_mlabel(j,temp);
			GDispStringAt(rt_params[i].row,rt_params[i].col,temp);
#ifdef TEST_WRITE_DATA
//			mvwprintw(win, DISP_OFFSET+24, 2+(i*4),"%d ",j);
//			mvwprintw(win, DISP_OFFSET+25+i, 2,"%s                   ",temp);
//			wrefresh(win);
#endif
		}
	}
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static void blank_choices(void)
{
	int row,col,i;
	row = 0;
	col = 0;
	char blank[] = "                        ";
	for(i = 0;i < NUM_CHECKBOXES;i++)
	{
		GDispStringAt(row,col,blank);
		row++;
	}
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static void blank_display(void)
{
	int row,col;
	for(row = 0;row < 13;row++)
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
		for(col = 1;col < COLUMN;col++)
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
	last_checkbox = NUM_CBLABELS-1;
	last_execchoice = NUM_CBLABELS-1;
	curr_execchoice = 0;
	scale_type = 0;
	prev_scale_type = 1;
	cur_row = NUM_ENTRY_ROW;
	cur_col = NUM_ENTRY_BEGIN_COL;
	display_menus();
//	new_data_ch = new_data_ex = 0;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
static UCHAR non_func(UCHAR ch)
{
	UCHAR ret_char = ch;

#ifdef TEST_WRITE_DATA
	mvwprintw(win, DISP_OFFSET+17,2, "                              ");
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
	mvwprintw(win, DISP_OFFSET+17,2, "*non_func  %x                ",ret_char);
	wrefresh(win);
#endif
	return ret_char;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
// read from serial port and if a sequence of 3 0x55's and 3 0xAA's are sent
// then set the syncup flag, otherwise call read_get_key with the other chars
int parse_sync()
{
	int i,j,got_seq;
	UCHAR buf[8];
	int res = 0;
	UCHAR key;

	i = 0;
	got_seq = 1;
#ifdef TEST_WRITE_DATA

//	mvwprintw(win, LAST_ROW-4,2,"                                ");
//	mvwprintw(win, LAST_ROW-3,2,"                                ");
	do
	{
		res += read(global_fd,&buf[i],1);
		usleep(1000);
//		printf("%2x  %2d \n",buf[i],i);
		mvwprintw(win, LAST_ROW-4,2+(i*4),"%2x ",buf[i]);
		mvwprintw(win, LAST_ROW-3,2+(i*4),"%2d ",i);
		wrefresh(win);
		if(buf[i] != sync_buf[i])
			got_seq = 0;
		i++;

	}while(got_seq && i < 6);

	if(got_seq == 1 && i >= 5)
		syncup = 1;
	j = i;

	if(syncup)
	{
		key = 0x51;
		write(global_fd,&key,1);
		usleep(1000);
		key = 0x52;
		write(global_fd,&key,1);
		usleep(1000);
		key = 0xA1;
		write(global_fd,&key,1);
		usleep(1000);
		key = 0xA2;
		write(global_fd,&key,1);
		usleep(1000);
		mvwprintw(win, LAST_ROW-4,2,"sync'd            ");
		wrefresh(win);
		return 1;
	}
	else
	{
/*
		mvwprintw(win, LAST_ROW-5,2,"                                        ");
		mvwprintw(win, LAST_ROW-6,2,"                                        ");
		mvwprintw(win, LAST_ROW-7,2,"                                        ");
		mvwprintw(win, LAST_ROW-8,2,"                                        ");
		wrefresh(win);
*/
		for(i = 0;i < j;i++)
		{
			read_get_key(buf[i]);
			mvwprintw(win, LAST_ROW-5,2+(i*4),"%2x ",buf[i]);
			mvwprintw(win, LAST_ROW-6,2+(i*4),"%2d ",i);
			wrefresh(win);
		}
		return 0;
	}
#endif
	return 0;
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
//******************************************************************************************//
//*********************************** enter_checkboxes *************************************//
//******************************************************************************************//
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
char *get_rt_label(int index)
{
	return rt_labels[index];
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
#endif


