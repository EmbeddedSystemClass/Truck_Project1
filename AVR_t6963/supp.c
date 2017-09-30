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
	low_byte = ~low_byte;
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
	*low_byte = ~(*low_byte);
}


//******************************************************************************************//
//*********************************** get_mlabel_offsets ***********************************//
//******************************************************************************************//
// warning this needs to be written for the AVR main
int get_mlabel_offsets(void)
{
#ifdef TEST_WRITE_DATA
	int i, j,k;
	char *ch;
	char *pch;
	char temp_label[MAX_LABEL_LEN];
	int done = 0;

	goffset = 0;

	no_menu_labels = 0;
	no_rt_labels = 0;
	rt_params_offset = 0;
	i = 0;
	ch = pch = eeprom_sim;
	pch--;
// the eeprom image must have an extra '0' at the end of
// the menu labels and another extra '0' at the end
// of the rt labels
	do
	{
		if(*ch == 0)
			no_menu_labels++;
		i++;
		ch++;
		pch++;
	}while(*ch != 0 || *pch != 0);

	no_menu_labels++;

	if(i > EEPROM_SIZE-10)
		return -1;

	i = 0;
	do
	{
		if(*ch == 0)
			no_rt_labels++;
		i++;
		ch++;
		pch++;
	}while(*ch != 0 || *pch != 0);
	no_rt_labels;

	if(i > EEPROM_SIZE-10)
		return -1;

	for(i = 0;i < no_menu_labels+no_rt_labels;i++)
	{
		mlabel_offsets[i] = goffset;
		j = 0;
		memcpy(temp_label,eeprom_sim+goffset,MAX_LABEL_LEN);

//		eeprom_read_block(temp_label, eepromString+goffset, MAX_LABEL_LEN);
		ch = temp_label;
		while(*ch != 0 && j < MAX_LABEL_LEN)
		{
			ch++;
			j++;
		}
		j++;			// adjust for zero at end
		goffset += j;
	}
	rt_params_offset = goffset;
#endif
	return no_menu_labels;
}
//******************************************************************************************//
//*************************************** get_label ****************************************//
//******************************************************************************************//
void get_mlabel(int index, char *str)
{

		// void *dest, const void *src, size_t n
#ifdef TEST_WRITE_DATA
	memcpy(str,peeprom_sim+mlabel_offsets[index],MAX_LABEL_LEN);
#else
#ifdef MAIN_C
	eeprom_read_block((void *)str,eepromString+mlabel_offsets[index],MAX_LABEL_LEN);
#endif
#endif
}

//******************************************************************************************//
//*********************************** get_mlabel_offsets ***********************************//
//******************************************************************************************//
int get_cblabel_offsets(void)
{
	int i, j,k;
	char *ch;
	char temp_label[MAX_LABEL_LEN];

	goffset = 0;

	no_cblabels = 0;
	for(i = 0;i < CBLABEL_SIZE;i++)
	{
		if(cblabels[i] == 0 && cblabels[i-1] != 0)
			no_cblabels++;
	}

	for(i = 0;i < no_cblabels;i++)
	{
		cblabel_offsets[i] = goffset;
		j = 0;
		memcpy(temp_label,cblabels+goffset,MAX_LABEL_LEN);
		ch = temp_label;
		while(*ch != 0 && j < MAX_LABEL_LEN)
		{
			ch++;
			j++;
		}
		j++;			// adjust for zero at end
		goffset += j;
	}
	return no_cblabels;
}
//******************************************************************************************//
//*************************************** get_label ****************************************//
//******************************************************************************************//
void get_cblabel(int index, char *str)
{

		// void *dest, const void *src, size_t n
	memcpy(str,cblabels+cblabel_offsets[index],MAX_LABEL_LEN);
#ifdef SCREEN_EN
	printString(str);
	printString("\r\n");
#endif
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
//************************************* update_mlabels *************************************//
//******************************************************************************************//
int update_mlabels(int index, char *ramstr)
{
	int len;
	len = strlen(ramstr);
	len = (len > MAX_LABEL_LEN?MAX_LABEL_LEN:len);
	len++;
#ifdef TEST_WRITE_DATA
	memcpy(eeprom_sim+total_offset,ramstr, len);
#else
    eeprom_update_block(ramstr, eepromString+total_offset, len);
#endif
//	strncpy(menu_labels[index],ramstr,len);
	total_offset += len;
	index++;
	return index;
}
//******************************************************************************************//
//************************************* update_mlabels *************************************//
//******************************************************************************************//
int update_cblabels(int index, char *str)
{
	int len;
	len = strlen(str);
	len = (len > MAX_LABEL_LEN?MAX_LABEL_LEN:len);
	len++;
	memcpy(cblabels+total_offset,str, len);
//	strncpy(menu_labels[index],ramstr,len);
	total_offset += len;
	index++;
	return index;
}
//******************************************************************************************//
//************************************* update_rtparams*************************************//
//******************************************************************************************//
#ifndef MAIN_C
#ifndef SIM_AVR
int update_rtparams(int i, UCHAR row, UCHAR col, UCHAR shown, UCHAR dtype, UCHAR type)
{
	rt_params[i].row = row;					// row, col tells where the param will appear on screen
	rt_params[i].col = col;
	rt_params[i].shown = shown;				// if its shown or not
	rt_params[i].dtype = dtype;				// 0 - UCHAR; 1 - UINT; 2 - string
	rt_params[i].type = type;
	memcpy(eeprom_sim+RT_PARAMS_OFFSET_EEPROM_LOCATION+total_offset,&rt_params[i],sizeof(RT_PARAM));
//    eeprom_update_block(&rt_params[i], eepromString+RT_PARAMS_OFFSET_EEPROM_LOCATION+total_offset, sizeof(RT_PARAM));
	total_offset += sizeof(RT_PARAM);
//	printf("total_offset = %d\n",total_offset);
	i++;
	return i;
}
#endif
#endif
//******************************************************************************************//
//***************************************** burn_eeprom ************************************//
//******************************************************************************************//
#ifdef SHOW_EEPROM
int burn_eeprom(void)
{
	int i;
	no_rt_labels = 0;
	no_menu_labels = 0;
	no_rtparams = 0;
	total_offset = 0;
	char temp[MAX_LABEL_LEN];

    i = 0;
	i = update_mlabels(i,"home\0");		// 0
	i = update_mlabels(i,"MENU1a\0");	// 1
	i = update_mlabels(i,"MENU1b\0");	// 2
	i = update_mlabels(i,"MENU1c\0");	// 3
	i = update_mlabels(i,"MENU1d\0");	// 4
	i = update_mlabels(i,"MENU1e\0");	// 5
	i = update_mlabels(i,"MENU2a\0");	// 6
	i = update_mlabels(i,"MENU2B\0");	// 7
	i = update_mlabels(i,"MENU2c\0");	// 8
	i = update_mlabels(i,"MENU2d\0");	// 9
	i = update_mlabels(i,"MENU2e\0");	// 10
	i = update_mlabels(i,"MENU3a\0");	// 11
	i = update_mlabels(i,"MENU3b\0");	// 12

	i = update_mlabels(i,"enter\0");
	i = update_mlabels(i,"up\0");
	i = update_mlabels(i,"down\0");
	i = update_mlabels(i,"toggle\0");
	i = update_mlabels(i,"esc\0");
	i = update_mlabels(i,"reset\0");

	i = update_mlabels(i,"enter\0");
	i = update_mlabels(i,"forward\0");
	i = update_mlabels(i,"back\0");
	i = update_mlabels(i,"clear\0");
	i = update_mlabels(i,"escape\0");

	i = update_mlabels(i,"caps\0");
	i = update_mlabels(i,"small\0");
	i = update_mlabels(i,"spec\0");
	i = update_mlabels(i,"next\0");

	i = update_mlabels(i,"\0");
	no_menu_labels = i;

	i = update_mlabels(i,"RPM\0");
	i = update_mlabels(i,"ENG TEMP\0");
	i = update_mlabels(i,"TRIPPY\0");
	i = update_mlabels(i,"TIME\0");
	i = update_mlabels(i,"AIR TEMP\0");
	i = update_mlabels(i,"MPH\0");
	i = update_mlabels(i,"OIL PRES\0");
	i = update_mlabels(i,"MAP\0");
	i = update_mlabels(i,"OIL TEMP\0");
	i = update_mlabels(i,"O2\0");
	i = update_mlabels(i,"test\0");
	i = update_mlabels(i,"\0");

	no_rt_labels = i - no_menu_labels;
//	choice_offset = i;
	rt_params_offset = total_offset;
	i = 0;

	memcpy((void*)(eeprom_sim+NO_RT_LABELS_EEPROM_LOCATION),(void*)&no_rt_labels,sizeof(UINT));
	memcpy((void*)(eeprom_sim+NO_MENU_LABELS_EEPROM_LOCATION),(void*)&no_menu_labels,sizeof(UINT));
	total_offset = 0;

	i = update_rtparams(i, 0, 0, SHOWN_SENT, 1, RT_RPM);	// first label is at offset 0
	i = update_rtparams(i, 1, 0, NOSHOWN_SENT, 0, RT_ENGT);
	i = update_rtparams(i, 2, 0, NOSHOWN_NOSENT, 0, RT_TRIP);	// first element of offset_array has offset of 2nd label
	i = update_rtparams(i, 3, 0, SHOWN_SENT, 0, RT_TIME);
	i = update_rtparams(i, 4, 0, SHOWN_SENT, 0, RT_AIRT);
	i = update_rtparams(i, 0, 20, SHOWN_SENT, 0, RT_MPH);
	i = update_rtparams(i, 1, 20, SHOWN_SENT, 0, RT_OILP);
	i = update_rtparams(i, 2, 20, SHOWN_SENT, 0, RT_MAP);
	i = update_rtparams(i, 3, 20, SHOWN_SENT, 0, RT_OILT);
	i = update_rtparams(i, 4, 20, SHOWN_SENT, 0, RT_O2);

	no_rtparams = i;
// write to the number of rt_params location in eeprom the number of rt_params
	memcpy((void*)(eeprom_sim+NO_RTPARAMS_EEPROM_LOCATION),(void*)&no_rtparams,sizeof(UINT));

	update_ram();

	return 0;
}
#endif
#ifndef SIM_AVR
#ifndef MAIN_C
//******************************************************************************************//
//***************************************** update_ram *************************************//
//******************************************************************************************//
void update_ram(void)
{
	int i;
	i = 0;
	total_offset = 0;

//												'A' 	'B'		'C'		'D'		'#'		'0'
	i = update_menu_structs(i, _menu_change, 	MENU1C, MENU1D, MENU1E,  MENU2A, MENU2B, MENU2C, MAIN);
//	i = update_menu_structs(i, _menu_change, 	MENU2C, MENU2D, MENU2E,  MENU3A, MENU3B, MENU1C, MAIN);
// 1a
	i = update_menu_structs(i, _menu_change,	MENU2B, MENU2C, MENU2D, MENU2E, MENU3A, MENU3B, MENU1A);
// 1b
	i = update_menu_structs(i, _menu_change,	MAIN,   MENU2D, MENU1B, MENU1D, MENU2A, MENU2B, MENU1B);
// 1c
	i = update_menu_structs(i, _do_chkbox, 		ckup, ckdown, cktoggle, ckenter, ckesc, cclear, MENU1C);
// 1d
	i = update_menu_structs(i, _do_chkbox, 		ckup, ckdown, cktoggle, ckenter, ckesc, cclear, MENU1D);
// 1e
	i = update_menu_structs(i, _do_chkbox, 		ckup, ckdown, cktoggle, ckenter, ckesc, cclear, MENU1E);
// 2a
	i = update_menu_structs(i, _exec_choice,	ckup, ckdown, cktoggle, ckenter, ckesc, cclear, MENU2A);
// 2b
	i = update_menu_structs(i, _exec_choice,	ckup, ckdown, cktoggle, ckenter, ckesc, cclear, MENU2B);
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


#if 0
//												'A' 	'B'		'C'		'D'		'#'		'0'
	i = update_menu_structs(i, _menu_change, 	MENU1C, MENU1D, MENU1E, MENU2A, MENU2B, MENU1B, MAIN);
// 1a
	i = update_menu_structs(i, _menu_change,	MENU2C, MENU2D, MENU2E, MENU3A, MENU3B, MENU1B, MENU1A);
// 1b
	i = update_menu_structs(i, _menu_change,	MAIN,   MENU2D, MENU1B, MENU1D, MENU2A, MENU2B, MENU1B);
// 1c
	i = update_menu_structs(i, _do_chkbox, 		ckup, ckdown, cktoggle, ckenter, ckesc, blank, MENU1C);
// 1d
	i = update_menu_structs(i, _do_chkbox, 		ckup, ckdown, cktoggle, ckenter, ckesc, blank, MENU1D);
// 1e
	i = update_menu_structs(i, _do_chkbox, 		ckup, ckdown, cktoggle, ckenter, ckesc, blank, MENU1E);
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
#endif
	no_menu_structs = i;

	memset(cblabels,0,CBLABEL_SIZE);
	total_offset = 0;
	i = 0;
	i =  update_cblabels(i, "test 1\0");
	i =  update_cblabels(i, "test 2\0");
	i =  update_cblabels(i, "test 3\0");
	i =  update_cblabels(i, "test 4\0");
	i =  update_cblabels(i, "test 5\0");
	i =  update_cblabels(i, "test 6\0");
	i =  update_cblabels(i, "test 7\0");
	i =  update_cblabels(i, "test 8\0");
	i =  update_cblabels(i, "test 9\0");
	i =  update_cblabels(i, "test 10\0");
	i =  update_cblabels(i, "asdf1 1\0");
	i =  update_cblabels(i, "asdf1 2\0");
	i =  update_cblabels(i, "asdf1 3\0");
	i =  update_cblabels(i, "asdf1 4\0");
	i =  update_cblabels(i, "asdf1 5\0");
	i =  update_cblabels(i, "asdf1 6\0");
	i =  update_cblabels(i, "asdf1 7\0");
	i =  update_cblabels(i, "asdf1 8\0");
	i =  update_cblabels(i, "asdf1 9\0");
	i =  update_cblabels(i, "asdf1 10\0");
	i =  update_cblabels(i, "hello2 1\0");
	i =  update_cblabels(i, "hello2 2\0");
	i =  update_cblabels(i, "hello2 3\0");
	i =  update_cblabels(i, "hello2 4\0");
	i =  update_cblabels(i, "hello2 5\0");
	i =  update_cblabels(i, "hello2 6\0");
	i =  update_cblabels(i, "hello2 7\0");
	i =  update_cblabels(i, "hello2 8\0");
	i =  update_cblabels(i, "hello2 9\0");
	i =  update_cblabels(i, "hello2 10\0");
	i =  update_cblabels(i, "xyz3 1\0");
	i =  update_cblabels(i, "xyz3 2\0");
	i =  update_cblabels(i, "xyz3 3\0");
	i =  update_cblabels(i, "xyz3 4\0");
	i =  update_cblabels(i, "xyz3 5\0");
	i =  update_cblabels(i, "xyz3 6\0");
	i =  update_cblabels(i, "xyz3 7\0");
	i =  update_cblabels(i, "xyz3 8\0");
	i =  update_cblabels(i, "xyz3 9\0");
	i =  update_cblabels(i, "xyz3 10\0");
	i =  update_cblabels(i, "quick4 1\0");
	i =  update_cblabels(i, "quick4 2\0");
	i =  update_cblabels(i, "quick4 3\0");
	i =  update_cblabels(i, "quick4 4\0");
	i =  update_cblabels(i, "quick4 5\0");
	i =  update_cblabels(i, "quick4 6\0");
	i =  update_cblabels(i, "quick4 7\0");
	i =  update_cblabels(i, "quick4 8\0");
	i =  update_cblabels(i, "quick4 9\0");
	i =  update_cblabels(i, "quick4 10\0");
	i =  update_cblabels(i, "ending 1\0");
	i =  update_cblabels(i, "ending 2\0");
	i =  update_cblabels(i, "ending 3\0");
	i =  update_cblabels(i, "ending 4\0");
	i =  update_cblabels(i, "ending 5\0");
}
#endif
#endif

