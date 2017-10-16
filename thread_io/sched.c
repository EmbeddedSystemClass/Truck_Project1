/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *
 ********************************************************
 * sched.c
 *
 * Examples in setting scheduling policy
 */
#ifndef NOTARGET
#warning "NOTARGET not defined"
#else
#warning "NOTARGET defined"
#endif

#include<unistd.h>
#include<sys/mman.h>
#include<fcntl.h>
#include<assert.h>
#include<time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sched.h>
#include <sys/types.h>
#include <pthread.h>
#include <string.h>
#define closesocket close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "mytypes.h"
#include "ioports.h"
#include "serial_io.h"
#include "tasks.h"
#include "queue/illist_threads_rw.h"
#include "queue/ollist_threads_rw.h"

extern void init_mem(void);
extern void close_mem(void);
extern int comm_open;
/*
 * Thread initialization
 */
/*
 * Thread progress info
 */
extern void *work_routine(void *arg);

char oFileName[20];
char iFileName[20];

/****************************************************************************************************/
struct timeval tv;

static double curtime(void)
{
	gettimeofday (&tv, NULL);
	return tv.tv_sec + tv.tv_usec / 1000000.0;
}

/********************************************************************************************************/
int main(int argc, char **argv)
{
	int rtn, t, i, j, *id_arg, prio_min, prio_max;
	pthread_t threads[NUM_TASKS];
	int sched = PTIME_SLICE;
	pthread_attr_t pthread_custom_attr;
	struct sched_param priority_param;
	double current_time;
	char str2[10];
	char buf[200];
	UCHAR test1 = 0x21;
	UCHAR test2;
	int fd;
	int sd;
	int rc;
	char errmsg[50];
	I_DATA temp;

	if(argc < 2)
	{
		printf("usage: %s [idata filename][odata filename]\n",argv[0]);
		printf("loading default filenames: idata.dat & odata.dat\n");
		strcpy(iFileName,"idata.dat\0");
		strcpy(oFileName,"odata.dat\0");
	}
	else if(argc == 2)
	{
		printf("usage: %s %s [odata filename]\n",argv[0],argv[1]);
		printf("loading %s & odata.dat\n",argv[1]);
		strcpy(oFileName,argv[1]);
		strcpy(oFileName,"odata.dat\0");
	}
	else
	{
		strcpy(iFileName,argv[1]);
		strcpy(oFileName,argv[2]);
		printf("loading: %s & %s\n",argv[1],argv[2]);
	}

	id_arg = (int *)malloc(NUM_TASKS*sizeof(int));
	printf("test4\n");

	current_time = curtime();

	printf("\n\n");

	comm_open = -1;

	if(fd = init_serial() < 0)
	{
		printf("can't open comm port\n");
		return 0;
	}
	else
	{
//		printString("serial port opened\0");
		comm_open = 1;
	}
//	printf("fd (main) = %d\n",fd);

	init_mem();

	for(i = 0;i < NUM_TASKS;i++)
		id_arg[i] = i;

	sched = PTIME_SLICE;

	/* spawn the threads */
	for (i = 0; i < NUM_TASKS; i++)
	{
		if (sched == DEFAULT)
		{
			if (pthread_create(&(threads[i]), NULL,	work_routine, (void *) &(id_arg[i])) !=0)
			perror("main() pthread create with NULL attr obj failed"),exit(1);

		} else
		{
			/*
			* Just for the fun of it, let's up our priority, and
			* run with policy FIFO
			*/

			priority_param.sched_priority = sched_get_priority_max(SCHED_FIFO);
			pthread_setschedparam(pthread_self(), SCHED_FIFO, &priority_param);

			if (sched == INHERIT)
			{
				pthread_attr_init(&pthread_custom_attr);
				pthread_attr_setinheritsched(&pthread_custom_attr, PTHREAD_INHERIT_SCHED);
			} else
			{
			/*
			* Now, for the rest of the threads
			*/
			pthread_attr_init(&pthread_custom_attr);
			pthread_attr_setinheritsched(&pthread_custom_attr, PTHREAD_EXPLICIT_SCHED);
			if (sched == FIFO)
			{
				pthread_attr_setschedpolicy(&pthread_custom_attr, SCHED_FIFO);
				prio_min = sched_get_priority_max(SCHED_FIFO); /* PRI_FIFO_MIN;*/
				prio_max = prio_min; /* force to all same priority */
			}
			if (sched == TIME_SLICE)
			{
				pthread_attr_setschedpolicy(&pthread_custom_attr, SCHED_RR);
				prio_min = sched_get_priority_max(SCHED_RR); /* PRI_RR_MIN; */
				prio_max = prio_min; /* force to all same priority */
			}
			if (sched == PFIFO)
			{
				pthread_attr_setschedpolicy(&pthread_custom_attr, SCHED_FIFO);
				prio_max = sched_get_priority_max(SCHED_FIFO); /* PRI_FIFO_MAX; */
				prio_min = sched_get_priority_min(SCHED_FIFO); /* PRI_FIFO_MIN; */
			}
			if (sched == PTIME_SLICE)
			{
				pthread_attr_setschedpolicy(&pthread_custom_attr, SCHED_RR);
				prio_max = sched_get_priority_max(SCHED_RR); /* PRI_RR_MAX; */
				prio_min = sched_get_priority_min(SCHED_RR); /* PRI_RR_MIN; */
			}
//			priority_param.sched_priority = prio_min + 	(i)*(prio_max - prio_min)/(NUM_TASKS-1);
/*
			if(i == 5)
				priority_param.sched_priority = prio_max;
			else
				priority_param.sched_priority = prio_max - i - 2;
*/
			priority_param.sched_priority = prio_max;
//			printf("setting priority(%d-%d) of thread %d to %d\n", prio_min, \
//			prio_max, i,priority_param.sched_priority);
			if (pthread_attr_setschedparam(&pthread_custom_attr, &priority_param)!=0)
				fprintf(stderr,"pthread_attr_setschedparam failed\n");
			} /* else not inherit */


			if (pthread_create(&(threads[i]), &pthread_custom_attr, work_routine, (void *) &(id_arg[i])) !=0)
			perror("main() pthread create with attr obj failed"),exit(1);


		} /* else not default */
	} /* For Num Threads */

/*	printf("\nmain()\t\t\t\t%d threads created. Main running fifo at max\n", NUM_TASKS); */

	/* wait until all threads have finished */
	for (i = 0; i < NUM_TASKS; i++)
	{
		if (pthread_join(threads[i], NULL) !=0)
			perror("main() pthread_join failed"),exit(1);
	}
/*	close_mem(); */

//	RS232_CloseComport(1);
	strcpy(str2,"close");
//	send(sd,str2,5,0);
	usleep(100000);
//	closesocket(sd);
	printf("socket closed\n");
//	llist_show(&ll);
	printf("elapsed time: %f\n",curtime() - current_time);

	return 0;
}

