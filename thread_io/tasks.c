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

pthread_cond_t       threads_ready;
pthread_mutex_t     tcp_write_lock=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t     tcp_read_lock=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t     io_mem_lock=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t     serial_write_lock=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t     serial_read_lock=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t     serial_write_lock2=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t     serial_read_lock2=PTHREAD_MUTEX_INITIALIZER;
int total_count;
static int shutdown_all = 0;

//read_button_inputs,  task, serial_recv_task2, tcp_monitor_task};

#if CONSOLE_DISABLED
UCHAR (*fptr[NUM_TASKS])(int) = { get_host_cmd_task, monitor_input_task, timer_task, read_button_inputs,  serial_recv_task, tcp_monitor_task};
#else
UCHAR (*fptr[NUM_TASKS])(int) = { get_host_cmd_task, monitor_input_task, timer_task, read_button_inputs, tcp_monitor_task};
#endif

static int same_msg;
int threads_ready_count=0;
pthread_cond_t    threads_ready=PTHREAD_COND_INITIALIZER;
pthread_mutex_t   threads_ready_lock=PTHREAD_MUTEX_INITIALIZER;
static UCHAR check_inputs(int index, int test);

int send_tcp(UCHAR *str,int len);

illist_t ill;
ollist_t oll;

//rt_llist_t roll;

extern char oFileName[20];
extern char iFileName[20];

//int odometer;

extern UCHAR reboot_on_exit;
UCHAR upload_buf[UPLOAD_BUFF_SIZE];

static UCHAR rt_data[16];
static UCHAR rt_file_data[1000];
static int rt_fd_ptr;
static int odometer;
static int tcp_connected_time;

extern int ilLoadConfig(char *filename, illist_t *ill ,size_t size, char *errmsg);
extern int olLoadConfig(char *filename, ollist_t *oll, size_t size, char *errmsg);

#define SERIAL_BUFF_SIZE  20

static UCHAR serial_buff[SERIAL_BUFF_SIZE];
static int no_serial_buff;

static char dat_names[NUM_DAT_NAMES][DAT_NAME_STR_LEN];

static int put_sock(UCHAR *buf,int buflen, int block, char *errmsg);
static int get_sock(UCHAR *buf, int buflen, int block, char *errmsg);
static int serial_rec;
static int change_output(int index, int onoff);
static int uSleep(time_t sec, long nanosec);
static void basic_controls(UCHAR code);
static UCHAR inportstatus[OUTPORTF_OFFSET-OUTPORTA_OFFSET+1];
static int mask2int(UCHAR mask);
static int timer_inc;
static int timer_inc2;

//static double program_start_time;
static double diff;
static int starter_on;
static int tcp_window_on;
static int live_window_on;
static int engine_running;
static int fan_delay;
static UCHAR running_hours, running_minutes, running_seconds;
static UCHAR trunning_hours, trunning_minutes, trunning_seconds;

#define ON 1
#define OFF 0

typedef struct
{
	int i;
	int bank;
	int index;
}REAL_BANKS;

static REAL_BANKS real_banks[40];

CMD_STRUCT cmd_array[47] =
{
	{		TEST_IOPORT,"TEST_IOPORT\0" },
	{		TEST_IOPORT2,"TEST_IOPORT2\0" },
	{   	ENABLE_START,"ENABLE_START\0" },
	{   	STARTER_OFF,"STARTER_OFF\0" },
	{   	ON_ACC,"ON_ACC\0" },
	{   	OFF_ACC,"OFF_ACC\0" },
	{   	ON_FUEL_PUMP,"ON_FUEL_PUMP\0" },
	{   	OFF_FUEL_PUMP,"OFF_FUEL_PUMP\0" },
	{   	ON_FAN,"ON_FAN\0" },
	{   	OFF_FAN,"OFF_FAN\0" },
	{   	ON_LIGHTS,"ON_LIGHTS\0" },
	{   	OFF_LIGHTS,"OFF_LIGHTS\0" },
	{   	START_SEQ,"START_SEQ\0" },
	{   	SHUTDOWN,"SHUTDOWN\0" },
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
	{   	LIVE_WINDOW_ON,"LIVE_WINDOW_ON\0" },
	{   	LIVE_WINDOW_OFF,"LIVE_WINDOW_OFF\0" },
	{		TEST_WRITE_FILE,"TEST_WRITE_FILE\0" },
	{   	TOTAL_UP_TIME,"TOTAL_UP_TIME\0" },
	{   	UPLOAD_NEW,"UPLOAD_NEW\0" },
	{   	EXIT_PROGRAM,"EXIT_PROGRAM\0" }
};

/****************************************************************************************************/

static double curtime(void)
{
	struct timeval tv;
	gettimeofday (&tv, NULL);
	return tv.tv_sec + tv.tv_usec / 1000000.0;
}

/****************************************************************************************************/

static void send_live_code(UCHAR cmd)
{
	if(live_window_on == 0)
		return;

	UCHAR status_line[5];

	status_line[0] = cmd;
	status_line[1] = running_seconds;
	status_line[2] = running_minutes;
	status_line[3] = running_hours;
	send_tcp((UCHAR*)status_line,4);
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
//	RI_DATA tempr1;
	I_DATA *itp;
	O_DATA *otp;
	O_DATA **otpp = &otp;
//	RI_DATA *rtp;
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
	UCHAR test_io_num = 0;
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
	engine_running = 0;
	fan_delay = 0;
	running_hours = running_minutes = running_seconds = 0;
	trunning_hours = trunning_minutes = trunning_seconds = 0;
	
//	program_start_time = curtime();

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

//	rt_llist_init (&roll);		// make new list for rt data every time this program
								// runs - copy the old one to the sd card and rename it
//	rtp = &tempr1;
	same_msg = 0;
	lcd_init();

// flash green and red led's to signal we are up (if LCD screen not attached)
	for(i = 0;i < 200;i++)
	{
		red_led(1);
		uSleep(0,100000L);
		red_led(0);
		uSleep(0,100000L);
	}

	for(i = 0;i < 200;i++)
	{
		green_led(1);
		uSleep(0,100000L);
		green_led(0);
		uSleep(0,100000L);
	}

	myprintf1("start....\0");

	myprintf1("sched v1.15\0");
	printf("sched v1.15\r\n");
	memset(rt_file_data,0,sizeof(rt_file_data));
	rt_fd_ptr = 0;
	odometer = 0;
	tcp_connected_time = 0;
	mask = 1;

	while(TRUE)
	{
		cmd = 0;
		if(test_sock() == 1)
//		if(1)
		{
			rc = recv_tcp(&cmd,1,1);			  // blocking
			tcp_connected_time = 0;
			if(cmd != LCD_SHIFT_RIGHT && cmd != LCD_SHIFT_LEFT && cmd != 
					SCROLL_DOWN && 	cmd != SCROLL_UP && cmd != TEST_IOPORT && cmd != TEST_IOPORT2)
				myprintf2(cmd_array[cmd].cmd_str,cmd);

//			printf("cmd:%s\r\n",cmd_array[cmd].cmd_str);

			if(rc > 0)
			{
				rc = 0;
 				switch(cmd)
				{
					case ENABLE_START:
					case STARTER_OFF:
					case ON_ACC:
					case OFF_ACC:
					case ON_FUEL_PUMP:
					case OFF_FUEL_PUMP:
					case ON_FAN:
					case OFF_FAN:
					case ON_LIGHTS:
					case OFF_LIGHTS:
					case START_SEQ:
					case SHUTDOWN:
					basic_controls(cmd);
						break;

					case TEST_IOPORT:
						recv_tcp((UCHAR *)&test_io_num,1,1);
						recv_tcp((UCHAR *)&mask,1,1);
						change_output((int)test_io_num, (int)mask);
						myprintf2("io port: ",(int)mask);
						printf("io port: %d %d\r\n",test_io_num,mask);
						if(mask == 1)
							mask = 0;
						break;

					case TEST_IOPORT2:
						recv_tcp((UCHAR*)&mask,1,1);
						printf("mask = %d\r\n",mask);
						for(test_io_num = 0;test_io_num < 6;test_io_num++)
						{

							rc = ollist_find_data((int)test_io_num,otpp,&oll);
							printf("%s %d\r\n",otp->label,otp->port);
							otp->onoff = (UCHAR)mask;
							ollist_insert_data(otp->port,&oll,otp);

							change_output((int)test_io_num, (int)mask);
//							printf("%d\r\n",test_io_num);
							usleep(100000);
						}
						break;

					case TCP_WINDOW_ON:
						tcp_window_on = 1;
						break;

					case TCP_WINDOW_OFF:
						tcp_window_on = 0;
						break;

					case LIVE_WINDOW_ON:
						live_window_on = 1;
						break;

					case LIVE_WINDOW_OFF:
						live_window_on = 0;
						break;

					case SET_TIME:
						curtime2 = 0L;
						rc += recv_tcp((UCHAR *)&test2,1,1);
						curtime2 = (time_t)test2;
						rc += recv_tcp((UCHAR *)&test2,1,1);
						curtime2 |= (time_t)(test2<<8);
						rc += recv_tcp((UCHAR *)&test2,1,1);
						curtime2 |= (time_t)(test2<<16);
						rc += recv_tcp((UCHAR *)&test2,1,1);
						curtime2 |= (time_t)(test2<<24);
						rc = stime(&curtime2);
						break;

					case GET_TIME:
						gettimeofday(&mtv, NULL);
						curtime2 = mtv.tv_sec;
						strftime(tempx,30,"%m-%d-%Y %T.\0",localtime(&curtime2));
						myprintf1(tempx);
						break;
						
					case SHOW_IDATA:
						illist_show(&ill);
						break;

					case SHOW_ODATA:
//						myprintf1("show O_DATA (tcp_win)\0");
						ollist_show(&oll);
						break;

					case SEND_IDATA:
						rc += recv_tcp((UCHAR *)&rec_no,1,1);
						rc += recv_tcp((UCHAR *)&tempi1,sizeof(I_DATA),1);
						illist_insert_data(rec_no, &ill, &tempi1);
						illist_show(&ill);
						break;

					case SEND_ODATA:
						rc += recv_tcp((UCHAR *)&rec_no,1,1);
						rc += recv_tcp((UCHAR *)&tempo1,sizeof(O_DATA),1);
						ollist_insert_data(rec_no, &oll, &tempo1);
						ollist_show(&oll);
						break;

					case SEND_ALL_IDATA:
						rc = 0;
						itp = &tempi1;
						for(i = 0;i < NUM_PORT_BITS;i++)
						{
							rc += recv_tcp((UCHAR *)itp,sizeof(I_DATA),1);
							illist_insert_data(i,&ill,itp);
						}
						break;

					case SEND_ALL_ODATA:
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
						break;

/*
					case PASSWORD_MODE:
						test2 = 0xFC;
						pthread_mutex_lock( &serial_write_lock);
						write_serial(test2);
						pthread_mutex_unlock(&serial_write_lock);
						break;
*/
					case SEND_SERIAL:
						test2 = 0x21;
						pthread_mutex_lock( &serial_write_lock);
						for(i = 0;i < 4*93;i++)
						{
							write_serial(test2);
//							uSleep(1,TIME_DELAY);
							if(++test2 > 0x7e)
								test2 = 0x21;
						}
/*
						test2 = 0x7e;
						for(i = 0;i < 4*93;i++)
						{
							write_serial2(test2);
//							uSleep(1,TIME_DELAY);
							if(--test2 < 0x21)
								test2 = 0x7e;
						}
*/
						pthread_mutex_unlock(&serial_write_lock);
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
							printf("%s\r\n",tempx);
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


					case CLEAR_SCREEN:
						lcd_cls();
						break;

					case CLOSE_SOCKET:
						close_tcp();
						break;

					case TEST_WRITE_FILE:
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
							rc = write(fp,(const void *)rt_file_data,(size_t)rt_fd_ptr);
							rt_fd_ptr = 0;
//							rt_llist_printfile(fp, &roll);
							close(fp);
//							printf("rc: %d\r\n",rc);
//							rt_llist_removeall_data(&roll);
						}
						break;

/*
					case NEW_PASSWORD:
						recv_tcp((UCHAR*)&fsize,8,1);
						cur_fsize = (int)fsize;
						memset(upload_buf,0,UPLOAD_BUFF_SIZE);
						rc = 0;
						rc1 = 0;
						rc += recv_tcp((UCHAR *)&upload_buf[0],cur_fsize,1);

						test2 = 0xFB;
						pthread_mutex_lock( &serial_write_lock);
						write_serial(test2);

						for(i = 0;i < cur_fsize;i++)
							write_serial(upload_buf);

						test2 = 0xFE;	
						write_serial(test2);
						pthread_mutex_unlock(&serial_write_lock);
						break;
*/
					case UPLOAD_NEW:
						recv_tcp((UCHAR*)&fsize,8,1);
						cur_fsize = (int)fsize;
						memset(upload_buf,0,UPLOAD_BUFF_SIZE);
						strcpy(filename,"sched2");
						fp = open((const char *)filename, O_RDWR | O_CREAT | O_TRUNC, 700);
						if(fp < 0)
						{
//							printf("could not create file: %s\n",filename);
							goto exit_program;
							break;
						}
						do {
							rc = 0;
							rc1 = 0;
							rc += recv_tcp((UCHAR *)&upload_buf[0],UPLOAD_BUFF_SIZE,1);
							rc1 += write(fp, upload_buf, rc);
							cur_fsize -= rc;

						}while(cur_fsize > UPLOAD_BUFF_SIZE);

						rc += recv_tcp((UCHAR *)&upload_buf[0],cur_fsize,1);
						rc1 += write(fp,upload_buf,cur_fsize);

						close(fp);

					case EXIT_PROGRAM:
exit_program:
						if(cmd == UPLOAD_NEW)
						{
							reboot_on_exit = 1;
							myprintf1("rebooting...\0");
						}
						else
						{
//							printf("exit program\r\n");
							recv_tcp((UCHAR*)&reboot_on_exit,1,1);
//							printf("exit code: %d\r\n",reboot_on_exit);
							if(reboot_on_exit == 1)
							{
								myprintf1("exit to shell\0");
//								printf("exit to shell\r\n");
							}	
							else if(reboot_on_exit == 2)
							{
								myprintf1("rebooting...\0");
//								printf("rebooting...\r\n");
							}
							else if(reboot_on_exit == 3)
							{
								myprintf1("shutting down...\0");
//								printf("shutting down...\r\n");
							}
						}

						if(rt_fd_ptr > 0)
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
//								printf("writing to sched.log\r\n");
								rc = write(fp,(const void *)rt_file_data,(size_t)rt_fd_ptr);
								close(fp);
							}
						}
						strcpy(tempx,"odometer.txt\0");
						fp = open((const char *)&tempx[0], O_RDWR | O_CREAT | O_TRUNC,
					//	fp = open((const char *)&tempx[0], O_RDWR | O_APPEND,
							S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
						if(fp < 0)
						{
							printf("can't create odometer.txt\n");
						}else
						{
//							printf("writing to odometer.txt\r\n");
							sprintf(tempx,"%d\r\n\0",odometer);
							rc = write(fp,(const void *)tempx,strlen(tempx));
							close(fp);
						}

						if(ilWriteConfig(iFileName,&ill,isize,errmsg) < 0)
							myprintf1(errmsg);
						if(olWriteConfig(oFileName,&oll,osize,errmsg) < 0)
							myprintf1(errmsg);
//						close_tcp();
						shutdown_all = 1;

//						strcpy(tempx,"sched.log\0");
//						WriteOdometer(tempx, &odometer, errmsg);
/*
						logfile_handle = open((const char *)&tempx[0], O_RDWR,
								S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

						if(logfile_handle < 0)
						{
							printf("handle: %d\n",logfile_handle);
							return logfile_handle;
						}
						write(logfile_handle,&odometer,sizeof(int));
						write(logfile_handle,&trip,sizeof(int));
						close(logfile_handle);
*/
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

						for(i = 0;i < 50;i++)
						{
							red_led(1);
							usleep(10000);
							red_led(0);
							usleep(10000);
						}

						return 1;
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
void basic_controls(UCHAR cmd)
{
	int i;
	UCHAR onoff;
	O_DATA *otp;
	O_DATA **otpp = &otp;
	int rc;
	int index;

	switch(cmd)
	{
		case ENABLE_START:
			index = STARTER;
			rc = ollist_find_data(index,otpp,&oll);
//			printf("%s %d %d\r\n",otp->label,otp->port,otp->onoff);
			otp->onoff = 1;
			ollist_insert_data(otp->port,&oll,otp);

			change_output(STARTER, 1);
			starter_on = 1;
			engine_running = 1;
			send_live_code(cmd);
			break;

		case STARTER_OFF:
			index = STARTER;
			rc = ollist_find_data(index,otpp,&oll);
//			printf("%s %d %d\r\n",otp->label,otp->port,otp->onoff);
			otp->onoff = 0;
			ollist_insert_data(otp->port,&oll,otp);

			change_output(STARTER, 0);
			starter_on = 0;
			send_live_code(cmd);
			break;

		case ON_ACC:
			index = ACCON;
			rc = ollist_find_data(index,otpp,&oll);
//			printf("%s %d %d\r\n",otp->label,otp->port,otp->onoff);
			otp->onoff =1;
			ollist_insert_data(otp->port,&oll,otp);
			change_output(ACCON, 1);
			send_live_code(cmd);
			break;

		case OFF_ACC:
			index = ACCON;
			rc = ollist_find_data(index,otpp,&oll);
			otp->onoff = 0;
//			printf("%s %d %d\r\n",otp->label,otp->port,otp->onoff);
			ollist_insert_data(otp->port,&oll,otp);
			change_output(ACCON, 0);
			send_live_code(cmd);
			break;

		case ON_FUEL_PUMP:
			index = FUELPUMP;
			rc = ollist_find_data(index,otpp,&oll);
			otp->onoff = 1;
//			printf("%s %d %d\r\n",otp->label,otp->port,otp->onoff);
			ollist_insert_data(otp->port,&oll,otp);
			change_output(FUELPUMP, 1);
			send_live_code(cmd);
			break;

		case OFF_FUEL_PUMP:
			index = FUELPUMP;
			rc = ollist_find_data(index,otpp,&oll);
			otp->onoff = 0;
//			printf("%s %d %d\r\n",otp->label,otp->port,otp->onoff);
			ollist_insert_data(otp->port,&oll,otp);
			change_output(FUELPUMP, 0);
			send_live_code(cmd);
			break;

		case ON_FAN:
			index = COOLINGFAN;
			rc = ollist_find_data(index,otpp,&oll);
			otp->onoff = 1;
			ollist_insert_data(otp->port,&oll,otp);
//			printf("%s %d %d\r\n",otp->label,otp->port,otp->onoff);
			change_output(COOLINGFAN, 1);
			send_live_code(cmd);
			break;

		case OFF_FAN:
			index = COOLINGFAN;
			rc = ollist_find_data(index,otpp,&oll);
			otp->onoff = 0;
			ollist_insert_data(otp->port,&oll,otp);
//			printf("%s %d %d\r\n",otp->label,otp->port,otp->onoff);
			change_output(COOLINGFAN, 0);
			send_live_code(cmd);
			break;

		case ON_LIGHTS:
			index = HEADLAMP;
			rc = ollist_find_data(index,otpp,&oll);
			otp->onoff = 1;
			ollist_insert_data(otp->port,&oll,otp);
	//		printf("%s %d %d\r\n",otp->label,otp->port,otp->onoff);
			change_output(HEADLAMP, 1);

			index = RUNNINGLIGHTS;
			rc = ollist_find_data(index,otpp,&oll);
			otp->onoff = 1;
			ollist_insert_data(otp->port,&oll,otp);
//			printf("%s %d %d\r\n",otp->label,otp->port,otp->onoff);
			change_output(RUNNINGLIGHTS, 1);
			break;

		case OFF_LIGHTS:
			index = HEADLAMP;
			rc = ollist_find_data(index,otpp,&oll);
			otp->onoff = 0;
			ollist_insert_data(otp->port,&oll,otp);
//			printf("%s %d %d\r\n",otp->label,otp->port,otp->onoff);
			change_output(HEADLAMP, 0);

			index = RUNNINGLIGHTS;
			rc = ollist_find_data(index,otpp,&oll);
			otp->onoff = 0;
			ollist_insert_data(otp->port,&oll,otp);
//			printf("%s %d %d\r\n",otp->label,otp->port,otp->onoff);
			change_output(RUNNINGLIGHTS, 0);
			break;

		case START_SEQ:
			engine_running = 1;
			change_output(STARTER, 1);
			starter_on = 1;
			send_live_code(ENABLE_START);

			usleep(100000);
			change_output(ACCON, 1);
			send_live_code(ON_ACC);

			usleep(100000);
			change_output(FUELPUMP, 1);
			send_live_code(ON_FUEL_PUMP);
			fan_delay = 1;
			break;

		case SHUTDOWN:
//						myprintf1("shutdown\0");
			for(i = STARTER;i < PRELUBE+1;i++)
			{
				change_output(i, 0);
				uSleep(0,TIME_DELAY/10000);
			}
			fan_delay = 0;
			engine_running = 0;
			starter_on = 0;
			send_live_code(STARTER_OFF);
			send_live_code(OFF_FUEL_PUMP);
			send_live_code(OFF_FAN);
			send_live_code(OFF_ACC);
			running_seconds = running_minutes = running_hours = 0;
			break;
	}
}
/*********************************************************************/
// if an input switch is changed, update the record for that switch
// and if an output applies to that input, change the output
// and record the event in llist
UCHAR monitor_input_task(int test)
{
	static I_DATA *itp;
	static I_DATA **itpp = &itp;

	static O_DATA *otp;
	static O_DATA **otpp = &otp;

//	RI_DATA rp;
//	RI_DATA *rtp = &rp;

	static int status = -1;
	static int bank, index;
	static UCHAR result,result2, mask, mask2, onoff;
	static int i, rc;
	

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

					rc = ollist_find_data(itp->affected_output,otpp,&oll);

/*
typedef struct o_data
{
	char label[OLABELSIZE];
	UCHAR port;
	UCHAR onoff;			// current state: 1 of on; 0 if off
	UCHAR polarity;			// 0 - on input turns output on; off input turns output off
							// 1 - on input turns output off; off input turns output on
	UCHAR type;				// see below
	UINT time_delay;
	UCHAR pulse_time;
	UCHAR reset;			// 
} O_DATA;


type:
0) regular - on/off state responds to assigned input (affected_output)
1) goes on/off and stays that way until some other event occurs
	this is useful for a lock-out condition (use reset field)
2) on for time delay seconds and then it goes back off
3) goes on/off at a pulse_time rate (ms) until turned off again
4) goes on/off at pulse_time rate for time_delay seconds and then back off

*/
					printf("%s\r\n",otp->label);

					if(otp->type == 1)
					{
						if(otp->reset == 0)
						{
							otp->reset = 1;

/*
							if(otp->polarity == 1)
								otp->onoff = (otp->onoff == 1?0:1);
							else	
								otp->onoff = (otp->onoff == 1?1:0);
*/
							otp->onoff = (otp->onoff == 1?0:1);
							change_output(otp->port,otp->onoff);
							ollist_insert_data(otp->port,&oll,otp);
							printf("onoff: %d reset: %d pol: %d\r\n", otp->onoff, otp->reset, 
									otp->polarity);
						}	
						else if(otp->reset == 1)
						{
							otp->reset = 0;
							printf("onoff: %d reset: %d pol: %d\r\n", otp->onoff, otp->reset, 
									otp->polarity);
						}
					}else
					{
						if(otp->polarity == 1)
							otp->onoff = (onoff == 1?0:1);
						else	
							otp->onoff = onoff;
						ollist_insert_data(otp->port,&oll,otp);
						change_output(otp->port,otp->onoff);
					}

					rt_file_data[rt_fd_ptr++] = otp->port;
					rt_file_data[rt_fd_ptr++] = otp->onoff;
					rt_file_data[rt_fd_ptr++] = trunning_hours;
					rt_file_data[rt_fd_ptr++] = trunning_minutes;
					rt_file_data[rt_fd_ptr++] = trunning_seconds;
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
				}
				inportstatus[bank] = result;

			}
			uSleep(0,TIME_DELAY/50);
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
	static int change_output(int index, int onoff)
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
	UCHAR timer_task(int test)
	{
		int i;
		uSleep(0,TIME_DELAY);
		UCHAR buffer[50];
		char tempx[20];
		int fp;

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
				if(++starter_on > 10)
				{
					starter_on = 0;
					change_output(STARTER, 0);
					myprintf1("STARTER OFF\0");
					send_live_code(STARTER_OFF);
				}
			}
			if(engine_running > 0)
			{
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
				if(fan_delay > 0)
				{
					if(++fan_delay > 30)
					{
						change_output(COOLINGFAN, 1);
						send_live_code(ON_FAN);
						fan_delay = 0;
					}
				}
			}

/*
			if(trunning_minutes < 1 && trunning_seconds < 30)
			{
				if(trunning_seconds % 2 == 0)
					red_led(0);
				else
					red_led(1);
			}
*/
//			if(trunning_minutes == 1 && trunning_seconds < 5)
//				red_led(0);

			odometer++;
/*
			if(tcp_connected_time++ > 30)
			{
				close_tcp();
			}
*/
			if(++trunning_seconds > 59)
			{
				trunning_seconds = 0;
				if(++trunning_minutes > 59)
				{
					trunning_minutes = 0;

// hours can be up to 255 or 10 1/2 days
//					if(++trunning_hours > 24)
//						trunning_hours = 0;
				}
			}
			if(live_window_on)
			{
//				pthread_mutex_lock( &serial_read_lock);

				buffer[0] = TOTAL_UP_TIME;
				buffer[1] = trunning_seconds;
				buffer[2] = trunning_minutes;
				buffer[3] = trunning_hours;

				buffer[4] = rt_data[0];
				buffer[5] = rt_data[1];
				buffer[6] = rt_data[2];
				buffer[7] = rt_data[3];
				buffer[8] = rt_data[4];
				buffer[9] = rt_data[5];
/*
				buffer[10] = rt_data[6];
				buffer[11] = rt_data[7];
*/
//				for(i = 0;i < 16;i++)
//					rt_data[i] = rt_data[i] + 1;

//				pthread_mutex_unlock(&serial_read_lock);

				send_tcp((UCHAR *)&buffer[0],10);
			}

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

// if console is enabled then we don't use 1st serial port because
// console is using it
#ifndef CONSOLE_DISABLED
//	printf("console enabled\n");
	return 0;
#endif

	memset(serial_buff,0,SERIAL_BUFF_SIZE);
	no_serial_buff = 0;
	memset(errmsg,0,20);

	uSleep(10,0);

	if(fd = init_serial() < 0)
	{
		myprintf1("can't open comm port 1\0");
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
		if(ch == RT_DATA)
		{
			// number of ADC channels + 2 bytes for the rpm
			for(i = 0;i < NUM_ADC_CHANNELS+2;i++)
			{
				rt_data[i] = read_serial(errmsg);
			}
		}
		pthread_mutex_unlock(&serial_read_lock);
		if(ch != RT_DATA)
		{
/*
basic_controls does any of these:
	ENABLE_START
	STARTER_OFF
	ON_FUEL_PUMP
	OFF_FUEL_PUMP
	ON_FAN
	OFF_FAN
	ON_ACC
	OFF_ACC
	START_SEQ
	SHUTDOWN
*/
			basic_controls(ch);
		}
/*
		if(errmsg[0] != 0)
		{
			myprintf1(errmsg);
			memset(errmsg,0,20);
			uSleep(5,0);
		}else
		{
			if(tcp_window_on == 1)
				send_tcp((UCHAR *)&ch,1);
		}
*/
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

/*********************************************************************/
// serial receive task
UCHAR serial_recv_task2(int test)
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

	if(fd = init_serial2() < 0)
	{
		myprintf1("can't open comm port 2\0");
//		printf("can't open comm port 2\n");
		return 0;
	}

	while(TRUE)
	{
		pthread_mutex_lock( &serial_read_lock2);
		ch = read_serial2(errmsg);
//		ch = read_serial2(errmsg);
		pthread_mutex_unlock(&serial_read_lock2);
		if(errmsg[0] != 0)
		{
			myprintf1(errmsg);
//			printf("%s\n",errmsg);
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

		if(shutdown_all)
		{
//			printf("shutting down serial task2\r\n");
			close_serial2();
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
	UCHAR cmd;
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
			uSleep(1,0);
			if(shutdown_all)
			{
//				cmd = DONE_PROGRAM;
//				send_tcp((UCHAR *)&cmd,1);

				close_tcp();
//				printf("done tcp_mon\r\n");
				return 0;
			}
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
			printf("connected to socket: \r\n");
			tcp_connected_time = 0;

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
//	myprintf1("closing socket\0");
//	send_tcp("close",5);
	if(sock_open)
	{
		sock_open = 0;
		close(global_socket);
		global_socket = -1;
	}else
	{
		myprintf1("socket already closed\0");
	}
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

