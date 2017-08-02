#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/param.h>
#include "../queue/illist_threads_rw.h"
#include "../queue/ollist_threads_rw.h"
#include "../ioports.h"
#include "test.priv.h"
#include <menu.h>
#include "setiodata.h"

//void func(IO_DATA *cur);

//#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

//extern void demo_forms(IO_DATA *cur);

static void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string, chtype color);
static ITEM **my_items;
extern int GetFileFormat(char *filename);
#define MENU_WIDTH 29

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
int file_menu2(int which, char *ret_str)
{
    ITEM *cur_item;
    int c;
    MENU *menu;
    WINDOW *win;
    int i,j;
    I_DATA *curr;
    I_DATA *pio = curr;
    I_DATA *tpio = curr;
    int index = 0;
    int finished = 0;
    char tempx[30];
    char tempx2[6];
    int num;
    char *chp;

/* Initialize curses */
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_CYAN, COLOR_BLACK);
    int format;

   struct dirent **namelist;

	num = scandir(".", &namelist, 0, alphasort);
	my_items = (ITEM **)calloc(num + 1, sizeof(ITEM *));

	DIR *d;
	struct dirent *dir;
	d = opendir( "." );
	if( d == NULL )
	{
		return 1;
	}
	i = 0;

	while( ( dir = readdir( d ) ) )
	{
		strcpy(tempx,dir->d_name);
		format = GetFileFormat(tempx);
		tempx[29] = 0;
		chp = tempx;
		j = 0;
		while(*chp++ != '.' && j < 30)
			j++;
		strncpy(tempx2,chp,5);
//		mvprintw(LINES - 21-i, 0,"%s  %s  %d",tempx2,tempx,j);
		strncpy(tempx,chp,4);
	    if(dir->d_type == DT_REG && strcmp(tempx2,"dat") == 0 )
//			my_items[i++] = new_item(dir->d_name,dir->d_name);
			if(format == 0)
				my_items[i++] = new_item(dir->d_name,"input");
			else if(format == 1) my_items[i++] = new_item(dir->d_name,"output");
			else
			{
				my_items[i++] = new_item(dir->d_name,"format?");
				item_opts_off(my_items[i-1],O_SELECTABLE);
			}
	}
	my_items[i++] = new_item((char*)NULL,(char *)NULL);
	closedir( d );
	my_items[i] = (ITEM *)NULL;

    menu = new_menu((ITEM **)my_items);

/* Create the window to be associated with the menu */
	num = (num < 20?num:20);
    win = newwin(num+3, MENU_WIDTH, 5, 40);
    keypad(win, TRUE);

/* Set main window and sub window */
    set_menu_win(menu, win);
    set_menu_sub(menu, derwin(win, num-3, MENU_WIDTH-2, 4, 1));
    set_menu_format(menu, num-3, 1);

/* Set menu mark to the string " * " */
    set_menu_mark(menu, " * ");

/* Print a border around the main window and print a title */
    box(win, 0, 0);

	if(which == 1)
	{
	    print_in_middle(win, 1, 0, MENU_WIDTH, "Choose Input", COLOR_PAIR(1));
	    disp_msg(win,"F2 to exit\0",num);
	}
	else if(which == 2)
	{
	    print_in_middle(win, 1, 0, MENU_WIDTH, "Choose Output", COLOR_PAIR(1));
	    disp_msg(win,"F2 to exit; F3 to turn on; F4 off\0",num);
	}

	mvwaddch(win,num,0,ACS_LTEE);
	mvwaddch(win,num,MENU_WIDTH-2,ACS_RTEE);
    mvwhline(win, num, 1, ACS_HLINE, MENU_WIDTH-2);

    mvwaddch(win, 2, 0, ACS_LTEE);
    mvwhline(win, 2, 1, ACS_HLINE, MENU_WIDTH-2);
    mvwaddch(win, 2, MENU_WIDTH-1, ACS_RTEE);

/* Post the menu */
    post_menu(menu);
    wrefresh(win);

//    attron(COLOR_PAIR(2));
//    mvprintw(LINES - 2, 0, "Use PageUp and PageDown to scoll down or up a page of items");
//    mvprintw(LINES - 1, 0, "Arrow Keys to navigate (F1 to Exit)");
//    mvprintw(LINES - 1, 0,"num = %d ",num);
//    attroff(COLOR_PAIR(2));
    refresh();

// if which == 1 then just return the index
	if(which)
	{
		while((c = wgetch(win)) != KEY_F(2))
		{
		    switch(c)
		    {
		        case KEY_DOWN:
		            menu_driver(menu, REQ_DOWN_ITEM);
		            break;
		        case KEY_UP:
		            menu_driver(menu, REQ_UP_ITEM);
		            break;
		        case KEY_NPAGE:
		            menu_driver(menu, REQ_SCR_DPAGE);
		            break;
		        case KEY_PPAGE:
		            menu_driver(menu, REQ_SCR_UPAGE);
		            break;
//				case 10: /* Enter */
				case KEY_F(3):
					break;
				case KEY_F(4):
					disp_msg(win,"output off\0",num);
					break;
		    }
		    cur_item = current_item(menu);
		    index = item_index(cur_item);
		    strcpy(ret_str,item_name(cur_item));
		}
	}
	mvprintw(LINES - 20, 2,"index: %d finished: %d",index,finished);
    wrefresh(win);

    unpost_menu(menu);
    free_menu(menu);
	refresh();
    for(i = 0; i < num; ++i)
        free_item(my_items[i]);
    endwin();
    return index;
}

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

#if 0
static int walker( char *searching, char *result )
{
  DIR           *d;
  struct dirent *dir;
  d = opendir( "." );
  if( d == NULL ) {
    return 1;
  }
  while( ( dir = readdir( d ) ) ) {
    if( strcmp( dir->d_name, "." ) == 0 ||
        strcmp( dir->d_name, ".." ) == 0 ) {
      continue;
    }
    if( dir->d_type == DT_DIR ) {
      chdir( dir->d_name );
      walker( searching, result );
      chdir( ".." );
    } else {
      if( strcmp( dir->d_name, searching ) == 0 ) {
        int  len;
        getcwd( result, MAXPATHLEN );
        len = strlen( result );
        snprintf( result + len, MAXPATHLEN - len, "/%s", dir->d_name );
        break;
      }
    }
  }
  closedir( d );
  return *result == 0;
}
#endif
