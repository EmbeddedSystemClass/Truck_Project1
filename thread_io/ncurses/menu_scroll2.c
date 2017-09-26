#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../queue/illist_threads_rw.h"
#include "../queue/ollist_threads_rw.h"
#include "../ioports.h"
#include "test.priv.h"
#include <menu.h>

//void func(IO_DATA *cur);

//#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

extern void demo_forms(void *cur,int which);

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
//int menu_scroll2(I_DATA *curr,int num,int which)
int menu_scroll2(void *curr,int num,int which)
{
    ITEM **my_items;
    ITEM *cur_item;
    int c;
    MENU *my_menu;
    WINDOW *twin;
    int i;
    I_DATA *pi = (I_DATA *)curr;
    O_DATA *po = (O_DATA *)curr;
    void *pcurr;
    int index = 0x7f;
    int finished = 0;
    char tempx[6];

/* Initialize curses */
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_CYAN, COLOR_BLACK);

	if(which == 1)
		pi = (I_DATA *)curr;
	else if(which == 0)
		po = (O_DATA *)curr;

//	char *sup_string = (char *)calloc(num+1,7);
	char sup_string[41][20];

/* Create items */
    my_items = (ITEM **)calloc(num+1, sizeof(ITEM *));

	if(which == 1)
	{
		for(i = 0; i < num; ++i)
		{
			sprintf(tempx,"%d",pi->port);
			mvprintw(LINES - 21-i, 0,"%s ",tempx);
			strcpy(sup_string[i],tempx);
			sprintf(tempx,"%d",pi->affected_output);
			strcat(sup_string[i],"  ");
			strcat(sup_string[i],tempx);
			sprintf(tempx,"%d",pi->type);
			strcat(sup_string[i],"  ");
			strcat(sup_string[i],tempx);
			sprintf(tempx,"%d",pi->inverse);
			strcat(sup_string[i],"  ");
			strcat(sup_string[i],tempx);
			mvprintw(LINES - 21-i, 0,"%s ",sup_string[i]);
			pi++;
		}
		pi = (I_DATA *)curr;
	}
	else if(which == 0)
	{
		for(i = 0; i < num; ++i)
		{
			sprintf(tempx,"%d",po->port);
			strcpy(sup_string[i],tempx);
			sprintf(tempx,"%d",po->onoff);
			strcat(sup_string[i],"  ");
			strcat(sup_string[i],tempx);
			po++;
		}
		po = (O_DATA *)curr;

	}
    for(i = 0; i < num; ++i)
    {
		if(which == 1)
		{
			my_items[i] = new_item(pi->label, sup_string[i]);
			pi++;
	    }
	    else if(which == 0)
		{
			my_items[i] = new_item(po->label, sup_string[i]);
			po++;
	    }
    }
	my_items[++i] = new_item((char*)NULL,(char *)NULL);
	my_items[i] = (ITEM *)NULL;

/* Create menu */
//	mvprintw(LINES-1, 0,"done");
//	getch();
    my_menu = new_menu((ITEM **)my_items);

/* Create the window to be associated with the menu */
    twin = newwin(num+3, 40, 4, 40);
    keypad(twin, TRUE);

/* Set main window and sub window */
    set_menu_win(my_menu, twin);
    set_menu_sub(my_menu, derwin(twin, num-3, 38, 3, 1));
    set_menu_format(my_menu, num-3, 1);

/* Set menu mark to the string " * " */
    set_menu_mark(my_menu, " * ");

/* Print a border around the main window and print a title */
    box(twin, 0, 0);

	if(which == 1)
	{
	    print_in_middle(twin, 1, 0, 40, "Choose Input", COLOR_PAIR(1));
	    disp_msg(twin,"F2 to exit\0",num);
	}
	else if(which == 0)
	{
	    print_in_middle(twin, 1, 0, 40, "Choose Output", COLOR_PAIR(1));
	    disp_msg(twin,"F2 to exit; F3 to turn on; F4 off\0",num);
	}

	mvwaddch(twin,num,0,ACS_LTEE);
	mvwaddch(twin,num,38,ACS_RTEE);
    mvwhline(twin, num, 1, ACS_HLINE, 38);

    mvwaddch(twin, 2, 0, ACS_LTEE);
    mvwhline(twin, 2, 1, ACS_HLINE, 38);
    mvwaddch(twin, 2, 39, ACS_RTEE);

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
	            break;
	        case KEY_UP:
	            menu_driver(my_menu, REQ_UP_ITEM);
	            break;
	        case KEY_NPAGE:
	            menu_driver(my_menu, REQ_SCR_DPAGE);
	            break;
	        case KEY_PPAGE:
	            menu_driver(my_menu, REQ_SCR_UPAGE);
	            break;
			case 10: /* Enter */
				cur_item = current_item(my_menu);
				index = item_index(cur_item);
				disp_msg(twin,"demo forms\0",num);
				if(which == 1)
				{
					pi = (I_DATA *)curr;
					for(i = 0;i < index;i++)
						pi++;
					pcurr = (void*)pi;
				}
				else if(which == 0)
				{
					po = (O_DATA *)curr;
					for(i = 0;i < index;i++)
						po++;
					pcurr = (void*)po;
				}
				demo_forms(pcurr,which);
				break;
			case KEY_F(2):
				finished = 1;
				break;
			case KEY_F(3):
				cur_item = current_item(my_menu);
				index = item_index(cur_item);
				if(which == 1)
				{
				    disp_msg(twin,"not valid\0",num);
				}
				else if(which == 0)
				{
					disp_msg(twin,"ouput on\0",num);
					for(i = 0;i < 40;i++)
					{
						mvprintw(LINES -63+i, 80,"                                  ");
					}
					po = (O_DATA *)curr;

					for(i = 0;i < index;i++)
					{
//						mvprintw(LINES -63+i, 80,"%s %d ",po->label,po->onoff);
//						wrefresh(twin);
						po++;
					}

					po->onoff = 1;
					pcurr = (void*)po;
				}
				break;
			case KEY_F(4):
				cur_item = current_item(my_menu);
				index = item_index(cur_item);
				if(which == 1)
				{
				    disp_msg(twin,"not valid\0",num);
				}
				else if(which == 0)
				{
					disp_msg(twin,"ouput off\0",num);
					for(i = 0;i < 40;i++)
					{
						mvprintw(LINES -63+i, 80,"                                  ");
					}
					po = (O_DATA *)curr;

					for(i = 0;i < index;i++)
					{
//						mvprintw(LINES  -63+i, 80,"%s %d ",po->label,po->onoff);
//						wrefresh(twin);
						po++;
					}

					po->onoff = 0;
					pcurr = (void*)po;
				}
				break;
			case KEY_F(5):
				disp_msg(twin,"F5\0",num);
				break;
			default:
				break;
	    } 
	    
//	    cur_item = current_item(my_menu);
//	    index = item_index(cur_item);
//		mvwprintw(twin,2,2,"index: %d",index);
	}

	mvprintw(LINES - 20, 2,"index: %d finished: %d",index,finished);
    wrefresh(twin);
//	mvprintw(LINES - 20, 2,"                            ");
//     wrefresh(twin);
/* Unpost and free all the memory taken up */
//	attron(COLOR_PAIR(2));
//	attroff(COLOR_PAIR(2));
	getch();
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


