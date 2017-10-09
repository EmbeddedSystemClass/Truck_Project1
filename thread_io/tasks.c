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
#include <dirent.h>
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
pthread_mutex_t		serial_write_lock=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t		serial_read_lock=PTHREAD_MUTEX_INITIALIZER;
extern int   total_count;
illist_t ill;
ollist_t oll;

UCHAR (*fptr[NUM_TASKS])(int) = { task1, task2, task3, task4, task5, task6};
//UCHAR (*fptr[NUM_TASKS])(int) = { task1, task2, task3};
static int same_msg;
int threads_ready_count=0;
pthread_cond_t    threads_ready=PTHREAD_COND_INITIALIZER;
pthread_mutex_t   threads_ready_lock=PTHREAD_MUTEX_INITIALIZER;
static UCHAR check_inputs(int i);
int comm_open = -1;
extern I_DATA *curr_i_array;
extern O_DATA *curr_o_array;
extern int iLoadConfig(char *filename, I_DATA *curr_i_array,size_t size, char *errmsg);
extern int oLoadConfig(char *filename, O_DATA *curr_o_array,size_t size, char *errmsg);

#define SERIAL_BUFF_SIZE 100

static UCHAR serial_buff[SERIAL_BUFF_SIZE];
static int no_serial_buff;

static int put_sock(UCHAR *buf,int buflen, int block, char *errmsg);
static int get_sock(UCHAR *buf, int buflen, int block, char *errmsg);
static int serial_rec;
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
	I_DATA **ppid;
	O_DATA **ppod;
	I_DATA tempi1, tempi2;
	O_DATA tempo1, tempo2;
	int rc = 0;
	UCHAR cmd;
	UCHAR num_i_recs, num_o_recs;
	char errmsg[50];
	char filename[15];
	char *fptr1;
	size_t size;
	int i;
	int j = 0;
	DIR *d;
	struct dirent *dir;
	char tempx[30];
	int num;
	char *chp;


	d = opendir( "." );
	if( d == NULL )
	{
		return -1;
	}
	i = 0;

/* this compiles for the embedded linux card but will it run?
	while(dir = readdir(d))
	{
		if(strcmp( dir->d_name, "." ) == 0 ||  strcmp( dir->d_name, ".." ) == 0 || dir->d_type == DT_DIR)
		  continue;

		memset(tempx,0,sizeof(tempx));
		strcpy(tempx,dir->d_name);
//		format = GetFileFormat(tempx);

		chp = tempx;
		j = 0;

		while(*chp++ != '.' && j < 30)
			j++;

		strncpy(tempx,chp,j+1);

//		mvprintw(LINES - 21-i++, 5,"%d  %s  ",j,tempx);
	    if(dir->d_type == DT_REG && strcmp(tempx,"dat") == 0 )
	    {
//	    	if(GetFileFormat(dir->d_name) == which)
//				strcpy(dat_names[num++],dir->d_name);
//			mvprintw(LINES - 21-i++, 0,"%s  %s  %d",dir->d_name,dat_names[k-1],j);
		}
	}
	closedir( d );
*/

	illist_init(&ill);
	same_msg = 0;
	pid = curr_i_array;

	// just insert some sample data for now

	for(i = 0;i < NUM_PORT_BITS;i++)
	{
		illist_insert_data(i,&ill,pid);
		pid++;
	}
	pid = curr_i_array;

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
	pod = curr_o_array;

	printf("\n\n\n");
	UCHAR bank;
	UCHAR test2;
	UCHAR rec_no;
	printf("task 1: %d\n",test);

	printf("sizeof O_DATA: %d\n",sizeof(O_DATA));

	while(TRUE)
	{
		cmd = 0;
		if(test_sock() == 1)
//		if(1)
		{
			rc = recv_tcp(&cmd,1,1);	// blocking
			if(rc > 0)
			{
				rc = 0;
				switch(cmd)
				{
					// update a single IDATA record
					case SEND_IDATA:
						rc += recv_tcp((UCHAR *)&rec_no,1,1);
						rc += recv_tcp((UCHAR *)&tempi1,sizeof(I_DATA),1);	// blocking
						printf("send idata: rec: %d rc: %d\n",rec_no,rc);
//						printf("%d\t%d\t%d\t%d\t%s\n\n",tempi1.port,
//							tempi1.affected_output,tempi1.type,tempi1.inverse,tempi1.label);
						illist_insert_data(rec_no, &ill, &tempi1);
	//					illist_show(&ill);
						break;

					case SEND_ODATA:
					// update a single ODATA record
						rc += recv_tcp((UCHAR *)&rec_no,1,1);
						rc += recv_tcp((UCHAR *)&tempo1,sizeof(O_DATA),1);	// blocking
						printf("send odata: rec: %d rc: %d\n",rec_no,rc);
//						printf("port: %d\tonoff: %d\tlabel: %s\n",tempo1.port,tempo1.onoff,tempo1.label);
						ollist_insert_data(rec_no, &oll, &tempo1);
//						ollist_show(&oll);
						break;

					case SEND_SHOW:		// F5 	(display data)
						printf("showing current list\n");
						illist_show(&ill);
						printf("\n");
						ollist_show(&oll);
						printf("\n");
						break;

					case SEND_ALL_IDATA:	// F9
						printf("send all IDATA:\n");
						rc = 0;
						pid = &tempi1;
						for(i = 0;i < NUM_PORT_BITS;i++)
						{
							rc += recv_tcp((UCHAR *)pid,sizeof(I_DATA),1);
							illist_insert_data(i,&ill,pid);
						}
						pid = curr_i_array;
						for(i = 0;i < NUM_PORT_BITS;i++)
						{
							printf("%2d\t%2d\t%2d\t%2d\t%s\n",
								pid->port,pid->affected_output,pid->type,pid->inverse,pid->label);
							pid++;
						}

						printf("%d\n",rc);
						printf("done\n");
						break;

					case SEND_ALL_ODATA:	// F10
						printf("send all ODATA: %d\n",sizeof(O_DATA));
						pod = &tempo1;
						rc = 0;
						for(i = 0;i < NUM_PORT_BITS;i++)
						{
							rc += recv_tcp((UCHAR *)pod,sizeof(O_DATA),1);
							printf("%2d\t%2d\t%s\t%d\n",pod->port,pod->onoff,pod->label,rc);
//							ollist_insert_data(i,&oll,pod);
						}

/*
						pod = curr_o_array;

						for(i = 0;i < NUM_PORT_BITS;i++)
						{
							printf("%2d\t%2d\t%s\n",pod->port,pod->onoff,pod->label);
							pod++;
						}
*/
						printf("%d\n",rc);
						printf("done\n");
						break;

					case SEND_SERIAL:	// F6
						test2 = 0x21;
						for(i = 0;i < 3000;i++)
						{
							SendByte(test2);
							if(++test2 > 0x7e)
								test2 = 0x21;
						}
						printf("sent serial\n");
						break;

					case LOAD_ORG:	// F7
						printf("loading from org.dat\n");
						illist_removeall_data(&ill);
						ollist_removeall_data(&oll);
						illist_init(&ill);
						ollist_init(&oll);

						fptr1 = filename;
						strcpy(filename,"iorg.dat\0");
						size = sizeof(I_DATA) * NUM_PORT_BITS;
						rc = iLoadConfig(fptr1,curr_i_array,size,errmsg);
						pid = curr_i_array;
						for(i = 0;i < NUM_PORT_BITS;i++)
						{
							illist_insert_data(i,&ill,pid);
							pid++;
						}
						pid = curr_i_array;

						strcpy(filename,"oorg.dat\0");
						size = sizeof(O_DATA) * NUM_PORT_BITS;
						rc = oLoadConfig(fptr1,curr_o_array,size,errmsg);
						pod = curr_o_array;
						for(i = 0;i < NUM_PORT_BITS;i++)
						{
							ollist_insert_data(i,&oll,pod);
							pod++;
						}
						pod = curr_o_array;
						printf("done\n");
						break;

					case RESTORE:	// F8	(restore)
						printf("restored from current data\n");
						pid = curr_i_array;
						memset(&tempi1,0,sizeof(I_DATA));
						memset(&tempo1,0,sizeof(O_DATA));
						for(i = 0;i < NUM_PORT_BITS;i++)
						{
							printf("%2d\t%2d\t%2d\t%2d\t%s\n",pid->port,pid->affected_output,
								pid->type,pid->inverse,pid->label);
							memcpy(&tempi1,pid,sizeof(I_DATA));
							illist_insert_data(i, &ill, &tempi1);
							pid++;
						}
						pod = curr_o_array;
						for(i = 0;i < NUM_PORT_BITS;i++)
						{
							printf("%s\t%2d\t%2d\n",pod->label,pod->port,pod->onoff);
							memcpy(&tempo1,pod,sizeof(O_DATA));
							ollist_insert_data(i, &oll, &tempo1);
							pod++;
						}
						printf("done\n");
						break;

					case SAVE_TO_DISK:
						printf("saving to disk as i/oSave.dat\n");
						strcpy(filename,"iSave.dat\0");
						pid = curr_i_array;
						size = sizeof(I_DATA) * NUM_PORT_BITS;
						iWriteConfig(filename, curr_i_array, size,errmsg);

						strcpy(filename,"oSave.dat\0");
						pod = curr_o_array;
						size = sizeof(O_DATA) * NUM_PORT_BITS;
						oWriteConfig(filename, curr_o_array, size,errmsg);
						printf("done\n");
						break;

					case CLEAR_SCREEN:
						for(i = 0;i < 150;i++)
							printf("\n");
						break;

					case CLOSE_SOCKET:
						close_tcp();
						break;

					case EXIT_PROGRAM:
						close_tcp();
						printf("exiting program\n");
						free(curr_i_array);
						free(curr_o_array);

						exit(0);
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
// this just sends a series of ascii chars to the tcp out port (is displayed in tcp_win when opened)
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
		rc = send_tcp(&test2,1);
		usleep(TIME_DELAY/2);
		if(++test2 > 0x70)
			test2 = 0x30;
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
		usleep(TIME_DELAY*2);
	}
#if 0
		SendByte(test2);
		if(++test2 > 0x7e)
		{
//			SendByte(0x0a);
//			SendByte(0x0d);
			test2 = 0x21;
//			usleep(TIME_DELAY*10);
		}
#endif
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
	serial_rec = 0;
	int i;
	memset(serial_buff,0,SERIAL_BUFF_SIZE);
	no_serial_buff = 0;

	while(TRUE)
	{

		usleep(TIME_DELAY);
		if(test < 2)
		{
			printf("exiting task 5\n");
			return 0;
		}

		if(comm_open == -1)
			return -1;

		if(no_serial_buff < SERIAL_BUFF_SIZE)
		{
			pthread_mutex_lock( &serial_read_lock);
		//	printf("rec'd %i bytes: %s %d\n",n,(char *)buf,n);
		//	printf("%s\n",(char *)buf);
			serial_buff[no_serial_buff] = read_serial();
			pthread_mutex_unlock(&serial_read_lock);
			no_serial_buff++;
		}
		else{
			printf("serial buff full\n\n");
			for(i = 0;i < SERIAL_BUFF_SIZE;i++)
			{
				printf("%c",serial_buff[i]);
				if((i % 93) == 0)
					printf("\n");
			}
			no_serial_buff = 0;
			printf("\n\n");
		}
	}
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

// getprotobyname doesn't work on TS-7200 because there's no /etc/protocols file
// so just use '6'
#ifdef NOTARGET
	if ( ((int)(ptrp = getprotobyname("tcp"))) == 0)
	{
		fprintf(stderr, "cannot map \"tcp\" to protocol number\n");
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
	ret = put_sock(str,len,1,&errmsg[0]);
	pthread_mutex_unlock(&tcp_write_lock);
	if(ret < 0 && (strcmp(errmsg,"Success") != 0))
	{
		if(same_msg == 0)
			printf("send_tcp: %s\n",errmsg);
		same_msg = 1;
	}
	else same_msg = 0;
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
		ret = get_sock(str,strlen,block,&errmsg[0]);
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
static int put_sock(UCHAR *buf,int buflen, int block, char *errmsg)
{
	int rc = 0;
	char extra_msg[10];
	if(test_sock())
	{
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
	}
	else
	{
// this keeps printing out until the client logs on
		strcpy(errmsg,"sock closed");
		rc = -1;
	}

	return rc;
}

/*********************************************************************/
static int get_sock(UCHAR *buf, int buflen, int block, char *errmsg)
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
	pthread_mutex_lock( &serial_write_lock);
	write_serial(byte);
	pthread_mutex_unlock(&serial_write_lock);
}

/*********************************************************************/
UCHAR ReceiveByte(void)
{
	int cport_nr = 1;
	int n,i;
//	unsigned char buf[100];
	UCHAR rec;
	if(comm_open == -1)
		return -1;

	if(no_serial_buff < SERIAL_BUFF_SIZE)
	{
		pthread_mutex_lock( &serial_read_lock);
	//	printf("rec'd %i bytes: %s %d\n",n,(char *)buf,n);
	//	printf("%s\n",(char *)buf);
		serial_buff[no_serial_buff] = read_serial();
		pthread_mutex_unlock(&serial_read_lock);
		no_serial_buff++;
	}
//	printf("%c",rec);
	return rec;
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


						printf("changing 1st 10 records in both input and output db\n");
						strcpy(tempi1.label,"testxyz");
						tempi1.port = 22;
						tempi1.type = 11;
						tempi1.inverse = 33;
//						tempi1.affected = 44;
						pid = &tempi1;
						for(i = 0;i < 10;i++)
						{
							illist_insert_data(i,&ill,pid);
							tempi1.port++;
//							tempi1.affected++;
							tempi1.type++;
							tempi1.inverse++;
						}
						strcpy(tempo1.label,"asdf_xyz");
						tempo1.port= 10;
						tempo1.onoff = 11;
						pod = &tempo1;
						for(i = 0;i < 10;i++)
						{
							ollist_insert_data(i, &oll,pod);
							tempo1.port++;
							tempo1.onoff++;
						}


#if 0
						pid = &tempi1;
						ppid = &pid;
//						rc = illist_find_data(2,ppid,&ill);
//						printf("current: %s %d %d\n",tempi1.label,tempi1.port,rc);
//						printf("current: %s %d %d\n",pid->label,pid->port,rc);
// don't need to remove - insert will replace the record
//						illist_remove_data(2,ppid,&ill);


						printf("changing record 2\n");
						strcpy(tempi1.label,"changed I");
						tempi1.port = 22;
						tempi1.type = 11;
//						pid->port = 22;
//						pid->type = 11;
//						pid->inverse = 33;
						pid = &tempi1;
						ppid = &pid;
						printf("current: %s %d\n",pid->label,pid->port);
//						printf("%x %x %x\n",&tempi1,pid,*ppid);
						illist_insert_data(2,&ill,pid);
#endif
//						rc = illist_change_data(2,ppid,&ill);
//						printf("changed: %s %d\n",tempi1.label,rc);

/*
						strcpy(tempo1.label,"changed O");
						tempo1.port = 100;
						tempo1.onoff = 1;

						pod = &tempo1;

						for(bank = 0;bank < 6;bank++)
						{
							printf("%d %x\n",bank,check_inputs(bank));
						}
						printf("a: %x\n",InPortByteA());
						printf("b: %x\n",InPortByteB());
						printf("c: %x\n",InPortByteC());
						printf("d: %x\n",InPortByteD());
						printf("e: %x\n",InPortByteE());
						printf("f: %x\n\n",InPortByteF());
*/


#endif
