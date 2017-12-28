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
#include "mytypes.h"
#include "ioports.h"
#include "serial_io.h"
#include "queue/illist_threads_rw.h"
#include "queue/ollist_threads_rw.h"
#include "tasks.h"
#include "ncurses/config_file.h"
#include "lcd_func.h"

pthread_cond_t       threads_ready;
pthread_mutex_t     tcp_write_lock=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t     tcp_read_lock=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t     io_mem_lock=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t     serial_write_lock=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t     serial_read_lock=PTHREAD_MUTEX_INITIALIZER;
int total_count;
static int shutdown_all = 0;

UCHAR (*fptr[NUM_TASKS])(int) = { get_host_cmd_task, monitor_input_task, timer_task, read_button_inputs, serial_recv_task, tcp_monitor_task};

static int same_msg;
int threads_ready_count=0;
pthread_cond_t    threads_ready=PTHREAD_COND_INITIALIZER;
pthread_mutex_t   threads_ready_lock=PTHREAD_MUTEX_INITIALIZER;
static UCHAR check_inputs(int index, int test);

int send_tcp(UCHAR *str,int len);

illist_t ill;
ollist_t oll;

extern char oFileName[20];
extern char iFileName[20];

extern UCHAR reboot_on_exit;
UCHAR upload_buf[UPLOAD_BUFF_SIZE];

extern int ilLoadConfig(char *filename, illist_t *ill ,size_t size, char *errmsg);
extern int olLoadConfig(char *filename, ollist_t *oll, size_t size, char *errmsg);

#define SERIAL_BUFF_SIZE  93

static UCHAR serial_buff[SERIAL_BUFF_SIZE];
static int no_serial_buff;

static char dat_names[NUM_DAT_NAMES][DAT_NAME_STR_LEN];

static int put_sock(UCHAR *buf,int buflen, int block, char *errmsg);
static int get_sock(UCHAR *buf, int buflen, int block, char *errmsg);
static int serial_rec;
static int change_output(int index, int onoff);
static int uSleep(time_t sec, long nanosec);

static UCHAR inportstatus[OUTPORTF_OFFSET-OUTPORTA_OFFSET+1];
static int mask2int(UCHAR mask);
static int timer_inc;
static int timer_inc2;

static double program_start_time;
static double diff;
static int starter_on;
static int fan_on;
static int tcp_window_on;

#define ON 1
#define OFF 0

typedef struct
{
	int i;
	int bank;
	int index;
}REAL_BANKS;

static REAL_BANKS real_banks[40];

CMD_STRUCT cmd_array[38] =
{
	{   	ENABLE_START,"ENABLE_START\0" },
	{   	ON_FUEL_PUMP,"ON_FUEL_PUMP\0" },
	{   	OFF_FUEL_PUMP,"OFF_FUEL_PUMP\0" },
	{   	ON_FAN,"ON_FAN\0" },
	{   	OFF_FAN,"OFF_FAN\0" },
	{   	ON_ACC,"ON_ACC\0" },
	{   	OFF_ACC,"OFF_ACC\0" },
	{   	SEND_IDATA,"SEND_IDATA\0" },
	{   	SEND_ODATA,"SEND_ODATA\0" },
	{   	EDIT_IDATA,"EDIT_IDATA\0" },
	{   	EDIT_ODATA,"EDIT_ODATA\0" },
	{   	EDIT_IDATA2,"EDIT_IDATA2\0" },
	{   	EDIT_ODATA2,"EDIT_ODATA2\0" },
	{   	SEND_ALL_IDATA,"SEND_ALL_IDATA\0" },
	{   	SEND_ALL_ODATA,"SEND_ALL_ODATA\0" },
	{   	RECV_ALL_IDATA,"RECV_ALL_IDATA\0" },
	{   	RECV_ALL_ODATA,"RECV_ALL_ODATA\0" },
	{   	SHOW_IDATA,"SHOW_IDATA\0" },
	{   	SHOW_ODATA,"SHOW_ODATA\0" },
	{   	SEND_SERIAL,"SEND_SERIAL\0" },
	{   	CLOSE_SOCKET,"CLOSE_SOCKET\0" },
	{   	CLEAR_SCREEN,"CLEAR_SCREEN\0" },
	{   	SAVE_TO_DISK,"SAVE_TO_DISK\0" },
	{   	TOGGLE_OUTPUTS,"TOGGLE_OUTPUTS\0" },
	{   	SHUTDOWN,"SHUTDOWN\0" },
	{   	GET_DIR,"GET_DIR\0" },
	{   	LCD_SHIFT_RIGHT,"LCD_SHIFT_RIGHT\0" },
	{   	LCD_SHIFT_LEFT,"LCD_SHIFT_LEFT\0" },
	{   	SCROLL_UP,"SCROLL_UP\0" },
	{   	SCROLL_DOWN,"SCROLL_DOWN\0" },
	{   	ENABLE_LCD,"ENABLE_LCD\0" },
	{   	SET_TIME,"SET_TIME\0" },
	{   	GET_TIME,"GET_TIME\0" },
	{   	TCP_WINDOW_ON,"TCP_WINDOW_ON\0" },
	{   	TCP_WINDOW_OFF,"TCP_WINDOW_OFF\0" },
	{   	UPLOAD_NEW,"UPLOAD_NEW\0" },
	{   	EXIT_PROGRAM,"EXIT_PROGRAM\0" },
	{   	BLANK,"BLANK\0" },
};

/****************************************************************************************************/

static double curtime(void)
{
	struct timeval tv;
	gettimeofday (&tv, NULL);
	return tv.tv_sec + tv.tv_usec / 1000000.0;
}


/*********************************************************************/
static int uSleep(time_t sec, long nanosec)
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
// task to get commands from the host
UCHAR get_host_cmd_task(int test)
{
	I_DATA tempi1;
	O_DATA tempo1;
	I_DATA *itp;
	O_DATA *otp;
	int rc = 0;
	int rc1 = 0;
	UCHAR cmd = 0x21;
	UCHAR onoff;
	char errmsg[50];
	char filename[15];
	size_t size;
	int i;
	int j = 0;
	size_t isize;
	size_t osize;
	UCHAR bank;
	UCHAR test2;
	int testx;
	UCHAR rec_no;
	struct dirent **namelist;
	DIR *d;
	struct dirent *dir;
	int num;
	char tempx[50];
	char *chp;
	int fname_index;
	UCHAR uch_fname_index;
	UCHAR mask;
	time_t curtime2;
	tcp_window_on = 0;	
	int fp;
	off_t fsize;
	int cur_fsize;
	struct timeval mtv;

// the check_inputs & change_outputs functions
// use the array to adjust from index to bank
// since there are only 4 bits in banks 3 & 5
	for(i = 0;i < 20;i++)
	{
		real_banks[i].i = i;
		real_banks[i].bank = i/8;
		real_banks[i].index = i - real_banks[i].bank*8;
	}

	for(i = 20;i < 40;i++)
	{
		real_banks[i].i = i;
		real_banks[i].bank = (i+4)/8;
		real_banks[i].index = i - (real_banks[i].bank*8)+4;
	}

	memset(dat_names,0,sizeof(dat_names));

	i = NUM_PORT_BITS;
	isize = sizeof(I_DATA);
	isize *= i;

	i = NUM_PORT_BITS;
	osize = sizeof(O_DATA);
	osize *= i;

	starter_on = 0;
	fan_on = 0;

	program_start_time = curtime();

	illist_init(&ill);
	if(access(iFileName,F_OK) != -1)
	{
		rc = ilLoadConfig(iFileName,&ill,isize,errmsg);
		if(rc > 0)
		{
			myprintf1(errmsg);
//			return 1;
		}
	}else										  // oh-boy! create a new file!
	{

	}
	ollist_init(&oll);
	if(access(oFileName,F_OK) != -1)
	{
		rc = olLoadConfig(oFileName,&oll,osize,errmsg);
		if(rc > 0)
		{
			myprintf1(errmsg);
		}
	}else										  // oh-boy! create a new file!
	{

	}

	same_msg = 0;
	lcd_init();

	myprintf1("start....\0");
//	myprintf1("sched v1.02\0");

	while(TRUE)
	{
		cmd = 0;
		if(test_sock() == 1)
//		if(1)
		{
			rc = recv_tcp(&cmd,1,1);			  // blocking
//			printf("%s %d\n",cmd_array[cmd].cmd_str,cmd);
			if(cmd != LCD_SHIFT_RIGHT && cmd != LCD_SHIFT_LEFT && cmd != SCROLL_DOWN && cmd != SCROLL_UP)
				myprintf2(cmd_array[cmd].cmd_str,cmd);
			if(rc > 0)
			{
				rc = 0;
				switch(cmd)
				{
// update a single IDATA record

					case TCP_WINDOW_ON:
						tcp_window_on = 1;
						break;

					case TCP_WINDOW_OFF:
						tcp_window_on = 0;
						break;

					case SET_TIME:
						curtime2 = 0L;
						rc += recv_tcp((UCHAR *)&test2,1,1);
//						printf("\n%2x ",test2);
						curtime2 = (time_t)test2;
						rc += recv_tcp((UCHAR *)&test2,1,1);
//						printf("%2x ",test2);
						curtime2 |= (time_t)(test2<<8);
						rc += recv_tcp((UCHAR *)&test2,1,1);
//						printf("%2x ",test2);
						curtime2 |= (time_t)(test2<<16);
						rc += recv_tcp((UCHAR *)&test2,1,1);
//						printf("%2x ",test2);
						curtime2 |= (time_t)(test2<<24);
//						printf("\n%d %ld\n",rc,curtime2);
						rc = stime(&curtime2);
//						printf("rc = %d\n",rc);
						break;

					case GET_TIME:
						gettimeofday(&mtv, NULL);
						curtime2 = mtv.tv_sec;
						strftime(tempx,30,"%m-%d-%Y %T.",localtime(&curtime2));
//						printf("%s\n",tempx);
						myprintf1(tempx);
						break;
						
					case SHOW_IDATA:
//						myprintf1("show I_DATA (tcp_win)\0");
						illist_show(&ill);
						break;

					case SHOW_ODATA:
//						myprintf1("show O_DATA (tcp_win)\0");
						ollist_show(&oll);
						break;

					case SEND_IDATA:
						rc += recv_tcp((UCHAR *)&rec_no,1,1);
// blocking
						rc += recv_tcp((UCHAR *)&tempi1,sizeof(I_DATA),1);
//						myprintf3("send idata:\0" ,rec_no,rc);
						illist_insert_data(rec_no, &ill, &tempi1);
//						illist_show(&ill);
						break;

					case SEND_ODATA:
// update a single ODATA record
						rc += recv_tcp((UCHAR *)&rec_no,1,1);
// blocking
						rc += recv_tcp((UCHAR *)&tempo1,sizeof(O_DATA),1);
//						myprintf3("send odata\0",rec_no,rc);
						ollist_insert_data(rec_no, &oll, &tempo1);
//						memcpy(pod,&tempo1,sizeof(O_DATA));
//						ollist_show(&oll);
						break;

					case SEND_ALL_IDATA:
//						myprintf1("send all IDATA:\0");
						rc = 0;
						itp = &tempi1;
						for(i = 0;i < NUM_PORT_BITS;i++)
						{
							rc += recv_tcp((UCHAR *)itp,sizeof(I_DATA),1);
							illist_insert_data(i,&ill,itp);
						}
//						myprintf1("done\0");
						break;

					case SEND_ALL_ODATA:
//						myprintf1("send all ODATA: \0");
						rc = 0;
						otp = &tempo1;
						for(i = 0;i < NUM_PORT_BITS;i++)
						{
							rc += recv_tcp((UCHAR *)otp,sizeof(O_DATA),1);
							ollist_insert_data(i,&oll,otp);
						}
//						myprintf1("done\0");
						break;

					case RECV_ALL_IDATA:
						rc = 0;
						itp = &tempi1;
						recv_tcp((UCHAR*)&uch_fname_index,1,1);
//						printf("%d\n",uch_fname_index);
//						myprintf1("recv all IDATA: \0");
//						myprintf2("index: \0",fname_index);
						if(uch_fname_index > 0 && uch_fname_index < NUM_DAT_NAMES)
						{
							uch_fname_index--;
							fname_index = (int)uch_fname_index;
							ilLoadConfig(dat_names[fname_index],&ill,isize,errmsg);
						}
						for(i = 0;i < NUM_PORT_BITS;i++)
						{
							illist_find_data(i,&itp,&ill);
							rc += send_tcp((UCHAR *)itp,sizeof(I_DATA));
						}
//						myprintf1("done\0");
						break;

					case RECV_ALL_ODATA:
//						myprintf1("recv all ODATA: \0");
						rc = 0;
						otp = &tempo1;
						recv_tcp((UCHAR*)&uch_fname_index,1,1);
//						myprintf2("index: \0",uch_fname_index);
						if(uch_fname_index > 0)
						{
							uch_fname_index--;
							fname_index = (int)uch_fname_index;
							olLoadConfig(dat_names[fname_index],&oll,isize,errmsg);
						}
						for(i = 0;i < NUM_PORT_BITS;i++)
						{
							ollist_find_data(i,&otp,&oll);
							rc += send_tcp((UCHAR *)otp,sizeof(O_DATA));
						}
//						myprintf1("done\0");
						break;

					case SEND_SERIAL:
						test2 = 0x21;
//#if 0
						pthread_mutex_lock( &serial_write_lock);
						for(i = 0;i < 2*93;i++)
						{
							write_serial(test2);
//							uSleep(1,TIME_DELAY);
							if(++test2 > 0x7e)
								test2 = 0x21;
						}
						pthread_mutex_unlock(&serial_write_lock);
//#endif
						break;

					case GET_DIR:
						d = opendir( "." );
						if( d == NULL )
						{
							myprintf1("bad OPEN_DIR\0");
//							return -1;
						}
						num = 0;

						memset(dat_names,0,NUM_DAT_NAMES*DAT_NAME_STR_LEN);
						memset(tempx,0,sizeof(tempx));

						while( ( dir = readdir( d ) ) && num < NUM_DAT_NAMES-1)
						{
							if(strcmp( dir->d_name, "." ) == 0 || \
								strcmp( dir->d_name, ".." ) == 0 || dir->d_type == DT_DIR)
								continue;

							memset(tempx,0,sizeof(tempx));
							strcpy(tempx,dir->d_name);

							chp = tempx;
							j = 0;

							while(*chp++ != '.' && j < DAT_NAME_STR_LEN)
								j++;

							strncpy(tempx,chp,j+1);

							if(dir->d_type == DT_REG && strcmp(tempx,"dat") == 0 )
//							if(dir->d_type == DT_REG)
							{
								strcpy(dat_names[num++],dir->d_name);
							}
						}
						closedir( d );
//						memset(tempx,0x20,sizeof(tempx));
						send_tcp((UCHAR*)&num,1);
						for(i = 0;i < num;i++)
						{
							cmd = (UCHAR)strlen(dat_names[i]);
							send_tcp(&cmd,1);
							send_tcp((UCHAR *)&dat_names[i],cmd);
						}
						for(i = 0;i < num;i++)
						{
							getFileCreationTime(dat_names[i],tempx);
							j = GetFileFormat(dat_names[i]);
							if(j < 0)
								test2 = 0xff;
							else
								test2 = (UCHAR)j;
							send_tcp((UCHAR*)&tempx,TDATE_STAMP_STR_LEN);
							send_tcp((UCHAR*)&test2,1);
						}
						break;

					case SAVE_TO_DISK:
						if(ilWriteConfig(iFileName,&ill,isize,errmsg) < 0)
							myprintf1(errmsg);
						if(olWriteConfig(oFileName,&oll,osize,errmsg) < 0)
							myprintf1(errmsg);
						break;

					case LCD_SHIFT_LEFT:
						shift_left();
						break;

					case LCD_SHIFT_RIGHT:
						shift_right();
						break;

					case SCROLL_UP:
						scroll_up();
						break;

					case SCROLL_DOWN:
						scroll_down();
						break;

					case ENABLE_START:
						change_output(STARTER, 1);
						ollist_change_output(STARTER, &oll, 1);
						starter_on = 1;
						break;

					case ON_ACC:
						ollist_change_output(ACCON, &oll, 1);
						change_output(ACCON, 1);
						break;

					case OFF_ACC:
						ollist_change_output(ACCON, &oll, 0);
						change_output(ACCON, 0);
						break;

					case ON_FUEL_PUMP:
						ollist_change_output(FUELPUMP, &oll, 1);
						change_output(FUELPUMP, 1);
						break;

					case OFF_FUEL_PUMP:
						ollist_change_output(FUELPUMP, &oll, 0);
						change_output(FUELPUMP, 0);
						break;

					case ON_FAN:
						ollist_change_output(COOLINGFAN, &oll, 1);
						change_output(COOLINGFAN, 1);
						fan_on = 1;
						break;

					case OFF_FAN:
						ollist_change_output(COOLINGFAN, &oll, 0);
						change_output(COOLINGFAN, 0);
						fan_on = 0;
						break;

					case SHUTDOWN:
//						myprintf1("shutdown\0");
						otp = &tempo1;
						for(i = STARTER;i < PRELUBE+1;i++)
						{
							ollist_change_output(i, &oll, 0);
							change_output(i, 0);
							uSleep(0,TIME_DELAY/10000);
						}
						break;

					case CLEAR_SCREEN:
						lcd_cls();
						break;

					case CLOSE_SOCKET:
						close_tcp();
						break;

					case UPLOAD_NEW:
						recv_tcp((UCHAR*)&fsize,8,1);
//						printf("fsize: %ld\n",fsize);
//						upload_buf = (UCHAR *)malloc(UPLOAD_BUFF_SIZE);
						cur_fsize = (int)fsize;
//						printf("cur_fsize: %d\n",cur_fsize);
//						printf("sizeof upload_buf: %ld\n",sizeof(upload_buf));
//#if 0
						memset(upload_buf,0,UPLOAD_BUFF_SIZE);
						strcpy(filename,"sched2");
//						printf("filename: %s\n",filename);
						fp = open((const char *)filename, O_RDWR | O_CREAT | O_TRUNC, 700);
						if(fp < 0)
						{
							printf("could not create file: %s\n",filename);
							goto exit_program;
							break;
						}
						do {
							rc = 0;
							rc1 = 0;
							rc += recv_tcp((UCHAR *)&upload_buf[0],UPLOAD_BUFF_SIZE,1);
//							printf("%ld ",rc);
							rc1 += write(fp, upload_buf, rc);
							cur_fsize -= rc;
//							printf(": %d ",cur_fsize);

						}while(cur_fsize > UPLOAD_BUFF_SIZE);

						rc += recv_tcp((UCHAR *)&upload_buf[0],cur_fsize,1);
//						printf("\n%d ",rc);
						rc1 += write(fp,upload_buf,cur_fsize);
//						printf("%d %d\n",rc, rc1);

						close(fp);
//						free(upload_buf);

//						recv_tcp((UCHAR*)&tempx[0],16,1);
//						printf("%2x %2x %2x %2x\n",tempx[0],tempx[1],tempx[2],tempx[3]);

//						printf("done\n");					
//						break;
//#endif
					case EXIT_PROGRAM:
exit_program:
						if(cmd == UPLOAD_NEW)
						{
							reboot_on_exit = 1;
							myprintf1("rebooting...");
//							printf("rebooting...");
						}
						else
						{
							recv_tcp((UCHAR*)&reboot_on_exit,1,1);
							if(reboot_on_exit == 0)
								myprintf1("exit to shell");
							else myprintf1("rebooting...");	
						}
						if(ilWriteConfig(iFileName,&ill,isize,errmsg) < 0)
							myprintf1(errmsg);
						if(olWriteConfig(oFileName,&oll,osize,errmsg) < 0)
							myprintf1(errmsg);
						close_tcp();
						shutdown_all = 1;

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

						return 0;
						break;

					default:
						break;
				}								  // end of switch
			}									  // if rc > 0
		}else									  // if test_sock
		{
			uSleep(1,1000);
		}
	}
	return test + 1;
}


/*********************************************************************/
// if an input switch is changed, update the record for that switch
// and if an output applies to that input, change the output
// and record the event in llist
UCHAR monitor_input_task(int test)
{
	I_DATA *itp;
	I_DATA **itpp = &itp;

	O_DATA *otp;
	O_DATA **otpp = &otp;

	int status = -1;
	int bank, index;
	UCHAR result,result2, mask, mask2;
	int onoff,i;

#if 0

	typedef struct o_data
	{
		char label[20];
		UCHAR port;
		UCHAR onoff;							  // 1 of on; 0 if off
		UCHAR type;
		UINT time_delay;
		UCHAR pulse_time;
	} O_DATA;

/*
type:
0) regular - on/off state doesn't change until user tells it to
1) on for time delay seconds and then it goes back off
2) goes on/off at a pulse_time rate until turned off again
4) goes on/off at pulse_time rate for time_delay seconds and then back off
5) toggle switch realized in momentary push-buton: push & release of a
	momentary push-button turns bit on or off
*/
	while(1)
	{
		uSleep(0,TIME_DELAY);
	}

	problem: if more than 1 button is pushed in same bank or diff bank?

			while(TRUE)
	{
		pthread_mutex_lock( &io_mem_lock);
		for(i = 0;i < NUM_PORTS;i++)
			result[i] = InPortByte(i);
		pthread_mutex_unlock( &io_mem_lock);
#endif

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
					if(mask > 0x80)
					{
						myprintf1("bad mask\0");
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

					for(i = 0;i < 40;i++)
					{
						if(real_banks[i].bank == bank && real_banks[i].index == index)
						{
							index = real_banks[i].i;
						}
					}

					illist_find_data(index,itpp,&ill);

					ollist_find_data(itp->affected_output,otpp,&oll);

					otp->onoff = onoff;
					ollist_insert_data(otp->port,&oll,otp);
					change_output(otp->port,otp->onoff);

				}
				inportstatus[bank] = result;

			}
			uSleep(0,TIME_DELAY/200);
			if(shutdown_all)
				return 0;
		}
		return 1;
	}

/*********************************************************************/
// pass in the index into the total list of outputs
// since each card only has 20 outputs, the last 4 bits of PORT C & F are ignored
// index 0->19 = PORTA(0:7)->PORTC(0:4)
// index 24->39 = PORTD(0:7)->PORTF(0:4)
	static int change_output(int index, int onoff)
	{
		int bank;

		bank = real_banks[index].bank;
		index = real_banks[index].index;

		pthread_mutex_lock( &io_mem_lock);

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
//#endif

//	printf("bank: %d\tindex\t%2d\t%2d\n",bank,index,onoff);

//	lcd_cls();
//		myprintf3("bank:\0",bank,index);
//		myprintf2(" \0",onoff);
		return index;
	}

/*********************************************************************/
	UCHAR timer_task(int test)
	{
		int i;

		uSleep(0,TIME_DELAY);

		while(TRUE)
		{
			timer_inc++;
			uSleep(0,TIME_DELAY/2);
			uSleep(0,TIME_DELAY/8);
			uSleep(0,TIME_DELAY/4);
			uSleep(0,TIME_DELAY/16);
			setdioline(7,0);
			uSleep(0,TIME_DELAY/16);
			setdioline(7,1);
// this is a kludge until I have time to redesign the whole mess from the ground up
/// using a much better pthread library
			if(starter_on > 0)
			{
				if(++starter_on > 8)
				{
					starter_on = 0;
					change_output(STARTER, 0);
					ollist_change_output(STARTER, &oll, 0);
					myprintf1("STARTER OFF\0");
				}
			}
			if(shutdown_all)
				return 0;
		}
		return 1;
	}

/*********************************************************************/
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
			return 0;
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

	memset(serial_buff,0,SERIAL_BUFF_SIZE);
	no_serial_buff = 0;
	memset(errmsg,0,20);

	uSleep(10,0);

	if(fd = init_serial() < 0)
	{
		myprintf1("can't open comm port\0");
		printf("can't open comm port\n");
		return 0;
	}

#if 0
	while(TRUE)
	{
		uSleep(5,TIME_DELAY);
		if(shutdown_all)
		{
			printf("shutting down serial task\n");
			close_serial();
			printf("serial port closed\n");
			return 0;
		}
	}
#endif
	
	while(TRUE)
	{
		pthread_mutex_lock( &serial_read_lock);
		ch = read_serial(errmsg);
		pthread_mutex_unlock(&serial_read_lock);
		if(errmsg[0] != 0)
		{
			printf("%s\n",errmsg);
			memset(errmsg,0,20);
			uSleep(5,0);
		}else
//			if(myprintf2("read:\0",ch) == 1)
//				printf("lcd not init'd\n");
			if(ch != 0x7e)
			{
				if(tcp_window_on == 1)
					send_tcp((UCHAR *)&ch,1);
//				printf("%c",ch);
			}
			uSleep(0,TIME_DELAY/100000);
#if 0
		if(no_serial_buff < SERIAL_BUFF_SIZE)
		{
			serial_buff[no_serial_buff] = read_serial();

			no_serial_buff++;
		}
		else
		{
//#if 0
			for(i = 0;i < SERIAL_BUFF_SIZE;i++)
			{
//				serprintf("%c",serial_buff[i]);
//				if((i % 93) == 0)
//					serprintf("\n");
			}
//#endif
			no_serial_buff = 0;
//			serprintf("\n\n");
		}
		pthread_mutex_unlock(&serial_read_lock);
#endif
		if(shutdown_all)
		{
//			printf("shutting down serial task\n");
			close_serial();
//			printf("serial port closed\n");
			return 0;
		}
	}
	return 1;
}

// client calls 'connect' to get accept call below to stop
// blocking and return sd2 socket descriptor

#define PROTOPORT         5193				  /* default protocol port number */
#define QLEN              6					  /* size of request queue        */

pthread_mutex_t  mut;
static int visits =  0;						  /* counts client connections     */

static struct  sockaddr_in sad;				  /* structure to hold server's address  */
static int sock_open;
static int global_socket;
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
	port = PROTOPORT;
	sock_open = 0;
	tv.tv_sec = 2;
	tv.tv_usec = 50000;

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
			uSleep(5,0);
//			pthread_yield();
		}
		else
		{
			myprintf1("Server Waiting...\0");

			if (  (global_socket=accept(listen_sd, (struct sockaddr *)&cad, &alen)) < 0)
			{
				myprintf1("accept failed\0");
//					exit (1);
			}
			if(global_socket > 0)
				sock_open = 1;
			myprintf1("connected to socket: \0");
/*
		if (setsockopt (global_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval)) < 0)
			return -2;
		if (setsockopt (global_socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(struct timeval)) < 0)
			return -3;
*/
		}
		if(shutdown_all)
			return 0;
	}
	return 1;
}

/*********************************************************************/
int test_sock(void)
{
	return sock_open;
}

/*********************************************************************/
int send_tcp(UCHAR *str,int len)
{
	int ret = 0;
	char errmsg[60];
	memset(errmsg,0,60);
	pthread_mutex_lock( &tcp_write_lock);
	ret = put_sock(str,len,1,&errmsg[0]);
	pthread_mutex_unlock(&tcp_write_lock);
	if(ret < 0 && (strcmp(errmsg,"Success") != 0))
	{
		if(same_msg == 0)
			myprintf1(errmsg);
		same_msg = 1;
	}
	else same_msg = 0;
	return ret;
}

/*********************************************************************/
int recv_tcp(UCHAR *str, int strlen,int block)
{
	int ret = 0;
	char errmsg[20];
	memset(errmsg,0,20);
	if(test_sock())
	{
		pthread_mutex_lock( &tcp_read_lock);
		ret = get_sock(str,strlen,block,&errmsg[0]);
		pthread_mutex_unlock(&tcp_read_lock);
		if(ret < 0 && (strcmp(errmsg,"Success") != 0))
		{
			myprintf1(errmsg);
//			printf("%s\n",errmsg);
		}
	}
	else
	{
		strcpy(errmsg,"sock closed");
		ret = -1;
	}
	return ret;
}

/*********************************************************************/
static int put_sock(UCHAR *buf,int buflen, int block, char *errmsg)
{
	int rc = 0;
	char extra_msg[10];
	if(test_sock())
	{
		if(block)
// block
			rc = send(global_socket,buf,buflen,MSG_WAITALL);
		else
// don't block
			rc = send(global_socket,buf,buflen,MSG_DONTWAIT);
		if(rc < 0 && errno != 11)
		{
			strcpy(errmsg,strerror(errno));
			sprintf(extra_msg," %d",errno);
			strcat(errmsg,extra_msg);
			strcat(errmsg," put_sock");
			close_tcp();
		}else strcpy(errmsg,"Success\0");
	}
	else
	{
// this keeps printing out until the client logs on
		strcpy(errmsg,"sock closed");
		rc = -1;
	}
	return rc;
}

/*********************************************************************/
static int get_sock(UCHAR *buf, int buflen, int block, char *errmsg)
{
	int rc;
	char extra_msg[10];
	if(block)
		rc = recv(global_socket,buf,buflen,MSG_WAITALL);
	else
		rc = recv(global_socket,buf,buflen,MSG_DONTWAIT);
	if(rc < 0 && errno != 11)
	{
		strcpy(errmsg,strerror(errno));
		sprintf(extra_msg," %d",errno);
		strcat(errmsg,extra_msg);
		strcat(errmsg," get_sock");
	}else strcpy(errmsg,"Success\0");
	return rc;
}

/*********************************************************************/
void close_tcp(void)
{
	myprintf1("closing socket\0");
//	send_tcp("close",5);
	if(sock_open)
	{
		sock_open = 0;
		close(global_socket);
		global_socket = -1;
	}else myprintf1("socket already closed\0");
}

/*********************************************************************/
void SendByte(unsigned char byte)
{
	pthread_mutex_lock( &serial_write_lock);
	write_serial(byte);
	pthread_mutex_unlock(&serial_write_lock);
}

/*********************************************************************/
UCHAR ReceiveByte(void)
{
	int n,i;
	UCHAR rec;
	return rec;
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
//	printf("\nwork_routine()\tthread %d\tI'm Alive\n", *my_id);

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
//	printf("\nwork_routine()\tthread %d\tI'm Proceeding\n", *my_id);

	while(not_done)
	{
//			pthread_mutex_lock( &io_mem_lock);
//			test_mem(pattern++);
//			pthread_mutex_unlock(&io_mem_lock);
//		}

		(*fptr[*my_id])(i);
		i--;
		not_done--;
//		printf("not done: %d\n",not_done);
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
