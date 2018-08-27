#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../queue/illist_threads_rw.h"
#include "../queue/ollist_threads_rw.h"
#include "../ioports.h"
#include "test.priv.h"
#include <menu.h>
#include "client.h"

#define OWIN_WIDTH 66
#define IWIN_WIDTH 31

extern illist_t ill;
extern ollist_t oll;

extern illist_t ill2;
extern ollist_t oll2;

extern int tcp_connected;

extern int demo_forms(void *cur,int which, int index);

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
int menu_scroll2(int num,int which,char *filename)
{
	ITEM **my_items;
	ITEM *cur_item;
	int c;
	MENU *my_menu;
	WINDOW *twin;
	int i;
	I_DATA i_data;
	O_DATA o_data;
	I_DATA *pi = &i_data;
	O_DATA *po = &o_data;
	void *pcurr;
	int index = -1;
	int finished = 0;
	char tempx[6];
	UCHAR outc;
	char errmsg[40];
	int win_width;

/* Initialize curses */
/*
	initscr();
	start_color();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_CYAN, COLOR_BLACK);
*/
//	char *sup_string = (char *)calloc(num+1,7);
	char sup_string[41][50];
	memset(sup_string,0,41*50);

/* Create items */
	my_items = (ITEM **)calloc(num+1, sizeof(ITEM *));

	if(which == EDIT_IDATA || which == EDIT_IDATA2)
	{
		win_width = IWIN_WIDTH;
		mvprintw(LINES - 21, 0,"%ld ",sizeof(I_DATA));
		for(i = 0; i < num; ++i)
		{
			if(which == EDIT_IDATA)
				illist_find_data(i,&pi,&ill);
			else if(which == EDIT_IDATA2)
				illist_find_data(i,&pi,&ill2);

			sprintf(tempx,"%2d",pi->port);
//			mvprintw(LINES - 21-i, 0,"%s ",tempx);
			strcpy(sup_string[i],tempx);
			sprintf(tempx,"%2d",pi->affected_output);
			strcat(sup_string[i],"   ");
			strcat(sup_string[i],tempx);
//			sprintf(tempx,"%d",pi->type);
//			strcat(sup_string[i],"  ");
//			strcat(sup_string[i],tempx);
//			sprintf(tempx,"%d",pi->inverse);
//			strcat(sup_string[i],"  ");
//			strcat(sup_string[i],tempx);
//			mvprintw(LINES - 10-i, 0,"%s                 ",sup_string[i]);
		}
	}
	else if(which == EDIT_ODATA || which == EDIT_ODATA2)
	{
		win_width = OWIN_WIDTH;
		for(i = 0; i < num; ++i)
		{
			if(which == EDIT_ODATA)
				ollist_find_data(i,&po,&oll);
			else if(which == EDIT_ODATA2)
				ollist_find_data(i,&po,&oll2);

			sprintf(tempx,"%2d ",po->port);
			strcpy(sup_string[i],"     ");
			strcat(sup_string[i],tempx);
			strcat(sup_string[i],"    ");

			if(po->onoff != 0)
				strcpy(tempx, "ON    ");
			else strcpy(tempx,"OFF   ");
			strcat(sup_string[i],tempx);

			sprintf(tempx,"%2d",po->type);
			strcat(sup_string[i],tempx);
			strcat(sup_string[i],"   ");

			sprintf(tempx,"%4d",po->time_delay);
			strcat(sup_string[i],tempx);
			strcat(sup_string[i],"   ");

			sprintf(tempx,"%4d",po->pulse_time);
			strcat(sup_string[i],"   ");
			strcat(sup_string[i],tempx);

//			mvprintw(LINES - 2-i, 0,"%s      ",sup_string[i]);
//			mvprintw(LINES - 2-i, 30,"%d ",strlen(sup_string[i]));
		}
	}
#if 0
	else if(which == EDIT_IDATA)
	{
		win_width = IWIN_WIDTH;
		for(i = 0; i < num; ++i)
		{
			sprintf(tempx,"%2d",po->port);
			strcat(sup_string[i],"  ");
			strcpy(sup_string[i],tempx);
			strcat(sup_string[i],"   ");

			if(po->onoff > 0)
				sprintf(tempx,"ON");
			else
				sprintf(tempx,"OFF");
			strcat(sup_string[i],tempx);
/*
			sprintf(tempx,"%2d",po->type);
			strcat(sup_string[i],tempx);
			strcat(sup_string[i],"  ");

			sprintf(tempx,"%4d",po->time_delay);
			strcat(sup_string[i],tempx);
			strcat(sup_string[i],"  ");

			sprintf(tempx,"%4d",po->pulse_time);
			strcat(sup_string[i],"  ");
strcat(sup_string[i],tempx);
*/
//			mvprintw(LINES - 10-i, 0,"%s              ",sup_string[i]);
//			mvprintw(LINES - 10-i, 30,"%d ",strlen(sup_string[i]));
		}
	}
#endif

	for(i = 0; i < num; ++i)
	{
		if(which == EDIT_IDATA)
		{
			illist_find_data(i,&pi,&ill);
			my_items[i] = new_item(pi->label, sup_string[i]);
		}
		else if(which == EDIT_IDATA2)
		{
			illist_find_data(i,&pi,&ill2);
			my_items[i] = new_item(pi->label, sup_string[i]);
		}
		else if(which == EDIT_ODATA)
		{
			ollist_find_data(i,&po,&oll);
			my_items[i] = new_item(po->label, sup_string[i]);
		}
		else if(which == EDIT_ODATA2)
		{
			ollist_find_data(i,&po,&oll2);
			my_items[i] = new_item(po->label, sup_string[i]);
		}
	}
	my_items[++i] = new_item((char*)NULL,(char *)NULL);
	my_items[i] = (ITEM *)NULL;

/* Create menu */
//	mvprintw(LINES-1, 0,"done");
//	getch();
	my_menu = new_menu((ITEM **)my_items);

/* Create the window to be associated with the menu */

	twin = newwin(num+3, win_width+2, 2, 33);
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

	print_in_middle(twin, 1, 2, 40, filename, COLOR_PAIR(1));

	mvwaddch(twin,num,0,ACS_LTEE);
	mvwaddch(twin,num,win_width+1, ACS_RTEE);
	mvwhline(twin, num, 1, ACS_HLINE, win_width);

	mvwaddch(twin, 2, 0, ACS_LTEE);
	mvwhline(twin, 2, 1, ACS_HLINE, win_width);
	mvwaddch(twin, 2, win_width+1, ACS_RTEE);
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
				disp_msg(twin,"key down\0",num);
				break;
			case KEY_UP:
				menu_driver(my_menu, REQ_UP_ITEM);
				disp_msg(twin,"key up\0",num);
				break;
			case KEY_NPAGE:
				menu_driver(my_menu, REQ_SCR_DPAGE);
				disp_msg(twin,"page down\0",num);
				break;
			case KEY_PPAGE:
				menu_driver(my_menu, REQ_SCR_UPAGE);
				disp_msg(twin,"page up\0",num);
				break;
			case 10:							  /* Enter */
				cur_item = current_item(my_menu);
				index = item_index(cur_item);

				switch(which)
				{
					case EDIT_ODATA:			  // 0_DATA (choose)
						ollist_find_data(index,&po,&oll);
//					    mvprintw(LINES - 2, 2,"demo_forms: %s",po->label);
//						refresh();
//						getch();
						if(demo_forms(po,which,index) != TRUE)
						{
							finished = 1;
							ollist_insert_data(index,&oll,po);
						}
						disp_msg(twin,"EDIT_ODATA\0",num);
						break;
					case EDIT_IDATA:			  // I_DATA
						illist_find_data(index,&pi,&ill);
//					    mvprintw(LINES - 2, 2,"demo_forms: %s",pi->label);
//						refresh();
//						getch();
						if(demo_forms(pi,which,index) != TRUE)
						{
							finished = 1;
							illist_insert_data(index,&ill,pi);
						}
						disp_msg(twin,"EDIT_IDATA\0",num);
						break;
					case EDIT_ODATA2:			  // 0_DATA (choose)
						ollist_find_data(index,&po,&oll2);
//					    mvprintw(LINES - 2, 2,"demo_forms: %s",po->label);
//						refresh();
//						getch();
						if(demo_forms(po,which,index) != TRUE)
						{
							finished = 1;
							ollist_insert_data(index,&oll2,po);
						}
						disp_msg(twin,"EDIT_ODATA\0",num);
						break;
					case EDIT_IDATA2:			  // I_DATA
						illist_find_data(index,&pi,&ill2);
//					    mvprintw(LINES - 2, 2,"demo_forms: %s",pi->label);
//						refresh();
//						getch();
						if(demo_forms(pi,which,index) != TRUE)
						{
							finished = 1;
							illist_insert_data(index,&ill2,pi);
						}
						disp_msg(twin,"EDIT_IDATA\0",num);
						break;
					case TOGGLE_OUTPUTS:		  // O_DATA (toggle output)
						ollist_find_data(index,&po,&oll);
						if(po->onoff > 0)
							po->onoff = 0;
						else
							po->onoff = 1;
						disp_msg(twin,"TOGGLE_OUTPUTS\0",num);
						if(tcp_connected)
						{
							outc = TOGGLE_OUTPUTS;
							put_sock(&outc,1,1,errmsg);
							outc = (UCHAR)index;
							put_sock(&outc,1,1,errmsg);
							put_sock(&po->onoff,1,1,errmsg);
						}
						finished = 1;
						break;
					default:
						finished = 1;
						break;
				}

				break;
			case KEY_F(2):
				finished = 1;
				break;
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

//	mvprintw(LINES - 20, 2,"index: %d finished: %d",index,finished);
	wrefresh(twin);
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
