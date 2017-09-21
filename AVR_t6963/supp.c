#include "sfr_helper.h"
//#ifndef TEST_WRITE_DATA
#ifdef MAIN_C
#include <avr/io.h>
#include "avr8-gnu-toolchain-linux_x86/avr/include/util/delay.h"
#include <avr/eeprom.h>
#include "USART.h"
#include "t6963.h"
#include "macros.h"
#else
#ifdef TEST_WRITE_DATA
#include <ncurses.h>
#endif
//#warning "TEST_WRITE_DATA defined"
#endif
#include "main.h"
#ifdef SIM_AVR
#include "avr_main.h"
#endif
#ifdef SIM_PIC
#include "pic_main.h"
#endif
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef MAIN_C
extern char eepromString[EEPROM_SIZE] EEMEM;
#endif

#ifdef TEST_WRITE_DATA
extern UCHAR *peeprom_sim;
#endif

//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
int pack(UCHAR low_byte, UCHAR high_byte)
{
	int temp;
	int myint;
//	low_byte = ~low_byte;
	myint = (int)low_byte;
	temp = (int)high_byte;
	temp <<= 8;
	myint |= temp;
	return myint;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
void unpack(int myint, UCHAR *low_byte, UCHAR *high_byte)
{
	*low_byte = (UCHAR)myint;
	myint >>= 8;
	*high_byte = (UCHAR)myint;
//	*low_byte = ~(*low_byte);
}


//******************************************************************************************//
//*********************************** get_label_offsets ************************************//
//******************************************************************************************//
void get_label_offsets(WINDOW *win)
{
	int i, j,k;
	char *ch;
	char temp_label[MAX_LABEL_LEN];

	for(i = 0;i < no_menu_labels+no_rt_labels;i++)
	{
		label_offsets[i] = goffset;
		j = 0;
#ifdef TEST_WRITE_DATA
		memcpy(temp_label,peeprom_sim+goffset,MAX_LABEL_LEN);
		mvwprintw(win, LAST_ROW-8,2,"%s   ", temp_label);
		wrefresh(win);
		
#else
#ifdef MAIN_C
		eeprom_read_block(temp_label, eepromString+goffset, MAX_LABEL_LEN);
#endif
#endif
		ch = temp_label;
		while(*ch != 0 && j < MAX_LABEL_LEN)
		{
			ch++;
			j++;
		}
		j++;			// adjust for zero at end
		goffset += j;
	}
}
//******************************************************************************************//
//*************************************** get_label ****************************************//
//******************************************************************************************//
char *get_label(int index, char *str)
{

		// void *dest, const void *src, size_t n
#ifdef TEST_WRITE_DATA
	memcpy(str,peeprom_sim+label_offsets[index],MAX_LABEL_LEN);
#else
#ifdef MAIN_C
	eeprom_read_block((void *)str,eepromString+label_offsets[index],MAX_LABEL_LEN);
#endif
#endif
#ifdef SCREEN_EN
	printString(str);
	printString("\r\n");
#endif
	return str;
}
#ifndef SIM_AVR
#ifndef MAIN_C
//******************************************************************************************//
//********************************* update_menu_structs*************************************//
//******************************************************************************************//
int update_menu_structs(int i, UCHAR fptr, UCHAR menu0, UCHAR menu1, UCHAR menu2, UCHAR menu3,
			UCHAR menu4, UCHAR menu5, UCHAR index)
{
	int len;
	len = sizeof(MENU_FUNC_STRUCT);
	menu_structs[i].menus[0] = menu0;
	menu_structs[i].menus[1] = menu1;
	menu_structs[i].menus[2] = menu2;
	menu_structs[i].menus[3] = menu3;
	menu_structs[i].menus[4] = menu4;
	menu_structs[i].menus[5] = menu5;
	menu_structs[i].fptr = fptr;
	menu_structs[i].index = index;

	total_offset += len;
	i++;
	return i;
}
#endif
#endif
//******************************************************************************************//
//************************************* update_labels **************************************//
//******************************************************************************************//
int update_labels(int index, char *ramstr)
{
	int len;
	len = strlen(ramstr);
	len = (len > MAX_LABEL_LEN?MAX_LABEL_LEN:len);
	len++;
#ifdef TEST_WRITE_DATA
	memcpy(peeprom_sim+total_offset,ramstr, len);
#else
    eeprom_update_block(ramstr, eepromString+total_offset, len);
#endif
//	strncpy(menu_labels[index],ramstr,len);
	total_offset += len;
	index++;
	return index;
}
//******************************************************************************************//
//************************************* update_rtparams*************************************//
//******************************************************************************************//
int update_rtparams(int i, UCHAR row, UCHAR col, UCHAR shown, UCHAR dtype, UCHAR type)
{
	rt_params[i].row = row;					// row, col tells where the param will appear on screen
	rt_params[i].col = col;
	rt_params[i].shown = shown;				// if its shown or not
	rt_params[i].dtype = dtype;				// 0 - UCHAR; 1 - UINT; 2 - string
	rt_params[i].type = type;
#ifdef TEST_WRITE_DATA
	memcpy(peeprom_sim+RT_PARAMS_OFFSET_EEPROM_LOCATION+total_offset,&rt_params[i],sizeof(RT_PARAM));
#else
    eeprom_update_block(&rt_params[i], eepromString+RT_PARAMS_OFFSET_EEPROM_LOCATION+total_offset, sizeof(RT_PARAM));
#endif
	total_offset += sizeof(RT_PARAM);
//	printf("total_offset = %d\n",total_offset);
	i++;
	return i;
}

