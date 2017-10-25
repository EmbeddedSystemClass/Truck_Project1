#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../queue/illist_threads_rw.h"
#include "../queue/ollist_threads_rw.h"
#include "../ioports.h"
#include "test.priv.h"
#include <menu.h>
#include "client.h"

#define WIN_WIDTH 47

//void func(IO_DATA *cur);

//#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

char sup_string[NUM_DAT_NAMES][DAT_NAME_STR_LEN+4];
char tdate_string[NUM_DAT_NAMES][TDATE_STAMP_STR_LEN+10];
UCHAR dat_type[NUM_DAT_NAMES];
UCHAR dat_len[NUM_DAT_NAMES];

static void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string, chtype color);

//******************************************************************************************//
//**************************************** disp_msg ****************************************//
//******************************************************************************************//
static void disp_msg(WINDOW *win,char *str,int line)
{
	wmove(win,line+1,1);
	// this overwrites the vert line char at the end of line
	wclrtoeol(win);
	mvwprintw(win,line+1,1,"%s ",str);
}

//******************************************************************************************//
//************************************** menu_scroll2 **************************************//
//******************************************************************************************//
int menu_scroll3(int num, UCHAR *str, char *filename)
{
    ITEM **my_items;
    ITEM *cur_item;
    int c;
    MENU *my_menu;
    WINDOW *twin;
    int i,j,k;
    int index = -1;
    int finished = 0;
    UCHAR outc;
	int win_width;
	UCHAR *chptr;

/* Create items */

	chptr = str;
//	num--;

	memset(sup_string,0,(NUM_DAT_NAMES*(DAT_NAME_STR_LEN+4)));
	memset(tdate_string,0,(NUM_DAT_NAMES*(TDATE_STAMP_STR_LEN+10)));

	win_width = WIN_WIDTH;
	for(i = 0; i < num; i++)
	{
		memcpy((UCHAR*)&dat_len[i],chptr,1);
		chptr++;
//			memcpy((UCHAR*)&sup_string[i],chptr,dat_len[i]);
		memcpy((UCHAR*)&sup_string[i],chptr,dat_len[i]);
		strcat(sup_string[i],"   ");
//		mvprintw(LINES - 2-i, 2,"%d   ",dat_len[i]);
//		mvprintw(LINES - 2-i, 4,"%s   ",sup_string[i]);
//		refresh();
		chptr += dat_len[i];
		dat_type[i] = *(chptr);
		chptr++;
		memcpy((UCHAR*)&tdate_string[i],chptr,TDATE_STAMP_STR_LEN);
//		mvprintw(LINES - 2-i, 30,"%s   ",tdate_string[i]);
//		refresh();
		chptr += TDATE_STAMP_STR_LEN;

#if 0
		memcpy((UCHAR*)&dat_len,chptr,1);
		chptr++;
		memset(sup_string,0,sizeof(sup_string));
		memcpy((UCHAR*)&sup_string,chptr,dat_len);
		strcat(sup_string,"  ");
//		mvprintw(LINES - 2-i, 2,"%d   ",dat_len);
//		mvprintw(LINES - 2-i, 10,"%s   ",sup_string);
//		refresh();
//		sup_string[dat_len+2] = 0;
		chptr += dat_len;
		dat_type = *(chptr);
		chptr++;
		memset(tdate_string,0,sizeof(tdate_string));
		memcpy((UCHAR*)&tdate_string,chptr,TDATE_STAMP_STR_LEN);
//		tdate_string[TDATE_STAMP_STR_LEN] = 0;
//		mvprintw(LINES - 2-i, 40,"%s   ",tdate_string);
//		refresh();
		chptr += TDATE_STAMP_STR_LEN;
#endif
	}
    my_items = (ITEM **)calloc(num+2, sizeof(ITEM *));

	for(i = 0;i < num;i++)
	{
		if(dat_type[i] != 0xFF)
		{
//#if 0
			strcat(tdate_string[i]," ");

			if(dat_type[i] == 0)
				strcat(tdate_string[i],"   I");
			else strcat(tdate_string[i],"   O");
//#endif
			mvprintw(LINES - 4-i, 0,"%2d    %s     %s ",i,tdate_string[i],sup_string[i]);
			my_items[i] = new_item(sup_string[i],tdate_string[i]);
//			my_items[i]->index = i;
//			my_items[i]->y = i;
//			mvprintw(LINES - 4-i, 2,"%s  %d  %d  %d",my_items[i]->name.str,
//					my_items[i]->name.length,my_items[i]->index,my_items[i]->y);
			refresh();
		}
	}

	my_items[i] = new_item((char*)NULL,(char *)NULL);
	i++;
	my_items[i] = (ITEM *)NULL;

/* Create menu */
//	mvprintw(LINES-1, 0,"done");
//	getch();
    my_menu = new_menu((ITEM **)my_items);

/*
	for(i = 0;i < num;i++)
	{
			mvprintw(LINES - 4-i, 2,"%s  %d  %d  %d",my_items[i]->name.str,
					my_items[i]->name.length,my_items[i]->index,my_items[i]->y);
			refresh();
	}
*/
/* Create the window to be associated with the menu */

//getch();

    twin = newwin(num+3, win_width, 0, 40);
    keypad(twin, TRUE);

/* Set main window and sub window */
    set_menu_win(my_menu, twin);
/*
WINDOW *derwin(WINDOW *orig, int nlines, int ncols, int begin_y, int begin_x);
*/
    set_menu_sub(my_menu, derwin(twin, num, win_width-2, 3, 1));
    set_menu_format(my_menu, num-3, 1);

/* Set menu mark to the string " * " */
    set_menu_mark(my_menu, " * ");

/* Print a border around the main window and print a title */
    box(twin, 0, 0);

    print_in_middle(twin, 1, 0, 40, "target dir", COLOR_PAIR(1));

	mvwaddch(twin,num,0,ACS_LTEE);
	mvwaddch(twin,num,win_width-1,ACS_RTEE);
    mvwhline(twin, num, 1, ACS_HLINE, win_width-2);

    mvwaddch(twin, 2, 0, ACS_LTEE);
    mvwhline(twin, 2, 1, ACS_HLINE, win_width-2);
    mvwaddch(twin, 2, win_width-1, ACS_RTEE);
/*
	mvwaddch(twin,num,0,ACS_LTEE);
	mvwaddch(twin,num,38,ACS_RTEE);
    mvwhline(twin, num, 1, ACS_HLINE, 38);

    mvwaddch(twin, 2, 0, ACS_LTEE);
    mvwhline(twin, 2, 1, ACS_HLINE, 38);
    mvwaddch(twin, 2, 39, ACS_RTEE);
*/
/* Post the menu */
    post_menu(my_menu);
    wrefresh(twin);

//    attron(COLOR_PAIR(2));
//    mvprintw(LINES - 2, 0, "Use PageUp and PageDown to scoll down or up a page of items");
//    mvprintw(LINES - 1, 0, "Arrow Keys to navigate (F1 to Exit)");
//    mvprintw(LINES - 1, 0,"num = %d ",num);
//    attroff(COLOR_PAIR(2));
    refresh();

//	disp_msg(twin,"demo forms\0",num+5);

	while(!finished)
	{
		c = wgetch(twin);

	    switch(c)
	    {
	        case KEY_DOWN:
	            menu_driver(my_menu, REQ_DOWN_ITEM);
//				disp_msg(twin,"key down\0",num);
	            break;
	        case KEY_UP:
	            menu_driver(my_menu, REQ_UP_ITEM);
//				disp_msg(twin,"key up\0",num);
	            break;
	        case KEY_NPAGE:
	            menu_driver(my_menu, REQ_SCR_DPAGE);
//				disp_msg(twin,"page down\0",num);
	            break;
	        case KEY_PPAGE:
	            menu_driver(my_menu, REQ_SCR_UPAGE);
//				disp_msg(twin,"page up\0",num);
	            break;
			case 10: /* Enter */
				cur_item = current_item(my_menu);
				index = item_index(cur_item);
//				mvprintw(LINES - 1, 2,"%d %s       ",index,my_items[index]->name.str);
				strcpy(filename,my_items[index]->name.str);
				chptr = filename;
				i = 0;
				do{
					i++;
					chptr++;
				}while(*chptr != 0x20 && i < 40);
				*chptr = 0;
//				refresh();
				finished = 1;
				break;
			case KEY_F(2):
				finished = 1;
				disp_msg(twin,"F2\0",num);
				break;
			case KEY_F(3):
				disp_msg(twin,"F3\0",num);
				break;
			case KEY_F(4):
				disp_msg(twin,"F4\0",num);
				break;
			case KEY_F(5):
				disp_msg(twin,"F5\0",num);
				break;
			case KEY_F(6):
				disp_msg(twin,"F6\0",num);
				break;
			default:
				break;
	    }

//	    cur_item = current_item(my_menu);
//	    index = item_index(cur_item);
//		mvwprintw(twin,2,2,"index: %d",index);
	}

//	mvprintw(LINES - 20, 2,"index: %d finished: %d",index,finished);
//  wrefresh(twin);
//	mvprintw(LINES - 20, 2,"                            ");
//     wrefresh(twin);
/* Unpost and free all the memory taken up */
//	attron(COLOR_PAIR(2));
//	attroff(COLOR_PAIR(2));
//	getch();
    unpost_menu(my_menu);
    free_menu(my_menu);
	for(i = 0; i < num; ++i)
		free_item(my_items[i]);
//    endwin();
//	delwin(twin);
	werase(twin);
	wrefresh(twin);
	refresh();
    return index;
}

/*
void func(IO_DATA *cur)
{
	nonl();
//	intrflush(stdscr, FALSE);
//	keypad(stdscr, TRUE);
	demo_forms(cur);
//	move(LINES-18, 0);
//	clrtoeol();
//	mvprintw(LINES-18, 0, "Item selected is : %s", name);
}
*/
static void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string, chtype color)
{
    int length, x, y;
    float temp;

    if(win == NULL)
        win = stdscr;
    getyx(win, y, x);
    if(startx != 0)
        x = startx;
    if(starty != 0)
        y = starty;
    if(width == 0)
        width = 80;

    length = strlen(string);
    temp = (width - length)/ 2;
    x = startx + (int)temp;
    wattron(win, color);
    mvwprintw(win, y, x, "%s", string);
    wattroff(win, color);
    refresh();
}


