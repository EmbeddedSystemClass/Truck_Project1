#if 1
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h> 
#include <string.h>
#include <sched.h>
#include <sys/types.h>
#include <pthread.h>
#define closesocket close
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <dirent.h>
#include "../mytypes.h"
#include "ioports.h"
#include "serial_io.h"
#include "queue/ollist_threads_rw.h"
#include "minmea.h"
#include "tasks.h"
//#include "cs_client/config_file.h"
#include "lcd_func.h"
#define TOGGLE_OTP otp->onoff = (otp->onoff == 1?0:1)

pthread_cond_t       threads_ready;
pthread_mutex_t     tcp_write_lock=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t     tcp_read_lock=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t     io_mem_lock=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t     serial_write_lock=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t     serial_read_lock=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t     serial_write_lock2=PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_t     serial_read_lock2=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t     msg_queue_lock=PTHREAD_MUTEX_INITIALIZER;
int total_count;
extern int lights_on_delay[13];
extern int enable_gps_send_data;

UCHAR (*fptr[NUM_TASKS])(int) = { get_host_cmd_task, monitor_input_task, 
monitor_fake_input_task, timer_task, timer2_task, LCD_serial_queue, 
serial_recv_task, tcp_monitor_task, basic_controls_task};

int threads_ready_count=0;
pthread_cond_t    threads_ready=PTHREAD_COND_INITIALIZER;
pthread_mutex_t   threads_ready_lock=PTHREAD_MUTEX_INITIALIZER;
static UCHAR check_inputs(int index, int test);
extern CMD_STRUCT cmd_array[58];
ollist_t oll;
PARAM_STRUCT ps;

//extern pthread_t serial_thread;	// workaround for closing serial task

//extern int olLoadConfig(char *filename, ollist_t *oll, size_t size, char *errmsg);

static UCHAR read_serial_buffer[SERIAL_BUFF_SIZE];
static UCHAR write_serial_buffer[SERIAL_BUFF_SIZE];
static int no_serial_buff;
char password[PASSWORD_SIZE];

static int serial_rec;
int engine_running;
int lights_on_countdown = 0;
static void set_output(O_DATA *otp, int onoff);
static UCHAR inportstatus[OUTPORTF_OFFSET-OUTPORTA_OFFSET+1];
static UCHAR fake_inportstatus1[OUTPORTF_OFFSET-OUTPORTA_OFFSET+1];
static UCHAR fake_inportstatus2[OUTPORTF_OFFSET-OUTPORTA_OFFSET+1];
static int mask2int(UCHAR mask);
static int test_lblinkers;
static int test_rblinkers;
extern int shutdown_all;
extern int actual_lights_on_delay;
//double double getDistance(double lat1, double lon1, double lat2, double lon2, double *yards, double *miles);
extern double getDistance(double lat1, double lon1, double lat2, double lon2, double *yards, double *miles);
//extern int time_set;

int max_ips;
IP ip[40];
WAYPOINTS wp[36];	// see make_waypoints for how many records are made
int no_waypoints;
static UCHAR msg_queue[MSG_QUEUE_SIZE];
static int msg_queue_ptr;

FORMAT_STR rtlabel_str[NUM_RT_LABELS];
FORMAT_STR status_label_str[NUM_STATUS_LABELS];

//static double program_start_time;

#define ON 1
#define OFF 0

/****************************************************************************************************/
static double curtime(void)
{
	struct timeval tv;
	gettimeofday (&tv, NULL);
	return tv.tv_sec + tv.tv_usec / 1000000.0;
}

/****************************************************************************************************/
void init_ips(void)
{
	int i,j,k;
	O_DATA *otp;
	O_DATA **otpp = &otp;
	char errmsg[20];
	char tempx[20];
	UCHAR ucbuff[5];
	UCHAR xchar = 0x21;

	engine_running = 0;
	test_lblinkers = 0;
	test_rblinkers = 0;
	for(i = 0;i < 40;i++)
	{
		ip[i].port = 99;
		ip[i].input = 0;
		memset(ip[i].label,0,OLABELSIZE);
	}

	j = 0;
	// find all the inputs that have outputs assigned
	// to them and make a list to go in ip struct array
	for(i = 0;i < 40;i++)
	{
		for(k = 0;k < 40;k++)
		{
			if(ollist_find_data_op(i,k,&otp,&oll) > -1)
			{
				ip[j].port = k;
				ip[j].input = i;
//				printf("%d %d ",ip[j].port,ip[j].input);
				strcpy(ip[j++].label,otp->label);
//				printf("%s\r\n",ip[j-1].label);
			}
		}
	}
//	printf("\n");

	max_ips = 0;
	for(i = 0;i < 40;i++)
	{
		if(ip[i].port < 40)
		{
//			printf("%d: \tport: %d input: %d %s \n",i,ip[i].port,ip[i].input,ip[i].label);
			max_ips++;
		}
	}
//	printf("max_ips: %d\r\n",max_ips);
	i = LoadParams("param.conf",&ps, &password[0], errmsg);
	no_waypoints = LoadWayPoints("waypoints.conf",&wp[0], errmsg);
//	for(i = 0;i < no_waypoints;i++)
//		printf("%s %f %f\r\n",wp[i].name, wp[i].latitude, wp[i].longitude);
	//printString2(password);
	actual_lights_on_delay = lights_on_delay[ps.lights_on_delay];

	//sprintf(tempx,"%d %d %d",ps.engine_temp_limit,ps.cooling_fan_on,ps.cooling_fan_off);
	//printString2(tempx);
	if(i < 0)
	{
//		printf("%s\r\n",errmsg);
		myprintf1(errmsg);
	}
}

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
static int mask2int(UCHAR mask)
{
	int i = 0;
	do
	{
		mask >>= 1;
		i++;
	}while(mask);
	return i - 1;
}
#endif
/*********************************************************************/
// change the outputs according to the type
// type 0 is normal, 1 is reverse momentary-contact, 2 is time delayed
// type 1 makes a momentary-contact switch act like a toggle switch
// (a button press and release changes state where type 0 state 
// toggles with the switch)
static void set_output(O_DATA *otp, int onoff)
{
	O_DATA **otpp = &otp;
	UCHAR buff[1];

	switch(otp->type)
	{
		case 0:
			if(otp->polarity == 1)
				otp->onoff = (onoff == 1?0:1);
			else
				otp->onoff = onoff;
//			printf("type 0 port: %d onoff: %d\r\n", otp->port, otp->onoff);
			change_output(otp->port,otp->onoff);
			ollist_insert_data(otp->port,&oll,otp);
			break;
		case 1:
			if(otp->reset == 0)
			{
//				printf("type 1 port: %d onoff: %d\r\n", otp->port, otp->onoff);
				otp->reset = 1;
//				if(otp->polarity == 0)
				TOGGLE_OTP;
				change_output(otp->port,otp->onoff);
				ollist_insert_data(otp->port,&oll,otp);
//				printf("type 1 port: %d onoff: %d reset: %d pol: %d\r\n\r\n", otp->port,
//										otp->onoff, otp->reset, otp->polarity);
			}
			else if(otp->reset == 1)
			{
//				if(otp->polarity == 1)
//					TOGGLE_OTP;
				otp->reset = 0;
//				printf("type 1 port: %d onoff: %d reset: %d pol: %d\r\n", otp->port,
//							otp->onoff, otp->reset, otp->polarity);
			}
			break;
		case 2:
		case 3:
//			printf("type %d port: %d onoff: %d reset: %d\r\n",otp->type, otp->port, otp->onoff, otp->reset);
			if(otp->reset == 0)
			{
				otp->reset = 1;
				otp->time_left = otp->time_delay;
//							otp->onoff = onoff;
//				TOGGLE_OTP;
				otp->onoff = 1;
				change_output(otp->port,1);
				ollist_insert_data(otp->port,&oll,otp);
			}
			break;
		case 4:
			if(otp->reset == 0)
			{
//				printf("type 4 port: %d onoff: %d\r\n", otp->port, otp->onoff);
				otp->reset = 1;
//				if(otp->polarity == 0)
//				TOGGLE_OTP;
				otp->onoff = 0;
				change_output(otp->port,otp->onoff);
				ollist_insert_data(otp->port,&oll,otp);
				// this causes the server to disconnect for some reason
				//send_serial(ESTOP_SIGNAL);
				add_msg_queue(ESTOP_SIGNAL);
//				printf("type 4a port: %d onoff: %d reset: %d \r\n\r\n", otp->port,
//										otp->onoff, otp->reset);
			}
			else if(otp->reset == 1)
			{
//				if(otp->polarity == 1)
//					TOGGLE_OTP;
				otp->reset = 3;
//				printf("type 4 port: %d onoff: %d reset: %d \r\n\r\n", otp->port,
//										otp->onoff, otp->reset);
			}
			break;
		default:
			break;
	}
	// send message to monster box telling which port was toggled
//	send_serial(otp->port, otp->onoff);

}
/*********************************************************************/
void send_serial(UCHAR cmd)
{
	int i;
//	UCHAR buffer[21];
// send what just changed to the PIC24/AVR to dispaly on screen
//	memset(buffer,0,sizeof(buffer));
	pthread_mutex_lock( &serial_write_lock);
	write_serial(cmd);
/*
	for(i = 0;i < SERIAL_BUFF_SIZE;i++)
	{
		if(i == 10)
			write_serial(SYSTEM_UP);
		else
			write_serial(0);
	}
*/
	pthread_mutex_unlock(&serial_write_lock);
}
/*********************************************************************/
void send_serial2(UCHAR cmd)
{
	int i;
	if(ps.test_bank == 0)
	{
		pthread_mutex_lock( &serial_write_lock2);
		write_serial2(cmd);
		pthread_mutex_unlock(&serial_write_lock2);
	}
}
/*********************************************************************/
void send_serialother(UCHAR cmd, UCHAR *buf)
{
	int i;
	pthread_mutex_lock( &serial_write_lock);

	write_serial(cmd);
	for(i = 0;i < SERIAL_BUFF_SIZE;i++)
	{
		write_serial(buf[i]);
//		printHexByte(buf[i]);
	}
	pthread_mutex_unlock(&serial_write_lock);
}
/*********************************************************************/
void  send_lcd(UCHAR *buf, int size)
{
	int i;
	UCHAR start_byte = AVR_START_BYTE;
	UCHAR ch = 0;
	char errmsg[20];

	pthread_mutex_lock(&serial_write_lock2);
	for(i = 0;i < size;i++)
	{
		write_serial2(buf[i]);
		uSleep(0,5000);
	}
	write_serial2(start_byte);
	ch = 0;
	do 
	{
		printf("-%02x ",ch);
		ch = read_serial2(errmsg);
	}while(ch != AVR_END_BYTE);
	printf("%02x ",ch);
	pthread_mutex_unlock(&serial_write_lock2);
}
/*********************************************************************/
// if an input switch is changed, update the record for that switch
// and if an output applies to that input, change the output
// and record the event in llist - each input can be assigned any output
UCHAR monitor_input_task(int test)
{
//	I_DATA *itp;
//	I_DATA **itpp = &itp;
	O_DATA *otp;
	O_DATA **otpp = &otp;

	int status = -1;
	int bank, index;
	UCHAR result,result2, mask, onoff;
	int i, rc, flag;
	int input_port;


//	TODO: what if more than 1 button is pushed in same bank or diff bank at same time?

	pthread_mutex_lock( &io_mem_lock);
	inportstatus[0] =  ~InPortByteA();
	inportstatus[1] =  ~InPortByteB();
	inportstatus[2] =  ~InPortByteC();
	inportstatus[3] =  ~InPortByteD();
	inportstatus[4] =  ~InPortByteE();
	inportstatus[5] =  ~InPortByteF();
	pthread_mutex_unlock( &io_mem_lock);

	while(TRUE)
	{
		for(bank = 0;bank < NUM_PORTS;bank++)
		{
			pthread_mutex_lock( &io_mem_lock);
			result = InPortByte(bank);
			pthread_mutex_unlock( &io_mem_lock);
			result = ~result;

			if(result != inportstatus[bank])
			{
				mask = result ^ inportstatus[bank];
//				printf("enter 1: %02x\r\n",inportstatus[bank]);

				if(mask > 0x80)
				{
					myprintf1("bad mask\0");
					printf("bad mask %02x\r\n",mask);
					continue;
				}
				index = mask2int(mask);

				if((mask & result) == mask)
				{
					onoff = ON;
				}
				else
				{
					onoff = OFF;
				}
				// since each card only has 20 ports then the 1st 2 port access bytes
				// are 8-bit and the 3rd is only 4-bits, so we have to translate the
				// inportstatus array, representing 3 byts of each 2 (3x8x2 = 48) to
				// one of the 40 actual bits as index
				for(i = 0;i < 40;i++)
				{
					if(real_banks[i].bank == bank && real_banks[i].index == index)
					{
						index = real_banks[i].i;
					}
				}
				for(i = 0;i < max_ips;i++)
				{
//					printf("%d %d %d\r\n",ip[i].port,ip[i].input,index);
					if(ip[i].input == index)
					{
						ollist_find_data(ip[i].port,&otp,&oll);
						set_output(otp, onoff);
					}
				}
				inportstatus[bank] = result;
//				printf("leave 1: %02x\r\n\r\n",inportstatus[bank]);
			}
		}
		uSleep(0,TIME_DELAY/200);
//			uSleep(0,TIME_DELAY/2);
		if(shutdown_all)
		{
//				printf("done mon input tasks\r\n"); 
//				myprintf1("done mon input");
				//printString2("done mon");
			return 0;
		}
	}
	return 1;
}

/*********************************************************************/
// this is used so that inputs can be changed programatically by software
// as if an input button or switch were changed
int change_input(int index, int onoff)
{
	int bank;
	UCHAR mask = 1;
	UCHAR state = 0;

	bank = real_banks[index].bank;
	index = real_banks[index].index;

	mask <<= index;

	if(onoff)
	{
		fake_inportstatus2[bank] |= mask;
	}
	else
	{
		fake_inportstatus2[bank] &= ~mask;
	}
}
/*********************************************************************/
// do the same thing as monitor_input_tasks but with the fake arrays
// set by change_inputs()
UCHAR monitor_fake_input_task(int test)
{
//	I_DATA *itp;
//	I_DATA **itpp = &itp;
	O_DATA *otp;
	O_DATA **otpp = &otp;

	int status = -1;
	int bank, index;
	UCHAR result, mask, onoff;
	int i, rc, flag;

//	TODO: what if more than 1 button is pushed in same bank or diff bank at same time?

	for(i = 0;i < 6;i++)
	{
		fake_inportstatus1[i] = 0;
		fake_inportstatus2[i] = 0;
	}

	while(TRUE)
	{
		for(bank = 0;bank < NUM_PORTS;bank++)
		{
			result = fake_inportstatus2[bank];

			if(result != fake_inportstatus1[bank])
			{
				mask = result ^ fake_inportstatus1[bank];
//				printf("enter 2: %02x\r\n",fake_inportstatus1[bank]);

//				printf("mask: %02x\r\n",mask);
				if(mask > 0x80)
				{
					myprintf1("bad mask\0");
					printf("bad mask %02x\r\n",mask);
					continue;
				}
				index = mask2int(mask);

				if((mask & result) == mask)
				{
					onoff = ON;
	 				fake_inportstatus2[bank] |= mask;
				}
				else
				{
					onoff = OFF;
	 				fake_inportstatus2[bank] &= ~mask;
				}

				for(i = 0;i < 40;i++)
				{
					if(real_banks[i].bank == bank && real_banks[i].index == index)
					{
						index = real_banks[i].i;
					}
				}

				for(i = 0;i < max_ips;i++)
				{
					if(ip[i].input == index)
					{
						ollist_find_data(ip[i].port,&otp,&oll);
						set_output(otp, onoff);
					}
				}
 				fake_inportstatus1[bank] = fake_inportstatus2[bank];

//				printf("leave 2: %02x %02x\r\n\r\n",fake_inportstatus1[bank],fake_inportstatus2[bank]);
			}
		}
		uSleep(0,TIME_DELAY/200);
//			uSleep(0,TIME_DELAY/2);
		if(shutdown_all)
		{
//				printf("done mon fake input tasks\r\n");
//				myprintf1("done mon input");
			return 0;
		}
	}
	return 1;
}

/*********************************************************************/
// pass in the index into the total list of outputs
// since each card only has 20 outputs, the last 4 bits of PORT C & F are ignored
// index 0->19 = PORTA(0:7)->PORTC(0:4)
// index 24->39 = PORTD(0:7)->PORTF(0:4)
int change_output(int index, int onoff)
{
	int bank;

//	printf("change output: %d %d\r\n",index,onoff);
	pthread_mutex_lock( &io_mem_lock);

	bank = real_banks[index].bank;
	index = real_banks[index].index;

	switch(bank)
	{
		case 0:
			OutPortA(onoff, index);			  // 0-7
			break;
		case 1:
			OutPortB(onoff, index);			  // 0-7
			break;
		case 2:
			OutPortC(onoff, index);			  // 0-3
			break;
		case 3:
			OutPortD(onoff, index);			  // 0-7
			break;
		case 4:
			OutPortE(onoff, index);			  // 0-7
			break;
		case 5:
			OutPortF(onoff, index);			  // 0-3
			break;
		default:
			break;
	}
	pthread_mutex_unlock(&io_mem_lock);
//	printf("change output: %d %d\r\n",index,onoff);
	return index;
}

/*********************************************************************/
// this happens 10x a second
UCHAR timer2_task(int test)
{
	int i,j;
	char tempx[20];
	int index = 0;
	int bank = 0;
	int fp;
	UCHAR mask;
	static int led_counter;
	static int led_onoff;
	led_counter = 0;
	led_onoff = 0;
	static test_ctr = 0;
	static test_ctr2 = 0;

	//printf("\r\n");

	while(TRUE)
	{
		uSleep(0,TIME_DELAY/10); 

		if(++led_counter > 9)
		{
			if(led_onoff == 1)
			{
				led_counter = 0;
//				setdioline(7,1);
//				red_led(1);
//				green_led(0);
			}
			else
			{
				led_onoff = 7;
//				setdioline(7,0);
//				red_led(0);
//				green_led(1);
			}
			led_onoff = (led_onoff == 1?0:1);
		}

		if(shutdown_all)
		{
//			printf("done timer2 task\r\n");
			//printString2("done time2");
			return 0;
		}
	}
	return 1;

}
/*********************************************************************/
void init_LCD(int clr)
{
	UCHAR row, col, data_col, str;
	UCHAR xchar = 0x31;
	UCHAR ucbuff[5];
	int i,j;
	char tempx[20];

	col = START_RT_VALUE_COL;
	data_col = col + 9;
	row = START_RT_VALUE_ROW;
	str = 0;

	if(clr)
	{
		ucbuff[0] = LCD_CLRSCR;
		send_lcd(ucbuff, 1);
	}

	ucbuff[0] = EEPROM_STR;
	for(j = 0;j <= IND_TEMP;j++)		// index has to start from 1
	{
		if(row > ENDING_RT_VALUE_ROW)
		{
			col += RT_VALUE_COL_WIDTH;
			row = START_RT_VALUE_ROW;
			data_col = col + 9;
		}
		rtlabel_str[j].row = row;
		rtlabel_str[j].col = col;
		rtlabel_str[j].data_col = data_col;

		ucbuff[1] = row;
		ucbuff[2] = col;
		ucbuff[3] = (UCHAR)j+1;
		ucbuff[4] = 6;
		row++;
		send_lcd(ucbuff,5);
		usleep(1000);
	}

	col = START_RT_VALUE_COL;
//	row = rtlabel_str[TRIP].row + 2;
	row = 9;
	data_col = col + 9;
	ucbuff[0] = EEPROM_STR;
	//printf("\r\n");
	for(j = LAT;j <= GPS_ALT;j++)
	{
		rtlabel_str[j].row = row;
		rtlabel_str[j].col = col;
		if(j == GPS_SPEED)
			rtlabel_str[j].data_col = data_col + 2;
		else
			rtlabel_str[j].data_col = data_col;
		ucbuff[1] = row;
		ucbuff[2] = col;
		ucbuff[3] = (UCHAR)j + 1;
		ucbuff[4] = 8;
		row++;
		send_lcd(ucbuff,5);
		usleep(1000);
		//for(i = 1;i < 5;i++)
			//printf("%d ",ucbuff[i]);
		//printf("\r\n");
	}
	//for(i = 0;i < NUM_RT_LABELS;i++)
		//printf("\r\n%d %d %d",rtlabel_str[i].row, rtlabel_str[i].col, rtlabel_str[i].data_col);
	//printf("\r\n");
}
/*********************************************************************/
// this happens once a second
UCHAR timer_task(int test)
{
	int i;
	UCHAR time_buffer[20];
	char tempx[20];
	int index = 0;
	int bank = 0;
	int fp;
	UCHAR mask;
	time_t curtime2;
	struct timeval mtv;
	O_DATA *otp;
	O_DATA **otpp = &otp;
	O_DATA *otp2;
	O_DATA **otpp2 = &otp2;
	static int led_counter = 0;
//	static int test_ctr = 0;
//	static int test_ctr2 = 0;
//	static UCHAR nav = NAV_DOWN;
	UCHAR cmd;
	UCHAR ucbuff[6];
	int temp;
	odometer = trip = 0;

	memset(write_serial_buffer,0,SERIAL_BUFF_SIZE);

	while(TRUE)
	{
		if(shutdown_all)
		{
//			printf("done timer_task\r\n");
			//printString2("done timer");
			return 0;
		}
//		uSleep(0,TIME_DELAY/2);		// 1/2 sec
		uSleep(1,0);

		if(led_counter == 1)
		{
			led_counter = 0;
//			 (1);
//			green_led(0);
		}
		else
		{
			led_counter = 1;
//			red_led(0);
//			green_led(1);
		}

		if(engine_running == 1)
		{
			ucbuff[0] = SEND_BYTE_RT_VALUES;
			ucbuff[1] = rtlabel_str[RUN_TIME].row;
			ucbuff[2] = rtlabel_str[RUN_TIME].data_col;
			ucbuff[3] = running_hours;		// hours
			send_lcd(ucbuff, 4);
			uSleep(0,1000);

			ucbuff[1] = rtlabel_str[RUN_TIME].row;
			ucbuff[2] = rtlabel_str[RUN_TIME].data_col + 3;
			ucbuff[3] = running_minutes;		// minutes
			send_lcd(ucbuff, 4);
			uSleep(0,1000);

			ucbuff[1] = rtlabel_str[RUN_TIME].row;
			ucbuff[2] = rtlabel_str[RUN_TIME].data_col + 6;
			ucbuff[3] = running_seconds;		// seconds
			send_lcd(ucbuff, 4);
			uSleep(0,1000);

			ucbuff[0] = SEND_INT_RT_VALUES;
			ucbuff[1] = rtlabel_str[ODOM].row;
			ucbuff[2] = rtlabel_str[ODOM].data_col;
			temp = odometer;
			ucbuff[4] = (UCHAR)temp;
			temp >>= 8;
			ucbuff[3] = (UCHAR)temp; 
			send_lcd(ucbuff, 5);
			uSleep(0,1000);

			ucbuff[0] = SEND_INT_RT_VALUES;
			ucbuff[1] = rtlabel_str[TRIP].row;
			ucbuff[2] = rtlabel_str[TRIP].data_col;

			temp = trip;
			ucbuff[4] = (UCHAR)temp;
			temp >>= 8;
			ucbuff[3] = (UCHAR)temp; 
			uSleep(0,1000);

			send_lcd(ucbuff, 5);

			if(++running_seconds > 59)
			{
				running_seconds = 0;
				if(++running_minutes > 59)
				{
					running_minutes = 0;
					running_hours++;
				}
			}
			sprintf(tempx,"%dh %dm %ds ",running_hours, running_minutes, running_seconds);
			send_msg(strlen((char*)tempx)*2,(UCHAR*)tempx, ENGINE_RUNTIME);

			if(running_minutes == 0 && running_seconds == 15)
			{
				add_msg_queue(ON_LIGHTS);
				add_msg_queue(ON_RUNNING_LIGHTS);
				send_serialother(ON_LIGHTS,(UCHAR*)tempx);
				send_serialother(ON_RUNNING_LIGHTS,(UCHAR*)tempx);
				send_msg(strlen((char*)tempx)*2,(UCHAR*)tempx, ON_LIGHTS);
				send_msg(strlen((char*)tempx)*2,(UCHAR*)tempx, ON_RUNNING_LIGHTS);
			}
		}else
		{
			running_seconds = running_minutes = running_hours = 0;
			if(lights_on_countdown > 0)
			{
				lights_on_countdown--;
				//sprintf(tempx,"countdown: %d",lights_on_countdown);
				//printString3(tempx);
				if(lights_on_countdown == 0)
				{
					add_msg_queue(OFF_LLIGHTS);
					add_msg_queue(OFF_RLIGHTS);
					add_msg_queue(OFF_LIGHTS);
					add_msg_queue(OFF_RUNNING_LIGHTS);
					add_msg_queue(BLOWER_OFF);
					add_msg_queue(OFF_FAN);
					usleep(1000);
					send_serialother(OFF_LIGHTS,(UCHAR*)tempx);
					usleep(1000);
					send_serialother(OFF_RUNNING_LIGHTS,(UCHAR*)tempx);
					usleep(1000);
//					send_serialother(BLOWER_OFF,(UCHAR*)tempx);
//					usleep(1000);
					send_serialother(OFF_FAN,(UCHAR*)tempx);
					usleep(1000);
					send_msg(strlen((char*)tempx)*2,(UCHAR*)tempx, OFF_LIGHTS);
				}
			}
		}
		odometer++;
		if(odometer % 2 == 0)
			trip++;

		if(++trunning_seconds > 59)
		{
			trunning_seconds = 0;
			if(++trunning_minutes > 59)
			{
				trunning_minutes = 0;
				trunning_hours++;
			}
		}
		sprintf(tempx,"%dh %dm %ds ",trunning_hours, trunning_minutes, trunning_seconds);
		send_msg(strlen((char*)tempx)*2,(UCHAR*)tempx, SERVER_UPTIME);

		if(test_lblinkers > 0)
		{
			if(test_lblinkers == 1)
				test_lblinkers++;
			if(test_lblinkers == 2)
			{
				test_lblinkers = 0;
				change_input(LEFTBLINKER_INPUT,0);
			}
		}
		if(test_rblinkers > 0)
		{
			if(test_rblinkers == 1)
				test_rblinkers++;
			if(test_rblinkers == 2)
			{
				test_rblinkers = 0;
				change_input(RIGHTBLINKER_INPUT,0);
			}
		}

		// check if one of the outputs is set to type 2 (time_delay)
//		for(i = 0;i < NUM_PORTS;i++)
		for(i = STARTER;i < TESTOUTPUT27;i++)
		{
			ollist_find_data(i,otpp,&oll);
//			printf("%d ",otp->port);
//			if(i == NUM_PORTS-1)
//				printf("\r\n");
//			printf("(timer) type %d port: %d onoff: %d reset: %d pol: %d\r\n",otp->type, otp->port,
//						otp->onoff, otp->reset, otp->polarity);
			if((otp->type == 2 || otp->type == 3) && otp->reset == 1)
			{

				// if blink type, toggle on and off every second
				// this doesn't toggle on and off for time_delay, it 
				// toggles every second so set the time delay for 2x 
				// no. of seconds you want it to last
				if(otp->type == 3)
					change_output(otp->port,((otp->time_delay - otp->time_left) % 2 == 0?0:1));

				otp->time_left--;
//				printf("time left: %d %d\r\n",otp->port,otp->time_left);

				if(otp->time_left == 0)
				{
					// delay_time was not even then toggle again
					if(otp->type == 3 && (otp->time_delay % 2 == 0))
						TOGGLE_OTP;

//					change_output(i,otp->onoff);
					change_output(i,0);
					otp->onoff = 0;
					otp->reset = 0;
					ollist_insert_data(otp->port,&oll,otp);

//					printf("time up: port: %d onoff: %d\r\n", otp->port, otp->onoff);

					// if this is a result of a fake input, we turn it 
					// off here but the fake input has know way of
					// knowing it got turned off (toggled)
					// so look it up and reset it in the
					// fake_inportstatus if its a result
					// of implicit or explicit input
					mask = 1;
					bank = real_banks[otp->port].bank;
					index = real_banks[otp->port].index;
					mask <<= index;

					fake_inportstatus1[bank] &= ~mask;
					fake_inportstatus2[bank] &= ~mask;
//					send_serial(otp->port,otp->onoff);
/*
					if(otp->onoff)
						fake_inportstatus1[bank] |= mask;
					else
						fake_inportstatus1[bank] &= ~mask;
*/
//					printf("cd1: %02x %02x %02x %d %d\r\n",fake_inportstatus1[bank],
//						fake_inportstatus2[bank],mask,bank,index);
				}
//				printf("cd2: %02x %02x\r\n",fake_inportstatus1[0], fake_inportstatus2[0]);
//				printf("%d %s\r\n",otp->time_left,otp->label);
			}
		}

		// write to the sched.log file if buffer gets near full

	}
	return 1;
}

static struct minmea_sentence_rmc rmc;
static struct minmea_sentence_gga gga;
static struct minmea_sentence_gll gll;
//static struct minmea_sentence_gst gst;
static struct minmea_sentence_gsv gsv;
static struct minmea_sentence_vtg vtg;
static struct minmea_sentence_gsa gsa;

typedef struct
{
	float yards;
	float miles;
	char name[30];
	int units;
}FARRAY;

static char gps_strings[20][80];

/*********************************************************************/
// baud rate factory set to 4800
// initial cmd: $PSRF100,1,4800,8,1,0*0C
UCHAR LCD_serial_queue(int test)
{
	UCHAR ch;
	int i,k;
	FARRAY fa[35];		// this should be big enough to store all the waypoints in wp[]
	double curr_lat, curr_long;
	float float1, float2;
	double ydistance1, ydistance2;
	double mdistance1, mdistance2;
	int closest_wp, next_closest_wp;
	char tempx[200];
	char tempy[30];
	char errmsg[30];
	UCHAR ucbuff[20];
	int size_msg;
	FILE *fp;
	int no_recs;

	struct minmea_sentence_rmc *prmc = &rmc;
	struct minmea_sentence_gga *pgga = &gga;
	struct minmea_sentence_gll *pgll = &gll;
//	struct minmea_sentence_gst *pgst = &gst;
	struct minmea_sentence_gsv *pgsv = &gsv;
	struct minmea_sentence_vtg *pvtg = &vtg;
	struct minmea_sentence_gsa *pgsa = &gsa;

	int ret_val = -1;
	usleep(_5SEC);
	usleep(_5SEC);
	float tfloat;
	static int onoff = 0;
	void *vstruct;
	int client_send_data = 0;
	//printf("\r\n\r\n");
	//printf("starting GPS task\r\n");
	k = 0;
	enable_gps_send_data = 0;
	fp = fopen ("test3.txt", "r");

	memset(gps_strings,0,sizeof(char)*124*80);

	//printf("starting...\r\n");

	i = 0;
	while(fscanf(fp, "%s", tempx)!=EOF)
	{
		//printf("%s - ",tempx);
		strcpy(gps_strings[i],tempx);
		strcat(gps_strings[i],"\r\n");
//		printf("%s",gps_strings[i]);
		memset(tempx,0,sizeof(tempx));
		i++;
	}
	fclose(fp);
	no_recs = i;
	//printf("\r\nno_recs: %d no waypoints: %d\r\n",no_recs, no_waypoints);

	//uSleep(0,_1SEC);
	//printf("\r\n\r\n");

//	for(i = 0;i < 125;i++)
//		printf("%s",gps_strings[i]);
	tfloat = 0.001;
	k = 0;
	while(TRUE)
	{
		//uSleep(_1SEC,0);
		usleep(10000000);
		memset(tempx,0,sizeof(tempx));
//		i = -1;
		client_send_data = 0;
/*
		do
		{
			i++;
			tempx[i] = read_serial3(errmsg);
			if(tempx[i] < 0)
				printf("%s\r\n",errmsg);
 		}while(i < 200 && tempx[i] != 0x0D && tempx[i-1] != 0x0A);	// if CRLF, quit
*/
		
		//printf("%s...\r\n",tempx);
		strcpy(tempx,gps_strings[k]);
		//printf("%s",tempx);
		if(k >= no_recs)
			k = 0;
		else k++;

		ret_val = minmea_sentence_id(tempx, false);

		switch(ret_val)
		{
			// recommended minimum specific data
			// time, date, lat/long, speed, direction
			case MINMEA_SENTENCE_RMC:
				if(get_rmc_frame(tempx, prmc) > 0)
				{
					sprintf(tempy,"%.2f\0",(curr_lat = minmea_tocoord(&rmc.latitude)));
					//printf("%s\r\n",tempy);
					ucbuff[0] = DISPLAY_STR;
					ucbuff[1] = rtlabel_str[LAT].row;
					ucbuff[2] = rtlabel_str[LAT].data_col;
					memcpy(ucbuff+3,tempy,strlen(tempy));
					send_lcd(ucbuff,strlen(tempy)+5);
					usleep(1000);

					sprintf(tempy,"%.2f\0",(curr_long = minmea_tocoord(&rmc.longitude)));
					//printf("%s\r\n",tempy);
					ucbuff[0] = DISPLAY_STR;
					ucbuff[1] = rtlabel_str[LONG].row;
					ucbuff[2] = rtlabel_str[LONG].data_col;
					memcpy(ucbuff+3,tempy,strlen(tempy));
					send_lcd(ucbuff,strlen(tempy)+5);
					usleep(1000);

					memset(tempy,0,sizeof(tempy));
					sprintf(tempy,"%.1f\0",minmea_tofloat(&rmc.speed)+ tfloat);
					//printf("%s\r\n",tempy);
					ucbuff[0] = DISPLAY_STR;
					ucbuff[1] = rtlabel_str[GPS_SPEED].row;
					ucbuff[2] = rtlabel_str[GPS_SPEED].data_col;
					memcpy(ucbuff+3,tempy,strlen(tempy));
					send_lcd(ucbuff,strlen(tempy)+5);
/*
					sprintf(tempx,"time: hours: %d minutes: %d seconds %d",
						rmc.time.hours,rmc.time.minutes,rmc.time.seconds);
					sprintf(tempx,"date: %d %d %d",rmc.date.day,rmc.date.month,rmc.date.year);
						minmea_tocoord(&rmc.longitude));
*/
//					sprintf(tempx,"direction: %.0f",minmea_tofloat(&rmc.course));
					client_send_data = SEND_GPS_RMC_DATA;

					if(no_waypoints > 0)
					{
						for(i = 0;i < no_waypoints;i++)		// store all the relative distances in fa
						{
							getDistance((double)wp[i].latitude, (double)wp[i].longitude, 
												curr_lat, curr_long, &fa[i].yards, &fa[i].miles);
							strcpy(fa[i].name,wp[i].name);
							//printf("%d: %s %f\r\n",i, fa[i].name, fa[i].miles);
						}
						mdistance2 = 1000000.0;
						for(i = 0;i < no_waypoints;i++)		// find the nearest waypoint
						{
							mdistance1 = fa[i].miles;
							if(mdistance1 < mdistance2)
							{	
								mdistance2 = mdistance1;
								closest_wp = i;
							}
						}
						mdistance2 = 1000000.0;
						for(i = 0;i < no_waypoints;i++)		// find the next nearest waypoint
						{
							if(i != closest_wp)
							{
								mdistance1 = fa[i].miles;
								if(mdistance1 < mdistance2)
								{	
									mdistance2 = mdistance1;
									next_closest_wp = i;
								}
							}
						}
						//printf("%d %d\r\n",closest_wp, next_closest_wp);
					}
				}
				break;

			// system fixed data
			case MINMEA_SENTENCE_GGA:
				if(get_gga_frame(tempx, pgga) > 0)
				{
					memset(tempy,0,sizeof(tempy));
					sprintf(tempy,"%.0f\0",3.2808*minmea_tofloat(&gga.altitude)+ tfloat);
					//printf("%s\r\n",tempy);
					ucbuff[0] = DISPLAY_STR;
					ucbuff[1] = rtlabel_str[GPS_ALT].row;
					ucbuff[2] = rtlabel_str[GPS_ALT].data_col;
					memcpy(ucbuff+3,tempy,strlen(tempy));
					send_lcd(ucbuff,strlen(tempy)+5);
					client_send_data = SEND_GPS_GGA_DATA;
				}
				break;

			// this was in the minmea code but not in docs (deprecated?)
/*  
			case MINMEA_SENTENCE_GST:
				if(get_gsa_frame(tempx, pgsa) > 0)
				{
					sprintf(tempx,"GST ?");
					client_send_data = SEND_GPS_GST_DATA;
				}
				break;
*/
			// satellites in view
			case MINMEA_SENTENCE_GSV:
				if(get_gsv_frame(tempx, pgsv) > 0)
				{
					//sprintf(tempx,"GSV total sats: %d",gsv.total_sats);
					client_send_data = SEND_GPS_GSV_DATA;
				}
				break;

			// active satellites
			case MINMEA_SENTENCE_GSA:
				if(get_gsa_frame(tempx, pgsa) > 0)
				{
					//sprintf(tempx,"GSA total sats: %d",gsa.fix_type);
					client_send_data = SEND_GPS_GSA_DATA;
				}
				break;

			// lat/long, time 
			case MINMEA_SENTENCE_GLL:
				if(get_gll_frame(tempx, pgll) > 0)
				{
	//				sprintf(tempx,"long: %f lat %f",minmea_tocoord(&gll.latitude), 
	//					minmea_tocoord(&gll.longitude));
	//				sprintf(tempx,"long: %f lat %f",minmea_tofloat(&gll.latitude),
	//						minmea_tofloat(&gll.longitude));
	//				sprintf(tempx,"%d %d",gll.latitude.value, gll.longitude.value);
					client_send_data = SEND_GPS_GLL_DATA;
				}
				break;

			case MINMEA_SENTENCE_VTG:
				if(get_vtg_frame(tempx, pvtg) > 0)
				{
/*
					sprintf(tempx,"VTG true course: %f mag track: %f speed (kph) %f",
							minmea_tofloat(&vtg.true_track_degrees),
							minmea_tofloat(&vtg.magnetic_track_degrees),
							minmea_tofloat(&vtg.speed_kph));
*/
					client_send_data = SEND_GPS_VTG_DATA;
				}
				break;

			case MINMEA_SENTENCE_ZDA:
//				memcpy(pzda,vstruct,sizeof(struct minmea_sentence_zda));
//				sprintf(tempx,"ZDA");
					client_send_data = SEND_GPS_ZDA_DATA;
				break;

			case MINMEA_INVALID:
				printf("MINMEA_INVALID");
				break;

			default:
				printf("%d UNKNOWN",ret_val);
				break;
		}
		if(test_sock() && enable_gps_send_data == 1)
		{
			//printf("ret_val: %d\r\n",ret_val);
			size_msg = strlen(tempx) - 7;

			memcpy((void *)tempx, (const void *)(tempx + 7),(size_t)size_msg);
			tempx[size_msg] = 0;
			switch(ret_val)
			{
				case MINMEA_SENTENCE_RMC:
					send_msg(160,(UCHAR*)tempx, client_send_data);

					break;
				case MINMEA_SENTENCE_GGA:
					send_msg(160,(UCHAR*)tempx, client_send_data);
					break;
//				case MINMEA_SENTENCE_GST:
//					send_msg(160,(UCHAR*)tempx, client_send_data);
//					break;
				case MINMEA_SENTENCE_GSV:
					send_msg(160,(UCHAR*)tempx, client_send_data);
					break;
				case MINMEA_SENTENCE_GSA:
					send_msg(160,(UCHAR*)tempx, client_send_data);
					break;
				case MINMEA_SENTENCE_GLL:
					send_msg(160,(UCHAR*)tempx, client_send_data);
					break;
				case MINMEA_SENTENCE_VTG:
					send_msg(160,(UCHAR*)tempx, client_send_data);
					break;
				default:
					break;
			}
		}
		//printf("\r\n");

		if(shutdown_all)
		{
			return 0;
		}
	}
	return 1;
}

/*********************************************************************/
// serial receive task
UCHAR serial_recv_task(int test)
{
	serial_rec = 0;
	int i = 0;
	int j = 0;
	int k = 0;
	UCHAR ch, ch2;
	UCHAR cmd;
	UCHAR low_byte, high_byte;
	int engine_temp, rpm, mph, indoor_temp;
	int fd;
	char errmsg[20];
	char tempx[30];
	UCHAR send_buffer[15];
	int s;
	UINT temp;
	UCHAR ucbuff[6];
	int brights = 0;
	int fan_on = 0;
	int running_lights = 0;
	int lights_on = 0;
	int wipers = 0;

	memset(errmsg,0,20);

	usleep(_5SEC);	// delay 5 seconds because it hangs when trying
					// to send to STM32 when starting up

	if(fd = init_serial() < 0)
	{
		myprintf1("can't open comm port 1");
		printf("can't open comm port 1");
		//return 0;
	}

	if(fd = init_serial2() < 0)
	{
		myprintf1("can't open comm port 2");
		printf("can't open comm port 2");
	}

	init_LCD(1);

	//printString2("trying to open comm3");

	if(fd = init_serial3() < 0)
	{
		//myprintf1("can't open comm port 3");
		//printf("can't open comm port 3");
		//printString2("can't open comm3");
	}else
	{
		//printString2("comm3 open");
		//printString3("comm3 open");
	}
	ch = ch2 = 0x7e;

//	red_led(0);
//	green_led(0);

	s = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

	temp = ps.engine_temp_limit;
	send_buffer[0] = (UCHAR)temp;
	temp >>= 8;
	send_buffer[1] = (UCHAR)temp;

	temp = ps.cooling_fan_on;
	send_buffer[2] = (UCHAR)temp;
	temp >>= 8;
	send_buffer[3] = (UCHAR)temp;

	temp = ps.cooling_fan_off;
	send_buffer[4] = (UCHAR)temp;
	temp >>= 8;
	send_buffer[5] = (UCHAR)temp;

	temp = ps.blower1_on;
	send_buffer[6] = (UCHAR)temp;
	temp >>= 8;
	send_buffer[7] = (UCHAR)temp;

	temp = ps.blower2_on;
	send_buffer[8] = (UCHAR)temp;
	temp >>= 8;
	send_buffer[9] = (UCHAR)temp;

	temp = ps.blower3_on;
	send_buffer[10] = (UCHAR)temp;
	temp >>= 8;
	send_buffer[11] = (UCHAR)temp;

	temp = ps.blower_enabled;
	send_buffer[12] = (UCHAR)temp;
	temp >>= 8;
	send_buffer[13] = (UCHAR)temp;
/*
	temp = ps.password_timeout;
	send_buffer[14] = (UCHAR)temp;
	temp >>= 8;
	send_buffer[15] = (UCHAR)temp;

	temp = ps.password_retries;
	send_buffer[16] = (UCHAR)temp;
	temp >>= 8;
	send_buffer[17] = (UCHAR)temp;
*/

	// send msg to STM32 so it can play a set of beeps & blips
	send_serialother(SERVER_UP,&send_buffer[0]);
	//printString3("server up");

	while(TRUE)
	{
		pthread_mutex_lock( &serial_read_lock); 

		// serial data is SERIAL_BUFF_SIZE preceded by an 0xFF

		do {
			ch = read_serial(errmsg);
		}while(ch != 0xFF);

		for(i = 0;i < SERIAL_BUFF_SIZE;i++)
		{
			read_serial_buffer[i] = read_serial(errmsg);
		}
		pthread_mutex_unlock(&serial_read_lock);
		cmd = read_serial_buffer[0];

		if((cmd >= KP_1 && cmd <= KP_9) || cmd == KP_0)
		{
			switch(cmd)
			{
				case KP_0:
					break;
				case KP_1:
					cmd = START_SEQ;
					add_msg_queue(cmd);
//					strcpy(tempx,"ENGINE\0");
//					i = ENGINE;
					break;
				case KP_2:
					cmd = SHUTDOWN;
					add_msg_queue(cmd);
//					memset(tempx,0x20,22);
//					tempx[22] = 0;
//					i = ENGINE;
					break;
				case KP_3:
					if(lights_on == 1)
					{
						if(brights == 0)
						{
							cmd = ON_BRIGHTS;
//							i = BRIGHTS;
//							strcpy(tempx,"BRIGHTS\0");
							brights = 1;
						}else
						{
							cmd = OFF_BRIGHTS;
//							i = BRIGHTS;
//							memset(tempx,0x20,22);
//							tempx[22] = 0;
							brights = 0;
						}
						add_msg_queue(cmd);
					}
					break;
				case KP_4:
					if(fan_on == 0)
					{
						cmd = ON_FAN;
//						i = COOLING_FAN;
//						strcpy(tempx,"COOLINGFAN\0");
						fan_on = 1;
					}else
					{
						cmd = OFF_FAN;
//						i = COOLING_FAN;
//						memset(tempx,0x20,22);
//						tempx[22] = 0;
						fan_on = 0;
					}
					add_msg_queue(cmd);
					break;
				case KP_5:
					break;
				case KP_6:
					if(running_lights == 0)
					{
						cmd = ON_RUNNING_LIGHTS;
//						i = RUNNING_LIGHTS;
//						strcpy(tempx,"RUNNING LIGHTS\0");
						running_lights = 1;
					}else
					{
						cmd = OFF_RUNNING_LIGHTS;
//						i = RUNNING_LIGHTS;
//						memset(tempx,0x20,22);
//						tempx[22] = 0;
						running_lights = 0;
					}
					add_msg_queue(cmd);
					break;
				case KP_7:
					if(lights_on == 0)
					{
						cmd = ON_LIGHTS;
//						i = HEAD_LIGHTS;
//						strcpy(tempx,"HEADLIGHTS\0");
						lights_on = 1;
					}else
					{
						cmd = OFF_LIGHTS;
//						memset(tempx,0x20,22);
//						tempx[22] = 0;
//						i = HEAD_LIGHTS;
						lights_on = 0;
					}
					add_msg_queue(cmd);
					if(brights == 1)
					{
						brights = 0;
						cmd = OFF_BRIGHTS;
//						i = BRIGHTS;
//						memset(tempx,0x20,22);
//						tempx[22] = 0;
						add_msg_queue(cmd);
					}
					break;
				case KP_8:
					switch(wipers)
					{
						case 0:
							cmd = WIPER1;
							wipers = 1;
						break;
						case 1:
							cmd = WIPER2;
							wipers = 2;
						break;
						case 2:
							cmd = WIPER_OFF;
							wipers = 0;
						break;
						default:
							cmd = WIPER_OFF;
							wipers = 0;
						break;
						add_msg_queue(cmd);
					}
					break;
				case KP_9:
					break;
				default:
					break;
			}
/*
			ucbuff[0] = DISPLAY_STR;
			ucbuff[1] = rtlabel_str[i].row;
			ucbuff[2] = rtlabel_str[i].col;
			memcpy(ucbuff+3,tempx,strlen(tempx));
			send_lcd(ucbuff,strlen(tempx)+4);
			sprintf(tempx,"%d %d",i,rtlabel_str[i].row);
			printString3(tempx);
*/
		}
/*
				case KP_A:
					break;
				case KP_B:
					break;
				case KP_C:
					break;
				case KP_D:
					break;
*/
		// these are the keypad cmd's sent from STM32
		// but if the menus of the STM32/LCD/keypad select the feature
		// then the cmd is between and including NAV_UP & NAV_CLOSE
		// (see below)
/*
		if(cmd >= ENABLE_START && cmd <= WIPER_OFF)
		{
			add_msg_queue(cmd);		// send msg to basic_controls_task
		}else
*/
		if(cmd == ENGINE_TEMP)
		{
			high_byte = read_serial_buffer[1];
			low_byte = read_serial_buffer[2];

			engine_temp = (int)high_byte;
			engine_temp <<= 8;
			engine_temp |= (int)low_byte;

			temp = (int)high_byte;
			temp <<= 8;
			temp |= (int)low_byte;

//			sprintf(tempx,"%d %2x %2x %.1f\0",engine_temp, high_byte, low_byte, convertF(engine_temp));
			sprintf(tempx,"%.1f\0", convertF(engine_temp));
			if(test_sock())
	 			send_msg(strlen((char*)tempx)*2,(UCHAR*)tempx, ENGINE_TEMP);

			ucbuff[0] = DISPLAY_STR;
			ucbuff[1] = rtlabel_str[ENG_TEMP].row;
			ucbuff[2] = rtlabel_str[ENG_TEMP].data_col;
			memcpy(ucbuff+3,tempx,strlen(tempx));
			send_lcd(ucbuff,strlen(tempx)+4);
/*
			ucbuff[0] = DISPLAY_TEMP;
			ucbuff[1] = rtlabel_str[ENGINE_TEMP].row;
			ucbuff[2] = rtlabel_str[ENGINE_TEMP].data_col;
			ucbuff[3] = high_byte;
			ucbuff[4] = low_byte;
*/
		}else

		if(cmd == INDOOR_TEMP)
		{
			high_byte = read_serial_buffer[1];
			low_byte = read_serial_buffer[2];
			//printHexByte(high_byte);
			//printHexByte(low_byte);
			indoor_temp = (int)high_byte;
			indoor_temp <<= 8;
			indoor_temp |= (int)low_byte;

			temp = (int)high_byte;
			temp <<= 8;
			temp |= (int)low_byte;

			sprintf(tempx,"%.1f\0", convertF(indoor_temp));
			if(test_sock())
	 			send_msg(strlen((char*)tempx)*2,(UCHAR*)tempx, INDOOR_TEMP);
			ucbuff[0] = DISPLAY_STR;
			ucbuff[1] = rtlabel_str[IND_TEMP].row;
			ucbuff[2] = rtlabel_str[IND_TEMP].data_col;
			memcpy(ucbuff+3,tempx,strlen(tempx));
			send_lcd(ucbuff,strlen(tempx)+4);
		}else

		if(cmd == TEMP_TOO_HIGH)
		{

		}else

		if(cmd == SEND_RT_VALUES1)
		{
			low_byte = read_serial_buffer[1];
			high_byte = read_serial_buffer[2];
//			printHexByte(low_byte);
//			printHexByte(high_byte);
//			printString2("\r\n");

			rpm = (int)high_byte;
			rpm <<= 8;
			rpm |= (int)low_byte;

			sprintf(tempx,"%d",rpm);
//			if(rpm > 0)
//				printString2(tempx);
			if(test_sock())
				send_msg(strlen((char*)tempx)*2,(UCHAR*)tempx,SEND_RPM);

			low_byte = read_serial_buffer[3];
			high_byte = read_serial_buffer[4];
			mph = (int)high_byte;
			mph <<= 8;
			mph |= (int)low_byte;
			sprintf(tempx,"%d",mph);
//			if(mph > 0)
//				printString2(tempx);
//			printHexByte(low_byte);
//			printHexByte(high_byte);
//			printString2("\r\n");
			if(test_sock())
				send_msg(strlen((char*)tempx)*2,(UCHAR*)tempx,SEND_MPH);

			ucbuff[0] = SEND_INT_RT_VALUES;
			ucbuff[1] = rtlabel_str[RPM].row;
			ucbuff[2] = rtlabel_str[RPM].data_col;
			temp = rpm;
			ucbuff[4] = (UCHAR)temp;
			temp >>= 8;
			ucbuff[3] = (UCHAR)temp; 
			send_lcd(ucbuff, 5);
			//printf("%02x %02x\r\n",ucbuff[3], ucbuff[4]);

			ucbuff[0] = SEND_INT_RT_VALUES;
			ucbuff[1] = rtlabel_str[MPH].row;
			ucbuff[2] = rtlabel_str[MPH].data_col;
			temp = mph;
			ucbuff[4] = (UCHAR)temp;
			temp >>= 8;
			ucbuff[3] = (UCHAR)temp; 
			send_lcd(ucbuff, 5);
			//printf("%02x %02x\r\n",ucbuff[3], ucbuff[4]);

		}else

		if(cmd == SEND_RT_VALUES2)
		{
			low_byte = read_serial_buffer[1];
			ucbuff[0] = SEND_BYTE_RT_VALUES;
			ucbuff[1] = rtlabel_str[OIL_PRES].row;
			ucbuff[2] = rtlabel_str[OIL_PRES].data_col;
			ucbuff[3] = low_byte;
			send_lcd(ucbuff, 4);

			low_byte = read_serial_buffer[2];
			ucbuff[0] = SEND_BYTE_RT_VALUES;
			ucbuff[1] = rtlabel_str[FUEL_LEVEL].row;
			ucbuff[2] = rtlabel_str[FUEL_LEVEL].data_col;
			ucbuff[3] = low_byte;
			send_lcd(ucbuff, 4);

			low_byte = read_serial_buffer[3];
			ucbuff[0] = SEND_BYTE_RT_VALUES;
			ucbuff[1] = rtlabel_str[MAP].row;
			ucbuff[2] = rtlabel_str[MAP].data_col;
			ucbuff[3] = low_byte;
			send_lcd(ucbuff, 4);

			low_byte = read_serial_buffer[4];
			ucbuff[0] = SEND_BYTE_RT_VALUES;
			ucbuff[1] = rtlabel_str[O2].row;
			ucbuff[2] = rtlabel_str[O2].data_col;
			ucbuff[3] = low_byte;
			send_lcd(ucbuff, 4);

			sprintf(tempx, "%02x %02x %02x %02x", read_serial_buffer[1], read_serial_buffer[2],
						read_serial_buffer[3],read_serial_buffer[4]);

			if(test_sock())
				send_msg(strlen((char*)tempx)*2,(UCHAR*)tempx,SEND_ADCS1);

			//printString3(tempx);
		}else

		if(cmd == SEND_RT_VALUES3)
		{
			sprintf(tempx, "%02x %02x %02x %02x", read_serial_buffer[1], read_serial_buffer[2],
						read_serial_buffer[3],read_serial_buffer[4]);

			if(test_sock())
				send_msg(strlen((char*)tempx)*2,(UCHAR*)tempx,SEND_ADCS2);

			//printString3(tempx);
		}else

		if(cmd >= NAV_UP && cmd <= NAV_CLOSE)
		{
			sprintf(tempx,"nav cmd: %d",cmd);
//			printString2(tempx);
			if(test_sock())
				send_msg(strlen((char*)tempx)*2,(UCHAR*)tempx, cmd);
		}else 

		if(cmd == NAV_NUM)
		{
			temp = read_serial_buffer[2];
			temp <<= 8;
			temp |= read_serial_buffer[1];
			sprintf(tempx,"engine temp limit: %.1f\0", convertF(temp));
			//printString2(tempx);

			temp = read_serial_buffer[4];
			temp <<= 8;
			temp |= read_serial_buffer[3];
			sprintf(tempx,"cooling fan on: %.1f\0", convertF(temp));
			//printString2(tempx);

			temp = read_serial_buffer[6];
			temp <<= 8;
			temp |= read_serial_buffer[5];
			sprintf(tempx,"cooling fan off: %.1f\0", convertF(temp));
			//printString2(tempx);
		}else

		if(cmd == GET_CONFIG2)
		{
			temp = read_serial_buffer[2];
			temp <<= 8;
			temp |= read_serial_buffer[1];
			sprintf(tempx,"fan on: %.1f\0", convertF(temp));
			//printString2(tempx);

			temp = read_serial_buffer[4];
			temp <<= 8;
			temp |= read_serial_buffer[3];
			sprintf(tempx,"fan off: %.1f\0", convertF(temp));
			//printString2(tempx);

			temp = read_serial_buffer[6];
			temp <<= 8;
			temp |= read_serial_buffer[5];
			sprintf(tempx,"engine temp limit: %.1f\0", convertF(temp));
			//printString2(tempx);
		}else

		if(cmd == PASSWORD_OK)
		{
			//printString2("password ok");
		}else

		if(cmd == PASSWORD_BAD)
		{
			sprintf(tempx,"password: %d %d",read_serial_buffer[1],read_serial_buffer[2]);
			//printString2(tempx);
		}


		if(shutdown_all)
		{
//			printf("shutting down serial task\r\n");
			close_serial();
			close_serial2();
			close_serial3();
//			myprintf1("done serial task\r\n");
			//printString2("closing serial ports");
//			printf("serial ports done\r\n");
			return 0;
		}

	}
	return 1;
}

// client calls 'connect' to get accept call below to stop
// blocking and return sd2 socket descriptor

static struct  sockaddr_in sad;				  /* structure to hold server's address  */
static int sock_open;

/*********************************************************************/
// task to monitor for a client requesting a connection
UCHAR tcp_monitor_task(int test)
{
	int ret = -1;
	struct timeval tv;

	struct  hostent   *ptrh;				  /* pointer to a host table entry */
	struct  protoent  *ptrp;				  /* pointer to a protocol table entry */
	struct  sockaddr_in sad;				  /* structure to hold server's address */
	struct  sockaddr_in cad;				  /* structure to hold client's address */
	int     listen_sd;						  /* socket descriptors */
	int     port;							  /* protocol port number */
	int     alen;							  /* length of address */
	UCHAR cmd;
	port = PROTOPORT;
	sock_open = 0;
	tv.tv_sec = 2;
	tv.tv_usec = 50000;
	UCHAR tempx[20];
	int s;

	s = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
//	if(s != 0)
//		handle_err_en(s, "pthread_setcancelstate");
//		printf("setcancelstate\r\n");

	memset((char  *)&sad,0,sizeof(sad));	  /* clear sockaddr structure   */
	sad.sin_family = AF_INET;				  /* set family to Internet     */
	sad.sin_addr.s_addr = INADDR_ANY;		  /* set the local IP address */

	sad.sin_port = htons((u_short)port);

	global_socket = -1;

// getprotobyname doesn't work on TS-7200 because there's no /etc/protocols file
// so just use '6'
#ifndef MAKE_TARGET
	if ( ((int)(ptrp = getprotobyname("tcp"))) == 0)
	{
		//printString2("cannot map tcp to protocol number");
//		printf("cannot map tcp to protocol number\r\n");
//			exit (1);
	}
	listen_sd = socket (PF_INET, SOCK_STREAM, ptrp->p_proto);

// getprotobyname doesn't work on TS-7200 because there's no /etc/protocols file
// so just use '6' as the tcp protocol number
#else
	listen_sd = socket (PF_INET, SOCK_STREAM, 6);
#endif
	if (listen_sd < 0)
	{
		//printString2("socket creation failed");
//		printf("socket creation failed\r\n");
//			exit(1);
	}

/* Bind a local address to the socket */
	if (bind(listen_sd, (struct sockaddr *)&sad, sizeof (sad)) < 0)
	{
		//printString2("bind failed");
//		printf("bind failed\r\n");
//		exit(1);
	}

/* Specify a size of request queue */
	if (listen(listen_sd, QLEN) < 0)
	{
		//printString2("listen failed");
//		printf("listen failed\r\n");
//			exit(1);
	}

	alen = sizeof(cad);

/* Main server loop - accept and handle requests */
	while (TRUE)
	{
		if(test_sock())
		{
			uSleep(0,1000);
			if(shutdown_all)
			{
//				strcpy(tempx,"shutdown...\0");
//				pthread_cancel(serial_thread);
//				printf("cancel serial_thread\r\n");
//				send_msg(strlen((char*)tempx)*2,(UCHAR*)tempx, SHUTDOWN);
				//printString2("closing socket");
//				uSleep(2,0);
				close_tcp();
//				printf("done tcp_mon\r\n");
				return 0;
			}
		}
		else
		{
//			myprintf1("Server Waiting...\0");
			//printString2("Server Waiting");
//			printf("Server Waiting...\r\n");
//
			if (  (global_socket=accept(listen_sd, (struct sockaddr *)&cad, (socklen_t *)&alen)) < 0)
			{
//				myprintf1("accept failed\0");
				//printString2("accept failed");
				return 0;
//					exit (1);
			}
			if(global_socket > 0)
				sock_open = 1;
//			myprintf1("connected to socket: \0");
			//printString2("connected to socket: \0");
//			printf("connected to socket: \r\n");

			 //send_param_msg();
//			 send_status_msg();

			// this is for when the engine is already running and the client
			// logs in 

			if(engine_running)
				strcpy(tempx,"START_SEQ");
			else strcpy(tempx,"SHUTDOWN");	

			send_msg(strlen((char*)tempx)*2,(UCHAR*)tempx, SEND_MSG);

			if(shutdown_all)
			{
//				printf("tcp task closing\r\n");
				//printString2("closing tcp");
				return 0;
			}
/*
		if (setsockopt (global_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval)) < 0)
			return -2;
		if (setsockopt (global_socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(struct timeval)) < 0)
			return -3;
*/
		}
	}
	return 1;
}

/*********************************************************************/
int test_sock(void)
{
	return sock_open;
}

/*********************************************************************/
void close_tcp(void)
{
	if(sock_open)
	{
		sock_open = 0;
//		printf("closing socket...\r\n");
		close(global_socket);
//		printf("socket closed!\r\n");
		global_socket = -1;
	}else
	{
		myprintf1("socket already closed\0");
//		printf("socket already closed\r\n");
	}
}

/**********************************************************************/
void *work_routine(void *arg)
{
	int *my_id=(int *)arg;
	int i;
	UCHAR pattern = 0;
	int not_done=1;
	i = not_done;
	shutdown_all = 0;

	pthread_mutex_lock(&threads_ready_lock);
	threads_ready_count++;
	if (threads_ready_count == NUM_TASKS)
	{
/* I was the last thread to become ready.  Tell the rest. */
		pthread_cond_broadcast(&threads_ready);
	}
	else
	{
/* At least one thread isn't ready.  Wait. */
		while (threads_ready_count != NUM_TASKS)
		{
			pthread_cond_wait(&threads_ready, &threads_ready_lock);
		}
	}
	pthread_mutex_unlock(&threads_ready_lock);

	while(not_done)
	{
		(*fptr[*my_id])(i);
		i--;
		not_done--;
	}
	return(NULL);
}
/*********************************************************************/
void add_msg_queue(UCHAR cmd)
{
//	while(msg_queue_ptr >= MSG_QUEUE_SIZE);
	pthread_mutex_lock(&msg_queue_lock);
	if(msg_queue_ptr < MSG_QUEUE_SIZE)
	{
		msg_queue_ptr++;
		msg_queue[msg_queue_ptr] = cmd;
	}
	pthread_mutex_unlock(&msg_queue_lock);
//	printf("add: %d %x\r\n",msg_queue_ptr,cmd);
}
/*********************************************************************/
UCHAR get_msg_queue(void)
{
	UCHAR cmd;
	pthread_mutex_lock(&msg_queue_lock);
	if(msg_queue_ptr > 0)
	{
		cmd = msg_queue[msg_queue_ptr];
		msg_queue_ptr--;
	}else cmd = 0;
	pthread_mutex_unlock(&msg_queue_lock);
//	if(cmd != 0)
//		printf("get: %d %x\r\n",msg_queue_ptr,cmd);
	return cmd;
}

/*********************************************************************/
UCHAR basic_controls_task(int test)
{
	int i,j;
	UCHAR onoff;
	O_DATA *otp;
	O_DATA **otpp = &otp;
	int rc;
	int index;
	size_t isize;
	size_t osize;
	char errmsg[50];
	UCHAR cmd;
	char tempx[SERIAL_BUFF_SIZE];

//	static UCHAR buffer[30] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
//				17,18,19,20,21,22,23,24,25,26,27,28,29};

	static SPECIAL_CMD_ARR sp_cmd_arr[16] = {
	{LHEADLAMP,1},
	{LBRIGHTS,1},
	{RHEADLAMP,1},
	{RBRIGHTS,1},
	{LBRIGHTS,0},
	{LHEADLAMP,0},
	{RBRIGHTS,0},
	{RHEADLAMP,0},
	{RHEADLAMP,1},
	{RBRIGHTS,1},
	{LHEADLAMP,1},
	{LBRIGHTS,1},
	{LBRIGHTS,0},
	{LHEADLAMP,0},
	{RBRIGHTS,0},
	{RHEADLAMP,0}};

	//strcpy(tempx,"shutdown...\0");
	memset(msg_queue,0,sizeof(msg_queue));
	msg_queue_ptr = 0;

	while(TRUE)
	{
		// wait for a new cmd to arrive in the msg_queue
		do{
			cmd = get_msg_queue();
			usleep(_5MS);
		}while(cmd == 0 && shutdown_all == 0);

		usleep(_5MS);

		switch(cmd)
		{
			case ON_FAN:
			case OFF_FAN:
			case ON_LIGHTS:
			case OFF_LIGHTS:
			case START_SEQ:
			case SHUTDOWN:
			case BLOWER_OFF:
			case BLOWER1:
			case BLOWER2:
			case BLOWER3:
			case ON_RUNNING_LIGHTS:
			case OFF_RUNNING_LIGHTS:
			case ON_BRIGHTS:
			case OFF_BRIGHTS:
			case ON_BRAKES:
			case OFF_BRAKES:
			case WIPER1:
			case WIPER2:
			case WIPER_OFF:
			case SPECIAL_CMD:
			case ON_FUEL_PUMP:
			case OFF_FUEL_PUMP:
			case ON_LLIGHTS:
			case OFF_LLIGHTS:
			case ON_LBRIGHTS:
			case OFF_LBRIGHTS:
			case ON_RLIGHTS:
			case OFF_RLIGHTS:
			case ON_RBRIGHTS:
			case OFF_RBRIGHTS:
//			case ESTOP_SIGNAL:
				//send_serialother(cmd,tempx);
//				myprintf1(cmd_array[cmd].cmd_str);
				//printString3(cmd_array[cmd].cmd_str);
				if(test_sock())
				{
					sprintf(tempx,"%s",cmd_array[cmd].cmd_str);
					send_msg(strlen((char*)tempx)*2,(UCHAR*)tempx, SEND_MSG);
				}
// comment out the printString2 when using on actual iobox
//				printString2(tempx);

			break;
			default:
			break;
		}
//#if 0
		switch(cmd)
		{
			case ON_FAN:
			case OFF_FAN:
			case ON_LIGHTS:
			case OFF_LIGHTS:
			case START_SEQ:
			case SHUTDOWN:
			case ON_RUNNING_LIGHTS:
			case OFF_RUNNING_LIGHTS:
			case ON_BRIGHTS:
			case OFF_BRIGHTS:
			case ON_BRAKES:
			case OFF_BRAKES:
				break;
			default:
				break;
		}
		switch(cmd)
		{

			case ENABLE_START:
	//			index = STARTER;
	//			rc = ollist_find_data(index,otpp,&oll);
	//			otp->onoff = 1;
	//			ollist_insert_data(index,&oll,otp);
				change_input(STARTER_INPUT,1);
				//printString2("starter on");
				break;

			case STARTER_OFF:	// starter shuts off by itself (type 2 - timed)
								// so turning it "off" will just start the timer again
	//			index = STARTER;
	//			rc = ollist_find_data(index,otpp,&oll);
	//			otp->onoff = 0;
	//			ollist_insert_data(index,&oll,otp);
				change_input(STARTER_INPUT,0);
				//printString2("starter off");
				break;

			case ON_ACC:
				index = ACCON;
				rc = ollist_find_data(index,otpp,&oll);
	//			printf("%s %d %d %d\r\n",otp->label,otp->port,otp->onoff,otp->reset);
	//			otp->onoff = 1;
				ollist_insert_data(index,&oll,otp);
	//			change_input(ACCON, 1);
				if(otp->reset == 0)
					change_output(ACCON, 1);
				break;

			case OFF_ACC:
				index = ACCON;
				rc = ollist_find_data(index,otpp,&oll);
	//			printf("%s %d %d %d\r\n",otp->label,otp->port,otp->onoff,otp->reset);
	//			otp->onoff = 0;
				ollist_insert_data(index,&oll,otp);
	//			change_input(ACCON, 0);
				if(otp->reset == 0)
					change_output(ACCON, 0);
				break;

			case ON_FUEL_PUMP:
				index = FUELPUMP;
				rc = ollist_find_data(index,otpp,&oll);
	//			printf("%s %d %d %d\r\n",otp->label,otp->port,otp->onoff,otp->reset);
				if(otp->reset == 0)
				{
					change_output(FUELPUMP, 1);
					otp->onoff = 1;
				}
				ollist_insert_data(index,&oll,otp);
				break;

			case OFF_FUEL_PUMP:

				index = FUELPUMP;
				rc = ollist_find_data(index,otpp,&oll);
	//			printf("%s %d %d %d\r\n",otp->label,otp->port,otp->onoff,otp->reset);

				if(otp->reset == 0)
				{
					change_output(FUELPUMP, 0);
					otp->onoff = 0;
					ollist_insert_data(index,&oll,otp);
				}
				break;

			case ON_FAN:
				change_input(COOLINGFAN_INPUT, 1);
				index = COOLINGFAN;
				rc = ollist_find_data(index,otpp,&oll);
				otp->onoff = 1;
				ollist_insert_data(index,&oll,otp);
				break;

			case OFF_FAN:
				change_input(COOLINGFAN_INPUT, 0);
				index = COOLINGFAN;
				rc = ollist_find_data(index,otpp,&oll);
				otp->onoff = 0;
				ollist_insert_data(index,&oll,otp);
				break;

			case ON_LIGHTS:
				change_input(HEADLAMP_INPUT, 1);
				index = LHEADLAMP;
				rc = ollist_find_data(index,otpp,&oll);
				otp->onoff = 1;
				ollist_insert_data(index,&oll,otp);
				index = RHEADLAMP;
				rc = ollist_find_data(index,otpp,&oll);
				otp->onoff = 1;
				ollist_insert_data(index,&oll,otp);
//				printString2("lights on");
				break;

			case OFF_LIGHTS:
				change_input(HEADLAMP_INPUT, 0);
				index = LHEADLAMP;
				rc = ollist_find_data(index,otpp,&oll);
				otp->onoff = 0;
				ollist_insert_data(index,&oll,otp);
				index = RHEADLAMP;
				rc = ollist_find_data(index,otpp,&oll);
				otp->onoff = 0;
				ollist_insert_data(index,&oll,otp);
//				printString2("off lights");
				break;

			case ON_LLIGHTS:
				index = LHEADLAMP;
				ollist_find_data(index,otpp,&oll);
				set_output(otp,1);
				break;

			case OFF_LLIGHTS:
				index = LHEADLAMP;
				ollist_find_data(index,otpp,&oll);
				set_output(otp,0);
				break;

			case ON_RLIGHTS:
				index = RHEADLAMP;
				ollist_find_data(index,otpp,&oll);
				set_output(otp,1);
				break;

			case OFF_RLIGHTS:
				index = RHEADLAMP;
				ollist_find_data(index,otpp,&oll);
				set_output(otp,0);
				break;

			case ON_RUNNING_LIGHTS:
				change_input(RUNNING_LIGHTS_INPUT, 1);
				index = RUNNINGLIGHTS;
				rc = ollist_find_data(index,otpp,&oll);
				otp->onoff = 1;
				ollist_insert_data(index,&oll,otp);
				break;

			case OFF_RUNNING_LIGHTS:
				change_input(RUNNING_LIGHTS_INPUT, 0);
				index = RUNNINGLIGHTS;
				rc = ollist_find_data(index,otpp,&oll);
				otp->onoff = 0;
				ollist_insert_data(index,&oll,otp);
				//printString2("off running lights");
				break;

			case SPECIAL_CMD:
				for(j = 0;j < 2;j++)
					for(i = 0;i < 16;i++)
					{
						index = sp_cmd_arr[i].index;
						ollist_find_data(index,otpp,&oll);
						set_output(otp,sp_cmd_arr[i].onoff);
						usleep(_100MS);
					}
				break;

			case ON_BRIGHTS:
				change_input(BRIGHTS_INPUT, 1);
				index = LBRIGHTS;
				rc = ollist_find_data(index,otpp,&oll);
				otp->onoff = 1;
				ollist_insert_data(index,&oll,otp);
				index = RBRIGHTS;
				rc = ollist_find_data(index,otpp,&oll);
				otp->onoff = 1;
				ollist_insert_data(index,&oll,otp);
				break;

			case OFF_BRIGHTS:
				change_input(BRIGHTS_INPUT, 0);
				index = LBRIGHTS;
				rc = ollist_find_data(index,otpp,&oll);
				otp->onoff = 0;
				ollist_insert_data(index,&oll,otp);
				index = RBRIGHTS;
				rc = ollist_find_data(index,otpp,&oll);
				otp->onoff = 0;
				ollist_insert_data(index,&oll,otp);
				break;

			case ON_RBRIGHTS:
				index = RBRIGHTS;
				ollist_find_data(index,otpp,&oll);
				set_output(otp,1);
				break;

			case OFF_RBRIGHTS:
				index = RBRIGHTS;
				ollist_find_data(index,otpp,&oll);
				set_output(otp,0);
				break;

			case ON_LBRIGHTS:
				index = LBRIGHTS;
				ollist_find_data(index,otpp,&oll);
				set_output(otp,1);
				break;

			case OFF_LBRIGHTS:
				index = LBRIGHTS;
				ollist_find_data(index,otpp,&oll);
				set_output(otp,0);
				break;

			case TEST_LEFT_BLINKER:
				change_input(LEFTBLINKER_INPUT,1);
 				test_lblinkers = 1;
			break;

			case TEST_RIGHT_BLINKER:
				change_input(RIGHTBLINKER_INPUT,1);
				test_rblinkers = 1;
			break;

			case ON_BRAKES:
				change_input(BRAKE_INPUT,1);
			break;

			case OFF_BRAKES:
				change_input(BRAKE_INPUT,0);
			break;

			// turn off all the others first then turn on
			// the one relay according to the command
			case BLOWER1:
				index = HTRBLOWERMED;
				ollist_find_data(index,otpp,&oll);
				set_output(otp,0);
				index = HTRBLOWERHIGH;
				ollist_find_data(index,otpp,&oll);
				usleep(_100MS);
				set_output(otp,0);
				index = HTRBLOWERLOW;
				ollist_find_data(index,otpp,&oll);
				usleep(_100MS);
				set_output(otp,1);
			break;

			case BLOWER2:
				index = HTRBLOWERHIGH;
				ollist_find_data(index,otpp,&oll);
				set_output(otp,0);
				index = HTRBLOWERLOW;
				ollist_find_data(index,otpp,&oll);
				usleep(_100MS);
				set_output(otp,0);
				index = HTRBLOWERMED;
				ollist_find_data(index,otpp,&oll);
				usleep(_100MS);
				set_output(otp,1);
			break;

			case BLOWER3:
				index = HTRBLOWERMED;
				ollist_find_data(index,otpp,&oll);
				set_output(otp,0);
				index = HTRBLOWERLOW;
				ollist_find_data(index,otpp,&oll);
				usleep(_100MS);
				set_output(otp,0);
				index = HTRBLOWERHIGH;
				ollist_find_data(index,otpp,&oll);
				usleep(_100MS);
				set_output(otp,1);
			break;

			// turn all off
			case BLOWER_OFF:
				index = HTRBLOWERMED;
				ollist_find_data(index,otpp,&oll);
				set_output(otp,0);
				index = HTRBLOWERHIGH;
				ollist_find_data(index,otpp,&oll);
				usleep(_100MS);
				set_output(otp,0);
				index = HTRBLOWERLOW;
				ollist_find_data(index,otpp,&oll);
				usleep(_100MS);
				set_output(otp,0);
			break;

			case WIPER1:
				index = WWIPER1;
				ollist_find_data(index,otpp,&oll);
				set_output(otp,1);
				index = WWIPER2;
				ollist_find_data(index,otpp,&oll);
				usleep(_100MS);
				set_output(otp,0);
			break;

			case WIPER2:
				index = WWIPER1;
				ollist_find_data(index,otpp,&oll);
				set_output(otp,1);
				index = WWIPER2;
				ollist_find_data(index,otpp,&oll);
				usleep(_100MS);
				set_output(otp,1);
			break;

			case WIPER_OFF:
				index = WWIPER1;
				ollist_find_data(index,otpp,&oll);
				set_output(otp,0);
				index = WWIPER2;
				ollist_find_data(index,otpp,&oll);
				usleep(_100MS);
				set_output(otp,0);
			break;

			case TEST_ALL_IO:
/*
				for(i = ps.test_bank*8;i < (ps.test_bank+1)*8;i++)
				{
					ollist_find_data(i,otpp,&oll);
					set_output(otp,1);
					myprintf2("test: ",i);
//					printf("\r\n %d \r\n");
					usleep(1000000);
					set_output(otp,0);
					usleep(_100MS);
	//				printf("%d\r\n",i);

				}
*/
			break;

			case START_SEQ:
				//myprintf1("start seq\0");
				ollist_find_data(ACCON,&otp,&oll);
				otp->onoff = 1;
				otp->reset = 0;
	//			printf("%d %s\r\n",otp->port,otp->label);
	//			change_output(otp->port,otp->onoff);
				change_output(ACCON,1);
				ollist_insert_data(otp->port,&oll,otp);
				ollist_find_data(FUELPUMP,&otp,&oll);
				otp->onoff = 1;
				otp->reset = 0;
	//			printf("%d %s\r\n",otp->port,otp->label);
	//			change_output(otp->port,otp->onoff);
				change_output(FUELPUMP,1);
				ollist_insert_data(otp->port,&oll,otp);

				index = STARTER;
				rc = ollist_find_data(index,otpp,&oll);
	//			printf("starter on: port: %d onoff: %d type: %d reset: %d\r\n",otp->port,otp->onoff,otp->type,otp->reset);
	//			otp->onoff = 0;
	//			otp->reset = 0;
	//			TOGGLE_OTP;
	//			ollist_insert_data(index,&oll,otp);
				usleep(_100MS);
				change_input(STARTER_INPUT, 0);
				usleep(_10MS);
				change_input(STARTER_INPUT, 1);
	//			printf("starter on: port: %d onoff: %d type: %d reset: %d\r\n",otp->port,otp->onoff,otp->type,otp->reset);
				engine_running = 1;
	//			printf("engine_running: %d\r\n",engine_running);
//				tempx[0] = 1;	// set key_mode to 'PASSWORD'
//				send_serialother(SET_KEYMODE,(UCHAR*)tempx);
				break;

			case SHUTDOWN:
				//myprintf1("shutdown engine\0");
				running_seconds = running_minutes = running_hours = 0;
//				printf("engine_running: %d\r\n",engine_running);
				engine_running = 0;
				change_output(ACCON, 0);
				ollist_find_data(ACCON,&otp,&oll);
				otp->onoff = 0;
				ollist_insert_data(otp->port,&oll,otp);
				usleep(_10MS);
				change_output(FUELPUMP, 0);
				ollist_find_data(FUELPUMP,&otp,&oll);
				otp->onoff = 0;
				ollist_insert_data(otp->port,&oll,otp);
//				index = STARTER;
//				rc = ollist_find_data(index,otpp,&oll);
//				otp->onoff = 0;
//				set_output(STARTER,0);
//				ollist_insert_data(index,&oll,otp);
				lights_on_countdown = actual_lights_on_delay;
//				add_msg_queue(OFF_RUNNING_LIGHTS);
				break;
/*
			case ESTOP_SIGNAL:
				sprintf(tempx,"engine off from estop");
				send_msg(strlen((char*)tempx)*2,(UCHAR*)tempx, ESTOP_SIGNAL);
				break;
*/
			case SHUTDOWN_IOBOX:
// read_led & green_led can only be seen on test bench
#if 0
				for(i = 0;i < 10;i++)
				{
					red_led(1);		// these lights on on the board
					usleep(2000);	// aren't visible in the iobox
					red_led(0);
					green_led(1);
					usleep(2000);
					green_led(0);
					red_led(1);
					usleep(2000);
					red_led(0);
					green_led(1);
					usleep(2000);
					green_led(0);
				}
#endif
				send_serial(SERVER_DOWN);
				myprintf1("shutdown iobox\0");
//				printf("shutdown iobox\r\n");
				//printString2("shutdown iobox");
				for(i = 0;i < 5;i++)		// scroll the display on the iobox
				{							// to see the last 10 msg's and pause
//					setdioline(7,1);		// for 1 sec each time
					uSleep(0,100);
//					scroll_up();
//					setdioline(7,0);
					uSleep(0,100);
//					scroll_up();
				}
//				setdioline(7,0);
//				printf("shutdown iobox\r\n");
				shutdown_all = 1;
				reboot_on_exit = 3;
				break;

			case REBOOT_IOBOX:
				send_serial(SERVER_DOWN);

#if 0
				setdioline(7,1);
				for(i = 0;i < 10;i++)
				{
					red_led(1);
					usleep(20000);
					red_led(0);
					green_led(1);
					usleep(20000);
					green_led(0);
					red_led(1);
					usleep(20000);
					red_led(0);
					green_led(1);
					usleep(20000);
					green_led(0);
				}
#endif
//				setdioline(7,1);
				myprintf1("reboot iobox\0");

				for(i = 0;i < 5;i++)
				{
//					setdioline(7,1);
					uSleep(0,100);
//					scroll_up();
//					setdioline(7,0);
					uSleep(0,100);
//					scroll_up();
				}
//				setdioline(7,0);

				shutdown_all = 1;
				reboot_on_exit = 2;
				break;

			case UPLOAD_NEW:
				send_serial(SERVER_DOWN);
				shutdown_all = 1;
				reboot_on_exit = 4;
				break;

			case UPLOAD_NEW_PARAM:
				send_serial(SERVER_DOWN);
				//printf("upload new param...\r\n");
				shutdown_all = 1;
				reboot_on_exit = 5;
				break;

			case SHELL_AND_RENAME:
				send_serial(SERVER_DOWN);
				//printString3("shell and rename...\r\n");
				shutdown_all = 1;
				reboot_on_exit = 6;
				break;

			case UPLOAD_OTHER:
				shutdown_all = 1;
				reboot_on_exit = 1;
				//printf("upload other...\r\n");
				break;

				default:
				break;
		}	// end of switch

		if(shutdown_all == 1)
		{
//			myprintf1("done basic task");
//			printf("done basic task\r\n");
			return 0;
		}

	}
	return 1;
}

float convertF(int raw_data)
{
	float T_F, T_celcius;
	int ret;
	if ((raw_data & 0x100) != 0)
	{
		raw_data = - (((~raw_data)+1) & 0xff); /* take 2's comp */
	}
	T_celcius = raw_data * 0.5;
	T_F = (T_celcius * 1.8) + 32;
	ret = (int)T_F;
	return ret;	// returns 257 -> -67
}

