/****************************************************************************
 * Copyright (c) 1998-2010,2011 Free Software Foundation, Inc.              *
 *                                                                          *
 * Permission is hereby granted, free of charge, to any person obtaining a  *
 * copy of this software and associated documentation files (the            *
 * "Software"), to deal in the Software without restriction, including      *
 * without limitation the rights to use, copy, modify, merge, publish,      *
 * distribute, distribute with modifications, sublicense, and/or sell       *
 * copies of the Software, and to permit persons to whom the Software is    *
 * furnished to do so, subject to the following conditions:                 *
 *                                                                          *
 * The above copyright notice and this permission notice shall be included  *
 * in all copies or substantial portions of the Software.                   *
 *                                                                          *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS  *
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF               *
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.   *
 * IN NO EVENT SHALL THE ABOVE COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,   *
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR    *
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR    *
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.                               *
 *                                                                          *
 * Except as contained in this notice, the name(s) of the above copyright   *
 * holders shall not be used in advertising or otherwise to promote the     *
 * sale, use or other dealings in this Software without prior written       *
 * authorization.                                                           *
 ****************************************************************************/
/****************************************************************************

NAME
   (borrowed from:) ncurses.c --- ncurses library exerciser

SYNOPSIS
   ncurses

DESCRIPTION
   An interactive test module for the ncurses library.

AUTHOR
Author: Eric S. Raymond <esr@snark.thyrsus.com> 1993
Thomas E. Dickey (beginning revision 1.27 in 1996).

$Id: ncurses.c,v 1.365 2011/01/22 19:48:33 tom Exp $

***************************************************************************/
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <assert.h>
#include "test.priv.h"
#include <form.h>
#include "form_sup.h"

int text_entry(char *str, char *ret_str, int which)
{
	WINDOW *win;
	FORM *form;
	FIELD *f[16];
	int finished = 0, c;
	unsigned n = 0;
	char tempx[50];
	char *buffer;
	char *fp;
	int i;

	refresh();

/* describe the form */
	memset(f, 0, sizeof(f));
	f[n++] = make_label(STARTY2-2, 15, "Text Entry");

	if(which == 1)
	{
		f[n++] = make_label(STARTY2, STARTX, str);
		f[n++] = make_field(STARTY2+1, STARTX, 1, 20, 0);
		set_field_type(f[n-1], TYPE_ALNUM,20);
		init_edit_field(f[n-1],"",1);
	}

	f[n] = (FIELD *) 0;

	if ((form = new_form(f)) != 0)
	{
		display_form(form);

		win = form_win(form);
//		raw();
//		nonl();									  /* lets us read ^M's */
		while (!finished)
		{
			switch (form_driver(form, c = form_virtualize(form, win)))
			{
				case E_OK:
					break;
				case E_UNKNOWN_COMMAND:
					finished = my_form_driver(form, c);
//					sprintf(tempx,"%d",finished);
//					MvAddStr(LINES-3, 2, tempx);
//					refresh();
					break;
				default:
					beep();
					break;
			}
			
		}
//		mvprintw(LINES-2,2,"finished: %d  ",finished);
//		refresh();

		if(finished == 2)
		{
			erase_form(form);
			free_form(form);
			free_field(f[c]);
			mvprintw(LINES,2,"escape from demo_forms");
			return 0;
		}
		buffer = field_buffer(f[2],0);
		fp = buffer;
		for(i = 0;i < 20;i++)
		{
			if(*fp == 0x20)
				*fp = 0;
			fp++;
		}
		buffer[19] = 0;
		strcpy(ret_str,buffer);
		erase_form(form);
		free_form(form);
	}
	for (c = 0; f[c] != 0; c++)
		free_field(f[c]);

	return 1;
}

#ifndef NOMAIN

int main(void)
{
	I_DATA idata;
	I_DATA *pid;
	pid = &idata;

	O_DATA odata;
	O_DATA *pod;
	pod = &odata;

	strcpy(idata.label,"test input\0");
	idata.port = 1;
	idata.affected_output = 3;
	odata.type = 4;
	strcpy(odata.label,"test output\0");
	odata.port = 2;
	odata.onoff = 3;
	initscr();
	noraw();
	cbreak();
	noecho();

	if (has_colors())
	{
		start_color();
		init_pair(1, COLOR_RED, COLOR_BLACK);
		init_pair(2, COLOR_BLUE, COLOR_WHITE);
		init_pair(3, COLOR_WHITE, COLOR_BLUE);
		init_pair(4, COLOR_GREEN, COLOR_BLACK);
		init_pair(5, COLOR_CYAN, COLOR_BLACK);
	}
//    status = newwin(5, COLS, LINES - 10, 0);
//	demo_forms4((void*)pid,1);
	demo_forms((void*)pod,0);
	endwin();
}
#endif
