// do_read.c - called from test_write_data.c
// this simulates the AVR reading from the PIC24
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/time.h>
#include <ncurses.h>
#include "../sfr_helper.h"
#include "../main.h"
#include "../t6963.h"

//******************************************************************************************//
//**************************************** do_read *****************************************//
//******************************************************************************************//
// do_read simulates the AVR
void do_read(WINDOW *win, int fd, int display_offset)
{
	UCHAR ch;
	char param_string[10];
	int i;
	char ready;

	init_list();
	display_labels();
	wrefresh(win);
	i = 0;
	ch = 0;
	while(TRUE)
	{
		read(fd,&ch,1);
		// get_key will filter out and process any keypresses in menus.c
		ch = get_key(ch);
		mvwprintw(win, DISP_OFFSET+6, 20,"ch: %x       ",ch);
		wrefresh(win);
//		write(fd,&ch,1);
		// ignore any chars with highbit set - they are processed by get_key()

		if(ch != 0xff)
		{
			mod_data_ready = 0;
		}			
		ch = 0;
		wrefresh(win);
	// this displays the RT params on the screen at their positions according to the rt_params struct
	//			mvwprintw(win, display_offset+17,5,"%d ",no_rtparams);
/*
		for(i = 0;i < no_rtparams;i++)
		{
			if(rt_params[i].shown == SHOWN_SENT)
			{
				if(rt_params[i].type == current_param)
				{
					GDispStringAt(rt_params[i].row,rt_params[i].col+9,"      ");
				}

				if(rt_params[i].type == current_param && (rt_params[i].shown == SHOWN_SENT))
				{
					GDispStringAt(rt_params[i].row,rt_params[i].col+10,param_string);
				}
			}
//				mvwprintw(win, display_offset+30, 2,"row: %d col: %d  ",rt_params[i].row,rt_params[i].col);
		}
*/
	}	// end of while(1)
}

