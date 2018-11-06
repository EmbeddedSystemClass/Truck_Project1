#ifndef __TASKS_H
#define  __TASKS_H

#define NUM_TASKS           	8

#define DEFAULT                 0
#define TIME_SLICE              1
#define FIFO                    2
#define PRIOR                   3
#define PTIME_SLICE             4
#define PFIFO                   5
#define INHERIT                 6

#define PROTOPORT         5193				  /* default protocol port number */
#define QLEN              6					  /* size of request queue        */
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
int change_output(int index, int onoff);
int change_input(int index, int onoff);
void basic_controls(UCHAR code);
void send_serial(int port, int onoff);
void send_serialother(UCHAR cmd, UCHAR data1, UCHAR data2, UCHAR data3, UCHAR data4);
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

int tcp_window_on;
static UCHAR running_hours, running_minutes, running_seconds;
static UCHAR trunning_hours, trunning_minutes, trunning_seconds;
static UINT rpm, mph;		// just sample data

REAL_BANKS real_banks[40];

static UCHAR rt_data[30];
static UCHAR rt_file_data[1000];
static int rt_fd_ptr;
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
