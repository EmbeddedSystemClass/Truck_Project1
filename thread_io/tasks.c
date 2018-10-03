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
#include "queue/illist_threads_rw.h"
#include "queue/ollist_threads_rw.h"
//#include "queue/rt_llist_threads_rw.h"
#include "tasks.h"
#include "ncurses/config_file.h"
#include "lcd_func.h"
#define TOGGLE_OTP otp->onoff = (otp->onoff == 1?0:1)

pthread_cond_t       threads_ready;
pthread_mutex_t     tcp_write_lock=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t     tcp_read_lock=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t     io_mem_lock=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t     serial_write_lock=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t     serial_read_lock=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t     serial_write_lock2=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t     serial_read_lock2=PTHREAD_MUTEX_INITIALIZER;
int total_count;

UCHAR (*fptr[NUM_TASKS])(int) = { get_host_cmd_task, monitor_input_task, 
monitor_fake_input_task, timer_task, timer2_task, read_button_inputs, serial_recv_task, tcp_monitor_task};

int threads_ready_count=0;
pthread_cond_t    threads_ready=PTHREAD_COND_INITIALIZER;
pthread_mutex_t   threads_ready_lock=PTHREAD_MUTEX_INITIALIZER;
static UCHAR check_inputs(int index, int test);

illist_t ill;
ollist_t oll;

//rt_llist_t roll;

extern int ilLoadConfig(char *filename, illist_t *ill ,size_t size, char *errmsg);
extern int olLoadConfig(char *filename, ollist_t *oll, size_t size, char *errmsg);

#define SERIAL_BUFF_SIZE  20

//static UCHAR serial_buff[SERIAL_BUFF_SIZE];
//static int no_serial_buff;

static int serial_rec;
static void set_output(O_DATA *otp, int onoff);
static UCHAR inportstatus[OUTPORTF_OFFSET-OUTPORTA_OFFSET+1];
static UCHAR fake_inportstatus1[OUTPORTF_OFFSET-OUTPORTA_OFFSET+1];
static UCHAR fake_inportstatus2[OUTPORTF_OFFSET-OUTPORTA_OFFSET+1];
static int mask2int(UCHAR mask);

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

void send_live_code(UCHAR cmd)
{
	if(live_window_on == 0)
		return;

	UCHAR status_line[10];

	memset(status_line,0,10);
	status_line[0] = cmd;
	status_line[1] = running_seconds;
	status_line[2] = running_minutes;
	status_line[3] = running_hours;
	send_tcp((UCHAR*)status_line,10);
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

//	myprintf2("port: ",otp->port);	
//	myprintf2("onoff: ",otp->onoff);	
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
			if(otp->reset == 0)
			{
				otp->reset = 1;
				otp->time_left = otp->time_delay;
//							otp->onoff = onoff;
//				TOGGLE_OTP;
				otp->onoff = 1;
				change_output(otp->port,1);
				ollist_insert_data(otp->port,&oll,otp);
//				printf("type %d port: %d onoff: %d reset: %d pol: %d\r\n",otp->type, otp->port,
//							otp->onoff, otp->reset, otp->polarity);
			}
			break;
		case 4:
			if(otp->reset == 0)
			{
//				printf("type 1 port: %d onoff: %d\r\n", otp->port, otp->onoff);
				otp->reset = 1;
//				if(otp->polarity == 0)
//				TOGGLE_OTP;
				otp->onoff = 0;
				change_output(otp->port,otp->onoff);
				ollist_insert_data(otp->port,&oll,otp);
				send_serial(ESTOPSWITCH,0);

//				printf("type 4a port: %d onoff: %d reset: %d \r\n\r\n", otp->port,
//										otp->onoff, otp->reset);
			}	
			else if(otp->reset == 1)
			{
//				if(otp->polarity == 1)
//					TOGGLE_OTP;
				otp->reset = 3;
//				printf("type 4b port: %d onoff: %d reset: %d \r\n\r\n", otp->port,
//										otp->onoff, otp->reset);
			}
			break;
		default:
			break;
	}
	// send message to monster box telling which port was toggled
	send_serial(otp->port, otp->onoff);
	
}
/*********************************************************************/
void send_serial(int port, int onoff)
{
//return;
// send what just changed to the PIC24/AVR to dispaly on screen
	pthread_mutex_lock( &serial_write_lock);
	
	write_serial(OUTPUT_MSG);
	write_serial((UCHAR)port);
 	write_serial((UCHAR)onoff);

	pthread_mutex_unlock(&serial_write_lock);
}
/*********************************************************************/
void send_serialother(UCHAR cmd, UCHAR data1, UCHAR data2, UCHAR data3, UCHAR data4)  
{
//return;
	pthread_mutex_lock( &serial_write_lock);

	write_serial(cmd);
	write_serial(data1);
	write_serial(data2);
	write_serial(data3);
	write_serial(data4);

	pthread_mutex_unlock(&serial_write_lock);
}
/*********************************************************************/
// if an input switch is changed, update the record for that switch
// and if an output applies to that input, change the output
// and record the event in llist - each input can be assigned any output
UCHAR monitor_input_task(int test)
{
	I_DATA *itp;
	I_DATA **itpp = &itp;
	O_DATA *otp;
	O_DATA **otpp = &otp;

	int status = -1;
	int bank, index;
	UCHAR result,result2, mask, onoff;
	int i, rc, flag;
	

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
				// itpp is a pointer to itp
				illist_find_data(index,itpp,&ill);

				// find the input assigned to the output(s)

				for(i = 0;i < 10;i++)
				{
					if(itp->affected_output[i] < 40)
					{
						ollist_find_data(itp->affected_output[i],otpp,&oll);
						set_output(otp,  onoff);
//						printf("port: %d\r\n",otp->port);
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
	I_DATA *itp;
	I_DATA **itpp = &itp;
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
//					printf("bad mask %02x\r\n",mask);
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
				illist_find_data(index,itpp,&ill);

				for(i = 0;i < 10;i++)
				{
					if(itp->affected_output[i] < 40)
					{
						ollist_find_data(itp->affected_output[i],otpp,&oll);
						set_output(otp,  onoff);
//						printf("port: %d\r\n",otp->port);
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
//				printf("done mon input tasks\r\n");
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
	return index;
}

/*********************************************************************/
// this happens 10x a second
UCHAR timer2_task(int test)
{
	int i;
	char tempx[20];
	int index = 0;
	int bank = 0;
	int fp;
	UCHAR mask;
	static int led_counter;
	static int led_onoff;
	static int seconds_counter;

	static I_DATA *itp2;
	static I_DATA **itpp2 = &itp2;

	static O_DATA *otp2;
	static O_DATA **otpp2 = &otp2;
	
	led_counter = 0;
	led_onoff = 0;
	
	while(TRUE)
	{
		uSleep(0,TIME_DELAY/10); 

		if(++led_counter > 9)
		{
				if(led_onoff == 1)
			{
				led_counter = 0;
				setdioline(7,1);
			}
			else
			{
				led_onoff = 7;
				setdioline(7,0);
			}
			led_onoff = (led_onoff == 1?0:1);
		}
		
		if(shutdown_all)
			return 0;
	}
	return 1;

}
/*********************************************************************/
// this happens once a second
UCHAR timer_task(int test)
{
	int i;
	UCHAR time_buffer[20];
	UCHAR s_buffer[5];
	char tempx[20];
	int index = 0;
	int bank = 0;
	int fp;
	UCHAR mask;
	time_t curtime2;
	struct timeval mtv;
	O_DATA *otp;
	O_DATA **otpp = &otp;

	rpm = mph = 0;
	while(TRUE)
	{
		uSleep(1,0);

		gettimeofday(&mtv, NULL);
		curtime2 = mtv.tv_sec;
		strftime((char*)&time_buffer[0],30,"%m-%d-%Y %T.\0",localtime(&curtime2));

		// send only date, month, minutes
		// 0,1 - month
		// 3,4 - date
		// 8,9 - year
		// 11,12 - hour
		// 14,15 - minute
		// 17,18 - seconds

		if(time_set)
		{
			send_serialother(TIME_DATA1,time_buffer[0],time_buffer[1],time_buffer[3],time_buffer[4]);
			send_serialother(TIME_DATA2,time_buffer[8],time_buffer[9],time_buffer[11],time_buffer[12]);
			send_serialother(TIME_DATA3,time_buffer[14],time_buffer[15],time_buffer[17],time_buffer[18]);
		}

// if driver seat switch goes low the tell monster box to start re-enter password sequence
//		send_serialother(RE_ENTER_PASSWORD1,0,0,0,0);

		if(engine_running > 0)
		{
			if(rpm > 5000)
				rpm = 500;

			if(++running_seconds > 59)
			{
				running_seconds = 0;
				if(++running_minutes > 59)
				{
					running_minutes = 0;
					if(++running_hours > 24)
						running_hours = 0;
				}
			}
		}

		odometer++;
		if(odometer % 2 == 0)
			trip++;

		s_buffer[0] = TOTAL_UP_TIME;
		s_buffer[1] = trunning_seconds;
		s_buffer[2] = trunning_minutes;
		s_buffer[3] = trunning_hours;

//		buffer[4] = rt_data[0];		// ADC data
//		buffer[5] = rt_data[1];
//		buffer[6] = rt_data[2];
//		buffer[7] = rt_data[3];

		if(++trunning_seconds > 59)
		{
			trunning_seconds = 0;
			if(++trunning_minutes > 59)
				trunning_minutes = 0;
		}

		if(live_window_on)
		{
//				pthread_mutex_lock( &serial_read_lock);

/*
			buffer[8] = (UCHAR)(rpm >> 8);
			buffer[9] = (UCHAR)rpm;
			buffer[10] = (UCHAR)(mph >> 8);
			buffer[11] = (UCHAR)mph;
*/
			send_tcp((UCHAR *)&s_buffer[0],4);
		}
		
		// check if one of the outputs is set to type 2 (time_delay)
		for(i = 0;i < NUM_PORTS;i++)
		{
			ollist_find_data(i,otpp,&oll);
			if((otp->type == 2 || otp->type == 3) && otp->reset == 1)
			{

				// if blink type, toggle on and off every second
				// this doesn't toggle on and off for time_delay, it 
				// toggles every second so set the time delay for 2x 
				// no. of seconds you want it to last
				if(otp->type == 3)
					change_output(otp->port,((otp->time_delay - otp->time_left) % 2 == 0?0:1));

				otp->time_left--;
//				printf("%d %d\r\n",otp->port,otp->time_left);

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

//					printf("time up: port: %d onoff: %d reset: %d pol: %d\r\n", otp->port,
//							otp->onoff, otp->reset, otp->polarity);

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
					send_serial(otp->port,otp->onoff);
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
		if(rt_fd_ptr > 950)
		{
			strcpy(tempx,"sched.log\0");
//						fp = open((const char *)&tempx[0], O_RDWR | O_CREAT | O_TRUNC,
			fp = open((const char *)&tempx[0], O_RDWR | O_APPEND,
				S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
			if(fp < 0)
			{
				printf("can't create sched.log\n");
			}else
			{
//							printf("writing to sched.log\r\n");
				write(fp,(const void *)rt_file_data,(size_t)rt_fd_ptr);
				rt_fd_ptr = 0;
//							rt_llist_printfile(fp, &roll);
				close(fp);
//							printf("rc: %d\r\n",rc);
//							rt_llist_removeall_data(&roll);
			}
		}
		
		if(shutdown_all)
		{
//				printf("done timer_task\r\n");
			return 0;
		}
	}
	return 1;
}

/*********************************************************************/
// task to handle the 2 buttons on the IO box
UCHAR read_button_inputs(int test)
{
	int i,j;
	UCHAR inputs,mask;

	while(TRUE)
	{
		uSleep(0,TIME_DELAY/6);
		inputs = 0;
		mask = 1;
		for(i = 0;i < 7;i++)
		{
			j = getdioline(i);
			if(j)
				inputs |= mask;
			mask <<= 1;
		}
		inputs = ~inputs;
		inputs &= 0x3f;

		if(inputs != 0)
		{
			inputs = mask2int(inputs);
			inputs++;
			switch(inputs)
			{
				case 1:
//					shift_left();
					scroll_up();
				break;
				case 2:
//					shift_right();
					scroll_down();
				break;
				case 3:
//					lcd_home();
				break;
				case 4:
//					lcd_cls();
				break;
				case 5:
//					scroll_up();
				break;
				case 6:
//					scroll_down();
				break;
				default:
				break;
			}
		}
		if(shutdown_all)
		{
//			printf("done read_buttons task\r\n");
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
	int i;
	int j = 0;
	UCHAR ch;
	int fd;
	char errmsg[20];

//	memset(serial_buff,0,SERIAL_BUFF_SIZE);
//	no_serial_buff = 0;
	memset(errmsg,0,20);

	if(fd = init_serial() < 0)
	{
		myprintf1("can't open comm port 1\0");
		return 0;
	}
	
	while(TRUE)
	{
		if(serial_recv_on == 1)
		{
			pthread_mutex_lock( &serial_read_lock);
			ch = read_serial(errmsg);
/*			
			if(ch == RT_DATA)
			{
				// number of ADC channels + 2 bytes for the rpm
				for(i = 0;i < NUM_ADC_CHANNELS;i++)
					rt_data[i] = read_serial(errmsg);
			}
*/
			pthread_mutex_unlock(&serial_read_lock);

			if(ch >= ENABLE_START && ch <= REBOOT_IOBOX)
			{
				myprintf2("test: ",ch);			
				basic_controls(ch);
/*
				if(ch == REBOOT_IOBOX || ch == SHUTDOWN_IOBOX)
				{
					for(i = 0;i < 20;i++)
					{
						setdioline(7,0);
						uSleep(0,TIME_DELAY/30);
						setdioline(7,1);
						uSleep(0,TIME_DELAY/30);
					}
					setdioline(7,0);
					uSleep(2,0);
					setdioline(7,1);
				}
*/
			}
		}
						
		if(shutdown_all)
		{
//			printf("shutting down serial task\r\n");
			close_serial();
//			myprintf1("done serial task");
			return 0;
		}
	}
	return 1;
}


// client calls 'connect' to get accept call below to stop
// blocking and return sd2 socket descriptor

pthread_mutex_t  mut;
static int visits =  0;						  /* counts client connections     */

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
	int s;

	s = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
	if(s != 0)
//		handle_err_en(s, "pthread_setcancelstate");
		printf("setcancelstate\r\n");

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
		myprintf1("cannot map tcp to protocol number\0");
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
		myprintf1("socket creation failed\0");
//			exit(1);
	}

/* Bind a local address to the socket */
	if (bind(listen_sd, (struct sockaddr *)&sad, sizeof (sad)) < 0)
	{
		myprintf1("bind failed\0");
//		exit(1);
	}

/* Specify a size of request queue */
	if (listen(listen_sd, QLEN) < 0)
	{
		myprintf1("listen failed\0");
//			exit(1);
	}

	alen = sizeof(cad);

/* Main server loop - accept and handle requests */
	while (TRUE)
	{
		if(sock_open == 1)
		{
			uSleep(1,0);
			if(shutdown_all)
			{
				close_tcp();
//				printf("done tcp_mon\r\n");
				return 0;
			}
		}
		else
		{
			myprintf1("Server Waiting...\0");
//			printf("Server Waiting...\r\n");

			if (  (global_socket=accept(listen_sd, (struct sockaddr *)&cad, (socklen_t *)&alen)) < 0)
			{
				myprintf1("accept failed\0");
//				printf("accept failed\r\n");
				return 0;
//					exit (1);
			}
			if(global_socket > 0)
				sock_open = 1;
			myprintf1("connected to socket: \0");
//			printf("connected to socket: \r\n");
			tcp_connected_time = 0;
			if(shutdown_all)
			{
//				printf("tcp task closing\r\n");
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
		close(global_socket);
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
//			pthread_mutex_lock( &io_mem_lock);
//			test_mem(pattern++);
//			pthread_mutex_unlock(&io_mem_lock);
//		}

		(*fptr[*my_id])(i);
		i--;
		not_done--;
//		printf("not done: %d\r\n",not_done);
//		printf("arg: %d\r\n",*my_id);
//		pthread_mutex_lock(&total_count_lock);
//		if (total_count < TOTAL_END_COUNT)
//			total_count+=LOOP_COUNT;
//		else
//			not_done=0;

//		pthread_mutex_unlock(&total_count_lock);

	}
//	printf("\nworkroutine()\tthread %d\tI'm done\n", *my_id);
//	myprintf2("thread done:\0",*my_id);
	return(NULL);
}
/*********************************************************************/
void basic_controls(UCHAR cmd)
{
	int i;
	UCHAR onoff;
	O_DATA *otp;
	O_DATA **otpp = &otp;
	int rc;
	int index;
	size_t isize;
	size_t osize;
	char errmsg[50];

//	myprintf2("basic ctls: ",cmd);
	switch(cmd)
	{
		case ENABLE_START:
			index = STARTER;
			rc = ollist_find_data(index,otpp,&oll);
//			printf("on: %s %d %d\r\n",otp->label,otp->port,otp->onoff);
			change_input(STARTER,1);
			engine_running = 1;
			send_live_code(cmd);
			break;

		case STARTER_OFF:
			index = STARTER;
			rc = ollist_find_data(index,otpp,&oll);
//			printf("off: %s %d %d\r\n",otp->label,otp->port,otp->onoff);
			change_input(STARTER,0);
			send_live_code(cmd);
			break;

		case ON_ACC:
			index = ACCON;
			rc = ollist_find_data(index,otpp,&oll);
//			printf("%s %d %d %d\r\n",otp->label,otp->port,otp->onoff,otp->reset);
//			otp->onoff = 1;
//			ollist_insert_data(index,&oll,otp);
//			change_input(ACCON, 1);
			if(otp->reset == 0)
				change_output(ACCON, 1);
			send_live_code(cmd);
			break;

		case OFF_ACC:
			index = ACCON;
			rc = ollist_find_data(index,otpp,&oll);
//			printf("%s %d %d %d\r\n",otp->label,otp->port,otp->onoff,otp->reset);
//			otp->onoff = 0;
//			ollist_insert_data(index,&oll,otp);
//			change_input(ACCON, 0);
			if(otp->reset == 0)
				change_output(ACCON, 0);
			send_live_code(cmd);
			break;

		case ON_FUEL_PUMP:

			index = FUELPUMP;
			rc = ollist_find_data(index,otpp,&oll);
//			printf("%s %d %d %d\r\n",otp->label,otp->port,otp->onoff,otp->reset);
//			otp->onoff = 1;
//			otp->reset = 0;
//			ollist_insert_data(index,&oll,otp);
			if(otp->reset == 0)
				change_output(FUELPUMP, 1);

			send_live_code(cmd);
			break;

		case OFF_FUEL_PUMP:

			index = FUELPUMP;
			rc = ollist_find_data(index,otpp,&oll);
//			printf("%s %d %d %d\r\n",otp->label,otp->port,otp->onoff,otp->reset);
//			otp->onoff = 0;
//			otp->reset = 0;
//			change_input(FUELPUMP, 0);
//			ollist_insert_data(index,&oll,otp);

			if(otp->reset == 0)
				change_output(FUELPUMP, 0);
			send_live_code(cmd);
			break;

		case ON_FAN:
			index = COOLINGFAN;
			rc = ollist_find_data(index,otpp,&oll);
//			printf("%s %d %d\r\n",otp->label,otp->port,otp->onoff);
			change_input(COOLINGFAN, 1);
			send_live_code(cmd);
			break;

		case OFF_FAN:
			index = COOLINGFAN;
			rc = ollist_find_data(index,otpp,&oll);
//			printf("%s %d %d\r\n",otp->label,otp->port,otp->onoff);
			change_input(COOLINGFAN, 0);
			send_live_code(cmd);
			break;

		case ON_LIGHTS:
			index = LHEADLAMP;
			rc = ollist_find_data(index,otpp,&oll);
//			printf("%s %d %d\r\n",otp->label,otp->port,otp->onoff);
			change_input(LHEADLAMP, 1);

			usleep(100000);

			index = RHEADLAMP;
			rc = ollist_find_data(index,otpp,&oll);
//			printf("%s %d %d\r\n",otp->label,otp->port,otp->onoff);
			change_input(RHEADLAMP, 1);

			usleep(100000);

			index = RUNNINGLIGHTS;
			rc = ollist_find_data(index,otpp,&oll);
//			printf("%s %d %d\r\n",otp->label,otp->port,otp->onoff);
			change_input(RUNNINGLIGHTS, 1);

			usleep(100000);

			index = LBRIGHTS;
			rc = ollist_find_data(index,otpp,&oll);
//			printf("%s %d %d\r\n",otp->label,otp->port,otp->onoff);
			change_input(LBRIGHTS, 1);

			usleep(100000);

			index = RBRIGHTS;
			rc = ollist_find_data(index,otpp,&oll);
//			printf("%s %d %d\r\n",otp->label,otp->port,otp->onoff);
			change_input(RBRIGHTS, 1);
			break;

		case OFF_LIGHTS:
			index = LHEADLAMP;
			rc = ollist_find_data(index,otpp,&oll);
//			printf("%s %d %d\r\n",otp->label,otp->port,otp->onoff);
			change_input(LHEADLAMP, 0);

			usleep(100000);

			index = RHEADLAMP;
			rc = ollist_find_data(index,otpp,&oll);
//			printf("%s %d %d\r\n",otp->label,otp->port,otp->onoff);
			change_input(RHEADLAMP, 0);

			usleep(100000);
			index = RUNNINGLIGHTS;
			rc = ollist_find_data(index,otpp,&oll);
//			printf("%s %d %d\r\n",otp->label,otp->port,otp->onoff);
			change_input(RUNNINGLIGHTS, 0);
			break;

		case ON_BRIGHTS:
			index = LBRIGHTS;
			rc = ollist_find_data(index,otpp,&oll);
//			printf("%s %d %d\r\n",otp->label,otp->port,otp->onoff);
			change_input(LBRIGHTS, 1);

			usleep(100000);

			index = RBRIGHTS;
			rc = ollist_find_data(index,otpp,&oll);
//			printf("%s %d %d\r\n",otp->label,otp->port,otp->onoff);
			change_input(RBRIGHTS, 1);
			break;

		case OFF_BRIGHTS:
			index = LBRIGHTS;
			rc = ollist_find_data(index,otpp,&oll);
//			printf("%s %d %d\r\n",otp->label,otp->port,otp->onoff);
			change_input(LBRIGHTS, 1);

			usleep(100000);

			index = LBRIGHTS;
			rc = ollist_find_data(index,otpp,&oll);
//			printf("%s %d %d\r\n",otp->label,otp->port,otp->onoff);
			change_input(LBRIGHTS, 1);
			break;

		case START_SEQ:
			myprintf1("start seq\0");
			ollist_find_data(ACCON,&otp,&oll);
			otp->onoff = 1;
			otp->reset = 0;
			change_output(otp->port,otp->onoff);
			usleep(100000);
			ollist_insert_data(otp->port,&oll,otp);
			ollist_find_data(FUELPUMP,&otp,&oll);
			otp->onoff = 1;
			otp->reset = 0;
			change_output(otp->port,otp->onoff);
			usleep(100000);
			ollist_insert_data(otp->port,&oll,otp);
			change_input(STARTER, 1);
			send_live_code(ENABLE_START);
			break;

		case SHUTDOWN:
			myprintf1("shutdown\0");
			running_seconds = running_minutes = running_hours = 0;
			engine_running = 0;
			change_input(STARTER, 0);
			usleep(100000);
			change_output(ACCON, 0);
			usleep(100000);
			change_output(FUELPUMP, 0);
			send_live_code(SHUTDOWN);
			break;

		case SHUTDOWN_IOBOX:
		case REBOOT_IOBOX:
			for(i = 0;i < 20;i++)
			{
				setdioline(7,0);
				uSleep(0,TIME_DELAY/30);
				setdioline(7,1);
				uSleep(0,TIME_DELAY/30);
			}
			setdioline(7,0);
			uSleep(2,0);
			setdioline(7,1);
//			printf("shutdown iobox\r\n");
#if 0
			i = NUM_PORT_BITS;
			isize = sizeof(I_DATA);
			isize *= i;

			i = NUM_PORT_BITS;
			osize = sizeof(O_DATA);
			osize *= i;
			for(i = 0;i < NUM_PORT_BITS;i++)
			{
				ollist_find_data(i,otpp,&oll);
//				printf("%d %d\r\n",otp->port,otp->onoff);
				usleep(1000);
				ollist_insert_data(i,&oll,otp);
			}

			if(ilWriteConfig(iFileName,&ill,isize,errmsg) < 0)
				myprintf1(errmsg);
			if(olWriteConfig(oFileName,&oll,osize,errmsg) < 0)
				myprintf1(errmsg);
#endif

			if(cmd == SHUTDOWN_IOBOX)
			{
				reboot_on_exit = 3;
				myprintf1("shutdown iobox\0");
			}	
			else 
			{
				reboot_on_exit = 2;
				myprintf1("reboot iobox\0");
			}
//			printf("reboot on exit: %d\r\n",reboot_on_exit);

			shutdown_all = 1;
			return;
			break;
	}
	send_serial(index,0);

}
/*
					rt_file_data[rt_fd_ptr++] = otp->port;				// 0
					rt_file_data[rt_fd_ptr++] = otp->onoff;				// 1
					rt_file_data[rt_fd_ptr++] = otp->type;				// 2
					rt_file_data[rt_fd_ptr++] = otp->time_delay;		// 3
					rt_file_data[rt_fd_ptr++] = otp->time_left;			// 4
					rt_file_data[rt_fd_ptr++] = otp->reset;				// 5
					rt_file_data[rt_fd_ptr++] = otp->polarity;			// 6
					rt_file_data[rt_fd_ptr++] = trunning_hours;			// 7
					rt_file_data[rt_fd_ptr++] = trunning_minutes;		// 8
					rt_file_data[rt_fd_ptr++] = trunning_seconds;		// 9
*/
//					printf("fd: %d\r\n",rt_fd_ptr);
/*
					rtp->port = otp->port;
					rtp->onoff = otp->onoff;
					rtp->hours = trunning_hours;
					rtp->minutes = trunning_minutes;
					rtp->seconds = trunning_seconds;

					rt_llist_insert_data(&roll,rtp);
					printf("%d %d\r\n",rtp->port,rtp->onoff);
*/


