#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "../../mytypes.h"
#include "test.priv.h"
#include "../queue/illist_threads_rw.h"
#include "../queue/ollist_threads_rw.h"
#include "../ioports.h"
#include <ncurses.h>

#define HEIGHT 65
#define status_line HEIGHT-4
#define error_line1 HEIGHT-3
#define error_line2 HEIGHT-2
#define WIDTH 60
extern int file_menu2(int which, char *ret_str);

//******************************************************************************************//
//************************************* clear_screen ***************************************//
//******************************************************************************************//
static void clr_scr(WINDOW *win)
{
	int y,i;
	werase(win);
	for(y = 1;y < status_line - 1;y++)
	{
		box(win,ACS_VLINE,ACS_HLINE);
		wrefresh(win);
	}
	MvWAddCh(win,HEIGHT-5,0,ACS_LTEE);
	MvWAddCh(win,HEIGHT-5,WIDTH-1,ACS_RTEE);
	for(i = 1;i < WIDTH-1;i++)
		MvWAddCh(win,HEIGHT-5,i,ACS_HLINE);
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
//*************************************** file_win *****************************************//
//******************************************************************************************//
//int file_win(I_DATA *i_data, O_DATA *o_data)
int file_win(void)
{
	WINDOW *twin;
	int startx, starty;
	int ch;
	int rc;
	int x,y,i;
	char chc;
	char test = 0x21;
	char buffer[50];
	char errmsg[50];
	int noerrors = 0;

	keypad(stdscr, TRUE);		/* I need that nifty F1 	*/
//	nodelay(stdscr,TRUE);
	starty = (LINES - HEIGHT) / 2;	/* Calculating for a center placement */
	startx = (COLS - WIDTH) / 2;	/* of the window		*/
//	printw("Press F1 to exit");
	refresh();
	twin = newwin(HEIGHT, WIDTH, starty, startx);
	keypad(twin,TRUE);
//	nodelay(twin,TRUE);
	box(twin,ACS_VLINE,ACS_HLINE);

	MvWAddCh(twin,HEIGHT-5,0,ACS_LTEE);
	MvWAddCh(twin,HEIGHT-5,WIDTH-1,ACS_RTEE);
	for(i = 1;i < WIDTH-1;i++)
		MvWAddCh(twin,HEIGHT-5,i,ACS_HLINE);

	mvwprintw(twin,status_line,1,"F2 - quit F3 - send I_DATA F4 - send O_DATA");
	mvwprintw(twin,error_line1,1,"error msg line 1");
	mvwprintw(twin,error_line2,1,"error msg line 2");

	wrefresh(twin);

	x = 1;
	y = 20;
	while((ch = wgetch(twin)) != KEY_F(2))
	{
		switch(ch)
		{
			case KEY_F(3):
				disp_msg(twin,"key F3\0");
				nodelay(twin,FALSE);
				file_menu2(1, buffer);
				clr_scr(twin);
				mvwprintw(twin,5,1,"%s",buffer);
				wrefresh(twin);

				break;
			case KEY_F(4):
				disp_msg(twin,"key F4\0");
				x = 1;
				y = 10;
				nodelay(twin,TRUE);
				break;
			case KEY_F(5):
				disp_msg(twin,"key F5\0");
				break;
			case KEY_F(6):
				disp_msg(twin,"key F6\0");
				break;
			case KEY_F(7):
				disp_msg(twin,"key F7\0");
				break;
			case KEY_F(8):
				disp_msg(twin,"key F8\0");
				break;
			case KEY_F(9):
				disp_msg(twin,"key F9\0");
				break;
			case KEY_F(10):
				disp_msg(twin,"key F10 (don't try F11)\0");
				break;
				break;
			case 'a':
				break;
			case 'b':
				break;
		}
		wrefresh(twin);
	}
	delwin(twin);
	return 0;
}


