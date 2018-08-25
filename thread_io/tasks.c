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
static int shutdown_all = 0;

UCHAR (*fptr[NUM_TASKS])(int) = { get_host_cmd_task, monitor_input_task, monitor_fake_input_task, timer_task, read_button_inputs, serial_recv_task, tcp_monitor_task};

static int same_msg;
int threads_ready_count=0;
pthread_cond_t    threads_ready=PTHREAD_COND_INITIALIZER;
pthread_mutex_t   threads_ready_lock=PTHREAD_MUTEX_INITIALIZER;
static UCHAR check_inputs(int index, int test);

int send_tcp(UCHAR *str,int len);

illist_t ill;
ollist_t oll;
static O_DATA *otp;
static O_DATA **otpp = &otp;

//rt_llist_t roll;

extern char oFileName[20];
extern char iFileName[20];

extern UCHAR reboot_on_exit;
UCHAR upload_buf[UPLOAD_BUFF_SIZE];

static UCHAR rt_data[20];
static UCHAR rt_file_data[1000];
static int rt_fd_ptr;
static int odometer;
static int trip;
static int tcp_connected_time;
static int serial_recv_on;

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
static int change_input(int index, int onoff);
static int uSleep(time_t sec, long nanosec);
static void basic_controls(UCHAR code);
static void set_output(int type, int onoff);
static void send_PIC_serial(int port, int onoff, int type, int time_delay);
static void send_PIC_serialother(UCHAR cmd, UCHAR data1, UCHAR data2, UCHAR data3, UCHAR data4);
static UCHAR inportstatus[OUTPORTF_OFFSET-OUTPORTA_OFFSET+1];
static UCHAR fake_inportstatus1[OUTPORTF_OFFSET-OUTPORTA_OFFSET+1];
static UCHAR fake_inportstatus2[OUTPORTF_OFFSET-OUTPORTA_OFFSET+1];
static int mask2int(UCHAR mask);
static int timer_inc;
static int timer_inc2;

//static double program_start_time;
static double diff;
static int tcp_window_on;
static int live_window_on;
static int engine_running;
static UCHAR running_hours, running_minutes, running_seconds;
static UCHAR trunning_hours, trunning_minutes, trunning_seconds;
static UINT rpm, mph;		// just sample data

#define ON 1
#define OFF 0

typedef struct
{
	int i;
	int bank;
	int index;
}REAL_BANKS;

static REAL_BANKS real_banks[40];

CMD_STRUCT cmd_array[52] =
{
	{		RE_ENTER_PASSWORD,"RE_ENTER_PASSWORD\0" },
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
	{		GET_DEBUG_INFO,"GET_DEBUG_INFO\0" },
	{		GET_DEBUG_INFO2,"GET_DEBUG_INFO2\0" },
	{		NEW_PASSWORD1,"NEW_PASSWORD1\0" },
	{		SET_SERIAL_RECV_ON,"SET_SERIAL_RECV_ON\0" },
	{		SET_SERIAL_RECV_OFF,"SET_SERIAL_RECV_OFF\0" },
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

	UCHAR status_line[10];

	memset(status_line,0,10);
	status_line[0] = cmd;
	status_line[1] = running_seconds;
	status_line[2] = running_minutes;
	status_line[3] = running_hours;
	send_tcp((UCHAR*)status_line,10);
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
	serial_recv_on = 1;
	
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

	engine_running = 0;
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

	for(i = 0;i < 10;i++)
	{
		red_led(1);
		usleep(30000);
		red_led(0);
		green_led(1);
		usleep(30000);
		green_led(0);
		red_led(1);
		usleep(30000);
		red_led(0);
		green_led(1);
		usleep(30000);
		green_led(0);
	}

//	myprintf1("start....\0");

	myprintf1("sched v1.16\0");
//	printf("sched v1.15\r\n");
	memset(rt_file_data,0,sizeof(rt_file_data));
	rt_fd_ptr = 0;
	odometer = 0;
	trip = 0;
	tcp_connected_time = 0;
	mask = 1;

#if 0
	for(i = 0;i < 40;i++)
	{
		ollist_find_data(i,otpp,&oll);
//		printf("%d %d %d %d\r\n",otp->port,otp->onoff,otp->type,otp->reset);
		if(otp->polarity == 1)
		{
			otp->onoff = 1;
			change_output(otp->port,otp->onoff);
		}
		otp->reset = 0;
		ollist_insert_data(i,&oll,otp);
		
		usleep(10000);
//		change_output(otp->port,otp->onoff);
//		change_output(otp->port,0);
//		usleep(5000);
	}

	for(i = 0;i < 4;i++)
	{
		ollist_find_data(i,otpp,&oll);
		printf("port: %d onoff: %d type: %d reset %d\ pol: %d\r\n",otp->port,otp->onoff,
			otp->type,otp->reset,otp->polarity);
	}	
#endif
/*
	for(i = 0;i < 10;i++)
	{
		change_output(i,0);
		usleep(500000);
	}
*/
	while(TRUE)
	{
		cmd = 0;
		if(test_sock() == 1)
//		if(1)
		{
			rc = recv_tcp(&cmd,1,1);			  // blocking
			tcp_connected_time = 0;
			if(cmd != LCD_SHIFT_RIGHT && cmd != LCD_SHIFT_LEFT && cmd != 
					SCROLL_DOWN && 	cmd != SCROLL_UP && cmd != LIVE_WINDOW_ON 
						&& cmd != LIVE_WINDOW_OFF)
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

					case SET_SERIAL_RECV_ON:
						send_live_code(cmd);
						if(fd = init_serial() < 0)
						{
							myprintf1("can't open comm port 1\0");
							return 0;
						}
						serial_recv_on = 1;
						break;

					case SET_SERIAL_RECV_OFF:
						send_live_code(cmd);
						close_serial();
						serial_recv_on = 0;
						break;

					case NEW_PASSWORD1:
						rc = 0;
						memset(tempx,0,50);
						rc += recv_tcp((UCHAR *)&tempx[0],12,1);
						myprintf2("read: ",rc);

						send_PIC_serialother(NEW_PASSWORD2,tempx[0],
							tempx[1],tempx[2],tempx[3]);
						send_PIC_serialother(NEW_PASSWORD3,tempx[4],
							tempx[5],tempx[6],tempx[7]);
						send_PIC_serialother(NEW_PASSWORD4,tempx[8],
							tempx[9],tempx[10],tempx[11]);
//						for(i = 0;i < 12;i++)
//							myprintf2("tempx: ",tempx[i]);
//						myprintf1("done");
						break;

					case CLEAR_SCREEN:
//						lcd_cls();
						send_PIC_serialother(CLEAR_SCREEN,CLEAR_SCREEN,
							CLEAR_SCREEN,CLEAR_SCREEN,CLEAR_SCREEN);
						break;

					case GET_DEBUG_INFO:
						send_PIC_serialother(GET_DEBUG_INFO,(UCHAR)engine_running,
							(UCHAR)running_seconds,(UCHAR)running_minutes,(UCHAR)running_hours);
						break;
						
					case GET_DEBUG_INFO2:
						send_PIC_serialother(GET_DEBUG_INFO2,(UCHAR)(rpm >> 8),
							(UCHAR)rpm, (UCHAR)(mph >> 8),(UCHAR)mph);
						break;
						
					case RE_ENTER_PASSWORD:
						send_PIC_serialother(RE_ENTER_PASSWORD1,0,0,0,0);
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
						break;
*/
					case SEND_SERIAL:
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


//						for(i = 0;i < NUM_PORT_BITS;i++)
/*
						for(i = 0;i < 6;i++)
						{
							ollist_find_data(i,otpp,&oll);
//							printf("%d %d\r\n",otp->port,otp->onoff);
							usleep(100000);
							change_output(otp->port,0);
							usleep(100000);
						}
*/
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
/*
						for(i = 0;i < NUM_PORT_BITS;i++)
						{
							ollist_find_data(i,otpp,&oll);
					//		printf("%d %d\r\n",otp->port,otp->onoff);
							usleep(1000);
					//		change_output(otp->port,otp->onoff);
							change_output(otp->port,0);
							usleep(1000);
							ollist_insert_data(i,&oll,otp);
						}
*/
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

						for(i = 0;i < 20;i++)
						{
							red_led(1);
							usleep(20000);
							red_led(0);
							green_led(1);
							usleep(20000);
							green_led(0);
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
static void set_output(int type, int onoff)
{

	switch(type)
	{
		case 0:
			if(otp->polarity == 1)
				otp->onoff = (onoff == 1?0:1);
			else	
				otp->onoff = onoff;
//			printf("0 port: %d onoff: %d reset: %d pol: %d\r\n", otp->port,
//							otp->onoff, otp->reset, otp->polarity);
			change_output(otp->port,otp->onoff);
			break;
		case 1:
			if(otp->reset == 0)
			{
//				printf("1a port: %d onoff: %d reset: %d pol: %d\r\n", otp->port,
//							otp->onoff, otp->reset, otp->polarity);
				otp->reset = 1;
//				if(otp->polarity == 0)
				TOGGLE_OTP;
				change_output(otp->port,otp->onoff);
//							printf("1 port: %d onoff: %d reset: %d pol: %d\r\n\r\n", otp->port,
//										otp->onoff, otp->reset, otp->polarity);
			}	
			else if(otp->reset == 1)
			{
//				if(otp->polarity == 1)
//					TOGGLE_OTP;
				otp->reset = 0;
//				printf("1b port: %d onoff: %d reset: %d pol: %d\r\n", otp->port,
//							otp->onoff, otp->reset, otp->polarity);
			}
			break;
		case 2:
			if(otp->reset == 0)
			{
				otp->reset = 1;
				otp->time_left = otp->time_delay;
//							otp->onoff = onoff;
				TOGGLE_OTP;
				change_output(otp->port,onoff);
//				printf("2 port: %d onoff: %d reset: %d pol: %d\r\n", otp->port,
//							otp->onoff, otp->reset, otp->polarity);
			}
			break;
		case 3:
			break;
		case 4:
			break;
		default:
			break;
	}
	send_PIC_serial(otp->port,otp->onoff,otp->type, otp->time_delay);
	
}
/*********************************************************************/
static void send_PIC_serial(int port, int onoff, int type, int time_delay)
{
// send what just changed to the PIC24/AVR to dispaly on screen
	pthread_mutex_lock( &serial_write_lock);
	
	write_serial(OUTPUT_MSG);
	write_serial((UCHAR)port);
	write_serial((UCHAR)onoff);
	write_serial((UCHAR)type);
	write_serial((UCHAR)time_delay);

	pthread_mutex_unlock(&serial_write_lock);
}
/*********************************************************************/
static void send_PIC_serialother(UCHAR cmd, UCHAR data1, UCHAR data2, UCHAR data3, UCHAR data4)  
{
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
// and record the event in llist
UCHAR monitor_input_task(int test)
{
	static I_DATA *itp;
	static I_DATA **itpp = &itp;

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

				for(i = 0;i < 40;i++)
				{
					if(real_banks[i].bank == bank && real_banks[i].index == index)
					{
						index = real_banks[i].i;
					}
				}
				illist_find_data(index,itpp,&ill);

				rc = ollist_find_data(itp->affected_output,otpp,&oll);

				set_output(otp->type, onoff);

				ollist_insert_data(otp->port,&oll,otp);

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
static int change_input(int index, int onoff)
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

//	printf("change input: state: %02x %02x bank: %d mask: %02x\r\n",pstate,state,bank,mask);

}
/*********************************************************************/
// do the same thing as monitor_input_tasks but with the fake arrays
// set by change_inputs()
UCHAR monitor_fake_input_task(int test)
{
	static I_DATA *itp;
	static I_DATA **itpp = &itp;

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
				illist_find_data(index,itpp,&ill);

				rc = ollist_find_data(itp->affected_output,otpp,&oll);

				set_output(otp->type, onoff);

				ollist_insert_data(otp->port,&oll,otp);

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
	int index = 0;
	int bank = 0;
	int fp;
	UCHAR mask;
	time_t curtime2;
	struct timeval mtv;

	rpm = mph = 0;

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

		gettimeofday(&mtv, NULL);
		curtime2 = mtv.tv_sec;
		strftime(buffer,30,"%m-%d-%Y %T.\0",localtime(&curtime2));

		// send only date, month, minutes
		send_PIC_serialother(TIME_DATA1,buffer[0],buffer[1],buffer[3],buffer[4]);
		send_PIC_serialother(TIME_DATA2,buffer[8],buffer[9],buffer[11],buffer[12]);
		send_PIC_serialother(TIME_DATA3,buffer[14],buffer[15],buffer[17],buffer[18]);

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

		buffer[0] = TOTAL_UP_TIME;
		buffer[1] = trunning_seconds;
		buffer[2] = trunning_minutes;
		buffer[3] = trunning_hours;

		buffer[4] = rt_data[0];		// ADC data
		buffer[5] = rt_data[1];
		buffer[6] = rt_data[2];
		buffer[7] = rt_data[3];

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
			buffer[8] = 1;
			buffer[9] = 2;
			buffer[10] = 3;
			buffer[11] = 4;

			send_tcp((UCHAR *)&buffer[0],12);
		}
		
		// check if one of the outputs is set to type 2 (time_delay)
		for(i = 0;i < NUM_PORTS;i++)
		{
			ollist_find_data(i,otpp,&oll);
			if(otp->type == 2 && otp->reset == 1)
			{
				otp->time_left--;
				if(otp->time_left == 0)
				{
					TOGGLE_OTP;
					change_output(i,otp->onoff);
					otp->reset = 0;
					ollist_insert_data(otp->port,&oll,otp);
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
					send_PIC_serial(otp->port,otp->onoff,otp->type, otp->time_delay);
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

	memset(serial_buff,0,SERIAL_BUFF_SIZE);
	no_serial_buff = 0;
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
			if(ch == RT_DATA && live_window_on)
				// number of ADC channels + 2 bytes for the rpm
				for(i = 0;i < NUM_ADC_CHANNELS;i++)
					rt_data[i] = read_serial(errmsg);

			pthread_mutex_unlock(&serial_read_lock);

			if(ch >= ENABLE_START && ch <= ON_LIGHTS)
			{
				basic_controls(ch);
			}
		}
						
//		else if(ch == 
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
//			printf("Server Waiting...\r\n");

			if (  (global_socket=accept(listen_sd, (struct sockaddr *)&cad, &alen)) < 0)
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
		printf("socket already closed\r\n");
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
//	O_DATA *otp;
//	O_DATA **otpp = &otp;
	int rc;
	int index;

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
//			printf("%s %d %d\r\n",otp->label,otp->port,otp->onoff);
			change_input(ACCON, 1);
			send_live_code(cmd);
			break;

		case OFF_ACC:
			index = ACCON;
			rc = ollist_find_data(index,otpp,&oll);
//			printf("%s %d %d\r\n",otp->label,otp->port,otp->onoff);
			change_input(ACCON, 0);
			send_live_code(cmd);
			break;

		case ON_FUEL_PUMP:
			index = FUELPUMP;
			rc = ollist_find_data(index,otpp,&oll);
//			printf("%s %d %d\r\n",otp->label,otp->port,otp->onoff);
			change_input(FUELPUMP, 1);
			send_live_code(cmd);
			break;

		case OFF_FUEL_PUMP:
			index = FUELPUMP;
			rc = ollist_find_data(index,otpp,&oll);
//			printf("%s %d %d\r\n",otp->label,otp->port,otp->onoff);
			change_input(FUELPUMP, 0);
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
			index = HEADLAMP;
			rc = ollist_find_data(index,otpp,&oll);
//			printf("%s %d %d\r\n",otp->label,otp->port,otp->onoff);
			change_input(HEADLAMP, 1);

			index = RUNNINGLIGHTS;
			rc = ollist_find_data(index,otpp,&oll);
//			printf("%s %d %d\r\n",otp->label,otp->port,otp->onoff);
			change_input(RUNNINGLIGHTS, 1);
			break;

		case OFF_LIGHTS:
			index = HEADLAMP;
			rc = ollist_find_data(index,otpp,&oll);
//			printf("%s %d %d\r\n",otp->label,otp->port,otp->onoff);
			change_input(HEADLAMP, 0);

			index = RUNNINGLIGHTS;
			rc = ollist_find_data(index,otpp,&oll);
//			printf("%s %d %d\r\n",otp->label,otp->port,otp->onoff);
			change_input(RUNNINGLIGHTS, 0);
			break;

		case START_SEQ:
			engine_running = 1;
			change_input(STARTER, 1);
			send_live_code(ENABLE_START);

			uSleep(1,0);
			change_input(ACCON, 1);
			send_live_code(ON_ACC);

			uSleep(1,0);
			change_input(FUELPUMP, 1);
			send_live_code(ON_FUEL_PUMP);

			uSleep(1,0);
			change_input(COOLINGFAN, 1);
			send_live_code(ON_FAN);
			break;

		case SHUTDOWN:
//						myprintf1("shutdown\0");
			running_seconds = running_minutes = running_hours = 0;
			for(i = STARTER;i < COOLINGFAN+1;i++)
			{
				change_input(i, 0);
				usleep(100000);
			}
			engine_running = 0;
			send_live_code(STARTER_OFF);
			uSleep(0,100000);
			send_live_code(OFF_FUEL_PUMP);
			uSleep(0,100000);
			send_live_code(OFF_FAN);
			uSleep(0,100000);
			send_live_code(OFF_ACC);
			myprintf1("stopping all ctls\0");
			uSleep(0,100000);
			break;
	}
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

