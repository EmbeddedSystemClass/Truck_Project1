#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "../mytypes.h"
#include "test.priv.h"
#include "../queue/illist_threads_rw.h"
#include "../queue/ollist_threads_rw.h"
#include "../ioports.h"
#include <ncurses.h>
#include "client.h"

#define HEIGHT 50
#define status_line HEIGHT-4
#define error_line1 HEIGHT-3
#define error_line2 HEIGHT-2
#define WIDTH 70

extern int init_server(void);
//extern int send_tcp(char *send_buf, int len, char *errmsg);
//extern int recv_tcp(char *recv_buf, int len, char *errmsg);
extern int tcp_connected;

//******************************************************************************************//
//************************************* clear_screen ***************************************//
//******************************************************************************************//
void clr_scr(WINDOW *win)
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
int tcp_win(I_DATA *i_data, O_DATA *o_data)
{
	WINDOW *twin;
	int startx, starty;
	int ch;
	int rc;
	int x,y,i;
	char chc;
	char test = 0x21;
	char buffer[100];
	char errmsg[50];
	int noerrors = 0;
	UCHAR cmd;
	UCHAR param;
	I_DATA *i_datap;
	O_DATA *o_datap;
	i_datap = i_data;
	o_datap = o_data;
	UCHAR num_i_recs = 0;
	UCHAR num_o_recs = 0;
	int finished;

	for(i = 0;i < NUM_PORT_BITS;i++)
	{
		if(i_datap->label[0] != 0)
			num_i_recs++;
		i_datap++;
	}

	for(i = 0;i < NUM_PORT_BITS;i++)
	{
		if(o_datap->label[0] != 0)
			num_o_recs++;
		o_datap++;
	}

	i_datap = i_data;
	o_datap = o_data;

	keypad(stdscr, TRUE);		/* I need that nifty F1 	*/
	nodelay(stdscr,TRUE);
	starty = (LINES - HEIGHT) / 2;	/* Calculating for a center placement */
	startx = (COLS - WIDTH) / 2;	/* of the window		*/
//	printw("Press F1 to exit");
	refresh();
	twin = newwin(HEIGHT, WIDTH, starty, startx);
	keypad(twin,TRUE);
	nodelay(twin,TRUE);
	box(twin,ACS_VLINE,ACS_HLINE);

	MvWAddCh(twin,HEIGHT-5,0,ACS_LTEE);
	MvWAddCh(twin,HEIGHT-5,WIDTH-1,ACS_RTEE);
	for(i = 1;i < WIDTH-1;i++)
		MvWAddCh(twin,HEIGHT-5,i,ACS_HLINE);

	mvwprintw(twin,status_line,1,"F2 - quit F3 - send I_DATA F4 - send O_DATA");
	mvwprintw(twin,error_line1,1,"%d",num_i_recs);
	mvwprintw(twin,error_line2,1,"%d",num_o_recs);

//	mvwprintw(twin,2,2,"starting server...");
	wrefresh(twin);
/*
	rc = init_server();
	if(rc < 0)
	{
		mvwprintw(twin,2,2,"server failed - press any key");
		if(rc == -2)
			mvwprintw(twin,3,2,"recv failed on opt");
		if(rc == -3)
			mvwprintw(twin,4,2,"send failed on opt");
		wrefresh(twin);
		getch();
		delwin(twin);
		return -1;
	}else
	{
		mvwprintw(twin,2,2,"connected       ");
		tcp_connected = 1;
	}

	wrefresh(twin);
*/
	x = 1;
	y = 1;
	finished = 1;
	while((ch = wgetch(twin)) != KEY_F(2) && finished != 0)
	{
#if 0
		if(ch > 0x21 && ch < 0x7e)
		{
			chc = (char)ch;
			rc = send_tcp(&chc,1,errmsg);
			if(rc < 0 && errno != 11)
			{
				x = y = 1;
				mvwprintw(twin,y,x,"%s",errmsg);
				wrefresh(twin);
				delwin(twin);
				return 1;
			}
			if(errno == 11)
			{
				noerrors++;
				mvwprintw(twin,height-4,1,"%s %d  %d           ",errmsg,errno,noerrors);
				errno = 0;
			}
		}
#endif
		switch(ch)
		{
			case KEY_F(3):
				if(tcp_connected == 0)
				{
					rc = init_client();
					if(rc < 0)
					{
						rc = tcp_connect();
						if(rc < 0)
						{
							mvwprintw(twin,2,2,"server failed - press any key");
							if(rc == -2)
								mvwprintw(twin,3,2,"recv failed on opt");
							if(rc == -3)
								mvwprintw(twin,4,2,"send failed on opt");
							tcp_connected = 0;
						}else
						{
							tcp_connected = 1;
							disp_msg(twin,"tcp connected");
						}
					}
				}
				break;
			case KEY_F(4):
				if(tcp_connected == 1)
				{
					cmd = CLOSE_SOCKET;
					put_sock(&cmd,1,1,errmsg);
					usleep(TIME_DELAY*100);
					close_sock();
					disp_msg(twin,"connection closed");
					finished = 1;
				}
				tcp_connected = 0;
				break;
			case KEY_F(5):
				if(tcp_connected == 1)
				{
					cmd =  SEND_SHOW;
//					rc = send_tcp(&cmd,1,errmsg);
					rc = put_sock(&cmd,1,1,errmsg);
					disp_msg(twin,"key F5\0");
				}
				else
					disp_msg(twin,"no connection\0");
				break;
			case KEY_F(6):
				if(tcp_connected == 1)
				{
					cmd =  SEND_SERIAL;
//					rc = send_tcp(&cmd,1,errmsg);
					rc = put_sock(&cmd,1,1,errmsg);
					disp_msg(twin,"key F6\0");
				}
				else
					disp_msg(twin,"no connection\0");
				break;
			case KEY_F(7):
				if(tcp_connected == 1)
				{
					cmd = SEND_TEST3;
//					rc = send_tcp(&cmd,1,errmsg);
					rc = put_sock(&cmd,1,1,errmsg);
					disp_msg(twin,"key F7\0");
				}
				else
					disp_msg(twin,"no connection\0");
				break;
			case KEY_F(8):
				if(tcp_connected == 1)
				{
					cmd = SEND_TEST4;
/*
					rc = send_tcp(&cmd,1,errmsg);
					rc = send_tcp(&num_o_recs,1,errmsg);
*/
					rc = put_sock(&cmd,1,1,errmsg);
					rc = put_sock(&num_o_recs,1,1,errmsg);
					disp_msg(twin,"key F8\0");
				}
				else
					disp_msg(twin,"no connection\0");
				break;
			case KEY_F(9):
				if(tcp_connected == 1)
				{
					cmd = SEND_TEST5;
/*
					rc = send_tcp(&cmd,1,errmsg);
					rc = send_tcp(&num_i_recs,1,errmsg);
*/
					rc = put_sock(&cmd,1,1,errmsg);
					rc = put_sock(&num_i_recs,1,1,errmsg);
					disp_msg(twin,"key F9\0");
				}
				else
					disp_msg(twin,"no connection\0");
				break;
			case KEY_F(10):
				if(tcp_connected == 1)
				{
					cmd = SEND_TEST6;
//					rc = send_tcp(&cmd,1,errmsg);
					rc = put_sock(&cmd,1,1,errmsg);
					disp_msg(twin,"key F10 (don't try F11)\0");
				}
				else
					disp_msg(twin,"no connection\0");
				break;
		}
		if(tcp_connected == 1)
		{
//			rc = recv_tcp(&test,1,errmsg);
			rc = get_sock(&test,1,1,errmsg);
			if(rc < 0 && errno != 11)
			{
				x = 1;
				y = HEIGHT - 2;
				mvwprintw(twin,y,x,"%s             ",errmsg);
				wrefresh(twin);
				delwin(twin);
				return 1;
			}
			if(errno == 11)
			{
				noerrors++;
				mvwprintw(twin,error_line2,1,"%s %d  %d           ",errmsg,errno,noerrors);
				errno = 0;
			}

			usleep(TIME_DELAY);

			if(++x > WIDTH-2)
			{
				x = 1;
				if(++y > status_line - 2)
				{
					clr_scr(twin);
					y = 1;
				}
			}
			mvwprintw(twin,y,x,"%c",test);

			wrefresh(twin);
		}
	}	// while getch() != F2

	delwin(twin);
	return 0;
}


