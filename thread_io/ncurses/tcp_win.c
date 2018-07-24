#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "../../mytypes.h"
#include "test.priv.h"
#include "../queue/illist_threads_rw.h"
#include "../queue/ollist_threads_rw.h"
#include "../ioports.h"
#include <ncurses.h>
#include "client.h"

#define SERIAL_HEIGHT 50
#define SERIAL_WIDTH 71

#define SHOW_DATA_HEIGHT 47
#define SHOW_DATA_WIDTH 47

static int status_line;
static int error_line1;
static int error_line2;

static int inc_y(WINDOW *win, int y);
static void clr_scr(WINDOW *win);
static void disp_msg(WINDOW *win,char *str);

extern int tcp_connected;

//******************************************************************************************//
//************************************* clear_screen ***************************************//
//******************************************************************************************//
static void clr_scr(WINDOW *win)
{
	int y;
	for(y = 1;y < status_line - 1;y++)
	{
		wmove(win, y, 1);
		wclrtoeol(win);
		box(win,ACS_VLINE,ACS_HLINE);
		wrefresh(win);
	}
}

//******************************************************************************************//
static int inc_y(WINDOW *win, int y)
{
	if(++y > status_line - 2)
	{
		clr_scr(win);
		y = 1;
	}
	return y;
}

//******************************************************************************************//
//*************************************** disp_msg *****************************************//
//******************************************************************************************//
static void disp_msg(WINDOW *win,char *str)
{
	wmove(win,error_line2,1);
	// this overwrites the vert line char at the end of line
	wclrtoeol(win);
	mvwprintw(win,error_line2,1,"%s ",str);
}

//******************************************************************************************//
//**************************************** tcp_win *****************************************//
//******************************************************************************************//
int tcp_win(int cmd)
{
	WINDOW *twin;
	int startx, starty;
	int ch;
	int rc;
	int x,y,i;
	UCHAR ret_char = 0x21;
	char buffer[100];
	char errmsg[50];
	int noerrors = 0;
	int ascii = 1;
	int no_non_ascii = 0; 
	int height, width;

	keypad(stdscr, TRUE);		/* I need that nifty F1 	*/
	nodelay(stdscr,TRUE);
	if(cmd == SHOW_IDATA || cmd == SHOW_ODATA)
	{
		height = SHOW_DATA_HEIGHT;
		width = SHOW_DATA_WIDTH;
		starty = ((LINES - height)/3);
	}
	else if(cmd == TCP_WINDOW_ON)
	{
		height = SERIAL_HEIGHT;
		width = SERIAL_WIDTH;
		starty = ((LINES - height)/2);	/* Calculating for a center placement */
	}

	status_line = height-4;
	error_line1 = height-3;
	error_line2 = height-2;

	startx = ((COLS - width)/2)+15;	/* of the window		*/
//	printw("Press F1 to exit");
	refresh();
	twin = newwin(height, width, starty, startx);
	keypad(twin,TRUE);
	nodelay(twin,TRUE);
	box(twin,ACS_VLINE,ACS_HLINE);

	MvWAddCh(twin,height-5,0,ACS_LTEE);
	MvWAddCh(twin,height-5,width-1,ACS_RTEE);
	for(i = 1;i < width-1;i++)
		MvWAddCh(twin,height-5,i,ACS_HLINE);

	mvwprintw(twin,status_line,1,"F2 - quit; F3 - toggle ascii");

//	mvwprintw(twin,2,2,"starting server...");
	wrefresh(twin);
	x = 1;
	y = 1;
	while((ch = wgetch(twin)) != KEY_F(2))
	{
		switch(ch)
		{
			case KEY_F(3):
				if(ascii == 0)
				{
					ascii = 1;
					mvwprintw(twin,status_line,30,"ascii on  ");
				}
				else
				{ 
					ascii = 0;
					x = width;
					y++;
					mvwprintw(twin,status_line,30,"ascii off  ");
				}
				break;

			case KEY_F(4):
				clr_scr(twin);
				x = y = 1;
				break;

			case KEY_F(5):
				break;

			case KEY_F(6):
				break;
			default:
				break;
		}
		if(tcp_connected == 1)
		{
			usleep(100);
			rc = get_sock(&ret_char,1,1,errmsg);
			if(rc < 0 && errno != 11)
			{
				x = 1;
				y = height - 2;
				mvwprintw(twin,y,x,"%s             ",errmsg);
				wrefresh(twin);
				delwin(twin);
				return 1;
			}
			if(errno == 11)
			{
				noerrors++;
//				mvwprintw(twin,error_line2,1,"%s %d  %d           ",errmsg,errno,noerrors);
// 				wrefresh(twin);
				errno = 0;
			}else
			{
				x++;
				if(ascii)
				{
					if(ret_char != 0)
					{
						if(ret_char < 0x20 || ret_char > 0x7e)
						{
							if(++no_non_ascii > 50)
							{
								ascii = 0;
								no_non_ascii = 0;
							}
							mvwprintw(twin,y,x,"_");
						}
						else 
							mvwprintw(twin,y,x,"%c",ret_char);
					}
					else x = width;
				}else mvwprintw(twin,y,(x+=2),"%2x ",ret_char);

				if(ascii)
				{
					if(x > width-3)
					{
						y = inc_y(twin,y);
						x = 1;
					}
//					mvwprintw(twin,error_line1,1,"%2d %2d   ",x,y);
					wrefresh(twin);
				}
				else
				{
					if(x > width-5)
					{
						y = inc_y(twin,y);
						x = 1;
					}
				}
				wrefresh(twin);
			}
		}
	}	// while getch() != F2
	werase(twin);
	wrefresh(twin);
	refresh();
	return 0;
}
