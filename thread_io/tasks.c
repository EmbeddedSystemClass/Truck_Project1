#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>
#include <sys/types.h>
#include <pthread.h>
#define closesocket close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include "mytypes.h"
#include "ioports.h"
#include "serial_io.h"
#include "queue/illist_threads_rw.h"
#include "queue/ollist_threads_rw.h"
#include "tasks.h"

extern int threads_ready_count;
extern pthread_cond_t		threads_ready;
pthread_mutex_t		tcp_write_lock=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t		tcp_read_lock=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t		io_mem_lock=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t		serial_lock=PTHREAD_MUTEX_INITIALIZER;
extern int   total_count;
illist_t ill;
ollist_t oll;
extern int init_tcp(char *);

UCHAR (*fptr[NUM_TASKS])(int) = { task1, task2, task3, task4, task5, task6};
//UCHAR (*fptr[NUM_TASKS])(int) = { task1, task2, task3};

int threads_ready_count=0;
pthread_cond_t    threads_ready=PTHREAD_COND_INITIALIZER;
pthread_mutex_t   threads_ready_lock=PTHREAD_MUTEX_INITIALIZER;
static UCHAR check_inputs(int i);
int comm_open = -1;
extern I_DATA *curr_i_array;
extern O_DATA *curr_o_array;

/*********************************************************************/
static void mydelay(unsigned long i)
{
	unsigned long j;
	struct timespec sleepTime;
	struct timespec rettime;
	sleepTime.tv_sec = 0;
	sleepTime.tv_nsec = 948540;
	for(j = 0;j < i;j++)
	{
		nanosleep(&sleepTime, &rettime);
	}
}

/*********************************************************************/
UCHAR task1(int test)
{
	I_DATA *pid;
	O_DATA *pod;
	I_DATA temp;
	O_DATA temp2;
	int rc = 0;
	UCHAR recv_buf[100];
	UCHAR cmd;
	UCHAR num_i_recs, num_o_recs;
	int i;
	int j = 0;

	illist_init(&ill);

	pid = curr_i_array;

	// just insert some sample data for now

	for(i = 0;i < NUM_PORT_BITS;i++)
	{
		illist_insert_data(i,&ill,pid);
		pid++;
	}

//	illist_show(&ill);

	ollist_init(&oll);
	// just insert some sample data for now
	pod = curr_o_array;
	for(i = 0;i < NUM_PORT_BITS;i++)
	{
		ollist_insert_data(i,&oll,pod);
		pod++;
	}
//	ollist_show(&oll);

//	printf("\n\n\n");
	UCHAR bank;
	UCHAR test2;
	UCHAR rec_no;
	printf("task 1: %d\n",test);

	while(TRUE)
	{
		cmd = 0;
		if(test_sock() == 1)
		{
			rc = recv_tcp(&cmd,1,1);	// blocking
			if(rc > 0)
			{
				switch(cmd)
				{
					// update a single IDATA record
					case SEND_IDATA:
						rc = recv_tcp((void *)&rec_no,1,1);
						rc = recv_tcp((void*)&temp,sizeof(I_DATA),1);	// blocking
						printf("rec_no: %d \n",rec_no);
						printf("%d\t%d\t%d\t%d\t%s\n",temp.port,temp.affected_output,temp.type,temp.inverse,temp.label);
						illist_insert_data(rec_no, &ill, &temp);
	//					illist_show(&ill);
						break;
					case SEND_ODATA:
					// update a single ODATA record
						rc = recv_tcp((void *)&rec_no,1,1);
						rc = recv_tcp((void*)&temp2,sizeof(O_DATA),1);	// blocking
						printf("rec_no: %d \n",rec_no);
						printf("%d\t%s\n",temp2.port,temp2.label);
						ollist_insert_data(rec_no, &oll, &temp2);
						ollist_show(&oll);
						break;
					case SEND_SHOW:		// F5
						illist_show(&ill);
						printf("\n\n");
						ollist_show(&oll);
						printf("\n\n\n");
						break;
					case SEND_SERIAL:	// F6
						SendByte(test2);
						if(++test2 > 0x7e)
							test2 = 0x21;
						break;
					case SEND_TEST3:	// F7
						printf("changing 1st 10 records in both input and output db\n");
						strcpy(temp.label,"testxyz");
						temp.port = 22;
						temp.type = 11;
						temp.inverse = 33;
						pid = &temp;
						for(i = 0;i < 10;i++)
						{
							illist_insert_data(i,&ill,pid);
							temp.port++;
							temp.type++;
							temp.inverse++;
						}
						strcpy(temp2.label,"asdf_xyz");
						temp2.port= 10;
						temp2.onoff = 11;
						pod = &temp2;
						for(i = 0;i < 10;i++)
						{
							ollist_insert_data(i, &oll,pod);
							temp2.port++;
							temp2.onoff++;
						}

						break;
					case SEND_TEST4:	// F8
						pid = curr_i_array;
						rc = recv_tcp((void *)&num_i_recs,1,1);
						printf("updating %d records\n",num_i_recs);
						memset(&temp,0,sizeof(I_DATA));
						for(i = 0;i < num_i_recs;i++)
						{
							printf("%d\t%d\t%d\t%d\t%s\n",pid->port,pid->affected_output,pid->type,pid->inverse,pid->label);
							memcpy(&temp,pid,sizeof(I_DATA));
							illist_insert_data(i, &ill, &temp);
							pid++;
						}
						printf("done inserting data\n");
						break;
						break;
					case SEND_TEST5:	// F9
						pod = curr_o_array;
						rc = recv_tcp((void *)&num_o_recs,1,1);
						printf("updating %d records\n",num_o_recs);
						memset(&temp2,0,sizeof(O_DATA));
						for(i = 0;i < num_i_recs;i++)
						{
							printf("%d\t%d\t%s\n",pod->port,pod->onoff,pod->label);
							memcpy(&temp2,pod,sizeof(O_DATA));
							ollist_insert_data(i, &oll, &temp2);
							pod++;
						}
						printf("done inserting data\n");
						break;
					case SEND_TEST6:

						for(bank = 0;bank < 6;bank++)
						{
							printf("%d %x\n",bank,check_inputs(bank));
						}
/*
						printf("a: %x\n",InPortByteA());
						printf("b: %x\n",InPortByteB());
						printf("c: %x\n",InPortByteC());
						printf("d: %x\n",InPortByteD());
						printf("e: %x\n",InPortByteE());
						printf("f: %x\n\n",InPortByteF());
*/
						break;
					case CLOSE_SOCKET:
						close_tcp();
						break;
					default:
						break;
				}	// end of switch
			}	// if rc > 0
		}else	// if test_sock
		{
//			printf("no connection %d\n",j);
			usleep(TIME_DELAY);
			j++;
		}
		if(test < 2)
		{
			printf("exiting task 1\n");
			return 0;
		}
	}
	return test + 1;
}

/*********************************************************************/
// if an input switch is changed, update the record for that switch
// and if an output applies to that input, change the output
// and record the event in llist
// TODO: add a function that reads the current state of the
// outputs to use to bit-or in the changed bit
UCHAR task2(int test)
{
	I_DATA *itp;
	I_DATA **itpp = &itp;

	O_DATA *otp;
	O_DATA **otpp = &otp;

	int status = -1;
	int bank, bank2, bit, actual_bit;
	int rem;
	int i;
	UCHAR mask;
	UCHAR result;
	UCHAR result2;
	int inc;
	int onoff;
	static UCHAR inportstatus[OUTPORTF_OFFSET-OUTPORTA_OFFSET+1];
	static UCHAR inportstatus2[OUTPORTF_OFFSET-OUTPORTA_OFFSET+1];

	for(bank = 0;bank < 6;bank++)
	{
		inportstatus[bank] = check_inputs(bank);
		inportstatus2[bank] = inportstatus[bank];
	}

	while(TRUE)
	{
		result = 0;
// if bank=OUTPORTC_OFFSET or bank=OUTPORTF_OFFSET then there are only 4 bits
		for(bank = 0;bank <= OUTPORTF_OFFSET-OUTPORTA_OFFSET;bank++)
		{
			inportstatus[bank] = check_inputs(bank);
			mask = 1;
			result = inportstatus[bank] ^ inportstatus2[bank];
			if(bank == OUTPORTF_OFFSET || bank == OUTPORTC_OFFSET)
				actual_bit = 4;
			else
				actual_bit = 8;
			for(bit = 0;bit < actual_bit;bit++)
			{
				result2 = result & mask;

				if(result2 != 0)
				{
					if(inportstatus[bank] & mask)
						onoff = 1;
					else
						onoff = 0;

					inc = (bank*8)+bit;	// inc is which input port has changed
					if(bank > OUTPORTC_OFFSET)
						inc -= 4;
//					printf("inc: %d bank: %d bit: %d actual_bit: %d\n",inc,bank,bit,actual_bit);
					illist_find_data(inc,itpp,&ill);
					printf("affected_output: %d\tport: %d\t%s\n",itp->affected_output,itp->port,itp->label);
					ollist_find_data(itp->affected_output,otpp,&oll);
					printf("output affected: %d\t%d\t%s\n",otp->port,otp->onoff,otp->label);

					otp->onoff = onoff;
					ollist_insert_data(otp->port,&oll,otp);
					bank2 = itp->affected_output/8;
					rem = itp->affected_output%8;
					mask = 1;
					for(i = 0;i < rem;i++)
						mask <<= 1;

					pthread_mutex_lock( &io_mem_lock);
					switch(bank2)
					{
						case 0:
						OutPortA(onoff, mask);
						break;
						case 1:
						OutPortB(onoff, mask);
						break;
						case 2:
						OutPortC(onoff, mask);
						break;
						case 3:
						OutPortD(onoff, mask);
						break;
						case 4:
						OutPortE(onoff, mask);
						break;
						case 5:
						OutPortF(onoff, mask);
						break;
						default:
						break;
					}
					pthread_mutex_unlock(&io_mem_lock);

					printf("bank:%d bank2:%d inc:%d mask:%x on/off:%d rem: %d status:%x\n\n",bank,bank2,inc,mask,onoff,rem,inportstatus[i]);
				}
				mask <<= 1;
			}
			inportstatus2[bank] = inportstatus[bank];
		}
		usleep(TIME_DELAY);
	}
	return test + 2;
}

/*********************************************************************/
UCHAR task4(int test)
{
	UCHAR test2 = 0x21;
	UCHAR test1;
	int rc;
	int i = 0;
	printf("task 2: %d\n",test);
	UCHAR mask = 1;

	while(TRUE)
	{
//#if 0
		if(test_sock() == 0)
		{
			usleep(TIME_DELAY);
		}else
		{

//			rc = recv_tcp(&test1,1,1);
//			printf("%c",test1);
			rc = send_tcp(&test2,1);
			usleep(TIME_DELAY);
			if(++test2 > 0x7e)
				test2 = 0x21;
		}
	}
//#endif
	return test + 4;
}

/*********************************************************************/
UCHAR task3(int test)
{
	printf("task 3: %d\n",test);
	UCHAR test2 = 0x20;

	while(TRUE)
	{
		usleep(TIME_DELAY);
		SendByte(test2);
		if(++test2 > 0x7e)
		{
//			SendByte(0x0a);
//			SendByte(0x0d);
			test2 = 0x21;
//			usleep(TIME_DELAY*10);
		}
	}
//	printf("%c",test1);
	return test + 3;
}

/*********************************************************************/
// check_inputs reads the input port and returns what the actual port
// is at the machine level
static UCHAR check_inputs(int i)
{
	UCHAR result;
	int flag = 0;
	pthread_mutex_lock( &io_mem_lock);

	switch(i)
	{
		case 0:
			result = InPortByteA();
			break;
		case 1:
			result = InPortByteB();
			break;
		case 2:
			result = InPortByteC();
			break;
		case 3:
			result = InPortByteD();
			break;
		case 4:
			result = InPortByteE();
			break;
		case 5:
			result = InPortByteF();
			break;
		default:
			result = 0;
			break;
	}
	pthread_mutex_unlock(&io_mem_lock);
	return result;
}

/*********************************************************************/
UCHAR task5(int test)
{
	unsigned char j;
	usleep(TIME_DELAY);

	while(TRUE)
	{
		usleep(TIME_DELAY);
	}

	if(test < 2)
	{
		printf("exiting task 5\n");
		return 0;
	}

	ReceiveByte();
	return test + 5;
}

// client calls 'connect' to get accept call below to stop
// blocking and return sd2 socket descriptor

#define PROTOPORT         5193						/* default protocol port number */
#define QLEN              6							/* size of request queue        */

void * serverthread(void * parm);					//* thread function prototype    */

pthread_mutex_t  mut;
static int visits =  0;								  /* counts client connections     */

static struct  sockaddr_in sad;  /* structure to hold server's address  */
static struct timeval tv;
static int sock_open;
static int global_socket;

/*********************************************************************/
UCHAR task6(int test)
{
	int ret = -1;

	struct	hostent   *ptrh;				/* pointer to a host table entry */
	struct	protoent  *ptrp;				/* pointer to a protocol table entry */
	struct	sockaddr_in sad;				/* structure to hold server's address */
	struct	sockaddr_in cad;				/* structure to hold client's address */
	int		listen_sd;	 					/* socket descriptors */
	int		port;							/* protocol port number */
	int		alen;							/* length of address */
	port = PROTOPORT;
	sock_open = 0;

	memset((char  *)&sad,0,sizeof(sad));	/* clear sockaddr structure   */
	sad.sin_family = AF_INET;				/* set family to Internet     */
	sad.sin_addr.s_addr = INADDR_ANY;		/* set the local IP address */

	sad.sin_port = htons((u_short)port);

#ifdef NOTARGET
	if ( ((int)(ptrp = getprotobyname("tcp"))) == 0)
	{
		fprintf(stderr, "cannot map \"tcp\" to protocol number");
		exit (1);
	}
	listen_sd = socket (PF_INET, SOCK_STREAM, ptrp->p_proto);

// getprotobyname doesn't work on TS-7200 because there's no /etc/protocols file
// so just use '6' as the tcp protocol number
#else
	listen_sd = socket (PF_INET, SOCK_STREAM, 6);
#endif
	if (listen_sd < 0)
	{
		fprintf(stderr, "socket creation failed\n");
		exit(1);
	}

/* Bind a local address to the socket */
	if (bind(listen_sd, (struct sockaddr *)&sad, sizeof (sad)) < 0)
	{
		fprintf(stderr,"bind failed\n");
		exit(1);
	}

/* Specify a size of request queue */
	if (listen(listen_sd, QLEN) < 0)
	{
		fprintf(stderr,"listen failed\n");
		exit(1);
	}

	alen = sizeof(cad);

/* Main server loop - accept and handle requests */
	fprintf( stderr, "Server up and running.\n");
	while (TRUE)
	{
		if(sock_open == 1)
			usleep(TIME_DELAY);
		else
		{
			printf("SERVER: Waiting for contact ...\n");

			if (  (global_socket=accept(listen_sd, (struct sockaddr *)&cad, &alen)) < 0)
			{
				fprintf(stderr, "accept failed\n");
				exit (1);
			}
			sock_open = 1;
			printf("socket: %d\n",global_socket);
			if (setsockopt (global_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval)) < 0)
				return -2;
			if (setsockopt (global_socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(struct timeval)) < 0)
				return -3;
		}
	}
	close(listen_sd);
	printf("closing listen_sd\n");
	if(test < 2)
	{
		printf("exiting task 6\n");
		return 0;
	}
	return test + 6;
}

/*********************************************************************/
int test_sock(void)
{
	return sock_open;
}

/*********************************************************************/
int send_tcp(UCHAR *str,int len)
{
	int ret = 0;
	char errmsg[60];
	memset(errmsg,0,60);
	pthread_mutex_lock( &tcp_write_lock);
	ret = put_sock(str,len,1,errmsg);
	pthread_mutex_unlock(&tcp_write_lock);
	if(ret < 0 && (strcmp(errmsg,"Success") != 0))
	{
		printf("send_tcp: %s\n",errmsg);
	}
	return ret;
}

/*********************************************************************/
int recv_tcp(UCHAR *str, int strlen,int block)
{
	int ret = 0;
	char errmsg[60];
	memset(errmsg,0,60);
	if(test_sock())
	{
		pthread_mutex_lock( &tcp_read_lock);
		ret = get_sock(str,strlen,block,errmsg);
		pthread_mutex_unlock(&tcp_read_lock);
		if(ret < 0 && (strcmp(errmsg,"Success") != 0))
		{
			printf("recv_tcp: %s\n",errmsg);
		}
	}
	else
	{
		strcpy(errmsg,"sock closed");
		ret = -1;
	}
	return ret;
}

/*********************************************************************/
int put_sock(UCHAR *buf,int buflen, int block, char *errmsg)
{
	int rc;
	char extra_msg[10];
	if(block)
		rc = send(global_socket,buf,buflen,MSG_WAITALL);	// block
	else
		rc = send(global_socket,buf,buflen,MSG_DONTWAIT);	// don't block
	if(rc < 0 && errno != 11)
	{
		strcpy(errmsg,strerror(errno));
		sprintf(extra_msg," %d",errno);
		strcat(errmsg,extra_msg);
		strcat(errmsg," put_sock");
	}else strcpy(errmsg,"Success\0");
	return rc;
}

/*********************************************************************/
int get_sock(UCHAR *buf, int buflen, int block, char *errmsg)
{
	int rc;
	char extra_msg[10];
	if(block)
		rc = recv(global_socket,buf,buflen,MSG_WAITALL);
	else
		rc = recv(global_socket,buf,buflen,MSG_DONTWAIT);
	if(rc < 0 && errno != 11)
	{
		strcpy(errmsg,strerror(errno));
		sprintf(extra_msg," %d",errno);
		strcat(errmsg,extra_msg);
		strcat(errmsg," get_sock");
	}else strcpy(errmsg,"Success\0");
	return rc;
}

/*********************************************************************/
void close_tcp(void)
{
	printf("\nclosing socket\n");
//	send_tcp("close",5);
	sock_open = 0;
	close(global_socket);
}

/*********************************************************************/
void SendByte(unsigned char byte)
{
	int cport_nr = 1;
	if(comm_open == -1)
		return;
	pthread_mutex_lock( &serial_lock);
	write_serial(byte);
	pthread_mutex_unlock(&serial_lock);
}

/*********************************************************************/
void ReceiveByte(void)
{
	int cport_nr = 1;
	int n,i;
	unsigned char buf[100];
	if(comm_open == -1)
		return;
	pthread_mutex_lock( &serial_lock);
//	n = RS232_PollComport(cport_nr, buf, 100);
	n = read_serial(&buf[0]);
	if (n > 0)
	{
		buf[n] = 0;
		for(i = 0;i < n;i++)
		{
			if(buf[i] < 0x20 && buf[i] > 0x7e)
				buf[i] = '.';
		}
	}
//	printf("rec'd %i bytes: %s %d\n",n,(char *)buf,n);
	printf("%s\n",(char *)buf);
	pthread_mutex_unlock(&serial_lock);
}

/**********************************************************************/
void *work_routine(void *arg)
{
	int *my_id=(int *)arg;
	int i;
	UCHAR pattern = 0;
	int not_done=30;
	i = not_done;
	printf("\nwork_routine()\tthread %d\tI'm Alive\n", *my_id);

	pthread_mutex_lock(&threads_ready_lock);
	threads_ready_count++;
	if (threads_ready_count == NUM_TASKS)
	{
		/* I was the last thread to become ready.  Tell the rest. */
		pthread_cond_broadcast(&threads_ready);
	}
	else
	{
		/* At least one thread isn't ready.  Wait. */
		while (threads_ready_count != NUM_TASKS)
		{
			pthread_cond_wait(&threads_ready, &threads_ready_lock);
		}
	}
	pthread_mutex_unlock(&threads_ready_lock);
	printf("\nwork_routine()\tthread %d\tI'm Proceeding\n", *my_id);

	while(not_done)
	{
//			pthread_mutex_lock( &io_mem_lock);
//			test_mem(pattern++);
//			pthread_mutex_unlock(&io_mem_lock);
//		}

		(*fptr[*my_id])(i);
		i--;
		not_done--;
//		pthread_mutex_lock(&total_count_lock);
//		if (total_count < TOTAL_END_COUNT)
//			total_count+=LOOP_COUNT;
//		else
//			not_done=0;
//		printf("\nwork_routine()\tthread %d\tmy count %d, total_count %d\n", *my_id, thread_counts[*my_id], total_count);

//		pthread_mutex_unlock(&total_count_lock);

	}
//	printf("\nworkroutine()\tthread %d\tI'm done\n", *my_id);
	return(NULL);
	close_tcp();
}
#if 0
// test
/*********************************************************************/
UCHAR task3(int test)
{
	printf("task 3: %d\n",test);
	UCHAR test2 = 0x20;
	int bank = 0;
	UCHAR onoff = 0;
	int mask;
	UCHAR mask2;

	while(TRUE)
	{
		usleep(TIME_DELAY*10);
		printf("bank: %d mask2: %d mask: %d onoff: %d\n",bank,mask2,mask,onoff);
		switch(bank)
		{
			case 0:
				OutPortA(onoff,mask2);
				break;
			case 1:
				OutPortB(onoff,mask2);
				break;
			case 2:
				OutPortC(onoff,mask2);
				break;
			case 3:
				OutPortD(onoff,mask2);
				break;
			case 4:
				OutPortE(onoff,mask2);
				break;
			case 5:
				OutPortF(onoff,mask2);
				break;
		}
		if(++mask > 8)
		{
			if(++bank > 5)
				bank = 0;
			mask = 1;
		}
		mask2 = (UCHAR)mask;

		if(test < 2)
		{
			printf("exiting task 2\n");
			return 0;
		}
		onoff = ~onoff;
		onoff &= 1;
//		SendByte(test2);
		if(++test2 > 0x7e)
			test2 = 0x21;
	}
//	printf("%c",test1);
	return test + 3;
}
#endif
