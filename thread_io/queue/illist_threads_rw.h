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

enum bit_type
{
	DIRECT,		// port follows the switch (stays on as long as switch is on)
	MOMENTARY,	// port changes when button is pushed and released (momentary contact switch)
	REPEATING	// port goes on and off for a specified period as long as switch is on (blinker)
};

#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif

#define ILABELSIZE 30

// there is a record for all of the outputs that are hooked up (implemented in the hw)
// and also a record for all the inputs that directly effect the same output
// an input may not directly effect an output, but we may want to trigger an event
// upon a changing input
// an output may not be directly affected by an input because we may want to
// change it based upon a change in the table
// so there is one record for every active input and active output

typedef struct i_data
{
	char label[ILABELSIZE];
	UCHAR port;
	UCHAR affected_output[10];
	UCHAR temp[3];		// ARM aligns data on 4-byte alignment so this makes sizeof(I_DATA) 44 on both machines
						// otherwise the list/init_db programs won't work on ARM
} I_DATA;

typedef struct illist_node {
  int index;
  I_DATA *datap;
  struct llist_node *nextp;
} illist_node_t;

typedef struct illist {
  illist_node_t *first;
  pthread_rdwr_t rwlock;
} illist_t;

int illist_init (illist_t *illistp);
int illist_insert_data (int index, illist_t *llistp, I_DATA *datap);
int illist_remove_data(int index, I_DATA **datapp, illist_t *llistp);
int illist_removeall_data(illist_t *llistp);
int illist_find_data(int index, I_DATA **datapp, illist_t *llistp);
int illist_change_data(int index, I_DATA *datap, illist_t *llistp);
int illist_show(illist_t *llistp);

