// modified: 11:08
/****************************************************************************
 * Copyright (c) 2005-2013,2014 Free Software Foundation, Inc.              *
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
/*
 * $Id: demo_menus.c,v 1.54 2014/09/05 08:34:06 tom Exp $
 *
 * Demonstrate a variety of functions from the menu library.
 * Thomas Dickey - 2005/4/9
 */
#if 1
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <assert.h>
#include <dirent.h>
#include <time.h>
#include "test.priv.h"
#include <menu.h>
#include "../queue/illist_threads_rw.h"
#include "../queue/ollist_threads_rw.h"
#include "../ioports.h"
#include "../../mytypes.h"
#undef HAVE_RIPOFFLINE
#include <form.h>
#include "form_sup.h"
#include "client.h"
#include "config_file.h"
#include "text_entry.h"

extern int menu_scroll2(int num,int which,char *filename);
extern int menu_scroll3(int num, UCHAR *str, char *filename);
extern int tcp_win(int cmd);
extern int tcp_win_show(int cmd);
extern int tcp_win2(int cmd);
extern int file_win(void);
extern int file_menu2(int num, int which, char *ret_str);

static size_t isize, osize;
static UCHAR buf[UPLOAD_BUFF_SIZE];

// lists for the host
illist_t ill;
ollist_t oll;

// lists for the target
illist_t ill2;
ollist_t oll2;

static char iFileName[40];
static char oFileName[40];
int tcp_connected = 0;

typedef enum
{
	eBanner = -1
	,eHost
	,eTarget
	,eTool
	,eMAX
} MenuNo;

char hostid[20];

#define okMenuNo(n) (((n) > eBanner) && ((n) < eMAX))

#define MENU_Y  1

typedef struct
{
	NCURSES_CONST char *name;
	void (*func) (int);
	unsigned mask;
} MENU_DATA;

static void call_Host(int);

static MENU *mpBanner;
static MENU *mpHost;
static MENU *mpTarget;
static MENU *mpTool;

static WINDOW *status;

static bool loaded_file = FALSE;

static char empty[1];
#endif
static char dat_names[NUM_DAT_NAMES][DAT_NAME_STR_LEN];
static UCHAR dat_len[NUM_DAT_NAMES];
static UCHAR dat_type[NUM_DAT_NAMES];

static char tdate_stamp[NUM_DAT_NAMES][TDATE_STAMP_STR_LEN];

/*****************************************************************************/
static void
show_status2(char *str, char *str2, int a, int b, int c, int line)
{
	wmove(status, line, 0);
	wprintw(status, "%s  %s  %d  %d   %d   ",str,str2,a,b,c);
	wclrtoeol(status);
	wrefresh(status);
}


/*****************************************************************************/
static int
menu_virtualize(int c)
{
	int result;

	if (c == '\n' || c == KEY_EXIT)
		result = (MAX_COMMAND + 1);
	else if (c == 'u')
		result = (REQ_SCR_ULINE);
	else if (c == 'd')
		result = (REQ_SCR_DLINE);
	else if (c == 'b' || c == KEY_NPAGE)
		result = (REQ_SCR_UPAGE);
	else if (c == 'f' || c == KEY_PPAGE)
		result = (REQ_SCR_DPAGE);
	else if (c == 'l' || c == KEY_LEFT || c == KEY_BTAB)
		result = (REQ_LEFT_ITEM);
	else if (c == 'n' || c == KEY_DOWN)
		result = (REQ_NEXT_ITEM);
	else if (c == 'p' || c == KEY_UP)
		result = (REQ_PREV_ITEM);
	else if (c == 'r' || c == KEY_RIGHT || c == '\t')
		result = (REQ_RIGHT_ITEM);
	else if (c == ' ')
		result = (REQ_TOGGLE_ITEM);
	else
	{
		if (c != KEY_MOUSE)
			beep();
		result = (c);
	}
	return result;
}


/*****************************************************************************/
static int
menu_getc(MENU * m)
{
	return wGetchar(menu_win(m));
}


/*****************************************************************************/
static int
menu_offset(MenuNo number)
{
	int result = 0;

	if (okMenuNo(number))
	{
		int spc_desc, spc_rows, spc_cols;

#ifdef NCURSES_VERSION
		menu_spacing(mpBanner, &spc_desc, &spc_rows, &spc_cols);
#else
		spc_rows = 0;
#endif

/* FIXME: MENU.itemlen seems the only way to get actual width of items */
		result = (number - (eBanner + 1)) * (menu_itemwidth(mpBanner) + spc_rows);
	}
	return result;
}


/*****************************************************************************/
static void
my_menu_init(MENU * menu)
{
	mvwprintw(status, 2, 0, "menu_init %p", (void *) menu);
	wclrtoeol(status);
	wrefresh(status);
}


/*****************************************************************************/
static void
my_menu_term(MENU * menu)
{
	mvwprintw(status, 2, 0, "menu_term %p", (void *) menu);
	wclrtoeol(status);
	wrefresh(status);
}


/*****************************************************************************/
static void
my_item_init(MENU * menu)
{
	ITEM *item = current_item(menu);
	const char *name = item_name(item);

//    mvwprintw(status, 4, 0, "item_init %s", name);
	wclrtoeol(status);
	wrefresh(status);
}


/*****************************************************************************/
static void
my_item_term(MENU * menu)
{
	ITEM *item = current_item(menu);
	const char *name = item_name(item);

//    mvwprintw(status, 4, 0, "item_term %s", name);
	wclrtoeol(status);
	wrefresh(status);
}


/*****************************************************************************/
static MENU *
menu_create(ITEM ** items, int count, int ncols, MenuNo number)
{
	MENU *result;
	WINDOW *menuwin;
	int mrows, mcols;
	int y = okMenuNo(number) ? MENU_Y : 0;
	int x = menu_offset(number);
	int margin = (y == MENU_Y) ? 1 : 0;
	int maxcol = (ncols + x) < COLS ? ncols : (COLS - x - 1);
	int maxrow = (count + 1) / ncols;

	if ((maxrow + y) >= (LINES - 4))
		maxrow = LINES - 4 - y;

	result = new_menu(items);

	if (has_colors())
	{
		set_menu_fore(result, (chtype) COLOR_PAIR(1));
		set_menu_back(result, (chtype) COLOR_PAIR(2));
	}

	set_menu_format(result, maxrow, maxcol);
	scale_menu(result, &mrows, &mcols);

//	mvprintw(LINES - 3,0,"mrows: %d  mcols:%d    ",mrows,mcols);

	if (mcols + (2 * margin + x) >= COLS)
		mcols = COLS - (2 * margin + x);
/*
	mvprintw(LINES - 2,0,"mrows: %d  mcols:%d maxcol: %d maxrow: %d x: %d y: %d  ",
					mrows,mcols,maxcol,maxrow,x,y);
	getch();
*/

	menuwin = newwin(mrows + (2 * margin), mcols + (2 * margin), y, x);
	set_menu_win(result, menuwin);
	keypad(menuwin, TRUE);
	if (margin)
		box(menuwin, 0, 0);

	set_menu_sub(result, derwin(menuwin, mrows, mcols, margin, margin));

	post_menu(result);

	set_menu_init(result, my_menu_init);
	set_menu_term(result, my_menu_term);
	set_item_init(result, my_item_init);
	set_item_term(result, my_item_term);
	return result;
}


/*****************************************************************************/
static void
menu_destroy(MENU * m)
{
	int count;

	if (m != 0)
	{
		ITEM **items = menu_items(m);
		const char *blob = 0;

		count = item_count(m);
		if ((count > 0) && (m == mpTool))
		{
			blob = item_name(*items);
		}

		unpost_menu(m);
		free_menu(m);

/* free the extra data allocated in build_Tool_menu() */
		if ((count > 0) && (m == mpTool))
		{
			if (blob && loaded_file)
			{
				free((void *) blob);
			}
			free(items);
			items = 0;
		}
	}
}


/*****************************************************************************/
/* force the given menu to appear */
static void
menu_display(MENU * m)
{
	touchwin(menu_win(m));
	wrefresh(menu_win(m));
}


/*****************************************************************************/
static void
destroy_menus(void)
{
	menu_destroy(mpHost);
	menu_destroy(mpTarget);
	menu_destroy(mpTool);
	menu_destroy(mpBanner);
}


/*****************************************************************************/
static void
call_Host(int code)
{
	static int num = 0;
	static int index;
	static UCHAR *ptr;
	int format;
	int i,j,k;
	char errmsg[40];
	UCHAR cmd;
	static char filename[40];
	char save_as_str[40];
	char tempx[30];
	char *chp;
	UCHAR *ptr2;
	struct dirent **namelist;
	DIR *d;
	struct dirent *dir;
	int mem_size;
	int error;
	UCHAR reboot;

	switch (code)
	{
// OPEN
		case 0:
			d = opendir( "." );
			if( d == NULL )
			{
//				printf("bad OPEN_DIR\n");
//							return -1;
			}
			num = 0;

// fill dat_names array with the names of the *.dat files in the current directory
			memset(dat_names,0,NUM_DAT_NAMES*DAT_NAME_STR_LEN);
			memset(tempx,0,sizeof(tempx));

			while( ( dir = readdir( d ) ) && num < NUM_DAT_NAMES-1)
			{
				if(strcmp( dir->d_name, "." ) == 0 || \
					strcmp( dir->d_name, ".." ) == 0 || dir->d_type == DT_DIR)
					continue;

				memset(tempx,0,sizeof(tempx));
				strcpy(tempx,dir->d_name);

				chp = tempx;
				j = 0;

				while(*chp++ != '.' && j < DAT_NAME_STR_LEN)
					j++;

				strncpy(tempx,chp,j+1);

				if(dir->d_type == DT_REG && strcmp(tempx,"dat") == 0 )
//							if(dir->d_type == DT_REG)
				{
					strcpy(dat_names[num],dir->d_name);
					dat_len[num] = strlen(dat_names[num]);

					mvprintw(LINES-4-num,2,"%d %s  ",num,dat_names[num]);
					refresh();

					getFileCreationTime(dat_names[num],tdate_stamp[num]);
					j = GetFileFormat(dat_names[num]);
					if(j < 0)
						cmd = 0xff;
					else
						cmd = (UCHAR)j;
					dat_type[num] = cmd;
					num++;
				}
			}
			closedir( d );

// and then create another storage space for the dat names
// (which is kindof a waist of memory) and copy everything over to it
			mem_size = (DAT_NAME_STR_LEN*(NUM_DAT_NAMES))+(TDATE_STAMP_STR_LEN*(NUM_DAT_NAMES))+NUM_DAT_NAMES*5;

			ptr = (char *)malloc(mem_size);
			if(ptr == NULL)
			{
				mvprintw(LINES-2,2,"mad balloc");
				refresh();
				error = -1;
				getch();
			}else
			{
				ptr2 = ptr;
				memset(ptr,0,mem_size);

				for(i = 0;i < num;i++)
				{
					*ptr2 = (UCHAR)dat_len[i];
					ptr2++;
					memcpy((UCHAR*)ptr2,(UCHAR*)&dat_names[i],dat_len[i]);
//			mvprintw(LINES-2-i,2,"%d  %s ",dat_len[i],dat_names[i]);
//			refresh();
					ptr2 += dat_len[i];
					memcpy((UCHAR*)ptr2,(UCHAR*)&dat_type[i],1);
					ptr2++;
					memcpy((UCHAR*)ptr2,(UCHAR*)&tdate_stamp[i],TDATE_STAMP_STR_LEN);
//					mvprintw(LINES-2-i,20,"%s  ",tdate_stamp[i]);
//					mvprintw(LINES-2-i,40,"%s  ",ptr2);
//					refresh();
					ptr2 += TDATE_STAMP_STR_LEN;
				}
			}

			memset(filename,0,sizeof(filename));
// menu_scroll3 lets the user pick the dat file to work with
			index = menu_scroll3(num,(char *)ptr,filename);
//			mvprintw(LINES-5,30,"%sx  %d",filename,index);
//			refresh();
			if(index > 0)
			{
				format = GetFileFormat(filename);
				mvprintw(LINES-6,2,"%s  %d   %d",filename,index,format);
				refresh();

				if(format == 0)
				{
// load the chosen file into the ill/oll array
					ilLoadConfig(filename, &ill, isize, errmsg);
					strcpy(iFileName,filename);
//					mvprintw(LINES-7,2,"%sx  %d   %d",filename,index,format);
//					refresh();
// menu_scroll2 lets the user modify with the fields for
// the chosen dat file
					menu_scroll2(39, EDIT_IDATA, filename);
				}
				else if(format == 1)
				{
					olLoadConfig(filename, &oll, osize, errmsg);
					strcpy(oFileName,filename);
//					mvprintw(LINES-8,2,"%sx  %d   %d",filename,index,format);
//					refresh();
					menu_scroll2(39, EDIT_ODATA, filename);
				}
			}
			break;

			free(ptr);

// SAVE FILE
		case 1:
#if 0
//			strcpy(filename,"idata.dat\0");
//			ptr2 = filename;
			memset(filename,0,sizeof(filename));
			index = menu_scroll3(num,(char *)ptr,filename);
			int fp = open((const char *)ptr2, O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
			if(fp < 0)
			{
				strcpy(errmsg,strerror(errno));
				mvprintw(LINES-6,30,"%sx %sx  %s",filename,ptr2,errmsg);
				refresh();
				close(fp);
			}
			else
			{
				mvprintw(LINES-6,30,"%sx %sx %d",filename,ptr2, fp);
				refresh();
				close(fp);
			}
			getFileCreationTime(filename,tempx);
			mvprintw(LINES-7,30,"%s ",tempx);
			refresh();

//			index = menu_scroll3(num,(char *)ptr,filename);
#endif
//			mvprintw(LINES-1,2,"filename: %s %s %s  %d   %d",filename,iFileName,oFileName,index,format);
			format = GetFileFormat(filename);
/*
			mvprintw(LINES-2,2,"format: %d",format);
			format = GetFileFormat(iFileName);
			mvprintw(LINES-3,2,"format: %d",format);
			format = GetFileFormat(oFileName);
			mvprintw(LINES-4,2,"format: %d",format);
			refresh();
*/
			if(format == 0)
			{
				if(ilWriteConfig(filename,&ill,isize,errmsg) != 0)
					show_status2(errmsg,filename,0,format,index,4);
			}
			else if(format == 1)
			{
				if(olWriteConfig(filename,&oll,osize,errmsg) != 0)
					show_status2(errmsg,filename,0,format,index,4);
			}
			else
				show_status2(filename,"",0,format,index,4);

			break;

// EDIT IDATA
		case 2:
			index = menu_scroll2(39,EDIT_IDATA,iFileName);
			break;

// EDIT ODATA
		case 3:
			index = menu_scroll2(39,EDIT_ODATA,oFileName);
			break;

// EXIT but don't reboot
		case 4:
#ifndef CONSOLE_DISABLED
			cmd =  EXIT_PROGRAM;
			if(tcp_connected)
			{
				reboot = 1;
				put_sock(&cmd,1,1,errmsg);
				put_sock(&reboot,1,1,errmsg);
//				get_sock(&num,1,1,errmsg);
				close_sock();
			}
#endif
			break;

// EXIT and reboot
		case 5:
			cmd =  EXIT_PROGRAM;
			if(tcp_connected)
			{
				reboot = 2;
				put_sock(&cmd,1,1,errmsg);
				put_sock(&reboot,1,1,errmsg);
				
				close_sock();
			}
			break;

// EXIT and reboot
		case 6:
			cmd =  EXIT_PROGRAM;
			if(tcp_connected)
			{
				reboot = 3;
				put_sock(&cmd,1,1,errmsg);
				put_sock(&reboot,1,1,errmsg);
				close_sock();
			}
			break;

		case 7:
			cmd = LCD_SHIFT_LEFT;
			if(tcp_connected)
			{
				put_sock(&cmd,1,1,errmsg);
			}
			break;

		case 8:
			cmd = LCD_SHIFT_RIGHT;
			if(tcp_connected)
			{
				put_sock(&cmd,1,1,errmsg);
			}
			break;

		case 9:
			cmd = SCROLL_UP;
			if(tcp_connected)
			{
				put_sock(&cmd,1,1,errmsg);
			}
			break;

		case 10:
			cmd = SCROLL_DOWN;
			if(tcp_connected)
			{
				put_sock(&cmd,1,1,errmsg);
			}
			break;

		case 11:
			cmd = CLOSE_SOCKET;
			put_sock(&cmd,1,1,errmsg);
			close_sock();
			destroy_menus();
			endwin();
			ExitProgram(EXIT_SUCCESS);
		default:
			break;
	}
}


/*****************************************************************************/
static void
build_Host_menu(MenuNo number)
{
#define MY_DATA0(name) { name, call_Host, 0 }

	static MENU_DATA table[] =
	{
		MY_DATA0("Open"),						  // 0
		MY_DATA0("Save File"),					  // 1
		MY_DATA0("Edit idata"),					  // 2
		MY_DATA0("Edit odata"),					  // 3
		MY_DATA0("goto to sh"),					  // 4
		MY_DATA0("Reboot"),						  // 5
		MY_DATA0("Shutdown"),					  // 6
		MY_DATA0("Shift Left"),					  // 7
		MY_DATA0("Shift Right"),				  // 8
		MY_DATA0("Scroll Up"),					  // 9
		MY_DATA0("Scroll Down"),				  // 10
		MY_DATA0("Exit"),						  // 11
		{(char *) 0, 0, 0}
	};

	static ITEM **items;

	ITEM **ip;
	MENU_DATA *ap = 0;
	MENU_DATA *myList = 0;
	int i;
	size_t count = 0;

	if (ap == 0)
	{
		count = SIZEOF(table) - 1;
		items = typeCalloc(ITEM *, count + 1);
		ap = table;
	}

	ip = items;
	for (i = 0; ap[i].name != 0; ++i)
	{
		ap[i].func = call_Host;
		ap[i].mask = (unsigned) i;
		*ip = new_item(ap[i].name, empty);
		set_item_userptr(*ip, (void *) &table[i]);
		++ip;
	}
	*ip = 0;

	mpHost = menu_create(items, (int) count, 1, number);
	if (myList != 0)
		free(myList);
}


/*****************************************************************************/
// manage tcp connection to TS-7XXX
static void
call_Target(int code)
{
	static UCHAR *ptr;
	static UCHAR num;
	static int index;
	static UCHAR sindex;
	int i,j,k;
	char test = 0x21;
	static char filename[40];
	char errmsg[40];
	UCHAR cmd = 0;
	UCHAR str_len;
	UCHAR *ptr2 = NULL;
	int error = 0;
	int mem_size;
	(void) code;
	I_DATA i_data;
	O_DATA o_data;
	I_DATA *itp;
	O_DATA *otp;
	I_DATA **itpp;
	O_DATA **otpp;

	switch (code)
	{
// OPEN
		case 0:
			if(tcp_connected)
			{
//					mvprintw(LINES-1,2,"%x %x ",ptr,ptr2);
				ptr2 = ptr+mem_size;
//					mvprintw(LINES-1,20,"%x %x %d",ptr,ptr2,mem_size);
				cmd = GET_DIR;
				ptr2 = ptr;
				error = put_sock(&cmd,1,1,errmsg);
				if(error < 0)
					mvprintw(LINES-2,2,"%s",errmsg);
				error = get_sock(&num,1,1,errmsg);
				if(error < 0)
					mvprintw(LINES-2,2,"%s",errmsg);

				for(i = 0;i < num;i++)
				{
					error = get_sock((UCHAR*)&dat_len[i],1,1,errmsg);
					if(error < 0)
						mvprintw(LINES-2,2,"%s",errmsg);
					error = get_sock((UCHAR*)&dat_names[i],dat_len[i],1,errmsg);
					if(error < 0)
						mvprintw(LINES-2,2,"%s",errmsg);
//			show_status2(dat_names[i],"",dat_len[i],num,i,i+1);
				}
				for(i = 0;i < num;i++)
				{
					error = get_sock((UCHAR *)&tdate_stamp[i],TDATE_STAMP_STR_LEN,1,errmsg);
					if(error < 0)
						mvprintw(LINES-2,2,"%s",errmsg);
					error = get_sock((UCHAR *)&dat_type[i],1,1,errmsg);
					if(error < 0)
						mvprintw(LINES-2,2,"%s",errmsg);
//					mvprintw(LINES-2-i,1,"%2x  ",dat_type[i]);
				}
			}
			refresh();
			mem_size = (DAT_NAME_STR_LEN*(NUM_DAT_NAMES))+(TDATE_STAMP_STR_LEN*(NUM_DAT_NAMES))+NUM_DAT_NAMES*5;

			ptr = (char *)malloc(mem_size);
			if(ptr == NULL)
			{
				mvprintw(LINES-2,2,"mad balloc");
				error = -1;
				getch();
			}else
			{
				ptr2 = ptr;
				memset(ptr,0,mem_size);
				for(i = 0;i < num;i++)
				{
					*ptr2 = (UCHAR)dat_len[i];
					ptr2++;
					memcpy((UCHAR*)ptr2,(UCHAR*)&dat_names[i],dat_len[i]);
					ptr2 += dat_len[i];
					memcpy((UCHAR*)ptr2,(UCHAR*)&dat_type[i],1);
					ptr2++;
					memcpy((UCHAR*)ptr2,(UCHAR*)&tdate_stamp[i],TDATE_STAMP_STR_LEN);
					ptr2 += TDATE_STAMP_STR_LEN;
//					mvprintw(LINES-2-i,2,"%2x %d",ptr2,ptr2-ptr);
//					mvprintw(LINES-2-i,2,"%s         %s   ",dat_names[i],tdate_stamp[i]);
				}
			}
			memset(filename,0,sizeof(filename));
			index = menu_scroll3(num,(char *)ptr,filename);
			free(ptr);

			if(dat_type[index] == 0)
			{
				cmd = RECV_ALL_IDATA;
				error = put_sock(&cmd,1,1,errmsg);
				if(error < 0)
					mvprintw(LINES-2,2,"error: %s",errmsg);
				index++;
				sindex = (UCHAR)index;
				error = put_sock(&sindex,1,1,errmsg);	  // send the index into the list of filenames

				itp = &i_data;
				itpp = &itp;
				for(i = 0;i < NUM_PORT_BITS;i++)
				{
					error = get_sock((UCHAR *)&i_data,sizeof(I_DATA),1,errmsg);
//					mvprintw(LINES-2-i,2,"%d %d %s",i_data.port,i_data.affected_output,i_data.label);
					illist_insert_data(i,&ill2,&i_data);
//					j = illist_find_data(i,itpp,&ill2);
//					mvprintw(LINES-2-i,2,"%d: %d %d %s ",j,itp->port,itp->affected_output,itp->label);
				}
				menu_scroll2(39, EDIT_IDATA2, filename);
				refresh();
			}
			else if(dat_type[index] == 1)
			{

				cmd = RECV_ALL_ODATA;
				error = put_sock(&cmd,1,1,errmsg);
				if(error < 0)
					mvprintw(LINES-2,2,"%s",errmsg);
				index++;
				sindex = (UCHAR)index;
				put_sock(&sindex,1,1,errmsg);	  // send the index into the list of filenames

				otp = &o_data;
				otpp = &otp;
				for(i = 0;i < NUM_PORT_BITS;i++)
				{
					error = get_sock((UCHAR *)&o_data,sizeof(O_DATA),1,errmsg);
//					mvprintw(LINES-2-i,2,"%d %d %s",i_data.port,i_data.affected_output,i_data.label);
					ollist_insert_data(i,&oll2,&o_data);
//					j = illist_find_data(i,itpp,&ill2);
//					mvprintw(LINES-2-i,2,"%d: %d %d %s ",j,otp->port,otp->onoff,otp->label);
				}
				menu_scroll2(39, EDIT_ODATA2, filename);
				refresh();
			}
			else
			{
				mvprintw(LINES-1,2,"bad dat_type %d  %s   ",index,filename);
				refresh();
			}

			break;

// SAVE FILE
		case 1:
			cmd = SAVE_TO_DISK;
			put_sock(&cmd,1,1,errmsg);			  // if > 0 then this would be an index into the dat_names[] array
			break;

// SAVE AS
		case 2:
			break;
// DELETE
		case 3:
			break;

// LIST IDATA
		case 4:
			cmd = RECV_ALL_IDATA;
			error = put_sock(&cmd,1,1,errmsg);
			if(error < 0)
				mvprintw(LINES-2,2,"%s",errmsg);

			cmd = 0;
			put_sock(&cmd,1,1,errmsg);			  // if > 0 then this would be an index into the dat_names[] array

			itp = &i_data;
			itpp = &itp;
			for(i = 0;i < NUM_PORT_BITS;i++)
			{
				error = get_sock((UCHAR *)&i_data,sizeof(I_DATA),1,errmsg);
//				mvprintw(LINES-2-i,2,"%d %d %s",i_data.port,i_data.affected_output,i_data.label);
				illist_insert_data(i,&ill2,&i_data);
//				j = illist_find_data(i,itpp,&ill2);
//				mvprintw(LINES-2-i,2,"%d: %d %d %s ",j,itp->port,itp->affected_output,itp->label);
			}
			refresh();
			break;

// LIST ODATA
		case 5:
			cmd = RECV_ALL_ODATA;
			error = put_sock(&cmd,1,1,errmsg);
			if(error < 0)
				mvprintw(LINES-2,2,"%s",errmsg);

			cmd = 0;
			put_sock(&cmd,1,1,errmsg);			  // if > 0 then this would be an index into the dat_names[] array

			otp = &o_data;
			otpp = &otp;
			for(i = 0;i < NUM_PORT_BITS;i++)
			{
				error = get_sock((UCHAR *)&o_data,sizeof(O_DATA),1,errmsg);
//				mvprintw(LINES-2-i,2,"%d %d %s",o_data.port,o_data.onoff,o_data.label);
				ollist_insert_data(i,&oll2,&o_data);
//				j = ollist_find_data(i,otpp,&oll2);
//				mvprintw(LINES-2-i,2,"%d: %d %d %s",j,otp->port,otp->onoff,otp->label);
			}
			refresh();
			break;

// SEND IDATA
		case 6:
			cmd = SEND_ALL_IDATA;
			error = put_sock(&cmd,1,1,errmsg);
			if(error < 0)
				mvprintw(LINES-2,2,"%s",errmsg);
			itp = &i_data;
			itpp = &itp;
			for(i = 0;i < NUM_PORT_BITS;i++)
			{
				illist_find_data(i, itpp, &ill2);
//				mvprintw(LINES-2-i,2,"%d %d %s ",itp->port,itp->affected_output,itp->label);
//				put_sock((UCHAR*)&i_data,sizeof(I_DATA),1,errmsg);
				put_sock((UCHAR*)itp,sizeof(I_DATA),1,errmsg);
			}
//			refresh();
			break;

// SEND ODATA
		case 7:
			cmd = SEND_ALL_ODATA;
			error = put_sock(&cmd,1,1,errmsg);
			if(error < 0)
				mvprintw(LINES-2,2,"%s",errmsg);
			otp = &o_data;
			otpp = &otp;
			for(i = 0;i < NUM_PORT_BITS;i++)
			{
				ollist_find_data(i,otpp,&oll2);
//				mvprintw(LINES-2-i,2,"%2d %2d %s           ",otp->port,otp->onoff,otp->label);
//				put_sock((UCHAR*)&o_data,sizeof(O_DATA),1,errmsg);
				put_sock((UCHAR*)otp,sizeof(O_DATA),1,errmsg);
			}
//			refresh();
			break;

// SHOW ALL
		case 8:
			cmd = SHOW_IDATA;
			put_sock(&cmd,1,1,errmsg);
//			show_status2("show idata tcp win","",code,0,0,1);
			tcp_win_show(SHOW_IDATA);
			break;

		case 9:
			cmd = SHOW_ODATA;
			put_sock(&cmd,1,1,errmsg);
//			show_status2("show odata tcp win","",code,0,0,1);
			tcp_win_show(SHOW_ODATA);
			break;
//			j = ilLoadConfig(iFileName,&ill,isize,errmsg);
//			mvprintw(LINES-1,2,"%d   %s   %s  ",j,iFileName,errmsg);
#if 0
			itp = &i_data;
			itpp = &itp;
			for(i = 0;i < NUM_PORT_BITS;i++)
			{
				j = illist_find_data(i,itpp,&ill2);
				mvprintw(LINES-2-i,2,"%d %d %s",itp->port,itp->affected_output,itp->label);
//				mvprintw(LINES-2-i,2,"%d %d %s",i_data.port,i_data.affected_output,i_data.label);
			}

			otp = &o_data;
			otpp = &otp;
			for(i = 0;i < NUM_PORT_BITS;i++)
			{
				j = ollist_find_data(i,otpp,&oll2);
				mvprintw(LINES-40-i,2,"%d %d %s",otp->port,otp->onoff,otp->label);
//				mvprintw(LINES-40-i,2,"%d %d %s",o_data.port,o_data.onoff,o_data.label);
			}
			refresh();
#endif
//			break;
		default:
			break;
	}
}


/*****************************************************************************/
static void
build_Target_menu(MenuNo number)
{
#define MY_DATA1(name) { name, call_Target, 1 }

	static MENU_DATA table[] =
	{
		MY_DATA1("Open"),						  // 0
		MY_DATA1("Save File"),					  // 1
		MY_DATA1("Save As"),					  // 2
		MY_DATA1("Delete"),						  // 3
		MY_DATA1("Recv idata"),					  // 4
		MY_DATA1("Recv odata"),					  // 5
		MY_DATA1("Send idata"),					  // 6
		MY_DATA1("Send odata"),					  // 7
		MY_DATA1("Show idata"),					  // 8
		MY_DATA1("Show odata"),					  // 9
		{(char *) 0, 0, 0}
	};

	static ITEM **items;

	ITEM **ip;
	MENU_DATA *ap = 0;
	MENU_DATA *myList = 0;
	int i;
	size_t count = 0;

	if (ap == 0)
	{
		count = SIZEOF(table) - 1;
		items = typeCalloc(ITEM *, count + 1);
		ap = table;
	}

	ip = items;
	for (i = 0; ap[i].name != 0; ++i)
	{
		ap[i].func = call_Target;
		ap[i].mask = (unsigned) i;
		*ip = new_item(ap[i].name, empty);
		set_item_userptr(*ip, (void *) &table[i]);
		++ip;
	}
	*ip = 0;

	mpTarget = menu_create(items, (int) count, 1, number);
	if (myList != 0)
		free(myList);
}


/*****************************************************************************/

static void
call_Tool(int code)
{
	static int i_index;
	static int o_index;
	int i,j;
	int ret;
	(void) code;
	I_DATA *pid;
	O_DATA *pod;
	char errmsg[60];
	UCHAR cmd;
	time_t tt;
	struct timeval tv;
	double curtime;
	struct tm t;
	struct tm *pt = &t;
	char filename[20];
	int fp;
	off_t fsize;
	int buf_bytes;
	int buf_bytes2;
	// TODO: make all these toggle
	int lights_on, starter_on, acc_on, fuel_on, fan_on;

	switch (code)
	{
		// CONNECT
		case 0:
			if(tcp_connected)
			{
				cmd = CLOSE_SOCKET;
				put_sock(&cmd,1,1,errmsg);
				usleep(1000);
				close_sock();
				usleep(1000);
			}
#ifdef MAKE_SIM
			i = init_client(Host_Sim,errmsg);
			show_status2("init_client (sim)",Host_Sim,i,0,0,2);
#else
			i = init_client(hostid,errmsg);
//			show_status2("init_client",hostid,i,0,0,1);
#endif
			if(i > 0)
			{
				j = tcp_connect();
				if(j > 0)
					tcp_connected = 1;
				else
					tcp_connected = 0;
			}
			if(tcp_connected == 1)
				show_status2("Connected",hostid,i,j,tcp_connected,0);
			else
				show_status2("Not Connected",hostid,i,j,tcp_connected,0);
			
#if 0
			if(j < 0)
			{
//				close_sock();
				destroy_menus();
				endwin();
				ExitProgram(EXIT_SUCCESS);
			}
			cmd = GET_TIME;
			put_sock(&cmd,1,1,errmsg);
#endif
			break;

		// DISCONNECT
		case 1:
			if(tcp_connect)
			{
				show_status2("Disconnect","",code,0,i,2);
				cmd = CLOSE_SOCKET;
				put_sock(&cmd,1,1,errmsg);
				usleep(1000);
				close_sock();
				tcp_connected = 0;
			}
			break;

		case 2:
			if(tcp_connected)
			{
		        cmd = SET_TIME;
//				i = gettimeofday(&tv,NULL);
//		        curtime = tv.tv_sec + tv.tv_usec/1000000.0;
//				mvprintw(LINES-12,2,"%d %f",i,curtime);
		        tt = time(NULL);
//		        pt = localtime(&tt);

//				mvprintw(LINES-11,2,"sec:%d min:%d hour: %d day: %d month %d year: %d", pt->tm_sec,pt->tm_min,pt->tm_hour,pt->tm_mday,pt->tm_mon,pt->tm_year);
				
		        put_sock(&cmd,1,1,errmsg);
				cmd = (UCHAR)tt;
//				mvprintw(LINES-10,2,"%2x",cmd);
		        put_sock(&cmd,1,1,errmsg);
				cmd = (UCHAR)(tt >> 8);
//				mvprintw(LINES-10,6,"%2x",cmd);
		        put_sock(&cmd,1,1,errmsg);
				cmd = (UCHAR)(tt >> 16);
//				mvprintw(LINES-10,10,"%2x",cmd);
		        put_sock(&cmd,1,1,errmsg);
				cmd = (UCHAR)(tt >> 24);
//				mvprintw(LINES-10,14,"%2x",cmd);
		        put_sock(&cmd,1,1,errmsg);
//				mvprintw(LINES-11,2,"%ld",tt);
//				refresh();
			}
//			show_status2("set time  ","",code,0,0,2);
			cmd = GET_TIME;
			put_sock(&cmd,1,1,errmsg);
			break;

		case 3:
			if(tcp_connected)
			{
		        cmd = ENABLE_START;
		        put_sock(&cmd,1,1,errmsg);
			}
			show_status2("starter seq on  ","",code,cmd,0,0);
			break;

		case 4:
			if(tcp_connected)
			{
		        cmd = ON_ACC;
		        put_sock(&cmd,1,1,errmsg);
			}
			show_status2("ACC ON","",code,cmd,0,1);
			break;

		case 5:
			if(tcp_connected)
			{
		        cmd = OFF_ACC;
		        put_sock(&cmd,1,1,errmsg);
			}
			show_status2("ACC OFF","",code,cmd,0,1);
			break;

		case 6:
			if(tcp_connected)
			{
				cmd =  ON_FUEL_PUMP;
				ret = put_sock(&cmd,1,1,errmsg);
			}
			show_status2("FUEL PUMP ON","",code,cmd,0,2);
			break;

		case 7:
			if(tcp_connected)
			{
				cmd =  OFF_FUEL_PUMP;
				ret = put_sock(&cmd,1,1,errmsg);
			}
			show_status2("FUEL PUMP OFF","",code,cmd,0,2);
			break;

		case 8:
			if(tcp_connected)
			{
				cmd =  ON_FAN;
				ret = put_sock(&cmd,1,1,errmsg);
			}
			show_status2("FAN ON","",code,cmd,0,3);
			break;

		case 9:
			if(tcp_connected)
			{
				cmd = OFF_FAN;
				ret = put_sock(&cmd,1,1,errmsg);
			}
			show_status2("FAN OFF","",code,cmd,0,3);
			break;

		case 10:
			if(tcp_connected)
			{
				cmd = OFF_FAN;
				ret = put_sock(&cmd,1,1,errmsg);
			}
			show_status2("FAN OFF","",code,cmd,0,3);
			break;
/*
		case 10:
			if(tcp_connected)
			{
				cmd = ON_LIGHTS;
				ret = put_sock(&cmd,1,1,errmsg);
			}
			show_status2("lights on","",code,0,0,0);
			break;
*/
		case 11:
			if(tcp_connected)
			{
				cmd = CLEAR_SCREEN;
				ret = put_sock(&cmd,1,1,errmsg);
			}
			show_status2("clear lcd screen","",code,0,0,0);
			break;

		case 12:
			if(tcp_connected)
			{
				cmd = SEND_SERIAL;
				ret = put_sock(&cmd,1,1,errmsg);
			}
			show_status2("send serial","",code,0,0,6);
			break;

		// TCP_WINDOW
		case 13:
			if(tcp_connected)
			{
				show_status2("start tcp win","",code,0,0,1);
				cmd = TCP_WINDOW_ON;
				ret = put_sock(&cmd,1,1,errmsg);
				tcp_win(cmd);
				cmd = TCP_WINDOW_OFF;
				ret = put_sock(&cmd,1,1,errmsg);
			}else show_status2("tcp not conn","a",code,0,0,2);
			show_status2("done tcp win","",code,0,0,3);
			break;

		// LIVE_WINDOW
		case 14:
//			if(tcp_connected)
			if(1)
			{
				show_status2("start live window","",code,0,0,1);
				cmd = LIVE_WINDOW_ON;
				ret = put_sock(&cmd,1,1,errmsg);
				tcp_win2(cmd);
				cmd = LIVE_WINDOW_OFF;
				ret = put_sock(&cmd,1,1,errmsg);
			}else show_status2("tcp not conn","a",code,0,0,2);
			show_status2("done tcp win","",code,0,0,3);
			break;

		// UPLOAD_NEW
		case 15:
			if(tcp_connected)
			{
				show_status2("upload new sched","",code,0,0,1);
				strcpy(filename,"sched\0");
				if(access(filename,F_OK) != -1)
				{
					fp = open((const char *)filename, O_RDWR);
					if(fp < 0)
					{
//						printf("can't open file for writing");
						show_status2("can't open file for reading","",0,0,0,2);
					}else
					{
						cmd = UPLOAD_NEW;
						ret = put_sock(&cmd,1,1,errmsg);
						strcpy(filename,"sched\0");
						ret = 0;
						fsize = lseek(fp,0,SEEK_END);
//						printf("filesize: %ld\n",fsize);
						show_status2("filesize: ","",fsize,0,0,2);

						ret = put_sock((UCHAR *)&fsize,sizeof(off_t),1,errmsg);
						// if server is 32-bit machine the sizeof(off_t) is 4
						// but if client is 64-bit then sizeof(off_t) is 8
						//			ret = put_sock((UCHAR *)&fsize,4,1,errmsg);
						if(ret < 0)
						{
//							printf("%s\n",errmsg);
							show_status2("put_sock error: ",errmsg,0,0,0,0);
//							exit(1);
						}
//						printf("ret: %d\n",ret);
						lseek(fp,0,SEEK_SET);

						buf_bytes = (int)fsize;
//						printf("starting buf_bytes: %d\n",buf_bytes);
						show_status2("starting buf bytes: ","",buf_bytes,0,0,1);
//						usleep(1000000);
						do
						{
							buf_bytes2 = (buf_bytes > UPLOAD_BUFF_SIZE?UPLOAD_BUFF_SIZE:buf_bytes);
							buf_bytes -= UPLOAD_BUFF_SIZE;

			//				printf("buf_bytes: %d %d ",buf_bytes2,buf_bytes);
							ret = read(fp,&buf[0],buf_bytes2);

							ret = put_sock((UCHAR *)&buf[0],buf_bytes2,1,errmsg);
							usleep(10000);
							if(ret < 0)
							{
//								printf("%s\n",errmsg);
								show_status2("put_sock error:",errmsg,code,0,0,2);
								exit(1);
							}
							show_status2("bytes","",ret,0,0,2);
			//				else printf("ret: %d\n",ret);
			//				printf("%s\n",errmsg);

						}while(buf_bytes > 0);
//						printf("\ndone\n");
						close_sock();
						close(fp);
					}
				}else show_status2("can't find sched"," to upload",code,0,0,2);
			}else  show_status2("no tcp connection","",code,0,0,2);
			break;

		case 16:
			cmd = TEST_WRITE_FILE;
			ret = put_sock(&cmd,1,1,errmsg);
			break;

		// NEW PASSWORD
#if 0
		case 16:
			if(tcp_connected)
			{
				show_status2("set new password","",code,0,0,1);
				strcpy(filename,"newpassword.txt\0");
				if(access(filename,F_OK) != -1)
				{
					fp = open((const char *)filename, O_RDWR);
					if(fp < 0)
					{
//						printf("can't open file for writing");
						show_status2("can't open file for reading","",0,0,0,2);
					}else
					{
						cmd = NEW_PASSWORD;
						ret = put_sock(&cmd,1,1,errmsg);
						strcpy(filename,"newpassword.txt\0");
						ret = 0;
						fsize = lseek(fp,0,SEEK_END);
//						printf("filesize: %ld\n",fsize);
						show_status2("filesize: ","",fsize,0,0,2);

						ret = put_sock((UCHAR *)&fsize,sizeof(off_t),1,errmsg);
						// if server is 32-bit machine the sizeof(off_t) is 4
						// but if client is 64-bit then sizeof(off_t) is 8
						//			ret = put_sock((UCHAR *)&fsize,4,1,errmsg);
						if(ret < 0)
						{
//							printf("%s\n",errmsg);
							show_status2("put_sock error: ",errmsg,0,0,0,0);
//							exit(1);
						}
//						printf("ret: %d\n",ret);
						lseek(fp,0,SEEK_SET);

						buf_bytes = (int)fsize;
//						printf("starting buf_bytes: %d\n",buf_bytes);
						show_status2("starting buf bytes: ","",buf_bytes,0,0,1);
//						usleep(1000000);
						do
						{
							buf_bytes2 = (buf_bytes > UPLOAD_BUFF_SIZE?UPLOAD_BUFF_SIZE:buf_bytes);
							buf_bytes -= UPLOAD_BUFF_SIZE;

			//				printf("buf_bytes: %d %d ",buf_bytes2,buf_bytes);
							ret = read(fp,&buf[0],buf_bytes2);

							ret = put_sock((UCHAR *)&buf[0],buf_bytes2,1,errmsg);
							usleep(10000);
							if(ret < 0)
							{
//								printf("%s\n",errmsg);
								show_status2("put_sock error:",errmsg,code,0,0,2);
								exit(1);
							}
							show_status2("bytes","",ret,0,0,2);
			//				else printf("ret: %d\n",ret);
			//				printf("%s\n",errmsg);

						}while(buf_bytes > 0);
//						printf("\ndone\n");
						close_sock();
						close(fp);
					}
				}else show_status2("can't find newpassword.txt"," to upload",code,0,0,2);
			}else  show_status2("no tcp connection","",code,0,0,2);
			break;
#endif
		default:
			break;
	}
//	show_status2("test 0","",code,0,0,1);
}


/*****************************************************************************/
static void
build_Tool_menu(MenuNo number)
{
#define MY_DATA2(name) { name, call_Tool, 2 }

	static MENU_DATA table[] =
	{
		MY_DATA2("Connect"),					  // 0
		MY_DATA2("Disconnect"),					  // 1
		MY_DATA2("Set Time"),					  // 2
		MY_DATA2("Enable Start"),				  // 3
		MY_DATA2("On Acc"),						  // 4
		MY_DATA2("Off Acc"),					  // 5
		MY_DATA2("On Fuel Pump"),				  // 6
		MY_DATA2("Off Fuel Pump"),				  // 7
		MY_DATA2("On Fan"),						  // 8
		MY_DATA2("Off Fan"),					  // 9
		MY_DATA2("Shutdown"),					  // 10
		MY_DATA2("Clear Screen"),				  // 11
		MY_DATA2("Send Serial"),				  // 12
		MY_DATA2("TCP Window"),						//13
		MY_DATA2("Live Window"),					//14
		MY_DATA2("Upload New"),						//15
		MY_DATA2("Test Write"),						//16
		{(char *) 0, 0, 0}
	};

	static ITEM **items;

	ITEM **ip;
	MENU_DATA *ap = 0;
	MENU_DATA *myList = 0;
	int i;
	size_t count = 0;

	if (ap == 0)
	{
		count = SIZEOF(table) - 1;
		items = typeCalloc(ITEM *, count + 1);
		ap = table;
	}

	ip = items;
	for (i = 0; ap[i].name != 0; ++i)
	{
		ap[i].func = call_Tool;
		ap[i].mask = (unsigned) i;
		*ip = new_item(ap[i].name, empty);
		set_item_userptr(*ip, (void *) &table[i]);
		++ip;
	}
	*ip = 0;

	mpTool = menu_create(items, (int) count, 1, number);
	if (myList != 0)
		free(myList);
}


/*****************************************************************************/
static int
perform_Host_menu(int cmd)
{
	return menu_driver(mpHost, cmd);
}


/*****************************************************************************/
static int
perform_Target_menu(int cmd)
{
	return menu_driver(mpTarget, cmd);
}


/*****************************************************************************/
static int
perform_Tool_menu(int cmd)
{
	return menu_driver(mpTool, cmd);
}


/*****************************************************************************/
static int
menu_number(void)
{
	return item_index(current_item(mpBanner)) - (eBanner + 1);
}


/*****************************************************************************/
static MENU *
current_menu(void)
{
	MENU *result;

	switch (menu_number())
	{
		case eHost:
			result = mpHost;
			break;
		case eTarget:
			result = mpTarget;
			break;
		case eTool:
			result = mpTool;
			break;
		default:
			result = 0;
			break;
	}
	return result;
}


/*****************************************************************************/
static void
call_menus(int code)
{
	(void) code;
}


/*****************************************************************************/
static void
build_menus(void)
{
	static MENU_DATA table[] =
	{
		{"Host", call_menus, 0},
		{"Target", call_menus, 1},
		{"Tool", call_menus, 2},
		{(char *) 0, 0, 0}
	};
	static ITEM *items[SIZEOF(table)];

	ITEM **ip = items;
	int n;

	for (n = 0; table[n].name != 0; ++n)
	{
		*ip = new_item(table[n].name, empty);
		set_item_userptr(*ip, (void *) &table[n]);
		++ip;
	}
	*ip = (ITEM *) 0;

	mpBanner = menu_create(items, SIZEOF(table) - 1, SIZEOF(table) - 1, eBanner);
	set_menu_mark(mpBanner, ">");

	build_Host_menu(eHost);
	build_Tool_menu(eTool);
	build_Target_menu(eTarget);
}


/*****************************************************************************/
static int
move_menu(MENU * menu, MENU * current, int by_y, int by_x)
{
	WINDOW *top_win = menu_win(menu);
	WINDOW *sub_win = menu_sub(menu);
	int y0, x0;
	int y1, x1;
	int result;

	getbegyx(top_win, y0, x0);
	y0 += by_y;
	x0 += by_x;

	getbegyx(sub_win, y1, x1);
	y1 += by_y;
	x1 += by_x;

	if ((result = mvwin(top_win, y0, x0)) != ERR)
	{
#if defined(NCURSES_VERSION_PATCH) && (NCURSES_VERSION_PATCH < 20060218)
// this isn't defined
		sub_win->_begy = y1;
		sub_win->_begx = x1;
#else
		mvwin(sub_win, y1, x1);
#endif
		if (menu == current)
		{
			touchwin(top_win);
			wnoutrefresh(top_win);
		}
	}
	return result;
}


/*****************************************************************************/
/*
 * Move the menus around on the screen, to test mvwin().
 */
static void
move_menus(MENU * current, int by_y, int by_x)
{
	if (move_menu(mpBanner, current, by_y, by_x) != ERR)
	{
		erase();
		wnoutrefresh(stdscr);
		move_menu(mpHost, current, by_y, by_x);
		move_menu(mpTarget, current, by_y, by_x);
		move_menu(mpTool, current, by_y, by_x);
		doupdate();
	}
}


/*****************************************************************************/
static void
show_status1(int ch, MENU * menu, int line)
{
	wmove(status, line, 1);
	wprintw(status, "key %s, menu %d, mark %s, match %s",
		keyname(ch),
		menu_number(),
		menu_mark(menu),
		menu_pattern(menu));
	wclrtoeol(status);
	wrefresh(status);
}


/*****************************************************************************/
static void
perform_menus(void)
{
	MENU *this_menu;
	MENU *last_menu = mpHost;
	int code = E_UNKNOWN_COMMAND;
	int cmd;
	int ch = ERR;

#ifdef NCURSES_MOUSE_VERSION
	mousemask(ALL_MOUSE_EVENTS, (mmask_t *) 0);
#endif

	menu_display(last_menu);

	for (;;)
	{
/*
		if (ch != ERR)
		{
			show_status1(ch, last_menu,0);
		}
*/
		ch = menu_getc(mpBanner);
/*
 * Provide for moving the menu around in the screen using shifted
 * cursor keys.
 */
		switch (ch)
		{
			case KEY_SF:
				move_menus(last_menu, 1, 0);
				continue;
			case KEY_SR:
				move_menus(last_menu, -1, 0);
				continue;
			case KEY_SLEFT:
				move_menus(last_menu, 0, -1);
				continue;
			case KEY_SRIGHT:
				move_menus(last_menu, 0, 1);
				continue;
		}
		cmd = menu_virtualize(ch);

		switch (cmd)
		{
/*
 * The banner menu acts solely to select one of the other menus.
 * Move between its items, wrapping at the left/right limits.
 */
			case REQ_LEFT_ITEM:
			case REQ_RIGHT_ITEM:
				code = menu_driver(mpBanner, cmd);
				if (code == E_REQUEST_DENIED)
				{
					if (menu_number() > 0)
						code = menu_driver(mpBanner, REQ_FIRST_ITEM);
					else
						code = menu_driver(mpBanner, REQ_LAST_ITEM);
				}
				break;
			default:
				switch (menu_number())
				{
					case eHost:
						code = perform_Host_menu(cmd);
						break;
					case eTarget:
						code = perform_Target_menu(cmd);
						break;
					case eTool:
						code = perform_Tool_menu(cmd);
						break;
				}

				if ((code == E_REQUEST_DENIED) && (cmd == KEY_MOUSE))
				{
					code = menu_driver(mpBanner, cmd);
				}

				break;
		}

		if (code == E_OK)
		{
			this_menu = current_menu();
			if (this_menu != last_menu)
			{
				move(1, 0);
				clrtobot();
				box(menu_win(this_menu), 0, 0);
				refresh();

/* force the current menu to appear */
				menu_display(this_menu);

				last_menu = this_menu;
			}
		}
		wrefresh(menu_win(last_menu));
		if (code == E_UNKNOWN_COMMAND
			|| code == E_NOT_POSTED)
		{
			ITEM *item = current_item(last_menu);
			MENU_DATA *td = (MENU_DATA *) item_userptr(item);
			td->func((int) td->mask);
//            show_status2("func","",td->mask,0,0,4);
		}
		if (code == E_REQUEST_DENIED)
			beep();
		continue;
	}

#ifdef NCURSES_MOUSE_VERSION
	mousemask(0, (mmask_t *) 0);
#endif
}


#if HAVE_RIPOFFLINE
/*****************************************************************************/
static int
rip_footer(WINDOW *win, int cols)
{
	wbkgd(win, A_REVERSE);
	werase(win);
	wmove(win, 0, 0);
	wprintw(win, "footer: %d columns", cols);
	wnoutrefresh(win);
	return OK;
}


/*****************************************************************************/
static int
rip_header(WINDOW *win, int cols)
{
	wbkgd(win, A_REVERSE);
	werase(win);
	wmove(win, 0, 0);
	wprintw(win, "header: %d columns", cols);
	wnoutrefresh(win);
	return OK;
}
#endif											  /* HAVE_RIPOFFLINE */
/*****************************************************************************/
static void
usage(void)
{
	static const char *const tbl[] =
	{
		"Usage: demo_menus [options] [menu-file]"
		,""
		,"Options:"
#if HAVE_RIPOFFLINE
		,"  -f       rip-off footer line (can repeat)"
		,"  -h       rip-off header line (can repeat)"
#endif
	};
	size_t n;
	for (n = 0; n < SIZEOF(tbl); n++)
		fprintf(stderr, "%s\n", tbl[n]);
	ExitProgram(EXIT_FAILURE);
}


/*****************************************************************************/
int
main(int argc, char *argv[])
{
	int c, rc;
	int i,j;
//    size_t size;
	I_DATA i_data;
	O_DATA o_data;
	I_DATA *itp;
	O_DATA *otp;
	I_DATA **itpp;
	O_DATA **otpp;

	char errmsg[40];

/*
145 "192.168.42.145"
146 "192.168.42.146"
148 "192.168.42.148"
149 "192.168.42.149"
124 "192.168.42.124"
*/

	memset(iFileName,0,sizeof(iFileName));
	memset(oFileName,0,sizeof(oFileName));

	printf("argc = %d\n",argc);
	
	if(argc == 1)
	{
		printf("usage: %s [ip address] [idata filename] [odata filename] \n",argv[0]);
		printf("where ipaddress is: 192.168.42.<ipaddress>\n");
		printf("enter either: 145, 146, 148, 149 or 124\n");
		printf("using 145 as default and idata.dat/odata.dat as default data files\n");
		strcpy(hostid,"192.168.42.145\0");
		strcpy(iFileName,"idata.dat\0");
		strcpy(oFileName,"odata.dat\0");
	}
	else if(argc == 2)
	{
		strcpy(hostid,argv[1]);
		strcpy(iFileName,"idata.dat\0");
		strcpy(oFileName,"odata.dat\0");
	}
	else if(argc == 3)
	{
		strcpy(hostid,argv[1]);
		strcpy(iFileName,argv[2]);
		strcpy(oFileName,"odata.dat\0");
	}
	else if(argc == 4)
	{
		strcpy(hostid,argv[1]);
		strcpy(iFileName,argv[2]);
		strcpy(oFileName,argv[3]);
	}
	printf("idat file: %s odat file: %s host ipaddress: %s\n",iFileName,oFileName,hostid);
	
	i = NUM_PORT_BITS;
	isize = sizeof(I_DATA);
	isize *= i;

	i = NUM_PORT_BITS;
	osize = sizeof(O_DATA);
	osize *= i;

	illist_init(&ill);
	if(access(iFileName,F_OK) != -1)
	{
		rc = ilLoadConfig(iFileName,&ill,isize,errmsg);
		if(rc > 0)
		{
			printf("%s\n",errmsg);
			return 1;
		}
	}else										  // oh-boy! create a new file!
	{

	}

	ollist_init(&oll);
	if(access(oFileName,F_OK) != -1)
	{
		rc = olLoadConfig(oFileName,&oll,osize,errmsg);
		if(rc > 0)
		{
			printf("%s\n",errmsg);
			return 1;
		}
	}else										  // oh-boy! create a new file!
	{

	}
//	ollist_show(&oll);

// init the lists used to manage the target
	ollist_init(&oll2);
	illist_init(&ill2);

//	strcpy(iFileName,"idata2.dat\0");
//	strcpy(oFileName,"odata2.dat\0");
	rc = ilLoadConfig("idata2.dat",&ill2,isize,errmsg);
//	printf("%d %s\n",rc,errmsg);
	rc = olLoadConfig("odata2.dat",&oll2,isize,errmsg);
//	printf("%d %s\n",rc,errmsg);
#if 0
	itp = &i_data;
	itpp = &itp;
	for(i = 0;i < NUM_PORT_BITS;i++)
	{
		j = illist_find_data(i,itpp,&ill2);
//		mvprintw(LINES-2-i,2,"%d %d %s",itp->port,itp->affected_output,itp->label);
//		printf("%d %d %d %s\n",j,i_data.port,i_data.affected_output,i_data.label);
//		printf("%d %d %d %s\n",j,itp->port,itp->affected_output,itp->label);
	}
/*
	otp = &o_data;
	for(i = 0;i < NUM_PORT_BITS;i++)
	{
		j = ollist_find_data(i,&otp,&oll2);
//				mvprintw(LINES-40-i,2,"%d %d %s",otp->port,otp->onoff,otp->label);
		printf("%d %d %d %s\n",j, o_data.port,o_data.onoff,o_data.label);
	}
*/
	return 1;
#endif
	setlocale(LC_ALL, "");

#if 0
	while ((c = getopt(argc, argv, "a:de:fhmp:s:t:")) != -1)
	{
		switch (c)
		{
#if HAVE_RIPOFFLINE
			case 'f':
				ripoffline(-1, rip_footer);
				break;
			case 'h':
				ripoffline(1, rip_header);
				break;
#endif								  /* HAVE_RIPOFFLINE */
			default:
				usage();
		}
	}
#endif
	initscr();
	noraw();
	cbreak();
	noecho();
//	nodelay(stdscr,TRUE);

	if (has_colors())
	{
		start_color();
		init_pair(1, COLOR_RED, COLOR_BLACK);
		init_pair(2, COLOR_BLUE, COLOR_WHITE);
		init_pair(3, COLOR_WHITE, COLOR_BLUE);
		init_pair(4, COLOR_GREEN, COLOR_BLACK);
		init_pair(5, COLOR_CYAN, COLOR_BLACK);
	}
	status = newwin(5, COLS-10, LINES - 10, 1);
	wrefresh(status);

	build_menus();
/*
	strncpy(fname1,fptr1,40);
	strncpy(fname2,fptr2,40);
	show_status2(fname1,sfilename,0,0,0,2);
*/
	perform_menus();
	destroy_menus();

	endwin();
	ExitProgram(EXIT_SUCCESS);
}


#if 0
ptr2 = filename;
int fp = open((const char *)ptr2, O_RDONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
if(fp < 0)
{
	strcpy(errmsg,strerror(errno));
	mvprintw(LINES-6,30,"%s %s  %s",filename,ptr2,errmsg);
	refresh();
	close(fp);
}


else
{
	mvprintw(LINES-6,30,"%s %s %d",filename,ptr2, fp);
	refresh();
	close(fp);
}


getFileCreationTime(filename,tempx);
mvprintw(LINES-7,30,"%s ",tempx);
refresh();

}
#endif
