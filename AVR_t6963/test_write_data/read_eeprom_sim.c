// test_write_data.c - used to test the data protocol between the AVR and PIC24 where the
// data string is sent with FF,FE,FD.. and the next 3 bytes are shifted so they don't
// have the high bits set - displays in ncurses window
// when this is called with write params - 'w'... it simulates the PIC24
// when called with just the 'r' param, it simulates the AVR (calls do_read below)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <ncurses.h>
#include "../sfr_helper.h"
#include "../main.h"
#include "../pic_main.h"
#include "../t6963.h"

#define FALSE 0
#define TRUE 1
#define LEN 200
// really cranking
#define TIME_DELAY 200000
// readable
//#define TIME_DELAY 300000

//******************************************************************************************//
//****************************************** main ******************************************//
//******************************************************************************************//
int main(int argc, char *argv[])
{
	int fd;
	int type = 0;
	int i,j,k;
	WINDOW *win;
	MENU_FUNC_STRUCT mf;
	MENU_FUNC_STRUCT *pmf = &mf;
	int display_offset = 1;
	char temp_label[MAX_LABEL_LEN];

	burn_eeprom();
	// reserve an extra sample_data space for in case of 'escape'
	initscr();			/* Start curses mode 		*/
	clear();
	noecho();
//	nodelay(stdscr,TRUE);
	raw();				/* Line buffering disabled	*/
	cbreak();	/* Line buffering disabled. pass on everything */
	win = newwin(60, 65, 0,0);
	keypad(win, TRUE);
//	nodelay(win, TRUE);
	box(win,0,0);

// read	- simulate the AVR
//	memset(cur_param_string,0,sizeof(cur_param_string));
//	for(i = 0;i < NUM_UCHAR_PARAMS;i++)
//		cur_param_string[i] = i;
	j = 0;
	k = 0;
// comment out the next #if 0/#endif to test
	goffset = 0;
	get_label_offsets();
//#if 0
	mvwprintw(win, display_offset,2,"test  ");
	wrefresh(win);
	getch();
//	for(i = total_no_menu_labels;i < total_no_menu_labels+no_func_labels; i++)
//		mvwprintw(win, display_offset+i-total_no_menu_labels,2,"%d: %s  ",i-total_no_menu_labels,menu_labels[i]);
//	wrefresh(win);
//	getch();
	for(i = 0;i < 1023;i++)
	{
		if(eeprom_sim[i] < 0x7e && eeprom_sim[i] > 0x21)
			mvwprintw(win, display_offset+j, 2+k,"%c",eeprom_sim[i]);
		else if(eeprom_sim[i] == 0)	mvwprintw(win, display_offset+j, 2+k," ");
		else mvwprintw(win, display_offset+j, 2+k,"_");
		if(++k > 30)
		{
			k = 0;
			++j;
		}
	}

	wrefresh(win);
	getch();

	j = 1;
	k = 0;

	for(i = 0;i < 50;i++)
		mvwprintw(win, display_offset+i,2,"                                                 ");

	for(i = 0;i < 1023;i++)
	{
		mvwprintw(win, display_offset+j, 8+(k*3),"%2x ",eeprom_sim[i]);
		if(++k > 15)
		{
			k = 0;
			++j;
			mvwprintw(win, display_offset+j,2,"%d: ",i+1);
		}
	}

/*
	wrefresh(win);
	getch();

	for(i = 0;i < 50;i++)	// don't know why this doesn't work
		mvwprintw(win, display_offset+i,2,"                                                 ");
	mvwprintw(win, display_offset,2,"no_menu_structs: %d start_menu_structs: %d   ",no_menu_structs,start_menu_structs);
	j = 1;
	k = 0;
	for(i = start_menu_structs;i < (start_menu_structs+(no_menu_structs*sizeof(MENU_FUNC_STRUCT)));i++)
	{
		mvwprintw(win, display_offset+j,2+(k*3),"%2d ",eeprom_sim[i]);
		if(++k > 25)
		{
			k = 0;
			++j;
		}
	}
*/
	wrefresh(win);
	getch();

	j = 2;
	k = 0;
	for(i = 0;i < 70;i++)
		mvwprintw(win, display_offset+i,2,"                                                     ");
	mvwprintw(win, display_offset,2,"no_menu_structs: %d start_menu_structs: %d   ",no_menu_structs,start_menu_structs);
	for(i = 0;i < no_menu_structs;i++)
	{
		memcpy(&mf,eeprom_sim+(sizeof(MENU_FUNC_STRUCT)*i)+start_menu_structs,sizeof(MENU_FUNC_STRUCT));
		mvwprintw(win, display_offset+j,2,
			"%2d %2d %2d %2d %2d %2d %2d %2d",pmf->fptr,pmf->menus[0],pmf->menus[1],
				pmf->menus[2],pmf->menus[3],pmf->menus[4],pmf->menus[5],pmf->index);
		j++;
	}

	wrefresh(win);
	getch();
	for(i = 0;i < 50;i++)
		mvwprintw(win, display_offset+i,2,"                                                 ");
	mvwprintw(win, display_offset,2,"menu_offset: %d  rt_params_offset: %d ",menu_offset,rt_params_offset);
	k = 0;
	j = 1;
	for(i = RT_PARAMS_OFFSET_EEPROM_LOCATION;i < RT_PARAMS_OFFSET_EEPROM_LOCATION+(sizeof(RT_PARAM)*no_rtparams);i++)
	{
		mvwprintw(win, display_offset+j,2+(k*3),"%x ",eeprom_sim[i]);
		if(++k > 4)
		{
			k = 0;
			j++;
		}
	}
	wrefresh(win);
	getch();

	for(i = 0;i < 50;i++)
		mvwprintw(win, display_offset+i,2,"                                                       ");
	j = 0;
	for(i = NO_MENU_LABELS_EEPROM_LOCATION;i < NO_MENUS_EEPROM_LOCATION;i++)
	{
		mvwprintw(win, display_offset,2+(j*3),"%d ", eeprom_sim[i]);
		j++;
	}
	mvwprintw(win, display_offset+1,2,"%d %d %d ",no_menu_labels,no_rt_labels,no_rtparams);
	wrefresh(win);
	getch();

	mvwprintw(win, display_offset,2,"                                            ");
	mvwprintw(win, display_offset,2,"no_menu_labels: %d no_rt_labels %d  ",no_menu_labels,no_rt_labels);

	k = j = 0;
	for(i = 0;i < no_menu_labels+no_rt_labels;i++)
	{
		mvwprintw(win, display_offset+j+1,2+(k*5),"%4d ",label_offsets[i]);
		if(++k > 11)
		{
			k = 0;
			j++;
		}

	}

	for(i = 0;i < no_menu_labels;i++)
	{
		get_label(i,temp_label);
		mvwprintw(win, display_offset+i+12,2,"%d: %s  %d",i,temp_label,label_offsets[i]);
	}

	wrefresh(win);
	getch();

	for(i = 0;i < NUM_LABELS+NUM_RT_LABELS;i++)
		mvwprintw(win, display_offset+i,2,"                                                             ");
	for(i = 0;i < no_menu_structs;i++)
	{
		mvwprintw(win, display_offset+i,2,"%d ",menu_structs[i].fptr);
		for(j = 0;j < 6;j++)
		{
			get_label(menu_structs[i].menus[j],temp_label);
			mvwprintw(win, display_offset+i,2+(j*10),"%s ", temp_label);
		}
	}
	wrefresh(win);
	getch();

	delwin(win);
	clrtoeol();
	refresh();
	endwin();
	exit(1);
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

    memset(eeprom_sim,0,EEPROM_SIZE);
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

//												'A' 	'B'		'C'		'D'		'#'		'0'
	i = update_menu_structs(i, _menu_change, 	MENU1A, MENU1B, MENU1C, MENU1D, MENU2A, MENU2B,  MAIN);
// 1a
	i = update_menu_structs(i, _menu_change,	MENU2C, MENU2D, MENU2E, MENU3A, MENU3B, MENU1B, MENU1A);
// 1b
	i = update_menu_structs(i, _menu_change,	MAIN,   MENU2D, MENU1B, MENU1D, MENU2A, MENU2B, MENU1B);
// 1c
	i = update_menu_structs(i, _do_chkbox, 		ckup, ckdown, cktoggle, ckenter, ckesc, blank, MENU1C);
// 1d
	i = update_menu_structs(i, _do_chkbox, 		ckup, ckdown, cktoggle, ckenter, ckesc, blank, MENU1D);
// 1e
	i = update_menu_structs(i, _non_func,		blank, blank, blank,   blank, blank, blank, MENU1E);
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

	no_menu_structs = i;
	return 0;
}
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
	if(index > 0)
		no_data_index++;

//	memcpy(eeprom_sim+total_offset+start_menu_structs,&menu_structs[i], len);
//	strncpy(menu_labels[index],ramstr,len);
	total_offset += len;
	i++;
	return i;
}
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


