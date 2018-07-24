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
#define STARTER_STATUS 10
#define ACC_STATUS 28
#define FUEL_PUMP_STATUS 46
#define FAN_STATUS 62

static int status_line;
static int error_line1;
static int error_line2;
static UCHAR rt_data[50];

static int inc_y(WINDOW *win, int y);
static void clr_scr(WINDOW *win);
static void disp_msg(WINDOW *win,char *str);
static int acc_on, fp_on, fan_on, starter_on;

extern int tcp_connected;

/*
		        cmd = ENABLE_START;
		        put_sock(&cmd,1,1,errmsg);

		        cmd = ON_ACC;
		        put_sock(&cmd,1,1,errmsg);

		        cmd = OFF_ACC;
		        put_sock(&cmd,1,1,errmsg);

				cmd =  ON_FUEL_PUMP;
				ret = put_sock(&cmd,1,1,errmsg);

				cmd =  OFF_FUEL_PUMP;
				ret = put_sock(&cmd,1,1,errmsg);

				cmd =  ON_FAN;
				ret = put_sock(&cmd,1,1,errmsg);

				cmd = OFF_FAN;
				ret = put_sock(&cmd,1,1,errmsg);

				cmd = SHUTDOWN;
				ret = put_sock(&cmd,1,1,errmsg);
*/
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
//*************************************** disp_port ****************************************//
//******************************************************************************************//
static void disp_port(WINDOW *win, int row, int col, UCHAR cmd, UCHAR seconds, UCHAR minutes, UCHAR hours)
{
	wmove(win,1,row);
	switch(cmd)
	{
		case ENABLE_START:
			mvwprintw(win,row,col,"%02d:%02d:%02d  ENABLE_START",hours,minutes,seconds);
		break;
		case STARTER_OFF:
			mvwprintw(win,row,col,"%02d:%02d:%02d  STARTER_OFF",hours,minutes,seconds);
			mvwprintw(win,status_line,STARTER_STATUS,"OFF");
		break;
		case ON_FUEL_PUMP:
			mvwprintw(win,row,col,"%02d:%02d:%02d  ON_FUEL_PUMP",hours,minutes,seconds);
		break;
		case OFF_FUEL_PUMP:
			mvwprintw(win,row,col,"%02d:%02d:%02d  OFF_FUEL_PUMP",hours,minutes,seconds);
		break;
		case ON_FAN:
			mvwprintw(win,row,col,"%02d:%02d:%02d  ON_FAN",hours,minutes,seconds);
			mvwprintw(win,status_line,FAN_STATUS,"ON ");
		break;
		case OFF_FAN:
			mvwprintw(win,row,col,"%02d:%02d:%02d  OFF_FAN",hours,minutes,seconds);
		break;
		case ON_ACC:
			mvwprintw(win,row,col,"%02d:%02d:%02d  ON_ACC",hours,minutes,seconds);
		break;
		case OFF_ACC:
			mvwprintw(win,row,col,"%02d:%02d:%02d  OFF_ACC",hours,minutes,seconds);
		break;
		case TOTAL_UP_TIME:
//			mvwprintw(win,1,62,"%02d:%02d:%02d", hours,minutes,seconds);
		break;
		default:
		break;
	}
	wrefresh(win);
}

//******************************************************************************************//
//**************************************** tcp_win *****************************************//
//******************************************************************************************//
int tcp_win2(int cmd)
{
	WINDOW *twin;
	int startx, starty;
	int ch;
	int rc;
	int x,y,i;
	UCHAR ret_char = 0x21;
	char buffer[50];
	char errmsg[50];
	int noerrors = 0;
	int height, width;
	UCHAR hours;
	UCHAR seconds;
	UCHAR minutes;
	int engine_time;
	UCHAR cmd2;

	keypad(stdscr, TRUE);		/* I need that nifty F1 	*/
	nodelay(stdscr,TRUE);
	if(cmd == SHOW_IDATA || cmd == SHOW_ODATA)
	{
		height = SHOW_DATA_HEIGHT;
		width = SHOW_DATA_WIDTH;
		starty = ((LINES - height)/3);
	}
	else if(cmd == LIVE_WINDOW_ON)
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
	curs_set(0);

	MvWAddCh(twin,height-5,0,ACS_LTEE);
	MvWAddCh(twin,height-5,width-1,ACS_RTEE);
	for(i = 1;i < width-1;i++)
		MvWAddCh(twin,height-5,i,ACS_HLINE);

//	mvwprintw(twin,status_line,1,"STARTER:     ACC:     FUEL PUMP:    FAN:             ");
	mvwprintw(twin,status_line,STARTER_STATUS-9,"STARTER:");	
	mvwprintw(twin,status_line,ACC_STATUS-5,"ACC:");
	mvwprintw(twin,status_line,FUEL_PUMP_STATUS-11,"FUEL PUMP:");
	mvwprintw(twin,status_line,FAN_STATUS-5,"FAN:");
	mvwprintw(twin,error_line1,1,"F2 - quit; F3 - start; F4 - acc; F5 - fuel; F6 - fan; F7 - all off");
	mvwprintw(twin,error_line2,1,"F8 - clear screen; F9 - start seq");

//	mvwprintw(twin,2,2,"starting server...");
	wrefresh(twin);
	x = 1;
	y = 1;
	seconds = 0;
	minutes = 0;
	hours = 0;
	starter_on = acc_on = fp_on = fan_on = 0;

	while((ch = wgetch(twin)) != KEY_F(2))
	{
		switch(ch)
		{
			case KEY_F(3):
		        cmd2 = ENABLE_START;
		        put_sock(&cmd2,1,1,errmsg);
				mvwprintw(twin,status_line,STARTER_STATUS,"ON ");
				break;

			case KEY_F(4):
				if(acc_on == 1)
				{
			        cmd2 = OFF_ACC;
			        acc_on = 0;
					mvwprintw(twin,status_line,ACC_STATUS,"OFF");
				}
				else
				{
					cmd2 = ON_ACC;
					acc_on = 1;
					mvwprintw(twin,status_line,ACC_STATUS,"ON ");
				}
		        put_sock(&cmd2,1,1,errmsg);
				break;

			case KEY_F(5):
				if(fp_on == 1)
				{
					fp_on = 0;
			        cmd2 = OFF_FUEL_PUMP;
					mvwprintw(twin,status_line,FUEL_PUMP_STATUS,"OFF");
			    }
				else
				{
					cmd2 = ON_FUEL_PUMP;
					fp_on = 1;
					mvwprintw(twin,status_line,FUEL_PUMP_STATUS,"ON ");
				}
		        put_sock(&cmd2,1,1,errmsg);
				break;

			case KEY_F(6):
				if(fan_on == 1)
				{
					fan_on = 0;
			        cmd2 = OFF_FAN;
					mvwprintw(twin,status_line,FAN_STATUS,"OFF");
			    }
				else
				{
					cmd2 = ON_FAN;
					fan_on = 1;
					mvwprintw(twin,status_line,FAN_STATUS,"ON ");
				}
			    put_sock(&cmd2,1,1,errmsg);
				break;
			case KEY_F(7):		// all off
				cmd2 = SHUTDOWN;
				put_sock(&cmd2,1,1,errmsg);
/*
				cmd2 = OFF_FAN;
		        put_sock(&cmd2,1,1,errmsg);
		        usleep(500000);
				cmd2 = OFF_FUEL_PUMP;
		        put_sock(&cmd2,1,1,errmsg);
		        usleep(500000);
				cmd2 = OFF_ACC;
		        put_sock(&cmd2,1,1,errmsg);
		        usleep(500000);
				cmd2 = STARTER_OFF;
		        put_sock(&cmd2,1,1,errmsg);
*/
		        fan_on = 0;
		        fp_on = 0;
		        acc_on = 0;
		        starter_on = 0;
				mvwprintw(twin,status_line,STARTER_STATUS,"OFF");
				mvwprintw(twin,status_line,ACC_STATUS,"OFF");
				mvwprintw(twin,status_line,FUEL_PUMP_STATUS,"OFF");
				mvwprintw(twin,status_line,FAN_STATUS,"OFF");
		        break;
			case KEY_F(8):
				clr_scr(twin);
				x = 1;
				y = 1;
				break;
			case KEY_F(9):	// start sequence cmd which enables the starter, turns on acc & fp
							// and starts a timer which after 1 minute, turns the fan on
				cmd2 = START_SEQ;
		        put_sock(&cmd2,1,1,errmsg);
		        acc_on = 1;
		        fp_on = 1;
		        fan_on = 0;
				mvwprintw(twin,status_line,STARTER_STATUS,"ON ");
				mvwprintw(twin,status_line,ACC_STATUS,"ON ");
				mvwprintw(twin,status_line,FUEL_PUMP_STATUS,"ON ");
				mvwprintw(twin,status_line,FAN_STATUS,"OFF");
		        break;
			case KEY_F(10):
				cmd2 = PASSWORD_MODE;
		        put_sock(&cmd2,1,1,errmsg);
				mvwprintw(twin,status_line,FAN_STATUS,"F10 ");
				break;
				
			default:
				break;
		}
		if(tcp_connected == 1)
		{
			rc = get_sock((UCHAR*)buffer,4,1,errmsg);
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
//				mvwprintw(twin,error_line2,1,"row: %02d col: %02d",y,x);
 				wrefresh(twin);
				errno = 0;
			}else 
			{
				cmd2 = buffer[0];
				seconds = buffer[1];
				minutes = buffer[2];
				hours = buffer[3];
/*
				rt_data[0] = buffer[4];
				rt_data[1] = buffer[5];
				rt_data[2] = buffer[6];
				rt_data[3] = buffer[7];
				rt_data[4] = buffer[8];
				rt_data[5] = buffer[9];
				rt_data[6] = buffer[10];
				rt_data[7] = buffer[11];
*/
				disp_port(twin,y,x,cmd2,seconds,minutes,hours);
				if(cmd2 != TOTAL_UP_TIME)
				{
					y = inc_y(twin,y++);
					if(y == 1)
					{
						if(x == 1)
							x = 30;
						else
						{
							x = 1;
							clr_scr(twin);
						}
					}
				}else
				{
					mvwprintw(twin,1,62,"%02d:%02d:%02d", hours,minutes,seconds);
/*
					mvwprintw(twin,2,62,"%02x:%02x", rt_data[0],rt_data[1]);
					mvwprintw(twin,3,62,"%02x:%02x", rt_data[2],rt_data[3]);
					mvwprintw(twin,4,62,"%02x:%02x", rt_data[4],rt_data[5]);
					mvwprintw(twin,5,62,"%02x:%02x", rt_data[6],rt_data[7]);
//					mvwprintw(twin,6,62,"%02d",rc);
*/
					wrefresh(twin);
				}
					
				noerrors= 0;
			}
		}
	}	// while getch() != F2
	werase(twin);
	wrefresh(twin);
	refresh();
	return 0;
}