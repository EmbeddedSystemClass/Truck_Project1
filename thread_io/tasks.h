#ifndef __TASKS_H
#define  __TASKS_H

#if CONSOLE_DISABLED
#define NUM_TASKS           6
#else
#define NUM_TASKS           5
#endif

#define DEFAULT                 0
#define TIME_SLICE              1
#define FIFO                    2
#define PRIOR                   3
#define PTIME_SLICE             4
#define PFIFO                   5
#define INHERIT                 6

enum task_types
{
	GET_HOST_CMD,
	MONITOR_INPUTS,
	TIMER,
	UNUSED,
#if CONSOLE_DISABLED
	SERIAL_RECV,
#endif
//	SERIAL_RECV2,
	TCP_MONITOR
} TASK_TYPES;

UCHAR get_host_cmd_task(int test);
UCHAR monitor_input_task(int test);
UCHAR timer_task(int test);
UCHAR read_button_inputs(int test);
UCHAR serial_recv_task(int test);
UCHAR serial_recv_task2(int test);
UCHAR tcp_monitor_task(int test);

void *work_routine(void *arg);
int send_tcp(UCHAR *str,int len);
int recv_tcp(UCHAR *str, int strlen,int block);
void close_tcp(void);
int test_sd(int test);
int test_sock(void);
void SendByte(UCHAR byte);
UCHAR ReceiveByte(void);

#endif
