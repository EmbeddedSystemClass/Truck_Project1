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
#include "../queue/illist_threads_rw.h"
#include "../queue/ollist_threads_rw.h"
#include "../ioports.h"
#include "form_sup.h"

void demo_forms(void *curr, int which)
{
	WINDOW *win;
	FORM *form;
	FIELD *f[12];
	I_DATA *pid;
	O_DATA *pod;
//	 *secure;
//	FIELDTYPE *fty_middle = new_fieldtype(mi_field_check, mi_char_check);
//	FIELDTYPE *fty_passwd = new_fieldtype(pw_field_check, pw_char_check);
	int finished = 0, c;
	unsigned n = 0;
	char tempx[50];
	char *buffer;
	char *fp;
	int i;

	if(which == 1)
		pid = (I_DATA*)curr;
	else if(which == 0)
		pod = (O_DATA*)curr;
/*
	move(18, 0);
	addstr("Defined edit/traversal keys:   ^Q/ESC- exit form\n");
	addstr("^N   -- go to next field       ^P  -- go to previous field\n");
	addstr("Home -- go to first field      End -- go to last field\n");
	addstr("^L   -- go to field to left    ^R  -- go to field to right\n");
	addstr("^U   -- move upward to field   ^D  -- move downward to field\n");
	addstr("^W   -- go to next word        ^B  -- go to previous word\n");
	addstr("^S   -- go to start of field   ^E  -- go to end of field\n");
	addstr("^H   -- delete previous char   ^Y  -- delete line\n");
	addstr("^G   -- delete current word    ^C  -- clear to end of line\n");
	addstr("^K   -- clear to end of field  ^X  -- clear field\n");
	addstr("Arrow keys move within a field as you would expect. ^] toggles overlay mode.");
*/
//	MvAddStr(4, 57, "Forms Entry Test");

	refresh();

/* describe the form */
	memset(f, 0, sizeof(f));
	f[n++] = make_label(0, 15, "Sample Form");

	if(which == 1)
	{
		f[n++] = make_label(2, 0, "Label");
		f[n++] = make_field(3, 0, 1, 20, 0);
		set_field_type(f[n-1], TYPE_ALNUM,4);
		init_edit_field(f[n-1],pid->label,1);
	//	set_field_buffer(f[n-1],0,pid->label);

		f[n++] = make_label(5, 0, "Port");
		f[n++] = make_field(6, 0, 1, 4, 0);
		set_field_type(f[n - 1], TYPE_INTEGER,1,0,255);
		memset(tempx,0,sizeof(tempx));
		sprintf(tempx,"%d",pid->port);
		init_edit_field(f[n-1],tempx,1);
		field_opts_off(f[n-1],O_ACTIVE);

		f[n++] = make_label(5, 7, "Output");
		f[n++] = make_field(6, 7, 1, 4, 0);
		set_field_type(f[n - 1], TYPE_INTEGER,1,0,255);
		memset(tempx,0,sizeof(tempx));
		sprintf(tempx,"%d",pid->affected_output);
		init_edit_field(f[n-1],tempx,1);

		f[n++] = make_label(5, 14, "Type");
		f[n++] = make_field(6, 14, 1, 4, 0);
		set_field_type(f[n - 1], TYPE_INTEGER,1,0,255);
		memset(tempx,0,sizeof(tempx));
		sprintf(tempx,"%d",pid->type);
		init_edit_field(f[n-1],tempx,1);

		f[n++] = make_label(5, 21, "Inverse");
		f[n++] = make_field(6, 21, 1, 4, 0);
		set_field_type(f[n - 1], TYPE_INTEGER,1,0,255);
		memset(tempx,0,sizeof(tempx));
		sprintf(tempx,"%d",pid->inverse);
		init_edit_field(f[n-1],tempx,1);
	}else if (which == 0)
	{
		f[n++] = make_label(2, 0, "Label");
		f[n++] = make_field(3, 0, 1, 20, 0);
		set_field_type(f[n-1], TYPE_ALNUM,4);
		init_edit_field(f[n-1],pod->label,1);
	//	set_field_buffer(f[n-1],0,pid->label);

		f[n++] = make_label(5, 0, "Port");
		f[n++] = make_field(6, 0, 1, 4, 0);
		set_field_type(f[n - 1], TYPE_INTEGER,1,0,255);
		memset(tempx,0,sizeof(tempx));
		sprintf(tempx,"%d",pod->port);
		init_edit_field(f[n-1],tempx,1);
		field_opts_off(f[n-1],O_ACTIVE);

		f[n++] = make_label(5, 7, "OnOff");
		f[n++] = make_field(6, 7, 1, 4, 0);
		set_field_type(f[n - 1], TYPE_INTEGER,1,1,0);
		memset(tempx,0,sizeof(tempx));
		sprintf(tempx,"%d",pod->onoff);
		init_edit_field(f[n-1],tempx,1);
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
/*
					MvAddStr(50, 1, field_buffer(f[2], 0));
					MvAddStr(51, 1, field_buffer(f[4], 0));
					MvAddStr(52, 1, field_buffer(f[6], 0));
					MvAddStr(53, 1, field_buffer(f[8], 0));
					MvAddStr(54, 1, field_buffer(f[10], 0));
					print_req_type(w, c,55, 1);
					sprintf(tempx,"test: %d  %d  ",c,n);
					MvAddStr(56,1,tempx);
					MvAddStr(57,1,"              ");
					refresh();
*/
					break;
				case E_UNKNOWN_COMMAND:
					finished = my_form_driver(form, c);
//					print_req_type(w, c,57, 1);
//					MvAddStr(55,1,"              ");
//					MvAddStr(56,1,"              ");
					break;
				default:
					beep();
					break;
			}
		}
		if(which == 1)
		{
//			buffer = field_buffer(f[4],0);
//			pid->port = atoi(buffer);

			buffer = field_buffer(f[6],0);
			pid->affected_output = atoi(buffer);

			buffer = field_buffer(f[8],0);
			pid->type = atoi(buffer);

			buffer = field_buffer(f[10],0);
			pid->inverse = atoi(buffer);

			buffer = field_buffer(f[2],0);
			fp = buffer;
			for(i = 0;i < 20;i++)
			{
				if(*fp == 0x20)
					*fp = 0;
				fp++;
			}
			buffer[19] = 0;
			strcpy(pid->label,buffer);
			curr = (void*)pid;
		}else if(which == 0)
		{

//			buffer = field_buffer(f[4],0);
//			pod->port = atoi(buffer);

			buffer = field_buffer(f[6],0);
			pod->onoff = atoi(buffer);
//			if(pod->onoff > 1)
//				pod->onoff = 1;

			buffer = field_buffer(f[2],0);
			fp = buffer;
			for(i = 0;i < 20;i++)
			{
				if(*fp == 0x20)
					*fp = 0;
				fp++;
			}
			buffer[19] = 0;
			strcpy(pod->label,buffer);
			curr = (void *)pod;
		}
		erase_form(form);

		free_form(form);
	}
	for (c = 0; f[c] != 0; c++)
		free_field(f[c]);
//	free_fieldtype(fty_middle);
//	free_fieldtype(fty_passwd);
//	noraw();
//	nl();
//	werase(win);
//	delwin(win);
//	wrefresh(win);
//	refresh();
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
	idata.type = 4;
	idata.inverse = 5;
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
