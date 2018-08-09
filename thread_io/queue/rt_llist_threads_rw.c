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
#include "../../mytypes.h"
#include "../serial_io.h"
#include "rt_llist_threads_rw.h"
#ifdef MAKE_TARGET
extern int send_tcp(UCHAR *str,int len);
#endif
/******************************************************************************/
int rt_llist_init (rt_llist_t *llistp)
{
	int rtn;

	llistp->first = NULL;
	if ((rtn = pthread_rdwr_init_np(&(llistp->rwlock), NULL)) !=0)
	{
		printf("pthread_rdwr_init_np error %d\n\r",rtn);
		fprintf(stderr, "pthread_rdwr_init_np error %d",rtn), exit(1);
	}
//	printf("sizeof RI_DATA: %lu\n\r",sizeof(RI_DATA));
	return 0;
}

/******************************************************************************/
int rt_llist_insert_data (rt_llist_t *llistp, RI_DATA *datap2)
{
	rt_llist_node_t *cur, *prev, *new;
	RI_DATA *datap;
	static int index = 0;
	int tindex;

	pthread_rdwr_wlock_np(&(llistp->rwlock));
	datap = malloc(sizeof(RI_DATA));
	memcpy(datap,datap2,sizeof(RI_DATA));
//	printf("data: %d %d %02d:%02d:%02d\n\r",datap->port,datap->onoff,datap->hours,datap->minutes,datap->seconds);


	for (cur=prev=llistp->first; cur != NULL; prev=cur, cur=cur->nextp)
	{
		tindex = cur->index;
//		printf("%d ",tindex);
	}
//	printf("\n\r");

	new = (rt_llist_node_t *)malloc(sizeof(rt_llist_node_t));
	new->index = index;
	new->datap = datap;
	new->nextp = cur;

	if (cur==llistp->first)
	{
		llistp->first = new;
//		printf("first\r\n");
	}else
		prev->nextp = new;

//	printf("%d %d: data: %d %d %02d:%02d:%02d\n\r",index,prev->index,datap->port,datap->onoff,datap->hours,
//			datap->minutes,datap->seconds);

	index++;

	pthread_rdwr_wunlock_np(&(llistp->rwlock));
	return 0;
}

/******************************************************************************/
int rt_llist_remove_data(int index, RI_DATA **datapp, rt_llist_t *llistp)
{
	rt_llist_node_t *cur, *prev;

	/* Initialize to "not found" */
	*datapp = (RI_DATA*)NULL;

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
int rt_llist_removeall_data(rt_llist_t *llistp)
{
	rt_llist_node_t *cur, *prev;

	/* Initialize to "not found" */
	RI_DATA *datapp = (RI_DATA*)NULL;

	pthread_rdwr_wlock_np(&(llistp->rwlock));

	for (cur=prev=llistp->first; cur != NULL; prev=cur, cur=cur->nextp)
	{
		datapp = cur->datap;
//		printf("%d %d :",datapp->port,datapp->onoff);
		prev->nextp = cur->nextp;
		free(cur);
	}
	pthread_rdwr_wunlock_np(&(llistp->rwlock));
	return 0;
}
/******************************************************************************/
int rt_llist_find_data(int index, RI_DATA **datapp, rt_llist_t *llistp)
{
	rt_llist_node_t *cur, *prev;
	int status = -1;

	/* Initialize to "not found" */
	*datapp = (RI_DATA *)NULL;

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
int rt_llist_change_data(int index, RI_DATA *datap, rt_llist_t *llistp)
{
	rt_llist_node_t *cur, *prev;
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
int rt_llist_show(rt_llist_t *llistp)
{
	char list_buf[50];
	char *ptr;
	int iptr;
	rt_llist_node_t *cur;

	pthread_rdwr_rlock_np(&(llistp->rwlock));

	for (cur=llistp->first; cur != NULL; cur=cur->nextp)
	{
//		if(cur->datap->label[0] != 0)
		if(1)
		{
//			printf("port: %2d\tonoff: %2d\t%s\n",(int)cur->datap->port, (int)cur->datap->onoff,cur->datap->label);
			memset(list_buf,0,50);
			sprintf(list_buf,"port: %2d onoff: %2d %s",(int)cur->datap->port,\
			 (int)cur->datap->onoff,cur->datap->hours, cur->datap->minutes, cur->datap->seconds);
			 ptr = list_buf;
			 iptr = 0;
			 do
			 {
			 	iptr++;
			 }while(*(ptr++) != 0);
			send_tcp((UCHAR *)list_buf,iptr);
//			printString2(list_buf);

		}
	}
	pthread_rdwr_runlock_np(&(llistp->rwlock));
	return 0;
}

#endif

/******************************************************************************/
int rt_llist_printfile(int fp, rt_llist_t *llistp)
{
	char list_buf[20];
	char *ptr;
	int iptr;
	rt_llist_node_t *cur;

	memset(list_buf,0,20);

	pthread_rdwr_rlock_np(&(llistp->rwlock));

	for (cur=llistp->first; cur != NULL; cur=cur->nextp)
	{
//		if(cur->datap->label[0] != 0)
		if(1)
		{
//			printf("port: %2d\tonoff: %2d\t%s\n",(int)cur->datap->port, (int)cur->datap->onoff,cur->datap->label);
			sprintf(list_buf,":%2d %2d %02d:%02d:%02d",(int)cur->datap->port,\
			 (int)cur->datap->onoff,(int)cur->datap->hours, (int)cur->datap->minutes, (int)cur->datap->seconds);
//			 printf("%s\n\r",list_buf);
/*
			 ptr = list_buf;
			 iptr = 0;
			 do
			 {
			 	iptr++;
			 }while(*(ptr++) != 0);
*/
			write(fp,(char *)&list_buf[0],strlen(list_buf));
		}
	}
	pthread_rdwr_runlock_np(&(llistp->rwlock));
//	printf("done printfile\n\r");
	return 0;
}

