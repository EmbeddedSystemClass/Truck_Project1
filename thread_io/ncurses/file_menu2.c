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

#define NUM_DAT_NAMES 20

static char dat_names[NUM_DAT_NAMES][20];

//******************************************************************************************//
//************************************** menu_scroll2 **************************************//
//******************************************************************************************//

int file_menu2(int which, char *ret_str)
{
    ITEM *cur_item;
    int c;
    MENU *menu;
    WINDOW *win;
    int i,j,k;
    I_DATA *curr;
    I_DATA *pio = curr;
    I_DATA *tpio = curr;
    int index = 0;
    int finished = 0;
    char tempx[40];
    int num;
    char *chp;
    int format;
	struct dirent **namelist;
	DIR *d;
	struct dirent *dir;

/* Initialize curses */
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_CYAN, COLOR_BLACK);

//	num = scandir(".", &namelist, 0, alphasort);
//	my_items = (ITEM **)calloc(num + 1, sizeof(ITEM *));

//	mvprintw(LINES-20,2,"num: %d ",num);

	d = opendir( "." );
	if( d == NULL )
	{
		return -1;
	}
	i = 0;
	num = 0;

	memset(dat_names,0,NUM_DAT_NAMES*20);

	k = 0;
	while( ( dir = readdir( d ) ) && num < NUM_DAT_NAMES-1)
	{
		if(strcmp( dir->d_name, "." ) == 0 ||  strcmp( dir->d_name, ".." ) == 0 || dir->d_type == DT_DIR)
		  continue;

		memset(tempx,0,sizeof(tempx));
		strcpy(tempx,dir->d_name);
//		format = GetFileFormat(tempx);

		chp = tempx;
		j = 0;

		while(*chp++ != '.' && j < 30)
			j++;

		strncpy(tempx,chp,j+1);

//		mvprintw(LINES - 21-i++, 5,"%d  %s  ",j,tempx);
	    if(dir->d_type == DT_REG && strcmp(tempx,"dat") == 0 )
	    {
	    	if(GetFileFormat(dir->d_name) == which)
				strcpy(dat_names[num++],dir->d_name);
//			mvprintw(LINES - 21-i++, 0,"%s  %s  %d",dir->d_name,dat_names[k-1],j);
		}
	}
	closedir( d );

	my_items = (ITEM **)calloc(num + 2, sizeof(ITEM *));

	for(i = 0;i < num;i++)
	{
//		mvprintw(LINES - 21-i, 0,"%s  ",dat_names[i]);
		my_items[i] = new_item(dat_names[i],"input");
	}

	my_items[i++] = new_item((char*)NULL,(char *)NULL);
	my_items[i] = (ITEM *)NULL;
	num += 2;

    menu = new_menu((ITEM **)my_items);

/* Create the window to be associated with the menu */
	num = (num < NUM_DAT_NAMES?num:NUM_DAT_NAMES);
    win = newwin(num+3, MENU_WIDTH, 0, 40);
    keypad(win, TRUE);

/* Set main window and sub window */
    set_menu_win(menu, win);
    set_menu_sub(menu, derwin(win, num-3, MENU_WIDTH-3, 3, 1));
    set_menu_format(menu, num-3, 0);

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

    mvwaddch(win, 2, 0, ACS_LTEE);
    mvwhline(win, 2, 1, ACS_HLINE, MENU_WIDTH-2);
    mvwaddch(win, 2, MENU_WIDTH-1, ACS_RTEE);

	mvwaddch(win,num,0,ACS_LTEE);
	mvwaddch(win,num,MENU_WIDTH-2,ACS_RTEE);
    mvwhline(win, num, 1, ACS_HLINE, MENU_WIDTH-2);

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
//	if(which)
	if(1)
	{
		while((c = wgetch(win)) != KEY_F(2) && c != 10)
		{
		    switch(c)
		    {
		        case KEY_DOWN:
		            menu_driver(menu, REQ_DOWN_ITEM);
//					mvprintw(LINES - 19, 2,"down                ");
//					refresh();
		            break;
		        case KEY_UP:
		            menu_driver(menu, REQ_UP_ITEM);
//					mvprintw(LINES - 19, 2,"up                  ");
//					refresh();
		            break;
		        case KEY_NPAGE:
		            menu_driver(menu, REQ_SCR_DPAGE);
//					mvprintw(LINES - 19, 2,"page up                  ");
		            break;
		        case KEY_PPAGE:
		            menu_driver(menu, REQ_SCR_UPAGE);
//					mvprintw(LINES - 19, 2,"page down                  ");
		            break;
				default:
					break;
		    }
		    cur_item = current_item(menu);
		    index = item_index(cur_item);
		    memset(ret_str,0,20);
		    strcpy(ret_str,item_name(cur_item));
//			mvprintw(LINES - 20, 2,"%s    %d   ",ret_str,index);
			refresh();
		}
	}

//	mvprintw(LINES - 20, 2,"index: %d finished: %d %s",index,finished,ret_str);
//    wrefresh(win);

    unpost_menu(menu);
    free_menu(menu);
	for(i = 0; i < num; i++)
		free_item(my_items[i]);
    werase(win);
    wrefresh(win);
    refresh();
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
  while( ( dir = readdir( d ) ) )
  {
    if( strcmp( dir->d_name, "." ) == 0 ||  strcmp( dir->d_name, ".." ) == 0 )
    {
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



#if 0
	do
	{
		mvprintw(LINES-j, 0,"%s  %s  %s %d",dat_names[j],tempx,dir->d_name,i);

		my_items[i] = new_item(dat_names[i],"input");
		i++;

	}while(i < k && i < NUM_DAT_NAMES);
#endif

#if 0
			if(format == 0)
				my_items[i++] = new_item(dir->d_name,"input");
			else if(format == 1) my_items[i++] = new_item(dir->d_name,"output");
			else
			{
				my_items[i++] = new_item(dir->d_name,"bad format");
				item_opts_off(my_items[i-1],O_SELECTABLE);
			}
#endif



#endif
