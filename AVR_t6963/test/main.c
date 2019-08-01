/*	main.c - used to test the AVR/LCD by loading this on a TS-7200 card using only
	the serial port - see readme.txt
	this program sends all the printable ascii chars to both the AVR/LCD and the
	COMM2 port of the TS-7200. It clear the screen and waits for 1 second after
	sending 2 screen fulls of chars
*/
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include "../../mytypes.h"
#include "../../thread_io/serial_io.h"
#define BUFF_SIZE 20

void get_end_byte(void);
void display_labels(int delay);

/*********************************************************************/
int uSleep(time_t sec, long nanosec)
{
/* Setup timespec */
	struct timespec req;
	req.tv_sec = sec;
	req.tv_nsec = nanosec;

/* Loop until we've slept long enough */
	do
	{
/* Store remainder back on top of the original required time */
		if( 0 != nanosleep( &req, &req ) )
		{
/* If any error other than a signal interrupt occurs, return an error */
			if(errno != EINTR)
			{
				myprintf1("uSleep error\n");
//             return -1;
			}
		}
		else
		{
/* nanosleep succeeded, so exit the loop */
			break;
		}
	} while ( req.tv_sec > 0 || req.tv_nsec > 0 );

	return 0;									  /* Return success */
}
/*********************************************************************/

int main(void)
{
	int i,j;
	UCHAR xbyte = 0x21;
	int ctr;
	char errmsg[20];
	int clr = 0;
	int reset = 0;
	int font = 0;
	int font_ctr = 0;
	UCHAR buff[BUFF_SIZE];

	init_serial();
	usleep(1000);
	init_serial2();
	usleep(1000);
	lcd_init();
	usleep(1000);
	red_led(1);
	green_led(0);
	uSleep(1,0);
	red_led(0);
	green_led(1);
	uSleep(1,0);
	
/*
	for(;;)
	{
		xbyte = read_serial(errmsg);
		if(i == 0)
		{
			red_led(0);
			green_led(1);
			i = 1;
		}else
		{
			red_led(1);
			green_led(0);
			i = 0;
		}
		write_serial2(xbyte);
	}
*/

	buff[0] = 	START_SEQ;		// was going to use these
	buff[1] = 	SHUTDOWN;		// to test EEPROM_STR2
	buff[2] = 	ON_FAN;			// but it's not working (currently)
	buff[3] = 	OFF_FAN;
	buff[4] = 	ON_LIGHTS;
	buff[5] = 	OFF_LIGHTS;
	buff[6] = 	ON_BRIGHTS;
	buff[7] = 	OFF_BRIGHTS;
	buff[8] = 	BLOWER_OFF;
	buff[9] = 	BLOWER1;
	buff[10] = 	BLOWER2;
	buff[11] = 	BLOWER3;
	buff[12] = 	ON_RUNNING_LIGHTS;
	buff[13] = 	OFF_RUNNING_LIGHTS;
	buff[14] = 	ON_BRAKES;
	buff[15] = 	OFF_BRAKES;
	buff[16] = 	WIPER1;
	buff[17] = 	WIPER2;
	buff[18] = 	WIPER_OFF;
	buff[19] = 0;

	write_serial(AVR_START_BYTE);	
	write_serial(LCD_CLRSCR);
	get_end_byte();
	write_serial(AVR_START_BYTE);
	write_serial(GOTO_CMD);
	write_serial(0);
	write_serial(0);
	get_end_byte();

	for(;;)
	{
		write_serial(AVR_START_BYTE);
		write_serial(CHAR_CMD);
		//uSleep(0,100);
		write_serial(xbyte);
		get_end_byte();

		if(++ctr > 20)
		{
			ctr = 0;
			if(i == 0)
			{
				red_led(0);
				green_led(1);
				i = 1;
			}else
			{
				red_led(1);
				green_led(0);
				i = 0;
			}
//			uSleep(0,10000);
		}
		write_serial2(xbyte);
		if(++xbyte > 0x7e)
			xbyte = 0x21;


//		red_led(0);
//		green_led(0);


/*
		if(++reset == 4000)
		{
			printString2("doing a reset...");
			uSleep(10,0);
			write_serial(AVR_START_BYTE);
			write_serial(RESET);
			get_end_byte();
		}

		if(++font_ctr > 4000)
		{
			font_ctr = 0;

			if(font == 0)
			{
				write_serial(AVR_START_BYTE);	
				write_serial(SET_8X8_FONT);
				get_end_byte();
				font = 1;
			}else				
			{
				write_serial(AVR_START_BYTE);	
				write_serial(SET_6X8_FONT);
				get_end_byte();
				font = 0;
			}
		}
*/
		if(++clr > 640)
		{
			display_labels(0);
			uSleep(1,0);
			write_serial(AVR_START_BYTE);
			write_serial(LCD_CLRSCR);
			get_end_byte();
			write_serial(AVR_START_BYTE);
			write_serial(GOTO_CMD);
			write_serial(0);
			write_serial(0);
			get_end_byte();

			display_labels(1);	// see what it looks like with a delay
			uSleep(1,0);		// between display each label
			write_serial(AVR_START_BYTE);
			write_serial(LCD_CLRSCR);
			get_end_byte();
			write_serial(AVR_START_BYTE);
			write_serial(GOTO_CMD);
			write_serial(0);
			write_serial(0);
			get_end_byte();
/*
			write_serial(AVR_START_BYTE);
			write_serial(DISPLAY_RTLABELS);	// this not working yet
			
			write_serial(START_RT_VALUE_ROW);
			write_serial(START_RT_VALUE_COL);
			write_serial(ENDING_RT_VALUE_ROW);
			write_serial(RT_VALUE_COL_WIDTH);
			write_serial(NUM_RT_LABELS);
			get_end_byte();

			write_serial(AVR_START_BYTE);
			write_serial(LCD_CLRSCR);
			get_end_byte();

			write_serial(AVR_START_BYTE);
			write_serial(GOTO_CMD);
			write_serial(0);
			write_serial(0);
			get_end_byte();
			uSleep(2,0);
*/
			clr = 0;
		}
	}				
}

void display_labels(int delay)
{
	int i,j;
	int row, col;

	write_serial(AVR_START_BYTE);
	write_serial(LCD_CLRSCR);
	get_end_byte();
	write_serial(AVR_START_BYTE);
	write_serial(GOTO_CMD);
	write_serial(0);
	write_serial(0);
	get_end_byte();

	row = START_RT_VALUE_ROW;
	col = START_RT_VALUE_COL;

	for(j = 1;j < NUM_RT_LABELS;j++)		// index has to start from 1
	{
		write_serial(AVR_START_BYTE);
		write_serial(EEPROM_STR);
		write_serial(row++);
		write_serial(col);
		if(row > ENDING_RT_VALUE_ROW)
		{
			col += RT_VALUE_COL_WIDTH;
			row = START_RT_VALUE_ROW;
		}
		write_serial(j);		// write the index into the eeprom strings
		write_serial(10);		// number of spaces to blank out 
		get_end_byte();

		if(delay == 1)
			uSleep(0,10000);
	}
}


// if we reset the AVR by doing a './run-avrdude -t' this program
// hangs so I thought it was hanging the read, but this
// seems to make no diff...

void get_end_byte(void)
{
	int i = 0;
	char errmsg[20];
	while(read_serial(errmsg) != AVR_END_BYTE)
	{
		uSleep(0,100);
		write_serial2(0x21);
		if(++i > 1000)
		{
			printString2("out of sync\r\n");
			write_serial(AVR_START_BYTE);
			write_serial(GOTO_CMD);
			write_serial(0);
			write_serial(0);
			while(read_serial(errmsg) != AVR_END_BYTE);
		}
	}				
}			

