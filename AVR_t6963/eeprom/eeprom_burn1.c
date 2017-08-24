// eeprom_burn.c - get used by test_write_data/test_write_data.c and eeprom/main_burn.c
// TEST_WRITE_DATA is define when compiled in test_write_data
#ifndef EEPROM_BURN

#include "../sfr_helper.h"
#include <ncurses.h>
#include "../main.h"
#include "../pic_main.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
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

    i = 0;
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
// when adding up to <end> must modify get_fptr_label in PIC_menu.c (currently 25)
	i = 0;
	i = update_menu_labels(i,"home\0");
	i = update_menu_labels(i,"MENU1a\0");
	i = update_menu_labels(i,"MENU1b\0");
	i = update_menu_labels(i,"MENU1c\0");
	i = update_menu_labels(i,"MENU1d\0");
	i = update_menu_labels(i,"MENU1e\0");
	i = update_menu_labels(i,"MENU2a\0");
	i = update_menu_labels(i,"MENU2B\0");
	i = update_menu_labels(i,"MENU2c\0");
	i = update_menu_labels(i,"MENU2d\0");

	i = update_menu_labels(i,"up\0");
	i = update_menu_labels(i,"down\0");
	i = update_menu_labels(i,"toggle\0");
	i = update_menu_labels(i,"enter\0");
	i = update_menu_labels(i,"esc\0");

	i = update_menu_labels(i,"enter\0");
	i = update_menu_labels(i,"caps\0");
	i = update_menu_labels(i,"small\0");
	i = update_menu_labels(i,"spec\0");
	i = update_menu_labels(i,"next\0");
	i = update_menu_labels(i,"forward\0");
	i = update_menu_labels(i,"back\0");
	i = update_menu_labels(i,"escape\0");

	i = update_menu_labels(i,"test 0\0");
	i = update_menu_labels(i,"test 1\0");
	i = update_menu_labels(i,"test 2\0");
	i = update_menu_labels(i,"test 3\0");
	i = update_menu_labels(i,"test 4\0");
	i = update_menu_labels(i,"test 5\0");
	i = update_menu_labels(i,"test 6\0");
	i = update_menu_labels(i,"test 7\0");
	i = update_menu_labels(i,"test 8\0");

	i = update_menu_labels(i,"choice0\0");
	i = update_menu_labels(i,"choice1\0");
	i = update_menu_labels(i,"choice2\0");
	i = update_menu_labels(i,"choice3\0");
	i = update_menu_labels(i,"choice4\0");
	i = update_menu_labels(i,"choice5\0");
	i = update_menu_labels(i,"choice6\0");
	i = update_menu_labels(i,"choice7\0");
	i = update_menu_labels(i,"choice8\0");
	i = update_menu_labels(i,"choice9\0");
	i = update_menu_labels(i,"choice10\0");
	i = update_menu_labels(i,"choice11\0");
	i = update_menu_labels(i,"blank\0");
	no_menu_labels = i;
//	printf("no_menu_labels: %d\n",no_menu_labels);
//	getch();
	no_func_labels = 0;
	i = update_menu_labels(i,"mchange\0");
	i = update_menu_labels(i,"exec_ch\0");
	i = update_menu_labels(i,"chkbox\0");
	i = update_menu_labels(i,"non_fnc\0");
	i = update_menu_labels(i,"edit val\0");
	no_func_labels = i-no_menu_labels;
//	printf("no_func_labels: %d\n",no_func_labels);
//	getch();
	i = 0;
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

	i = 0;
	no_data_index = 0;
// main menu	0
//												'A' 	'B'		'C'		'D'		'#'		'0'
	i = update_menu_structs(i, _menu_change, 	MENU1A, MENU1B, MENU1C, MENU1D, MENU1E, MENU2A,  MAIN);
// 1a
	i = update_menu_structs(i, _menu_change,	MENU1B, MENU1C, MENU1D, MENU2A, MENU2B, MENU2C, MENU1A);
// 1b
	i = update_menu_structs(i, _menu_change,	MAIN,   MENU1A, MENU1B, MENU1D, MENU2A, MENU2B, MENU1B);
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
	i = update_menu_structs(i, _do_numentry, 	forward, back, entr, esc, blank, blank, MENU2C);

	no_menu_structs = i;
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
//	label_offsets[index] = len;
	total_offset += len;
//	printf("len = %d total_offset = %d\n",len,total_offset);
//	printf("len=%d i= %d\n",len,index);
// if TEST_WRITE_DATA then just copy to array of strings
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
//	label_offsets[index] = len;
	total_offset += len;
//	printf("len = %d total_offset = %d\n",len,total_offset);
//	printf("len=%d i= %d\n",len,index);
// if TEST_WRITE_DATA then just copy to array of strings
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
	total_offset += sizeof(RT_PARAM);
//	printf("total_offset = %d\n",total_offset);
	i++;
	return i;
}
//******************************************************************************************//
//********************************* update_menu_structs*************************************//
//******************************************************************************************//
//int update_menu_structs(int i, char *label, UCHAR row, UCHAR col, UCHAR choice, UCHAR ch_type, UCHAR type)
int update_menu_structs(int i, UCHAR fptr, UCHAR menu0, UCHAR menu1, UCHAR menu2, UCHAR menu3,
			UCHAR menu4, UCHAR menu5, UCHAR index)
{
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
	total_offset += sizeof(MENU_FUNC_STRUCT);
//	printf("total_offset = %d\n",total_offset);
	i++;
	return i;
}
#endif
