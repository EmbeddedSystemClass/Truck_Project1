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
#include "../mytypes.h"
//#include "../tasks.h"
#include "rdwr.h"

#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif

typedef struct o_data
{
	char label[20];
	UCHAR port;				// inputs are from 0 -> 39, outputs are from 40 -> 79
	UCHAR onoff;			// 1 of on; 0 if off
	UCHAR temp1,temp2;		// do this so both I_DATA oand O_DATA are 24 bytes
							// for some reason the target was make O_DATA 24 instead of 22
} O_DATA;

typedef struct ollist_node {
  int index;
  O_DATA *datap;
  struct llist_node *nextp;
} ollist_node_t;

typedef struct ollist {
  ollist_node_t *first;
  pthread_rdwr_t rwlock;
} ollist_t;

int ollist_init (ollist_t *llistp);
int ollist_insert_data (int index, ollist_t *llistp,O_DATA *datap);
int ollist_remove_data(int index, O_DATA **datapp, ollist_t *llistp);
int ollist_removeall_data(ollist_t *llistp);
int ollist_find_data(int index, O_DATA **datapp, ollist_t *llistp);
int ollist_change_data(int index, O_DATA *datap, ollist_t *llistp);
int ollist_show(ollist_t *llistp);
