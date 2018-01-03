/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *
 ********************************************************
 * llist_threads_rw.c
 *
 * Linked list library with reader/writer locks for
 * threads support
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../mytypes.h"
#include "../serial_io.h"
#include "ollist_threads_rw.h"
#ifdef MAKE_TARGET
extern int send_tcp(UCHAR *str,int len);
#endif
/******************************************************************************/
int ollist_init (ollist_t *llistp)
{
	int rtn;

	llistp->first = NULL;
	if ((rtn = pthread_rdwr_init_np(&(llistp->rwlock), NULL)) !=0)
		fprintf(stderr, "pthread_rdwr_init_np error %d",rtn), exit(1);
//	printf("sizeof O_DATA: %lu\n",sizeof(O_DATA));
	return 0;
}

/******************************************************************************/
int ollist_insert_data (int index, ollist_t *llistp,O_DATA *datap2)
{
	ollist_node_t *cur, *prev, *new;
	int found = FALSE;
	O_DATA *datap;

	pthread_rdwr_wlock_np(&(llistp->rwlock));
	datap = malloc(sizeof(O_DATA));
	memcpy(datap,datap2,sizeof(O_DATA));
/*
	if(datap != NULL)
	{
		strcpy(datap->label,"test");
		datap->port = 2 + index;
		datap->onoff = index%2?1:0;
//		printf("%d  %d  %d  %d\n", datap->in_port,datap->in_bit,datap->out_port,datap->out_bit);
	}
	else
		datap = (O_DATA *)NULL;
*/
	for (cur=prev=llistp->first; cur != NULL; prev=cur, cur=cur->nextp)
	{
		if (cur->index == index)
		{
			free(cur->datap);
			cur->datap = datap;
//			printf("\n\n%s\n\n\n",cur->datap->label);
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
		new = (ollist_node_t *)malloc(sizeof(ollist_node_t));
		new->index = index;
		new->datap = datap;
		new->nextp = cur;
		if (cur==llistp->first)
		llistp->first = new;
	else
		prev->nextp = new;
	}
	pthread_rdwr_wunlock_np(&(llistp->rwlock));
	return 0;
}

/******************************************************************************/
int ollist_remove_data(int index, O_DATA **datapp, ollist_t *llistp)
{
	ollist_node_t *cur, *prev;

	/* Initialize to "not found" */
	*datapp = (O_DATA*)NULL;

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
int ollist_removeall_data(ollist_t *llistp)
{
	ollist_node_t *cur, *prev;

	/* Initialize to "not found" */
	O_DATA *datapp = (O_DATA*)NULL;

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
int ollist_find_data(int index, O_DATA **datapp, ollist_t *llistp)
{
	ollist_node_t *cur, *prev;
	int status = -1;

	/* Initialize to "not found" */
	*datapp = (O_DATA *)NULL;

	pthread_rdwr_rlock_np(&(llistp->rwlock));

	/* Look through index for our entry */
	for (cur=prev=llistp->first; cur != NULL; prev=cur, cur=cur->nextp)
	{
		if (cur->index == index)
		{
			*datapp = cur->datap;
			status = 0;
//			printf("%d  %d  %d  %d\n", cur->datap->in_port,cur->datap->in_bit,cur->datap->out_port,cur->datap->out_bit);
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
int ollist_toggle_output(int index, ollist_t *llistp)
{
	ollist_node_t *cur, *prev;
	int status = -1; /* assume failure */
	O_DATA o_data;
	int onoff;

	pthread_rdwr_wlock_np(&(llistp->rwlock));

	for (cur=prev=llistp->first; cur != NULL; prev=cur, cur=cur->nextp)
	{
		if (cur->index == index)
		{
			memcpy(&o_data,cur->datap,sizeof(O_DATA));
//			printf("%d %s ",o_data.onoff,o_data.label);
			if(o_data.onoff == 1)
				o_data.onoff = 0;
			else o_data.onoff = 1;
			onoff = o_data.onoff;
			memcpy(cur->datap,&o_data,sizeof(O_DATA));
			status = 0;
			break;
		}
		else if (cur->index > index)
		{
			break;
		}
	}
	pthread_rdwr_wunlock_np(&(llistp->rwlock));
	return onoff;
}


/******************************************************************************/
int ollist_change_output(int index, ollist_t *llistp, int onoff)
{
	ollist_node_t *cur, *prev;
	int status = -1; /* assume failure */
	O_DATA o_data;

	pthread_rdwr_wlock_np(&(llistp->rwlock));

	for (cur=prev=llistp->first; cur != NULL; prev=cur, cur=cur->nextp)
	{
		if (cur->index == index)
		{
			memcpy(&o_data,cur->datap,sizeof(O_DATA));
//			printf("%d %s ",o_data.onoff,o_data.label);
			o_data.onoff = onoff;
			memcpy(cur->datap,&o_data,sizeof(O_DATA));
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

/******************************************************************************/
int ollist_change_data(int index, O_DATA *datap, ollist_t *llistp)
{
	ollist_node_t *cur, *prev;
	int status = -1; /* assume failure */

	pthread_rdwr_wlock_np(&(llistp->rwlock));

	for (cur=prev=llistp->first; cur != NULL; prev=cur, cur=cur->nextp)
	{
		if (cur->index == index)
		{
			cur->datap = datap;
			prev->nextp = cur->nextp;
//			printf("%d %s ",index,datap->label);
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

/******************************************************************************/
#ifdef MAKE_TARGET
int ollist_show(ollist_t *llistp)
{
	char list_buf[50];
	char *ptr;
	int iptr;
	ollist_node_t *cur;

	pthread_rdwr_rlock_np(&(llistp->rwlock));

	for (cur=llistp->first; cur != NULL; cur=cur->nextp)
	{
		if(cur->datap->label[0] != 0)
		{
//			printf("port: %2d\tonoff: %2d\t%s\n",(int)cur->datap->port, (int)cur->datap->onoff,cur->datap->label);
			memset(list_buf,0,50);
			sprintf(list_buf,"port: %2d onoff: %2d %s",(int)cur->datap->port,\
			 (int)cur->datap->onoff,cur->datap->label);
			 ptr = list_buf;
			 iptr = 0;
			 do
			 {
			 	iptr++;
			 }while(*(ptr++) != 0);
			send_tcp((UCHAR *)list_buf,iptr);
			printString2(list_buf);

		}
	}
	pthread_rdwr_runlock_np(&(llistp->rwlock));
	return 0;
}

#endif
