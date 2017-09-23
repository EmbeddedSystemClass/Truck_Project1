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
//static void init_execchoices(int);
static void blank_choices(void);
static void display_edit_value(void);
static UCHAR generic_menu_function(UCHAR ch);
static UCHAR backspace(UCHAR ch);
static UCHAR enter(UCHAR ch);
static UCHAR escape(UCHAR ch);
static int curr_menu_index;
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

static UCHAR (*fptr[NUM_FPTS])(UCHAR) = { menu_change, do_exec, do_chkbox, non_func, do_numentry, do_init };

//******************************************************************************************//
//********************************* generic_menu_function **********************************//
//******************************************************************************************//
static UCHAR generic_menu_function(UCHAR ch)
{
	UCHAR ret_char;
	int i,j,k;
	int aux_bytes_to_read;
	int aux_data_offset;
	UCHAR tfptr;
	int res,res2;
	UCHAR low_byte,high_byte;
	char tlabel[MAX_LABEL_LEN];
	res = 0;
	res2 = 0;
#ifdef TEST_WRITE_DATA
	res += read(global_fd,&tfptr,1);
	res += 	read(global_fd,&high_byte,1);
	res += 	read(global_fd,&low_byte,1);
	curr_menu_index = pack(low_byte,high_byte);

	mvwprintw(win, LAST_ROW-1, 1,"ch:%x  fptr:%x res:%d index:%d   ",ch,tfptr,res,curr_menu_index);

	mvwprintw(win, LAST_ROW-2,1,"                                           ");
	for(i = 0;i < 6;i++)
	{
		res += read(global_fd,&high_byte,1);
		res += read(global_fd,&low_byte,1);

		curr_menus[i] = pack(low_byte,high_byte);
		mvwprintw(win, LAST_ROW-20+i,1,"%2d     ",curr_menus[i]);
		wrefresh(win);
		if(curr_menus[i] > blank)
			curr_menus[i] = 0;
	}

	for(i = 0;i < 20;i++)
	{
		res2 += read(global_fd,&aux_string[i],1);
		mvwprintw(win, LAST_ROW-10, 2+(i*3),"%2x ",aux_string[i]);
		wrefresh(win);
	}

//	mvwprintw(win, LAST_ROW-21,1,"size %d start_addr %d read: %d    ",aux_bytes_to_read,aux_data_offset,res);
//	wrefresh(win);

	low_byte = 0;
	get_fptr_label(tfptr,tlabel);
//	mvwprintw(win, DISP_OFFSET+30, 2,
//	               "fptr: %s ch:%x res:%d res2:%d ",tlabel,ch,res,res2);
	wrefresh(win);

#else
	tfptr = receiveByte();
	high_byte = receiveByte();
	low_byte = receiveByte();
	curr_menu_index = pack(low_byte, high_byte);

	for(i = 0;i < 6;i++)
	{
		high_byte = receiveByte();
//		transmitByte(high_byte);
		low_byte = receiveByte();
//		transmitByte(low_byte);
		curr_menus[i] = pack(low_byte,high_byte);
	}
	high_byte = receiveByte();
//	transmitByte(high_byte);
	low_byte = receiveByte();
//	transmitByte(low_byte);
	aux_bytes_to_read = pack(low_byte, high_byte);

	aux_bytes_to_read = receiveByte();
	aux_data_offset = receiveByte();
/*
	for(i = 0;i < aux_bytes_to_read;i++)
		aux_string[i] = receiveByte();
*/
	// write a code of 0xAA back to sim PIC as a verification that everything is running
	// if not hooked up to LCD screen
/*
	low_byte = 0xAA;
	transmitByte(low_byte);

	aux_string[0] = (UCHAR)aux_bytes_to_read;
	aux_string[1] = (UCHAR)(aux_bytes_to_read>>8);
	aux_string[2] = low_byte;
	aux_string[3] = high_byte;
	aux_string[4] = (UCHAR)menu_index;
	aux_string[5] = (UCHAR)(menu_index>>8);

	for(i = 0;i < 6;i++)
		transmitByte(aux_string[i]);
*/
#endif
	if(tfptr <= NUM_FPTS)
	{
		ret_char = (*fptr[tfptr])(ch);		// execute the function pointer from the PIC
//	mvwprintw(win, DISP_OFFSET+20, 2,"%x  %x  ",ret_char,ch);
		display_menus();

		if(prev_menu_index != curr_menu_index)
		{
			blank_choices();
			switch (curr_menu_index)
			{
				case MAIN:
				case MENU1A:
				case MENU1B:
					display_labels();
					break;
				case MENU1C:
				case MENU1D:
				case MENU1E:
					init_checkboxes(curr_menu_index);
				break;
				case MENU2A:
				case MENU2B:
					init_checkboxes(curr_menu_index);
				break;
				case MENU2C:
				case MENU2D:
				case MENU2E:
				case MENU3A:
				case MENU3B:
					init_numentry(curr_menu_index);
				break;
			}
		}
		prev_menu_index = curr_menu_index;
		// show the contents of aux_string
	}
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

#ifdef TEST_WRITE_DATA
//	mvwprintw(win, LAST_ROW-4, 2+(index*8),"%s ",temp);
//	mvwprintw(win, LAST_ROW-5, 2+(index*4),"%d ",curr_menus[index]);
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
UCHAR read_get_key(UCHAR key)
{
	int i,j,k,done;
	UCHAR ret_char = key;
	int size = 0;
	int start_addr = 0;
	int res = 0;
	int res2 = 0;
	int type = 0;
	UCHAR low_byte, high_byte, recByte;
	int choice_aux_offset;
	int exec_aux_offset;
	char temp[MAX_LABEL_LEN];
	char *vptr;

#if 0
	if(ret_char > 0x21 && ret_char < 0x7e)
		mvwprintw(win, LAST_ROW-3,2,"%c  %d   ",ret_char,res);
	else
		mvwprintw(win, LAST_ROW-3,2,"%2x  %d   ",ret_char,res);
	wrefresh(win);

	if(!syncup)
	{
		mvwprintw(win, LAST_ROW-3,20,"not sync'd    ");
		wrefresh(win);
		return ret_char;
	}
	else if(done)
	{
		done = 0;
		mvwprintw(win, LAST_ROW-6,2,"done1: %2x  %d  ",key,syncup);
		wrefresh(win);
		return ret_char;
	}
//	mvwprintw(win, LAST_ROW-2,1,"                                        ");

	mvwprintw(win, LAST_ROW-7,2,"done2: %2x  %d  ",ret_char,syncup);
	wrefresh(win);
#endif

	switch(ret_char)
	{
		case TEST11:
#ifdef TEST_WRITE_DATA

			for(i = 0;i < 50;i++)
			{
				write(global_fd,&i,1);
				usleep(tdelay);
				mvwprintw(win, LAST_ROW-3,1,
				"%2x                                ",i);
				wrefresh(win);
			}
			k = j = 0;
			for(i = 500;i < EEPROM_SIZE;i++)
			{
				mvwprintw(win, LAST_ROW+j-40, 2+(k*3),"%2x ",eeprom_sim[i]);
				if(++k > 25)
				{
					k = 0;
					++j;
				}
			}
#endif
			break;
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
#endif
			break;
		case TEST5:
			res = 0;
//			for(i = 0;i < TOTAL_NUM_CBLABELS/2;i++)
//			for(i = 0;i < no_cblabels/2;i++)

			mvwprintw(win, LAST_ROW-1, 2,"test5                          ");

			k = j = 0;
			for(i = 0;i < CBLABEL_SIZE;i++)
			{
				mvwprintw(win, LAST_ROW+i,2,"                                     ");
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

			break;
		case READ_EEPROM:
#ifdef TEST_WRITE_DATA

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
			}

			mvwprintw(win, LAST_ROW-2,1,
				"read_eeprom1 - size: %d start: %d    ",size,start_addr);
			wrefresh(win);

			for(i = start_addr;i < size+start_addr;i++)
//			for(i = 0; i < 100;i++)
//			for(i = 0;i < size;i++)
			{
				usleep(tdelay);
				write(global_fd,&eeprom_sim[i],1);

				mvwprintw(win, LAST_ROW-3,1,
				"%2x   %d                               ",eeprom_sim[i],i);
				wrefresh(win);

//				low_byte = ~i;
//				res2 += write(global_fd,&low_byte,1);
			}
			mvwprintw(win, LAST_ROW-2,1,
				"read_eeprom1 - size: %d start: %d                 ",size,start_addr);
			wrefresh(win);

			j = 0;
			k = 0;

//			memcpy((void*)&no_rt_labels, (void*)(eeprom_sim+NO_RT_LABELS_EEPROM_LOCATION),sizeof(UINT));
//			memcpy((void*)&no_rtparams, (void*)(eeprom_sim+NO_RTPARAMS_EEPROM_LOCATION),sizeof(UINT));
//			memcpy((void*)&no_menu_labels, (void*)(eeprom_sim+NO_MENU_LABELS_EEPROM_LOCATION),sizeof(UINT));

//			mvwprintw(win, LAST_ROW-4,2,
//				"no_rt_labels: %d no_menu_labels: %d no_rtparams: %d  ",no_rt_labels,no_menu_labels,no_rtparams);

			for(i = 1;i < LAST_ROW+1;i++)
				mvwprintw(win, i,2,"                                                      ");

			for(i = start_addr;i < size+start_addr;i++)
//			for(i = 0;i < EEPROM_SIZE;i++)
			{
				if(eeprom_sim[i] < 0x7e && eeprom_sim[i] > 0x21)
					mvwprintw(win, LAST_ROW+j-40, 2+k,"%c",eeprom_sim[i]);
				else if(eeprom_sim[i] == 0)	mvwprintw(win, LAST_ROW+j-40, 24+k," ");
				else mvwprintw(win, LAST_ROW+j-40, 2+k,"_");
				if(++k > 40)
				{
					k = 0;
					++j;
				}
			}
			k = 0;
/*
			for(i = NO_MENU_LABELS_EEPROM_LOCATION;i < NO_MENUS_EEPROM_LOCATION;i++,k++)
			{
				mvwprintw(win, LAST_ROW-3,2+(k*3),"%d ", eeprom_sim[i]);
			}
*/
			mvwprintw(win, LAST_ROW-2,1,
				"read_eeprom2 - size: %d start: %d    ",size,start_addr);
			wrefresh(win);
#else
			high_byte = receiveByte();
			transmitByte(high_byte);
			transmitByte(low_byte);
			size = pack(low_byte,high_byte);

			transmitByte(high_byte);
			transmitByte(low_byte);
			start_addr = pack(low_byte,high_byte);
			memset(aux_string,0,AUX_STRING_LEN);
			eeprom_read_block((void *)&aux_string[0],(void *)eepromString+start_addr,(size_t)size);

			for(i = 0;i < size;i++)
			{
				low_byte = receiveByte();
				transmitByte(aux_string[i]);
			}
#endif
			get_mlabel_offsets();
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
			mvwprintw(win, LAST_ROW-2,1,"done %d  ",res2);
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

			for(i = 0;i < size;i++)
			{
				aux_string[i] = receiveByte();
				transmitByte(aux_string[i]);
			}
//			eeprom_update_block((const void*)&aux_string[0],(void *)eepromString+start_addr,(size_t)size);
#endif
			get_mlabel_offsets();

			break;

		case LOAD_RAM:
#ifdef TEST_WRITE_DATA

			res = 0;
			mvwprintw(win, LAST_ROW-2,1,"                                       ");

			mvwprintw(win, LAST_ROW-2,1,"loading checkboxes: ");

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

			for(i = 0;i < TOTAL_NUM_CBLABELS;i++)
			{
				res += read(global_fd,&check_boxes[i].checked,1);
				res += read(global_fd,&check_boxes[i].index,1);
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

			mvwprintw(win, LAST_ROW-3,33,"res: %2d            ",res);
			wrefresh(win);
			get_cblabel_offsets();
/*
			res = 0;

			mvwprintw(win, LAST_ROW-1,1,"loading eeprom:                   ");
			for(i = 0;i < EEPROM_SIZE;i++)
			{
				res += read(global_fd,&eeprom_sim[i],1);
			mvwprintw(win, LAST_ROW-1,17,"%2x %d ",eeprom_sim[i],res);
				if(i < NO_RT_LABELS_EEPROM_LOCATION)
					usleep(tdelay*20);
				wrefresh(win);
			}

			mvwprintw(win, LAST_ROW-1,24,"res: %2d  ", res);
*/
			wrefresh(win);

			j = 0;
/*
			for(i = NO_RT_LABELS_EEPROM_LOCATION;i < NO_MENU_LABELS_EEPROM_LOCATION+2;i++)
			{
				mvwprintw(win, LAST_ROW-2,1+(j*3),"%2x ",eeprom_sim[i],res);
				wrefresh(win);
				j++;
			}

			memcpy((void*)&no_rt_labels,(void*)(eeprom_sim+NO_RT_LABELS_EEPROM_LOCATION),sizeof(UINT));
			memcpy((void*)&no_rtparams,(void*)(eeprom_sim+NO_RTPARAMS_EEPROM_LOCATION),sizeof(UINT));
			memcpy((void*)&no_menu_labels,(void*)(eeprom_sim+NO_MENU_LABELS_EEPROM_LOCATION),sizeof(UINT));

			get_mlabel_offsets();

		//	no_menu_labels = 27;
		//	no_rt_labels = 10;

		//	mvwprintw(win, LAST_ROW-3,1,"eeprom locations: %2d %2d %2d ",no_rt_labels,no_rtparams,no_menu_labels);
*/
			wrefresh(win);
#endif

		break;

		default:
//			if(ret_char >= KP_POUND && ret_char <= KP_D)
			ret_char = generic_menu_function(ret_char);
/*
			eeprom_sim[bp] = ret_char;
			if(++bp >= EEPROM_SIZE)
				bp = EEPROM_SIZE-500;
*/
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
	int i,j,k;
//	mvwprintw(win, DISP_OFFSET+17,2, "                    ");

	k = (curr_menu_index-MENU1C);
	k *= NUM_CHECKBOXES;
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
		break;
		case KP_POUND:		// esc
			for(i = 0;i < NUM_CHECKBOXES;i++)
			{
				check_boxes[k+i].checked = prev_check_boxes[k+i].checked;
			}
		break;
		case KP_0:
		break;
		case KP_AST:
		break;
		default:
		break;
	}
#ifdef TEST_WRITE_DATA
	mvwprintw(win, DISP_OFFSET+19,2,"                                  ");
	mvwprintw(win, DISP_OFFSET+20,2,"                                  ");
	mvwprintw(win, DISP_OFFSET+20,2," %d %d %d ",curr_menu_index,k,j);

	mvwprintw(win, DISP_OFFSET+17,2, "*do_exec  %x   %d  ",ret_char,k);
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
	int i,j,k;
//	mvwprintw(win, DISP_OFFSET+17,2, "                    ");

	k = (curr_menu_index-MENU1C);
	k *= NUM_CHECKBOXES;
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
		break;
		case KP_POUND:		// esc
			for(i = 0;i < NUM_CHECKBOXES;i++)
			{
				check_boxes[k+i].checked = prev_check_boxes[k+i].checked;
			}
		break;
		case KP_0:
		break;
		case KP_AST:
		break;
		default:
		break;
	}
#ifdef TEST_WRITE_DATA
	mvwprintw(win, DISP_OFFSET+19,2,"                                  ");
	mvwprintw(win, DISP_OFFSET+20,2,"                                  ");
	mvwprintw(win, DISP_OFFSET+20,2," %d %d %d ",curr_menu_index,k,j);

	mvwprintw(win, DISP_OFFSET+17,2, "*do_chkbox  %x   %d  ",ret_char,k);
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
	int i,j,k;
	UCHAR row, col;
	char tlabel[MAX_LABEL_LEN];
//	scale_disp(SCALE_DISP_SOME);
	row = 1;
	col = 3;
	curr_checkbox = 0;

	blank_display();

	j = (menu_index-MENU1C);


#ifdef TEST_WRITE_DATA
//	mvwprintw(win, DISP_OFFSET+20, 2,"menu_index (init_checkboxes) %d  %d  ",menu_index,j);
//	wrefresh(win);
#endif

	j *= NUM_CHECKBOXES;
	for(i = 0;i < NUM_CHECKBOXES;i++)
	{
		k = j + i;
		get_cblabel(k,tlabel);
		GDispStringAt(row,col,tlabel);
#ifdef TEST_WRITE_DATA
//		mvwprintw(win, DISP_OFFSET+21+i, 2,"%d %d %s  ",k,j,tlabel);
#endif
		row++;

		if(check_boxes[k].checked == 1)
		{
			dispCharAt(1+check_boxes[k].index,0,120);
		}
		else
		{
			dispCharAt(1+check_boxes[k].index,0,0x20);
		}
	}
	wrefresh(win);
}
//******************************************************************************************//
//********************************** scrollup_checkboxes ***********************************//
//******************************************************************************************//
static UCHAR scrollup_checkboxes(int index)
{
	int k = index+curr_checkbox;
	dispCharAt(1+check_boxes[k].index,20,0x20);

	if(--curr_checkbox < 0)
		curr_checkbox = last_checkbox;
	// move cursor to 3 + check_boxes[curr_checkbox]
	dispCharAt(1+check_boxes[k].index,20,0x21);
	return k;
}
//******************************************************************************************//
//********************************* scrolldown_checkboxes **********************************//
//******************************************************************************************//
static UCHAR scrolldown_checkboxes(int index)
{
	int k = index+curr_checkbox;
	dispCharAt(1+check_boxes[k].index,20,0x20);

	if(++curr_checkbox > last_checkbox)
		curr_checkbox = 0;
	// move cursor
	dispCharAt(1+check_boxes[k].index,20,0x21);
	return k;
}
//******************************************************************************************//
//********************************** toggle_checkboxes *************************************//
//******************************************************************************************//
static UCHAR toggle_checkboxes(int index)
{
	int k = index+curr_checkbox;
	if(check_boxes[k].checked == 1)
	{
		check_boxes[k].checked = 0;
		dispCharAt(1+check_boxes[k].index,0,0x20);	// display 'blank'
	}
	else
	{
		check_boxes[k].checked = 1;
		dispCharAt(1+check_boxes[k].index,0,120);	// display 'x'
	}
	return k;
}
//******************************************************************************************//
//********************************** scrollup_execchoice ***********************************//
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
//********************************* scrolldown_execchoice **********************************//
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
	last_checkbox = TOTAL_NUM_CBLABELS-1;
	last_execchoice = TOTAL_NUM_CBLABELS-1;
	curr_execchoice = 0;
	scale_type = 0;
	prev_scale_type = 1;
	new_data_ready = 0;
	data_entry_mode = 0;
	mod_data_ready = 0;
	cur_row = NUM_ENTRY_ROW;
	cur_col = NUM_ENTRY_BEGIN_COL;
	aux_index = 0;
	display_menus();
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

