// eeprom_burn.c - get used by test_write_data/test_write_data.c and eeprom/main_burn.c
// TEST_WRITE_DATA is define when compiled in test_write_data

#include "sfr_helper.h"
#ifndef TEST_WRITE_DATA
#include <avr/io.h>
#include "avr8-gnu-toolchain-linux_x86/avr/include/util/delay.h"
#include <avr/eeprom.h>
#include "USART.h"
#include "t6963.h"
#include "macros.h"
#else
#include <ncurses.h>
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

#ifndef TEST_WRITE_DATA
extern char eepromString[EEPROM_SIZE] EEMEM;
#endif

//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
int pack(UCHAR low_byte, UCHAR high_byte)
{
	int temp;
	int myint;
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
}

int burn_eeprom2(void)
{
	int i = 0;
	total_offset = 0;
	i = update_labels(i,"home\0");
	i = update_labels(i,"testa\0");
	i = update_labels(i,"test1b\0");
	i = update_labels(i,"test1c\0");
	i = update_labels(i,"test1d\0");
	i = update_labels(i,"test1e\0");
	i = update_labels(i,"test2a\0");
	i = update_labels(i,"test2B\0");
	i = update_labels(i,"test2c\0");
	i = update_labels(i,"test2d\0");
	i = update_labels(i,"test2e\0");
	i = update_labels(i,"test3a\0");
	i = update_labels(i,"test3b\0");
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

#ifndef TEST_WRITE_DATA
    printString("\r\nwriting to eeprom...\r\n");
#else
    memset(eeprom_sim,0x22,EEPROM_SIZE);
#endif
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

	i = update_labels(i,"choice0\0");
	i = update_labels(i,"choice1\0");
	i = update_labels(i,"choice2\0");
	i = update_labels(i,"choice3\0");
	i = update_labels(i,"choice4\0");
	i = update_labels(i,"choice5\0");
	i = update_labels(i,"choice6\0");
	i = update_labels(i,"choice7\0");
	i = update_labels(i,"choice8\0");
	i = update_labels(i,"choice9\0");

	i = update_labels(i,"exec0\0");
	i = update_labels(i,"exec1\0");
	i = update_labels(i,"exec2\0");
	i = update_labels(i,"exec3\0");
	i = update_labels(i,"exec4\0");
	i = update_labels(i,"exec5\0");
	i = update_labels(i,"exec6\0");
	i = update_labels(i,"exec7\0");
	i = update_labels(i,"exec8\0");
	i = update_labels(i,"exec9\0");
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
	memcpy((void*)(eeprom_sim+NO_RT_LABELS_EEPROM_LOCATION),(void*)&no_rt_labels,sizeof(UINT));
	memcpy((void*)(eeprom_sim+NO_MENU_LABELS_EEPROM_LOCATION),(void*)&no_menu_labels,sizeof(UINT));
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
	memcpy((void*)(eeprom_sim+NO_RTPARAMS_EEPROM_LOCATION),(void*)&no_rtparams,sizeof(UINT));
#endif

	i = 0;
	no_data_index = 0;

	total_offset = 0;

	start_menu_structs = rt_params_offset + 10;

//#ifdef SIM_PIC
//												'A' 	'B'		'C'		'D'		'#'		'0'
	i = update_menu_structs(i, _menu_change, 	MENU1A, MENU1B, MENU1C, MENU1D, MENU1E, MENU2A,  MAIN);
// 1a
	i = update_menu_structs(i, _menu_change,	MENU2C, MENU2D, MENU2E, MENU3A, MENU3B, MENU1B, MENU1A);
// 1b
	i = update_menu_structs(i, _menu_change,	MAIN,   MENU2D, MENU1B, MENU1D, MENU2A, MENU2B, MENU1B);
// 1c
	i = update_menu_structs(i, _exec_choice,	ckup, ckdown, ckenter, blank, blank, blank, MENU1C);
// 1d
	i = update_menu_structs(i, _do_chkbox, 		ckup, ckdown, cktoggle, ckenter, ckesc, choice1, MENU1D);
// 1e
	i = update_menu_structs(i, _do_chkbox, 		ckup, ckdown, cktoggle, ckenter, ckesc, blank, MENU1E);
// 2a
	i = update_menu_structs(i, _non_func, 		choice1, choice2, choice3, choice4, choice5, choice6, MENU2A);
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
//#endif
	return 0;
}
//#ifdef SIM_PIC
//******************************************************************************************//
//********************************* update_menu_structs*************************************//
//******************************************************************************************//
//int update_menu_structs(int i, char *label, UCHAR row, UCHAR col, UCHAR choice, UCHAR ch_type, UCHAR type)
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
	if(index > 0)
		no_data_index++;

#ifdef TEST_WRITE_DATA
	memcpy(eeprom_sim+total_offset+start_menu_structs,&menu_structs[i], len);
#else
    eeprom_update_block(menu_structs, eepromString+start_menu_structs+total_offset, len);
#endif
//	strncpy(menu_labels[index],ramstr,len);
	total_offset += len;
	i++;
	return i;
}
//#endif
//******************************************************************************************//
//*********************************** get_label_offsets ************************************//
//******************************************************************************************//
void get_label_offsets(void)
{
	int i, j;
	char *ch;
	char temp_label[MAX_LABEL_LEN];

	for(i = 0;i < no_menu_labels+no_rt_labels;i++)
	{
		label_offsets[i] = goffset;
		j = 0;
#ifdef TEST_WRITE_DATA
		memcpy(temp_label,eeprom_sim+goffset,MAX_LABEL_LEN);
#else
		eeprom_read_block(temp_label, eepromString+goffset, MAX_LABEL_LEN);
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
	memcpy(str,eeprom_sim+label_offsets[index],MAX_LABEL_LEN);
#else
	eeprom_read_block((void *)str,eepromString+label_offsets[index],MAX_LABEL_LEN);
#endif
#ifdef SCREEN_EN
	printString(str);
	printString("\r\n");
#endif
	return str;
}
//******************************************************************************************//
//*********************************** update_labels ***********************************//
//******************************************************************************************//
int update_labels(int index, char *ramstr)
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
	memcpy(eeprom_sim+RT_PARAMS_OFFSET_EEPROM_LOCATION+total_offset,&rt_params[i],sizeof(RT_PARAM));
#else
    eeprom_update_block(&rt_params[i], eepromString+RT_PARAMS_OFFSET_EEPROM_LOCATION+total_offset, sizeof(RT_PARAM));
#endif
	total_offset += sizeof(RT_PARAM);
//	printf("total_offset = %d\n",total_offset);
	i++;
	return i;
}

#ifndef TEST_WRITE_DATA
//******************************************************************************************//
//************************************** read_eeprom****************************************//
//******************************************************************************************//
int read_eeprom(void)
{
	int i,j;
	UCHAR no_prompts = 0;
	UINT prompt_info_offset = 0;
	UCHAR temp;
	UINT temp2;
	char temp_label[MAX_LABEL_LEN];
	int offset = 0;
	char *ch;

	// read the labels into ram (if in TEST_WRITE_DATA mode)
	printString("reading no_rt_labels\r\n");
	no_rt_labels = eeprom_read_byte((UCHAR*)NO_RT_LABELS_EEPROM_LOCATION);
	printString("no_rt_labels: ");
	printHexByte((UCHAR)(no_rt_labels>>8));
	printHexByte((UCHAR)no_rt_labels);
	printString("\r\n");

	printString("reading no_menu_labels\r\n");
	no_menu_labels = eeprom_read_byte((UCHAR*)NO_MENU_LABELS_EEPROM_LOCATION);
	printString("no_menu_labels: ");
	printHexByte((UCHAR)(no_menu_labels>>8));
	printHexByte((UCHAR)no_menu_labels);
	printString("\r\n");

	printString("reading no_rtparams\r\n");
	no_rtparams = eeprom_read_byte((UCHAR*)NO_RTPARAMS_EEPROM_LOCATION);
	printString("no_rtparams: ");
	printHexByte((UCHAR)(no_rtparams>>8));
	printHexByte((UCHAR)no_rtparams);
	printString("\r\n");
/*
	printString("reading no_menu_structs\r\n");
	no_menu_structs = eeprom_read_byte((UCHAR*)NO_MENUS_EEPROM_LOCATION);
	printString("no_menu_structs: ");
	printHexByte((UCHAR)(no_menu_structs>>8));
	printHexByte((UCHAR)no_menu_structs);
	printString("\r\n");

	rt_params_offset = (UINT)eeprom_read_byte((UCHAR*)RTPARAMS_OFFSET_EEPROM_LOCATION_LSB);	// read lsb
	temp = eeprom_read_byte((UCHAR*)RTPARAMS_OFFSET_EEPROM_LOCATION_MSB);		// read msb
	temp2 = (UINT)temp;
	rt_params_offset |= (temp2 << 8);		// shift msb over and bit-or with lsb
	printString("rt_params_offset: ");
	printHexByte((UCHAR)(rt_params_offset>>8));
	printHexByte((UCHAR)rt_params_offset);
	printString("\r\n");
*/
	for(i = 0;i < no_rt_labels+no_menu_labels;i++)
	{
		j = 0;
		eeprom_read_block(temp_label, eepromString+offset, MAX_LABEL_LEN);
		ch = temp_label;
		while(*ch != 0 && j < MAX_LABEL_LEN)
		{
			ch++;
			j++;
		}
		j++;		// adjust for zero at end
		offset += j;
	}
/*
	menu_struct_offset = (UINT)eeprom_read_byte((UCHAR*)MENUSTRUCT_OFFSET_EEPROM_LOCATION_LSB);	// read lsb
	temp = eeprom_read_byte((UCHAR*)MENUSTRUCT_OFFSET_EEPROM_LOCATION_MSB);		// read msb
	temp2 = (UINT)temp;
	menu_struct_offset |= (temp2 << 8);		// shift msb over and bit-or with lsb
	printString("menu_struct_offset: ");
	printHexByte((UCHAR)(menu_struct_offset>>8));
	printHexByte((UCHAR)menu_struct_offset);
	printString("\r\n");
*/
	printString("done reading eeprom\r\n");
	return 0;
}
#endif

