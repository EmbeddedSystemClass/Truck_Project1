/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *
 ********************************************************
 * llist_threads_rw.c --
 *
 * Linked list library with reader/writer locks for
 * threads support
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../mytypes.h"
#include "../serial_io.h"
#include "illist_threads_rw.h"

/******************************************************************************/
int illist_init (illist_t *llistp)
{
	int rtn;

	llistp->first = NULL;
	if ((rtn = pthread_rdwr_init_np(&(llistp->rwlock), NULL)) !=0)
		fprintf(stderr, "pthread_rdwr_init_np error %d",rtn), exit(1);
//	printf("sizeof I_DATA: %lu\n",sizeof(I_DATA));
	return 0;
}

/******************************************************************************/
// this actually replaces the data at 'index'
int illist_insert_data (int index, illist_t *llistp, I_DATA *datap2)
{
	illist_node_t *cur, *prev, *new;
	int found = FALSE;
	I_DATA *datap;

	pthread_rdwr_wlock_np(&(llistp->rwlock));
	datap = malloc(sizeof(I_DATA));
	memcpy(datap,datap2,sizeof(I_DATA));

/*
	if(datap != NULL)
	{
		strcpy(datap->label,"test");
		datap->port = 2 + index;
		datap->affected_output = 3 + index;
		datap->type = DIRECT;
		datap->inverse = index%2?1:0;
//		printf("%d  %d  %d  %d\n", datap->in_port,datap->in_bit,datap->out_port,datap->out_bit);
	}
	else
		datap = (I_DATA *)NULL;
*/
	for (cur=prev=llistp->first; cur != NULL; prev=cur, cur=cur->nextp)
	{
		if (cur->index == index)
		{
//			printf("%d %s\n",index,cur->datap->label);
			free(cur->datap);
			cur->datap = datap;
			found=TRUE;
			break;
		}
		else if (cur->index > index)
		{
			break;
		}
	}
	if (!found)
	{
		new = (illist_node_t *)malloc(sizeof(illist_node_t));
		new->index = index;
		new->datap = datap;
		new->nextp = cur;
//		printf("%s\n",new->datap->label);
		if (cur==llistp->first)
		llistp->first = new;
	else
		prev->nextp = new;
	}
	pthread_rdwr_wunlock_np(&(llistp->rwlock));
	return 0;
}

/******************************************************************************/
int illist_remove_data(int index, I_DATA **datapp, illist_t *llistp)
{
	illist_node_t *cur, *prev;

	/* Initialize to "not found" */
	*datapp = (I_DATA*)NULL;

	pthread_rdwr_wlock_np(&(llistp->rwlock));

	for (cur=prev=llistp->first; cur != NULL; prev=cur, cur=cur->nextp)
	{
		if (cur->index == index)
		{
			*datapp = cur->datap;
			prev->nextp = cur->nextp;
			free(cur);
			break;
		}
		else if (cur->index > index)
		{
			break;
		}
	}
	pthread_rdwr_wunlock_np(&(llistp->rwlock));
	return 0;
}

/******************************************************************************/
int illist_removeall_data(illist_t *llistp)
{
	illist_node_t *cur, *prev;

	/* Initialize to "not found" */
	I_DATA *datapp = (I_DATA*)NULL;

	pthread_rdwr_wlock_np(&(llistp->rwlock));

	for (cur=prev=llistp->first; cur != NULL; prev=cur, cur=cur->nextp)
	{
		datapp = cur->datap;
//		printf("%s_\n",datapp->label);
		prev->nextp = cur->nextp;
		free(cur);
	}
	pthread_rdwr_wunlock_np(&(llistp->rwlock));
	return 0;
}

/******************************************************************************/
int illist_find_data(int index, I_DATA **datapp, illist_t *llistp)
{
	illist_node_t *cur, *prev;
	int status = -1;

	/* Initialize to "not found" */
	*datapp = (I_DATA *)NULL;

	pthread_rdwr_rlock_np(&(llistp->rwlock));

	/* Look through index for our entry */
	for (cur=prev=llistp->first; cur != NULL; prev=cur, cur=cur->nextp)
	{
		if (cur->index == index)
		{
			*datapp = cur->datap;
			status = 0;
//			printf("%d  %s\n", cur->datap->port,cur->datap->label);
//			printf("%d  %s\n", datapp->port,datapp->label);
			break;
		}
		else if (cur->index > index)
		{
			break;
		}
	}
	pthread_rdwr_runlock_np(&(llistp->rwlock));
	return status;
}

/******************************************************************************/
//#if 0
int illist_change_data(int index, I_DATA *datap, illist_t *llistp)
{
	illist_node_t *cur, *prev;
	int status = -1; /* assume failure */


	pthread_rdwr_wlock_np(&(llistp->rwlock));

	for (cur=prev=llistp->first; cur != NULL; prev=cur, cur=cur->nextp)
	{
//		printf("1:%s\n",cur->datap->label);
		if (cur->index == index)
		{
			I_DATA *newdatap = malloc(sizeof(I_DATA));
			if(newdatap == NULL)
			{
				printf("problem with malloc\n");
				return 1;
			}
			memcpy(newdatap->label,datap->label,ILABELSIZE);
			newdatap->port = datap->port;
			newdatap->affected_output = datap->affected_output;
			cur->datap = newdatap;
			prev->nextp = cur->nextp;
//			printf("2:%d\t%s\n",index,cur->datap->label);
			free(cur);
			status = 0;
			break;
		}
		else if (cur->index > index)
		{
			break;
		}
	}
	pthread_rdwr_wunlock_np(&(llistp->rwlock));
	return status;
}
//#endif
/******************************************************************************/
int illist_show(illist_t *llistp)
{
	illist_node_t *cur;

	pthread_rdwr_rlock_np(&(llistp->rwlock));

	for (cur=llistp->first; cur != NULL; cur=cur->nextp)
	{
		if(cur->datap->label[0] != 0)
		{
//			printf("port: %2d\taffected: %2d\t%s\n",cur->datap->port, \
				cur->datap->affected_output, cur->datap->label);
//			serprintf1("port: \0");
//			serprintf4(" \0",cur->datap->port, cur->datap->affected_output, cur->datap->label);
		}
	}
	pthread_rdwr_runlock_np(&(llistp->rwlock));
	return 0;
}

