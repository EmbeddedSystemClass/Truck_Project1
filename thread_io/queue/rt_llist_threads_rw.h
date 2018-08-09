/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *
 ********************************************************
 * llist_threads_rw.h --
 *
 * Include file for linked list with reader/writer locks
 * for threads support
 */
#include <pthread.h>
#include "../../mytypes.h"
//#include "../tasks.h"
#include "rdwr.h"

#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif

// database to make a record of which output was activated at what time

typedef struct ri_data
{
	UCHAR port;			// which I/O port
	UCHAR onoff;		// if turned on or off
	UCHAR hours;		// time since start of sched (up to 255 hours - 10 1/2 days)
	UCHAR minutes;
	UCHAR seconds;
} RI_DATA;

typedef struct rt_llist_node {
  int index;
  RI_DATA *datap;
  struct llist_node *nextp;
} rt_llist_node_t;

typedef struct rt_llist {
  rt_llist_node_t *first;
  pthread_rdwr_t rwlock;
} rt_llist_t;

int rt_llist_init (rt_llist_t *llistp);
int rt_llist_insert_data (rt_llist_t *llistp, RI_DATA *datap);
int rt_llist_remove_data(int index, RI_DATA **datapp, rt_llist_t *llistp);
int rt_llist_removeall_data(rt_llist_t *llistp);
int rt_llist_find_data(int index, RI_DATA **datapp, rt_llist_t *llistp);
int rt_llist_change_data(int index, RI_DATA *datap, rt_llist_t *llistp);
int rt_llist_show(rt_llist_t *llistp);
int rt_llist_printfile(int fp, rt_llist_t *llistp);

