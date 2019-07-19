#ifndef __TASKS_H
#define  __TASKS_H

#define NUM_TASKS           	9
#define DEFAULT                 0
#define TIME_SLICE              1
#define FIFO                    2
#define PRIOR                   3
#define PTIME_SLICE             4
#define PFIFO                   5
#define INHERIT                 6

#define PROTOPORT         5193				  /* default protocol port number */
#define QLEN              6					  /* size of request queue        */
#define MSG_QUEUE_SIZE		50

#define _1SEC	 	1000000 
#define _100MS		100000 	
#define _10MS		10000 	
#define _1MS		1000 	

#define _2SEC	 	2000000 
#define _200MS		200000 	
#define _20MS		20000 	
#define _2MS		2000 	

#define _5SEC	 	5000000 
#define _500MS		500000 	
#define _50MS		50000 	
#define _5MS		5000 	

int global_socket;

enum task_types
{
	GET_HOST_CMD,
	MONITOR_INPUTS,
	MONITOR_INPUTS2,
	TIMER,
	TIMER2,
	BUTTONS,
	SERIAL_RECV,
	TCP_MONITOR,
	BASIC_CONTROLS
} TASK_TYPES;

UCHAR get_host_cmd_task(int test);
UCHAR monitor_input_task(int test);
UCHAR monitor_fake_input_task(int test);
UCHAR timer_task(int test);
UCHAR timer2_task(int test);
UCHAR read_button_inputs(int test);
UCHAR serial_recv_task(int test);
UCHAR serial_recv_task2(int test);
UCHAR tcp_monitor_task(int test);
UCHAR basic_controls_task(int test);
int change_output(int index, int onoff);
int change_input(int index, int onoff);
void basic_controls(UCHAR code);
void send_serial(UCHAR cmd);
void send_serialother(UCHAR cmd, UCHAR *buf, int len);
void send_param_msg(void);
void add_msg_queue(UCHAR cmd);
UCHAR get_msg_queue(void);

int uSleep(time_t sec, long nanosec);
int put_sock(UCHAR *buf,int buflen, int block, char *errmsg);
int get_sock(UCHAR *buf, int buflen, int block, char *errmsg);
int get_msg(void);
void send_msg(int msg_len, UCHAR *msg, UCHAR msg_type);

void *work_routine(void *arg);
int send_tcp(UCHAR *str,int len);
int recv_tcp(UCHAR *str, int strlen,int block);
void close_tcp(void);
int test_sd(int test);
int test_sock(void);
void init_ips(void);

typedef struct
{
	int i;
	int bank;
	int index;
}REAL_BANKS;

typedef struct
{
	int index;
	int onoff;
}SPECIAL_CMD_ARR;

int tcp_window_on;
static UCHAR running_hours, running_minutes, running_seconds;
static UCHAR trunning_hours, trunning_minutes, trunning_seconds;
static UINT rpm, mph;		// just sample data

REAL_BANKS real_banks[40];

static UCHAR rt_data[30];
static UCHAR rt_file_data[1000];
static int odometer;
static int trip;
static int tcp_connected_time;
static int serial_recv_on;
static char dat_names[NUM_DAT_NAMES][DAT_NAME_STR_LEN];

extern char oFileName[20];
extern char iFileName[20];

extern UCHAR reboot_on_exit;
UCHAR upload_buf[UPLOAD_BUFF_SIZE];
static int same_msg;

#endif
