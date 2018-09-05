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
#define FAN_STATUS 60

static int status_line;
static int error_line1;
static int error_line2;
static UCHAR rt_data[20];

static int inc_y(WINDOW *win, int y);
static void clr_scr(WINDOW *win);
static void disp_msg(WINDOW *win,char *str);
static void help_screen(WINDOW *win);
static int acc_on, fp_on, fan_on, starter_on, lights_on, estop_on;

extern int tcp_connected;

static void help_screen(WINDOW *win)
{
	mvwprintw(win,1,1,"F2 - quit");
	mvwprintw(win,2,1,"F3 - enable starter for time delay");
	mvwprintw(win,3,1,"F4 - toggle acc");
	mvwprintw(win,4,1,"F5 - toggle fuelpump");
	mvwprintw(win,5,1,"F6 - toggle cooling fan");
	mvwprintw(win,6,1,"F7 - send shutdown message");
	mvwprintw(win,7,1,"F8 - start sequence");
	mvwprintw(win,8,1,"F9 - help");
	mvwprintw(win,9,1,"F10 - clear screen");
	mvwprintw(win,10,1,"F12 - toggle estop");
	mvwprintw(win,11,1,"A - clear mbox screen");
	mvwprintw(win,12,1,"B - get debug info");
	mvwprintw(win,13,1,"C - test RE_ENTER_PASSWORD");
	mvwprintw(win,14,1,"D - turn off serial_recv (do this before shutting off monster box)");
	mvwprintw(win,15,1,"E - turn on serial_recv");
	mvwprintw(win,16,1,"F - turn on Headlamp");
	mvwprintw(win,17,1,"G - turn off Headlamp");
	mvwprintw(win,18,1,"I - show all I_DATA");
	mvwprintw(win,19,1,"O - show all O_DATA");
	mvwprintw(win,20,1,"J - enable starter");
	mvwprintw(win,21,1,"K - toggle acc");
	mvwprintw(win,22,1,"L - toggle fuelpump");
	mvwprintw(win,23,1,"M - toggle cooling fan");
	mvwprintw(win,24,1,"N - E-Stop on");
	mvwprintw(win,25,1,"P - E-Stop off");
	mvwprintw(win,26,1,"Q - quit (same as F2)");
}

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
		case ON_ESTOP:
			mvwprintw(win,row,col,"%02d:%02d:%02d  ON_ESTOP",hours,minutes,seconds);
		break;
		case OFF_ESTOP:
			mvwprintw(win,row,col,"%02d:%02d:%02d  OFF_ESTOP",hours,minutes,seconds);
		break;
		case SET_SERIAL_RECV_ON:
			mvwprintw(win,row,col,"%02d:%02d:%02d  SERIAL_RECV_ON",hours,minutes,seconds);
		break;
		case SET_SERIAL_RECV_OFF:
			mvwprintw(win,row,col,"%02d:%02d:%02d  SERIAL_RECV_OFF",hours,minutes,seconds);
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
	int x,y,i,j;
	UCHAR ret_char = 0x21;
	char buffer[20];
	char errmsg[50];
	int noerrors = 0;
	int height, width;
	UCHAR hours;
	UCHAR seconds;
	UCHAR minutes;
	int engine_time;
	UCHAR cmd2;
	UCHAR ioport = 0;
	UCHAR onoff = 0;;
	int rpm,mph;
	UINT rpm2;

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
	mvwprintw(twin,error_line2,1,"F9 - help; F10 - clear screen");

//	mvwprintw(twin,2,2,"starting server...");
	wrefresh(twin);
	x = 1;
	y = 1;
	seconds = 0;
	minutes = 0;
	hours = 0;
	starter_on = acc_on = fp_on = fan_on = estop_on = 0;

	while((ch = wgetch(twin)) != KEY_F(2) && ch != 'Q')
	{
		switch(ch)
		{
			case 'A':
				mvwprintw(twin,error_line2,STARTER_STATUS,"A - clear screen on monster box");
				if(tcp_connected == 1)
				{
				    cmd2 = CLEAR_SCREEN;
				    put_sock(&cmd2,1,1,errmsg);
				}else mvwprintw(twin,error_line2,STARTER_STATUS,"no tcp connection ");
				break;

			case 'B':
				mvwprintw(twin,error_line2,STARTER_STATUS,"B - get debug info");
				if(tcp_connected == 1)
				{
				    cmd2 = GET_DEBUG_INFO;
				    put_sock(&cmd2,1,1,errmsg);
				    cmd2 = GET_DEBUG_INFO2;
				    put_sock(&cmd2,1,1,errmsg);
				}else mvwprintw(twin,error_line2,STARTER_STATUS,"no tcp connection ");
				break;
			case 'C':
				mvwprintw(twin,error_line2,STARTER_STATUS,"C - test RE_ENTER_PASSWORD");
				if(tcp_connected == 1)
				{
					cmd2 = RE_ENTER_PASSWORD;
					put_sock(&cmd2,1,1,errmsg);
				}else mvwprintw(twin,error_line2,STARTER_STATUS,"no tcp connection ");
				break;

			case 'D':
				mvwprintw(twin,error_line2,STARTER_STATUS,"D - turn off serial_recv");
				if(tcp_connected == 1)
				{
					cmd2 = SET_SERIAL_RECV_OFF;
					put_sock(&cmd2,1,1,errmsg);
				}else mvwprintw(twin,error_line2,STARTER_STATUS,"no tcp connection ");
				break;

			case 'E':
				mvwprintw(twin,error_line2,STARTER_STATUS,"E - turn on serial_recv");
				if(tcp_connected == 1)
				{
					cmd2 = SET_SERIAL_RECV_ON;
					put_sock(&cmd2,1,1,errmsg);
				}else mvwprintw(twin,error_line2,STARTER_STATUS,"no tcp connection ");
				break;

			case 'F':
				mvwprintw(twin,error_line2,STARTER_STATUS,"F - turn on headlamp");
				if(tcp_connected == 1)
				{
					cmd2 = ON_LIGHTS;
					put_sock(&cmd2,1,1,errmsg);
				}else mvwprintw(twin,error_line2,STARTER_STATUS,"no tcp connection ");
				break;

			case 'G':
				mvwprintw(twin,error_line2,STARTER_STATUS,"G - turn off headlamp");
				if(tcp_connected == 1)
				{
					cmd2 = OFF_LIGHTS;
					put_sock(&cmd2,1,1,errmsg);
				}else mvwprintw(twin,error_line2,STARTER_STATUS,"no tcp connection ");
				break;

			case 'I':
				mvwprintw(twin,error_line2,STARTER_STATUS,"H - show all I_DATA");
				if(tcp_connected == 1)
				{
					cmd2 = SHOW_IDATA;
					put_sock(&cmd2,1,1,errmsg);
				}else mvwprintw(twin,error_line2,STARTER_STATUS,"no tcp connection ");
				break;

			case 'O':
				mvwprintw(twin,error_line2,STARTER_STATUS,"I - show all O_DATA");
				if(tcp_connected == 1)
				{
					cmd2 = SHOW_ODATA;
					put_sock(&cmd2,1,1,errmsg);
				}else mvwprintw(twin,error_line2,STARTER_STATUS,"no tcp connection ");
				break;

			case 'N':
				if(tcp_connected == 1)
				{
				    cmd2 = ON_ESTOP;
					put_sock(&cmd2,1,1,errmsg);
			    }else mvwprintw(twin,error_line2,STARTER_STATUS,"no tcp connection ");
				break;


			case 'P':
				if(tcp_connected == 1)
				{
				    cmd2 = OFF_ESTOP;
					put_sock(&cmd2,1,1,errmsg);
			    }else mvwprintw(twin,error_line2,STARTER_STATUS,"no tcp connection ");
				break;

			case KEY_F(3):
			case 'J':
				if(tcp_connected == 1)
				{
				    cmd2 = ENABLE_START;;
				    put_sock(&cmd2,1,1,errmsg);
					mvwprintw(twin,status_line,STARTER_STATUS,"ON ");
				}else mvwprintw(twin,error_line2,STARTER_STATUS,"no tcp connection ");
				break;

			case KEY_F(4):
			case 'K':
				if(tcp_connected == 1)
				{
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
				} else mvwprintw(twin,error_line2,STARTER_STATUS,"no tcp connection ");
				break;

			case KEY_F(5):
			case 'L':
				if(tcp_connected == 1)
				{
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
		        }else mvwprintw(twin,error_line2,STARTER_STATUS,"no tcp connection ");
				break;

			case KEY_F(6):
			case 'M':
				if(tcp_connected == 1)
				{
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
			    }else mvwprintw(twin,error_line2,STARTER_STATUS,"no tcp connection ");
				break;

			case KEY_F(7):		// all off
#if 0
				cmd2 = TEST_IOPORT;
				onoff = 0;
				mvwprintw(twin,8,30,"port: %02d onoff: %d",ioport,onoff);
				put_sock(&cmd2,1,1,errmsg);
				put_sock(&ioport,1,1,errmsg);				
				put_sock(&onoff,1,1,errmsg);
				break;
#endif
				if(tcp_connected == 1)
				{
				    cmd2 = OFF_FUEL_PUMP;
					put_sock(&cmd2,1,1,errmsg);
					usleep(1000000);
				    cmd2 = OFF_FAN;
					put_sock(&cmd2,1,1,errmsg);
					usleep(1000000);
				    cmd2 = STARTER_OFF;
					put_sock(&cmd2,1,1,errmsg);
					usleep(1000000);
				    cmd2 = OFF_ACC;
					put_sock(&cmd2,1,1,errmsg);

				    fan_on = 0;
				    fp_on = 0;
				    acc_on = 0;
				    starter_on = 0;
					mvwprintw(twin,status_line,STARTER_STATUS,"OFF");
					mvwprintw(twin,status_line,ACC_STATUS,"OFF");
					mvwprintw(twin,status_line,FUEL_PUMP_STATUS,"OFF");
					mvwprintw(twin,status_line,FAN_STATUS,"OFF");
				}else mvwprintw(twin,error_line2,STARTER_STATUS,"no tcp connection ");
		        break;

			case KEY_F(8):
				if(tcp_connected == 1)
				{
				    cmd2 = ON_FUEL_PUMP;
					put_sock(&cmd2,1,1,errmsg);
					usleep(1000000);
				    cmd2 = ON_FAN;
					put_sock(&cmd2,1,1,errmsg);
					usleep(1000000);
				    cmd2 = ENABLE_START;
					put_sock(&cmd2,1,1,errmsg);
					usleep(1000000);
				    cmd2 = ON_ACC;
					put_sock(&cmd2,1,1,errmsg);

				    fan_on = 0;
				    fp_on = 0;
				    acc_on = 0;
				    starter_on = 0;
					mvwprintw(twin,status_line,STARTER_STATUS,"ON ");
					mvwprintw(twin,status_line,ACC_STATUS,"ON ");
					mvwprintw(twin,status_line,FUEL_PUMP_STATUS,"ON ");
					mvwprintw(twin,status_line,FAN_STATUS,"ON ");
				}else mvwprintw(twin,error_line2,STARTER_STATUS,"no tcp connection ");
				break;

			case KEY_F(9):
				clr_scr(twin);
				help_screen(twin);
				y = 27;
				x = 1;
		        break;

			case KEY_F(10):
				clr_scr(twin);
				y = 1;
		        break;

			case KEY_F(12):		// can't use F11 - that toggle to full screen & back (F1 is help)
				if(tcp_connected == 1)
				{
					if(estop_on == 1)
					{
					    cmd2 = OFF_ESTOP;
					    estop_on = 0;
					}
					else
					{
						cmd2 = ON_ESTOP;
						estop_on = 1;
					}
				    put_sock(&cmd2,1,1,errmsg);
				}else mvwprintw(twin,error_line2,STARTER_STATUS,"no tcp connection ");
				break;

			default:
				break;
		}
		if(tcp_connected == 1)
		{
			rc = get_sock((UCHAR*)buffer,12,0,errmsg);
			if(rc < 0 && errno != 11)
//			if(rc < 0)
			{
				x = 1;
				y = height - 2;
				mvwprintw(twin,y,x,"%s             ",errmsg);
				wrefresh(twin);
//				delwin(twin);
//				return 1;
			}
			if(errno == 11)
			{
				noerrors++;
//				mvwprintw(twin,error_line2,1,"%s %d            ",errmsg,errno);
//				mvwprintw(twin,error_line2,1,"row: %02d col: %02d",y,x);
// 				wrefresh(twin);
				errno = 0;
			}else 
			{
				cmd2 = buffer[0];
				seconds = buffer[1];
				minutes = buffer[2];
				hours = buffer[3];

				rt_data[0] = buffer[4];
				rt_data[1] = buffer[5];
				rt_data[2] = buffer[6];
				rt_data[3] = buffer[7];

				rt_data[4] = buffer[8];
				rt_data[5] = buffer[9];
				rt_data[6] = buffer[10];
				rt_data[7] = buffer[11];

				disp_port(twin,y,x,cmd2,seconds,minutes,hours);
				if(cmd2 != TOTAL_UP_TIME)
				{
					y = inc_y(twin,y);
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
					mvwprintw(twin,1,61,"%02d:%02d:%02d", hours,minutes,seconds);
					mvwprintw(twin,2,67,"%02d", rt_data[0]);
					mvwprintw(twin,3,67,"%02d", rt_data[1]);
					mvwprintw(twin,4,67,"%02d", rt_data[2]);
					mvwprintw(twin,5,67,"%02d", rt_data[3]);

//					mvwprintw(twin,6,60,"rpm: %d%d",rt_data[4],rt_data[5]);
//					mvwprintw(twin,7,60,"mph: %d%d",rt_data[6],rt_data[7]);
					mvwprintw(twin,8,63,"rc: %d ",rc);

					wrefresh(twin);
				}
					
				noerrors= 0;
			}
		}	// if tcp_connected
//		else mvwprintw(twin,status_line,STARTER_STATUS,"no tcp connection ");
	}	// while getch() != F2
	werase(twin);
	wrefresh(twin);
	refresh();
	return 0;
}

