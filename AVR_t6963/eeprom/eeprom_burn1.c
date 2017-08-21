// eeprom_burn.c - get used by test_write_data/test_write_data.c and eeprom/main_burn.c
// TEST_WRITE_DATA is define when compiled in test_write_data
#define EEPROM_BURN

#include "../sfr_helper.h"
#ifndef TEST_WRITE_DATA
#include <avr/io.h>
#include "../avr8-gnu-toolchain-linux_x86/avr/include/util/delay.h"
#include <avr/eeprom.h>
#include "../USART.h"
#include "../t6963.h"
#include "../macros.h"
#else
#include <ncurses.h>
//#warning "TEST_WRITE_DATA defined"
#endif
#include "../pic_main.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#ifndef TEST_WRITE_DATA
extern char eepromString[STRING_LEN] EEMEM;
#endif
//******************************************************************************************//
//***************************************** burn_eeprom ************************************//
//******************************************************************************************//
int burn_eeprom(void)
{
	int i;
	no_rt_labels = 0;
	no_menu_labels = 0;
	no_rtparams = 0;
	no_menu_structs = 0;
	total_offset = 0;

#ifndef TEST_WRITE_DATA
    printString("\r\nwriting to eeprom...\r\n");
#endif
    i = 0;
#if 1	// update labels
	i = update_rt_labels(i,"RPM\0");
	i = update_rt_labels(i,"ENG TEMP\0");
	i = update_rt_labels(i,"TRIP\0");
	i = update_rt_labels(i,"TIME\0");
	i = update_rt_labels(i,"AIR TEMP\0");
	i = update_rt_labels(i,"MPH\0");
	i = update_rt_labels(i,"OIL PRES\0");
	i = update_rt_labels(i,"MAP\0");
	i = update_rt_labels(i,"OIL TEMP\0");
	i = update_rt_labels(i,"O2\0");
	i = update_rt_labels(i,"AUX1\0");
	i = update_rt_labels(i,"AUX2\0");
	no_rt_labels = i;
	i = 0;
// when adding up to <end> must modify get_fptr_label in PIC_menu.c (currently 25)
	i = update_menu_labels(i,"home\0");
	i = update_menu_labels(i,"MENU1a\0");
	i = update_menu_labels(i,"MENU1b\0");
	i = update_menu_labels(i,"MENU1c\0");
	i = update_menu_labels(i,"MENU1d\0");
	i = update_menu_labels(i,"MENU2a\0");
	i = update_menu_labels(i,"MENU2B\0");
	i = update_menu_labels(i,"MENU2c\0");
	i = update_menu_labels(i,"MENU2d\0");
	i = update_menu_labels(i,"test 0\0");
	i = update_menu_labels(i,"test 1\0");
	i = update_menu_labels(i,"test 2\0");
	i = update_menu_labels(i,"test 3\0");
	i = update_menu_labels(i,"test 4\0");
	i = update_menu_labels(i,"test 5\0");
	i = update_menu_labels(i,"test 6\0");
	i = update_menu_labels(i,"test 7\0");
	i = update_menu_labels(i,"test 8\0");
	i = update_menu_labels(i,"choice 0\0");
	i = update_menu_labels(i,"choice 1\0");
	i = update_menu_labels(i,"choice 2\0");
	i = update_menu_labels(i,"choice 3\0");
	i = update_menu_labels(i,"choice 4\0");
	i = update_menu_labels(i,"choice 5\0");
	i = update_menu_labels(i,"choice 6\0");
	i = update_menu_labels(i,"choice 7\0");
	i = update_menu_labels(i,"choice 8\0");
	i = update_menu_labels(i,"choice 9\0");
	i = update_menu_labels(i,"choice 10\0");
	i = update_menu_labels(i,"choice 11\0");
	no_menu_labels = i;
//	printf("no_menu_labels: %d\n",no_menu_labels);
//	getch();
	no_func_labels = 0;
// end of menus
	i = update_menu_labels(i,"mchange\0");
	i = update_menu_labels(i,"exec_ch\0");
	i = update_menu_labels(i,"chkbox\0");
	i = update_menu_labels(i,"non_fnc\0");
	i = update_menu_labels(i,"enter\0");
	i = update_menu_labels(i,"back\0");
	i = update_menu_labels(i,"esc\0");
	i = update_menu_labels(i,"caps\0");
	i = update_menu_labels(i,"small\0");
	i = update_menu_labels(i,"spec\0");
	i = update_menu_labels(i,"next\0");
	i = update_menu_labels(i,"forward\0");
	i = update_menu_labels(i,"enter\0");
	i = update_menu_labels(i,"up\0");
	i = update_menu_labels(i,"down\0");
	i = update_menu_labels(i,"toggle\0");
	i = update_menu_labels(i,"enter\0");
	i = update_menu_labels(i,"esc\0");
	no_func_labels = i;
//	printf("no_func_labels: %d\n",no_func_labels);
//	getch();
#endif
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

	eeprom_update_word((UINT *)RTPARAMS_OFFSET_EEPROM_LOCATION_LSB,total_offset);
	printString("\r\total_offset: ");
	printHexByte((UCHAR)total_offset>>8);
	printHexByte((UCHAR)total_offset);
	printString("\r\n");
#else
//	printf("no_labels: %d\n",no_labels);
//	printf("total_offset: %d\n",total_offset);
#endif
	i = update_rtparams(i, 0, 0, SHOWN_SENT, 1, RT_RPM);	// first label is at offset 0
	i = update_rtparams(i, 1, 0, SHOWN_SENT, 0, RT_ENGT);
	i = update_rtparams(i, 2, 0, SHOWN_SENT, 0, RT_TRIP);	// first element of offset_array has offset of 2nd label
	i = update_rtparams(i, 3, 0, SHOWN_SENT, 0, RT_TIME);
	i = update_rtparams(i, 4, 0, SHOWN_SENT, 0, RT_AIRT);
	i = update_rtparams(i, 0, 15, SHOWN_SENT, 0, RT_MPH);
	i = update_rtparams(i, 1, 15, SHOWN_SENT, 0, RT_OILP);
	i = update_rtparams(i, 2, 15, SHOWN_SENT, 0, RT_MAP);
	i = update_rtparams(i, 3, 15, SHOWN_SENT, 0, RT_OILT);
	i = update_rtparams(i, 4, 15, SHOWN_SENT, 0, RT_O2);
	i = update_rtparams(i, 5, 0, SHOWN_SENT, 2, RT_AUX1);
	i = update_rtparams(i, 6, 0, SHOWN_SENT, 3, RT_AUX2);
	no_rtparams = i;
// write to the number of rt_params location in eeprom the number of rt_params
#ifndef TEST_WRITE_DATA
	eeprom_update_word((UINT *)NO_RTPARAMS_EEPROM_LOCATION,no_rtparams);
	printString("\r\nno_rtparams: ");
	printHexByte((UCHAR)no_rtparams>>8);
	printHexByte((UCHAR)no_rtparams);
	printString("\r\ndone writing no_rtparams to eeprom\r\n");
	eeprom_update_word((UINT *)MENUSTRUCT_OFFSET_EEPROM_LOCATION_LSB,total_offset);
	printString("\r\total_offset: ");
	printHexByte((UCHAR)total_offset>>8);
	printHexByte((UCHAR)total_offset);
	printString("\r\n");
#else
//	printf("no_rtparams: %d\n",no_rtparams);
//	printf("total_offset: %d\n",total_offset);
#endif

//	menu_structs[i].enabled = enabled;
//	menu_structs[i].fptr = fptr;
//	menu_structs[i].menu = menu;
//	menu_structs[i].label = label;
#if 0
	UCHAR enabled;		// if alt function will replace generic function
	UCHAR fptr;			// which function to call (menu_types)
	UCHAR menu;			// if fptr == 0 then it means goto this menu
	UCHAR label;		// which label to display in legend (labels)
	UCHAR index;		// if > 0 then this is index into sample_data
#endif
// A,B,C,D,#,0
	i = 0;
	no_data_index = 0;
// main menu	0
	i = update_menu_structs(i, 1, _menu_change,		MENU1A,0);
	i = update_menu_structs(i, 1, 0,				MENU1B,0);
	i = update_menu_structs(i, 1, 0,				MENU1C,0);
	i = update_menu_structs(i, 1, 0,				MENU1D,0);
	i = update_menu_structs(i, 1, 0,				MENU2A,0);
	i = update_menu_structs(i, 1, 0,				MENU2B,0);	// offset: 5
// 1a
	i = update_menu_structs(i, 1, _menu_change,		MENU1B,0);	// 6
	i = update_menu_structs(i, 1, 0,				MENU2D,0);
	i = update_menu_structs(i, 1, 0,				MENU1D,0);
	i = update_menu_structs(i, 1, 0,				MENU2A,0);
	i = update_menu_structs(i, 1, 0,				MENU2B,0);
	i = update_menu_structs(i, 1, 0,				MENU2C,0);
// 1b
	i = update_menu_structs(i, 1, _menu_change,		MAIN,0);	// 12
	i = update_menu_structs(i, 1, 0,				MENU1A,0);
	i = update_menu_structs(i, 1, 0,				MENU1B,0);
	i = update_menu_structs(i, 1, 0,				MENU1D,0);
	i = update_menu_structs(i, 1, 0,				MENU2A,0);
	i = update_menu_structs(i, 0, 0,				MENU2B,0);
// 1c
	i = update_menu_structs(i, 1, _exec_choice,		choice0,MENU1C);	// 18
	i = update_menu_structs(i, 1, _test1,			choice1,0);
	i = update_menu_structs(i, 1, _test2,			choice2,0);
	i = update_menu_structs(i, 1, _test3,			choice3,0);
	i = update_menu_structs(i, 1, _test4,			choice4,0);
	i = update_menu_structs(i, 1, 0,				choice5,0);
// 1d
	i = update_menu_structs(i, 1, _do_chkbox,		choice0,MENU1D);	// 24
	i = update_menu_structs(i, 1, 0,				choice1,0);
	i = update_menu_structs(i, 1, 0,				choice2,0);
	i = update_menu_structs(i, 1, 0,				choice3,0);
	i = update_menu_structs(i, 1, 0,				choice4,0);
	i = update_menu_structs(i, 1, 0,				choice5,0);
// 2a
	i = update_menu_structs(i, 1, _non_func,		choice0,MENU2A);	// 30
	i = update_menu_structs(i, 1, 0,				choice1,0);
	i = update_menu_structs(i, 1, 0,				choice2,0);
	i = update_menu_structs(i, 1, 0,				choice3,0);
	i = update_menu_structs(i, 1, 0,				choice4,0);
	i = update_menu_structs(i, 1, 0,				choice5,0);
// 2b
	i = update_menu_structs(i, 1, _exec_choice,		choice6,MENU2B);	// 36
	i = update_menu_structs(i, 1, 0,				choice7,0);
	i = update_menu_structs(i, 1, 0,				choice8,0);
	i = update_menu_structs(i, 1, 0,				choice9,0);
	i = update_menu_structs(i, 1, 0,				choice10,0);
	i = update_menu_structs(i, 1, 0,				choice11,0);
// 2c
	i = update_menu_structs(i, 1, _start_numentry,	testnum0,MENU2C);	// 42
	i = update_menu_structs(i, 1, 0,				MENU1B,0);
	i = update_menu_structs(i, 1, 0,				MENU1C,0);
	i = update_menu_structs(i, 1, 0,				MENU1D,0);
	i = update_menu_structs(i, 1, 0,				MENU2A,0);
	i = update_menu_structs(i, 1, 0,				MENU2B,0);
// 2d
	i = update_menu_structs(i, 1, _menu_change,		MENU1A,0);	// 48
	i = update_menu_structs(i, 1, 0,				MENU1B,0);
	i = update_menu_structs(i, 1, 0,				MENU1C,0);
	i = update_menu_structs(i, 1, 0,				MENU1D,0);
	i = update_menu_structs(i, 1, 0,				MENU2A,0);
	i = update_menu_structs(i, 1, 0,				MENU2B,0);

	i = update_menu_structs(i, 0, 0,				ENDMENU,0);
#if 0
	// menu 1a		6
	i = update_menu_structs(i, 1, 0,		num_entry,	testnum0,1);
	i = update_menu_structs(i, 1, 0,		num_entry,	testnum1,2);
	i = update_menu_structs(i, 1, 0,		num_entry,	testnum2,3);
	i = update_menu_structs(i, 1, 0,		num_entry,	testnum3,4);
	i = update_menu_structs(i, 1, 0,		num_entry,	testnum4,5);
	i = update_menu_structs(i, 1, 0,		num_entry,	testnum5,6);

	// menu 1b		12
	i = update_menu_structs(i, 1, 0,		num_entry,	testnum6,7);
	i = update_menu_structs(i, 1, 0,		num_entry,	testnum7,8);
	i = update_menu_structs(i, 1, 0,		num_entry,	testnum8,9);
	i = update_menu_structs(i, 1, 0,		chkboxes,	chkboxes,0);
	i = update_menu_structs(i, 0, 0, 		MAIN,		MAIN,0);
	i = update_menu_structs(i, 0, 0, 		MAIN,		MAIN,0);

	// menu 1c		18
	i = update_menu_structs(i, 1, 0,		num_entry,	num_entry,0);
	i = update_menu_structs(i, 1, 0,		MENU1B,		MENU1B,0);
	i = update_menu_structs(i, 1, 0,		MENU1C,		MENU1C,0);
	i = update_menu_structs(i, 1, 0,		MAIN,		MAIN,0);
	i = update_menu_structs(i, 0, 0, 		MAIN,		MAIN,0);
	i = update_menu_structs(i, 0, 0, 		MAIN,		MAIN,0);

	// menu 1d		24
	i = update_menu_structs(i, 1, 0,		MENU1A,		MENU1A,0);
	i = update_menu_structs(i, 1, 0,		MENU1B,		MENU1B,0);
	i = update_menu_structs(i, 1, 0,		MAIN,		MAIN,0);
	i = update_menu_structs(i, 0, 0, 		MAIN,		MAIN,0);
	i = update_menu_structs(i, 0, 0, 		MAIN,		MAIN,0);
	i = update_menu_structs(i, 0, 0, 		MAIN,		MAIN,0);

	// num_entry	30
	i = update_menu_structs(i, 1, entr, 	0, 			entr,0);
	i = update_menu_structs(i, 1, back, 	0,  		back,0);
	i = update_menu_structs(i, 1, esc, 		0, 			esc,0);
	i = update_menu_structs(i, 1, 0, 		0, 			0,0);
	i = update_menu_structs(i, 0, 0, 		0, 			0,0);
	i = update_menu_structs(i, 0, 0, 		MAIN,		MAIN,0);

	// alnum entry	36
	i = update_menu_structs(i, 1, alnum_ent, 0, 		alnum_ent,0);		// A
	i = update_menu_structs(i, 1, cur_for, 	0, 			cur_for,0);			// B
	i = update_menu_structs(i, 1, next, 	0, 			next,0);			// C
	i = update_menu_structs(i, 1, caps, 	0,  		caps,0);			// D
	i = update_menu_structs(i, 1, spec, 	0, 			spec,0);			// #
	i = update_menu_structs(i, 1, small, 	0,			small,0);			// 0

	// checkboxes	42
	i = update_menu_structs(i, 1, ckup, 	0, 			ckup,0);			// A
	i = update_menu_structs(i, 1, ckdown, 	0, 			ckdown,0);			// B
	i = update_menu_structs(i, 1, cktoggle, 0, 			cktoggle,0);		// C
	i = update_menu_structs(i, 1, ckenter, 	0,  		ckenter,0);			// D
	i = update_menu_structs(i, 0, ckesc, 	0, 			ckesc,0);			// #
	i = update_menu_structs(i, 0, 0, 		0, 			0,0);				// 0

	// menu 2a		48
	i = update_menu_structs(i, 1, 0,		MENU1A,		MENU1A,0);
	i = update_menu_structs(i, 1, 0,		MENU1B,		MENU1B,0);
	i = update_menu_structs(i, 0, 0, 		0, 			0,0);
	i = update_menu_structs(i, 0, 0, 		0, 			0,0);
	i = update_menu_structs(i, 0, 0, 		0, 			0,0);
	i = update_menu_structs(i, 0, 0, 		0,			0,0);

	// 2b			54
	i = update_menu_structs(i, 1, 0,		MENU1A,		MENU1A,0);
	i = update_menu_structs(i, 1, 0,		MENU1B,		MENU1B,0);
	i = update_menu_structs(i, 0, 0, 0, 0,0);
	i = update_menu_structs(i, 0, 0, 0, 0,0);
	i = update_menu_structs(i, 0, 0, 0, 0,0);
	i = update_menu_structs(i, 0, 0, 0, 0,0);
#endif
	no_menu_structs = i;
	// now we can write to the NO_MENUS_EEPROM_LOCATION in eeprom
#ifndef	TEST_WRITE_DATA
	eeprom_update_word((UINT *)NO_MENUS_EEPROM_LOCATION,no_menu_structs);
	printString("\r\nno_menu_structs: ");
	printHexByte((UCHAR)no_menu_structs>>8);
	printHexByte((UCHAR)no_menu_structs);
	printString("\r\ndone writing prompts to eeprom\r\n");
	printString("\r\total_offset: ");
	printHexByte((UCHAR)total_offset>>8);
	printHexByte((UCHAR)total_offset);
	printString("\r\n");
#else
//	printf("no_menu_structs: %d\n",no_menu_structs);
//	printf("total_offset: %d\n",total_offset);
#endif
	return 0;
}
//******************************************************************************************//
//*********************************** update_menu_labels ***********************************//
//******************************************************************************************//
int update_menu_labels(int index, char *ramstr)
{
	int len;
	len = strlen(ramstr);
	len = (len > MAX_LABEL_LEN?MAX_LABEL_LEN:len);
	len++;
	label_offsets[index] = len;
	total_offset += len;
//	printf("len = %d total_offset = %d\n",len,total_offset);
//	printf("len=%d i= %d\n",len,index);
// if TEST_WRITE_DATA then just copy to array of strings
#ifndef TEST_WRITE_DATA
    eeprom_update_block(ramstr, eepromString+len, len);
#endif
	if(index > NUM_MENU_LABELS)
	{
		printf("%d no labels > than storage space\n",index);
		exit(-1);	
	}
	strncpy(menu_labels[index],ramstr,len);
	index++;
	return index;
}
//******************************************************************************************//
//************************************ update_rt_labels ************************************//
//******************************************************************************************//
int update_rt_labels(int index, char *ramstr)
{
	int len;
	len = strlen(ramstr);
	len = (len > MAX_LABEL_LEN?MAX_LABEL_LEN:len);
	len++;
	label_offsets[index] = len;
	total_offset += len;
//	printf("len = %d total_offset = %d\n",len,total_offset);
//	printf("len=%d i= %d\n",len,index);
// if TEST_WRITE_DATA then just copy to array of strings
#ifndef TEST_WRITE_DATA
    eeprom_update_block(ramstr, eepromString+len, len);
#endif
	strncpy(rt_labels[index],ramstr,len);
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
#ifndef TEST_WRITE_DATA
    eeprom_update_block(&rt_params[i], eepromString+total_offset, sizeof(RT_PARAM));
#endif
	total_offset += sizeof(RT_PARAM);
//	printf("total_offset = %d\n",total_offset);
	i++;
	return i;
}
//******************************************************************************************//
//********************************* update_menu_structs*************************************//
//******************************************************************************************//
//int update_menu_structs(int i, char *label, UCHAR row, UCHAR col, UCHAR choice, UCHAR ch_type, UCHAR type)
int update_menu_structs(int i, UCHAR enabled, UCHAR fptr, UCHAR menu, UCHAR index)
{
/*
	UCHAR enabled;		// if alt function will replace generic function
	UCHAR key;			// which keypress applies
	UCHAR fptr;			// which function to call (menu_types)
	UCHAR menu;			// if fptr == 0 then it means goto a this menu
	UCHAR label;			// which label to display in legend (labels)
	UCHAR index;		// if > 0 then this is index into sample_data
*/
	menu_structs[i].enabled = enabled;
//	menu_structs[i]._index = i;
	if(enabled == 1)
	{
		menu_structs[i].fptr = fptr;
		menu_structs[i].menu = menu;
//		menu_structs[i].label = label;
		menu_structs[i].index = index;
		if(index > 0)
			no_data_index++;
	}
	else
	{
		menu_structs[i].fptr = 0;
		menu_structs[i].menu = 0;
//		menu_structs[i].label = label;
		menu_structs[i].index = 0;
	}
#ifndef TEST_WRITE_DATA
    eeprom_update_block(&menu_structs, eepromString+total_offset, sizeof(MENU_FUNC_STRUCT));
#endif
	total_offset += sizeof(MENU_FUNC_STRUCT);
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
		label_offsets[i] = j;	// fill label_offsets array so we can lookup and display all the labels in main_burn.c
	}

	menu_struct_offset = (UINT)eeprom_read_byte((UCHAR*)MENUSTRUCT_OFFSET_EEPROM_LOCATION_LSB);	// read lsb
	temp = eeprom_read_byte((UCHAR*)MENUSTRUCT_OFFSET_EEPROM_LOCATION_MSB);		// read msb
	temp2 = (UINT)temp;
	menu_struct_offset |= (temp2 << 8);		// shift msb over and bit-or with lsb
	printString("menu_struct_offset: ");
	printHexByte((UCHAR)(menu_struct_offset>>8));
	printHexByte((UCHAR)menu_struct_offset);
	printString("\r\n");

	printString("done reading eeprom\r\n");
	return 0;
}
#endif