#ifndef __TASKS_H
#define  __TASKS_H

#define NUM_TASKS           6

#define DEFAULT                 0
#define TIME_SLICE              1
#define FIFO                    2
#define PRIOR                   3
#define PTIME_SLICE             4
#define PFIFO                   5
#define INHERIT                 6

UCHAR task1(int test);
UCHAR task2(int test);
UCHAR task3(int test);
UCHAR task4(int test);
UCHAR task5(int test);
UCHAR task6(int test);
void *work_routine(void *arg);
int send_tcp(UCHAR *str,int len);
int recv_tcp(UCHAR *str, int strlen,int block);
void init_tcp2(void);
void close_tcp(void);
int test_sd(int test);
void SendByte(unsigned char byte);
void ReceiveByte(void);

#endif
