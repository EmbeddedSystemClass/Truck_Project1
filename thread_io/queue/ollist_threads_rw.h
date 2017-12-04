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

#define OLABELSIZE 20

typedef struct o_data
{
	char label[20];
	UCHAR port;
	UCHAR onoff;			// 1 of on; 0 if off
	UCHAR type;
	UINT time_delay;
	UCHAR pulse_time;
} O_DATA;

/*
type:
0) regular - on/off state doesn't change until user tells it to
1) on for time delay seconds and then it goes back off
2) goes on/off at a pulse_time rate (ms) until turned off again
4) goes on/off at pulse_time rate for time_delay seconds and then back off
5) toggle switch realized in momentary push-buton: push & release of a
	momentary push-button turns bit on or off
*/

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
int ollist_toggle_output(int index, ollist_t *llistp);
int ollist_change_output(int index, ollist_t *llistp, int onoff);
int ollist_change_data(int index, O_DATA *datap, ollist_t *llistp);
int ollist_show(ollist_t *llistp);
