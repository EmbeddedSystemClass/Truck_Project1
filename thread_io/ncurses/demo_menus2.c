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
/*
item_description		-
item_init			-
item_opts			-
item_opts_off			-
item_opts_on			-
item_term			-
item_visible			-
menu_back			-
menu_fore			-
menu_format			-
menu_grey			-
menu_init			-
menu_opts			-
menu_pad			-
menu_request_by_name		-
menu_request_name		-
menu_term			-
menu_userptr			-
set_current_item		-
set_item_opts			-
set_menu_grey			-
set_menu_items			-
set_menu_opts			-
set_menu_pad			-
set_menu_pattern		-
set_menu_spacing		-
set_menu_userptr		-
set_top_row			-
top_row				-
*/

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
#include "test.priv.h"
#include <menu.h>
#include "../queue/illist_threads_rw.h"
#include "../queue/ollist_threads_rw.h"
#include "../ioports.h"
#include "../mytypes.h"
#undef HAVE_RIPOFFLINE
#include <form.h>
#include "form_sup.h"
#include "client.h"

extern int iWriteConfig(char *filename, I_DATA *curr_i_array,size_t size,char *errmsg);
extern int iLoadConfig(char *filename, I_DATA *curr_i_array,size_t size,char *errmsg);
extern int oWriteConfig(char *filename, O_DATA *curr_o_array,size_t size,char *errmsg);
extern int oLoadConfig(char *filename, O_DATA *curr_o_array,size_t size,char *errmsg);

extern int iWriteConfig2(char *filename, I_DATA *curr_i_array,size_t size,int offset,char *errmsg);
extern int oWriteConfig2(char *filename, O_DATA *curr_o_array,size_t size,int offset,char *errmsg);

extern int GetFileFormat(char *filename);

extern int menu_scroll2(void *curr,int num,int which);
extern void demo_forms(void *cur,int which);
extern int tcp_win(I_DATA *i_data, O_DATA *o_data);
extern int file_win(void);
extern int file_menu2(int which, char *ret_str);

static size_t isize, osize;

static I_DATA *curr_i_array;
static O_DATA *curr_o_array;
static I_DATA *curr_i_array2;
static O_DATA *curr_o_array2;
static char isfilename[40];
static char osfilename[40];
int tcp_connected = 0;

typedef enum
{
    eBanner = -1
    ,eFile
    ,eOptions
    ,eSelect
    ,eMAX
} MenuNo;

#define okMenuNo(n) (((n) > eBanner) && ((n) < eMAX))

#define MENU_Y  1

typedef struct
{
    NCURSES_CONST char *name;
    void (*func) (int);
    unsigned mask;
} MENU_DATA;

static void call_files(int);

static MENU *mpBanner;
static MENU *mpFile;
static MENU *mpOptions;
static MENU *mpSelect;

static WINDOW *status;

static bool loaded_file = FALSE;

static char empty[1];

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
        if ((count > 0) && (m == mpSelect))
        {
            blob = item_name(*items);
        }

        unpost_menu(m);
        free_menu(m);

/* free the extra data allocated in build_select_menu() */
        if ((count > 0) && (m == mpSelect))
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
    menu_destroy(mpFile);
    menu_destroy(mpOptions);
    menu_destroy(mpSelect);
    menu_destroy(mpBanner);
}

/*****************************************************************************/
static void
call_files(int code)
{
    int format;
    int index;
    char errmsg[40];

    switch (code)
    {
    	case 0:		// New idata - (TODO: use the code from init_db.c and put in config_file.c to create a new dat file)
    		break;
    	case 1:		// New odata
    		break;
		case 2:
			index = file_menu2(1,isfilename);
			format = GetFileFormat(isfilename);
			if(format == 0)
				if(iLoadConfig(isfilename,curr_i_array,isize,errmsg) != 0)
				{
					printf("can't find %s %s\n",isfilename,errmsg);
				}
			else
			{
				printf("bad file format for %s\n",isfilename);
				memset(curr_i_array,0,isize);
			}
            show_status2(curr_i_array->label,isfilename,curr_i_array->port,0,index,1);
    		break;
    	case 3:
			index = file_menu2(0,osfilename);
			format = GetFileFormat(osfilename);
			if(format == 1)
				if(oLoadConfig(osfilename,curr_o_array,osize,errmsg) != 0)
				{
					printf("can't find %s %s\n",osfilename,errmsg);
				}
			else
			{
				printf("bad file format for %s\n",isfilename);
				memset(curr_o_array,0,osize);
			}
            show_status2(curr_i_array->label,isfilename,curr_i_array->port,0,index,1);
    		break;
        case 4:		// Exit
            destroy_menus();
            endwin();
            printf("\nDONE!\n");
            ExitProgram(EXIT_SUCCESS);
    }

}

/*****************************************************************************/
static void
build_file_menu(MenuNo number)
{
#define MY_DATA0(name) { name, call_files, 0 }

    static MENU_DATA table[] =
    {
        MY_DATA0("New idata"),
        MY_DATA0("New odata"),
        MY_DATA0("Open idata"),
        MY_DATA0("Open odata"),
        MY_DATA0("Exit"),
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
        ap[i].func = call_files;
        ap[i].mask = (unsigned) i;
        *ip = new_item(ap[i].name, empty);
        set_item_userptr(*ip, (void *) &table[i]);
        ++ip;
    }
    *ip = 0;

    mpFile = menu_create(items, (int) count, 1, number);
    if (myList != 0)
        free(myList);
}

/*****************************************************************************/
static int
perform_file_menu(int cmd)
{
    return menu_driver(mpFile, cmd);
}

/*****************************************************************************/
// manage tcp connection to TS-7200
static void
call_select(int code)
{
	int i,j;
	char test = 0x21;
	char buf[20];
	char errmsg[40];
	UCHAR cmd = 0;
    (void) code;
    switch (code)
    {
        case 0:
			i = init_client();
			show_status2("init_client","",i,0,0,1);
			if(i > 0)
			{
				j = tcp_connect();
				if(j > 0)
					tcp_connected = 1;
				else
					tcp_connected = 0;
			}
            show_status2("Connect","",i,j,tcp_connected,2);
            break;
        case 1:
            show_status2("Disconnect","",code,0,i,1);
            cmd = CLOSE_SOCKET;
            put_sock(&cmd,1,1,errmsg);
            usleep(TIME_DELAY*100);
            close_sock();
            tcp_connected = 0;
            break;
        case 2:
            show_status2("TCP Window","",code,0,i,1);
            if(tcp_connected == 1)
	            tcp_win(curr_i_array, curr_o_array);
            break;
        case 3:
            show_status2("test1","",code,0,0,1);
            break;
        case 4:
            show_status2("test2","",code,0,0,1);
            break;
    }
}

/*****************************************************************************/
static void
build_select_menu(MenuNo number)
{
#define MY_DATA1(name) { name, call_select, 1 }

    static MENU_DATA table[] =
    {
        MY_DATA1("Connect"),
        MY_DATA1("Disconnect"),
        MY_DATA1("TCP Window"),
        MY_DATA1(""),
        MY_DATA1(""),
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
        ap[i].func = call_select;
        ap[i].mask = (unsigned) i;
        *ip = new_item(ap[i].name, empty);
        set_item_userptr(*ip, (void *) &table[i]);
        ++ip;
    }
    *ip = 0;

    mpSelect = menu_create(items, (int) count, 1, number);
    if (myList != 0)
        free(myList);
}

/*****************************************************************************/
static void
call_options(int code)
{
	static int i_index;
	static int o_index;
	int i;
	int ret;
    (void) code;
    I_DATA *pid;
    O_DATA *pod;
    char errmsg[60];
    UCHAR cmd;


    switch (code)
    {
		// curr_io_array is the entire set of records loaded by LoadConfig
		// menu scroll returns the index into the db of the record chosen
        case 0:
			pid = curr_i_array;
            i_index = menu_scroll2((void*)curr_i_array,39,1);

			for(i = 0;i < i_index;i++)
				pid++;
//            show_status2(pid->label,isfilename,pid->port,pid->affected_output,pid->type,1);
        	ret = iWriteConfig(isfilename,curr_i_array,isize,errmsg);
        	if(tcp_connected)
        	{
				cmd =  SEND_IDATA;
//				ret = send_tcp(&cmd,1,errmsg);
				ret = put_sock(&cmd,1,1,errmsg);
//				ret = send_tcp((void*)&i_index,1,errmsg);
				ret = put_sock((void*)&i_index,1,1,errmsg);
//				ret = send_tcp((void*)pid,sizeof(I_DATA),errmsg);
				ret = put_sock((void*)pid,sizeof(I_DATA),1,errmsg);
			}

        	if(ret < 0)
	            show_status2(errmsg,isfilename,isize,ret,i_index,2);
			else
	            show_status2(pid->label,isfilename,pid->port,ret,i_index,2);

            break;
        case 1:
        	pod = curr_o_array;
//            show_status2(curr_o_array2->label,curr_o_array->label,o_index,code,0,1);
			o_index = menu_scroll2((void *)curr_o_array,25,0);
			for(i = 0;i < o_index;i++)
				pod++;
//            show_status2(curr_o_array2->label,curr_o_array->label,o_index,code,0,2);
        	ret = oWriteConfig(osfilename,pod,osize,errmsg);
        	if(tcp_connected)
        	{
				cmd =  SEND_ODATA;
/*
				ret = send_tcp(&cmd,1,errmsg);
				ret = send_tcp((void*)&o_index,1,errmsg);
				ret = send_tcp((void*)pod,sizeof(O_DATA),errmsg);
*/
				ret = put_sock(&cmd,1,1,errmsg);
				ret = put_sock((void*)&o_index,1,1,errmsg);
				ret = put_sock((void*)pod,sizeof(O_DATA),1,errmsg);
			}

        	if(ret < 0)
	            show_status2(errmsg,osfilename,osize,ret,o_index,3);
			else
	            show_status2(pid->label,osfilename,pid->port,ret,o_index,3);

            break;
        case 2:
            show_status2("test 1","",code,0,0,2);
			cmd =  SEND_SHOW;
//			send_tcp(&cmd,1,errmsg);
			put_sock(&cmd,1,1,errmsg);
            break;
        case 3:
            show_status2("test 2","",code,0,0,2);
            break;
        case 4:
            show_status2("test 3","",code,0,0,2);
            break;
        case 5:
            show_status2("test 4","",code,0,0,2);
            break;
	}
//	show_status2("test 0","",code,0,0,1);

}
/*
        	strcpy(i_pio->label,"test record\0");		// this is just for testing
        	i_pio->port = 5;
        	i_pio->affected_output = 6;
        	i_pio->inverse = 7;
        	i_pio->type = 8;
        	tcp_win(i_pio);
*/

/*****************************************************************************/
static void
build_options_menu(MenuNo number)
{
#define MY_DATA2(name) { name, call_options, 2 }

    static MENU_DATA table[] =
    {
        MY_DATA2("change input"),
        MY_DATA2("change output"),
        MY_DATA2("show llist"),
        MY_DATA2(""),
        MY_DATA2(""),
        MY_DATA2(""),
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
        ap[i].func = call_options;
        ap[i].mask = (unsigned) i;
        *ip = new_item(ap[i].name, empty);
        set_item_userptr(*ip, (void *) &table[i]);
        ++ip;
    }
    *ip = 0;

    mpOptions = menu_create(items, (int) count, 1, number);
    if (myList != 0)
        free(myList);
}

/*****************************************************************************/
static int
perform_select_menu(int cmd)
{
    return menu_driver(mpSelect, cmd);
}

/*****************************************************************************/
static int
perform_options_menu(int cmd)
{
    return menu_driver(mpOptions, cmd);
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
        case eFile:
            result = mpFile;
            break;
        case eOptions:
            result = mpOptions;
            break;
        case eSelect:
            result = mpSelect;
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
        {"File", call_menus, 0},
        {"Options", call_menus, 1},
        {"Select", call_menus, 2},
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

    build_file_menu(eFile);
    build_select_menu(eSelect);
    build_options_menu(eOptions);
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
        move_menu(mpFile, current, by_y, by_x);
        move_menu(mpOptions, current, by_y, by_x);
        move_menu(mpSelect, current, by_y, by_x);
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
    MENU *last_menu = mpFile;
    int code = E_UNKNOWN_COMMAND;
    int cmd;
    int ch = ERR;

#ifdef NCURSES_MOUSE_VERSION
    mousemask(ALL_MOUSE_EVENTS, (mmask_t *) 0);
#endif

    menu_display(last_menu);

    for (;;)
    {

        if (ch != ERR)
        {
            show_status1(ch, last_menu,0);
        }

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
                    case eFile:
                        code = perform_file_menu(cmd);
                        break;
                    case eOptions:
                        code = perform_options_menu(cmd);
                        break;
                    case eSelect:
                        code = perform_select_menu(cmd);
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
            show_status2("func","",td->mask,0,0,3);
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
#endif                                            /* HAVE_RIPOFFLINE */

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
    int c;
    int i;
//    size_t size;
	I_DATA *pid;
	O_DATA *pod;
	char *fptr1;
	char *fptr2;
	char fname1[20];
	char fname2[20];
	char errmsg[40];

	memset(isfilename,0,sizeof(isfilename));
	memset(osfilename,0,sizeof(osfilename));

	if(argc < 2)
	{
		printf("usage: %s [idata filename][odata filename]\n",argv[0]);
		return 1;
	}
	else if(argc < 3)
	{
		printf("usage: %s %s [odata filename]\n",argv[0],argv[1]);
		return 1;
	}
	fptr1 = argv[1];
	fptr2 = argv[2];
	strcpy(isfilename,fptr1);
	strcpy(osfilename,fptr2);
	i = NUM_PORT_BITS;

	isize = sizeof(I_DATA);
	isize *= i;

	curr_i_array = (I_DATA *)malloc(isize);

	if(curr_i_array == NULL)
	{
		printf("problems with malloc curr_i_array\n");
		return 1;
	}
	memset((void *)curr_i_array,0,isize);

	osize = sizeof(O_DATA);
	osize *= i;

	curr_o_array = (O_DATA *)malloc(osize);
	if(curr_o_array == NULL)
	{
		printf("problems with malloc curr_o_array\n");
		return 1;
	}
	memset((void *)curr_o_array,0,osize);

	curr_i_array2 = NULL;
	curr_o_array2 = NULL;

	if(iLoadConfig(fptr1,curr_i_array,isize,errmsg) != 0)
	{
		printf("can't find %s %s\n",fptr1,errmsg);
		return 1;
	}

	if(oLoadConfig(fptr2,curr_o_array,osize,errmsg) != 0)
	{
		printf("can't find %s %s\n",fptr2,errmsg);
		return 1;
	}
/*
	printf("\n\n%s %s %d %lu\n",fptr2,curr_o_array->label,curr_o_array->port,osize);
	getchar();
*/
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
#endif                                /* HAVE_RIPOFFLINE */
            default:
                usage();
        }
    }
#endif

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
    status = newwin(5, COLS, LINES - 10, 0);
    build_menus();
/*
	strncpy(fname1,fptr1,40);
	strncpy(fname2,fptr2,40);
	show_status2(fname1,sfilename,0,0,0,2);
*/
    perform_menus();
    destroy_menus();

	free(curr_i_array);
	free(curr_o_array);
    endwin();
    ExitProgram(EXIT_SUCCESS);
}

#if 0
        	if(curr_i_array2 != NULL)
        	{
	            show_status2(curr_i_array2->label,sfilename,curr_i_array->port,curr_i_array2->port,i_index,1);
	            nonl();
	            demo_forms((void*)curr_i_array2,1);
				memcpy(i_pio,curr_i_array2,sizeof(I_DATA));
	            show_status2(curr_i_array2->label,sfilename,i_pio->port,curr_i_array2->port,i_index,2);

//	        	ret = iWriteConfig2("itemp.dat",curr_i_array2,isize,i_index,errmsg);
	        	ret = iWriteConfig("itemp.dat",curr_i_array,isize,errmsg);
	        	if(ret < 0)
		            show_status2(errmsg,sfilename,isize,ret,i_index,3);
				else
		            show_status2(curr_i_array2->label,sfilename,i_pio->port,ret,i_index,3);
	        }
			else show_status2("null","",code,0,0,3);
#endif

