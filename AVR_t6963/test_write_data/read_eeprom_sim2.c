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
UCHAR *peeprom_sim;
static int update_labels2(int index, char *ramstr);
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
	int fp;
	peeprom_sim = eeprom_sim;
	char filename[20];
	char fpc[10];
	int res, size, start_addr;

	// reserve an extra sample_data space for in case of 'escape'

	printf("argc: %d\n",argc);
	printf("argv: %s\n",argv[1]);

	if(argc < 2)
	{
		strcpy(filename,"eeprom.bin\0");
		if(access(filename,F_OK) != -1)
		{
			fp = open((const char *)filename, O_RDWR);
			if(fp < 0)
			{
				printf("can't open eeprom.bin for reading\n");
				exit(1);
			}
			res = 0;
			lseek(fp,0,SEEK_SET);
			res = read(fp,eeprom_sim,EEPROM_SIZE);
		}
	}else
	{
		strcpy(filename,argv[1]);
		fp = open((const char *)filename, O_RDWR);
		if(fp < 0)
		{
			printf("can't open %s for reading\n",argv[1]);
			exit(1);
		}
		res = 0;
		lseek(fp,0,SEEK_SET);
		res = read(fp,eeprom_sim,EEPROM_SIZE);
		close(fp);
		printf("reading part into eeprom_sim: %d %d %d  \n",res,size,start_addr);
	}
	j = k = 0;

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
	mvwprintw(win, 2,2,"reading part into eeprom_sim: %d %d %d  ",res,size,start_addr);
	wrefresh(win);
	getch();
//	for(i = total_no_menu_labels;i < total_no_menu_labels+no_func_labels; i++)
//		mvwprintw(win, display_offset+i-total_no_menu_labels,2,"%d: %s  ",i-total_no_menu_labels,menu_labels[i]);
//	wrefresh(win);
//	getch();
#if 0
	strcpy(filename,"test.txt\0");
	fp = open((const char *)filename, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRUSR | S_IRGRP | S_IWGRP);
	if(fp < 0)
	{
		mvwprintw(win, LAST_ROW_DISP,2,"can't open file for writing");
		wrefresh(win);
		getch();
	}
#endif

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
#if 0
		if(eeprom_sim[i] == 0)
		{
			sprintf(fpc,"0, ",eeprom_sim[i]);
			write(fp,&fpc[0],2);
		}
		else if(eeprom_sim[i] < 0x10)
		{
			sprintf(fpc,"0x%x, ",eeprom_sim[i]);
			write(fp,&fpc[0],5);
		}
		else
		{
			sprintf(fpc,"0x%2x, ",eeprom_sim[i]);
			write(fp,&fpc[0],6);
		}
#endif
	}
	close(fp);

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

	wrefresh(win);
	getch();

	j = 2;
	k = 0;
	for(i = 0;i < 70;i++)
		mvwprintw(win, display_offset+i,2,"                                                     ");
	mvwprintw(win, display_offset,2,"no_menu_structs: %d    ",no_menu_structs);
	for(i = 0;i < no_menu_structs;i++)
	{
		memcpy(&mf,&menu_structs[i],sizeof(MENU_FUNC_STRUCT));
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

