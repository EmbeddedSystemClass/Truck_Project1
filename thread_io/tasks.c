#if 1
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sched.h>
#include <sys/types.h>
#include <pthread.h>
#define closesocket close
#include <sys/types.h>
#include <sys/stat.h>
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
#include "ncurses/config_file.h"
#include "lcd_func.h"

extern int threads_ready_count;
extern pthread_cond_t		threads_ready;
pthread_mutex_t		tcp_write_lock=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t		tcp_read_lock=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t		io_mem_lock=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t		serial_write_lock=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t		serial_read_lock=PTHREAD_MUTEX_INITIALIZER;
extern int   total_count;

UCHAR (*fptr[NUM_TASKS])(int) = { task1, task2, task3, task4, task5, task6};
//UCHAR (*fptr[NUM_TASKS])(int) = { task1, task2, task3};
static int same_msg;
int threads_ready_count=0;
pthread_cond_t    threads_ready=PTHREAD_COND_INITIALIZER;
pthread_mutex_t   threads_ready_lock=PTHREAD_MUTEX_INITIALIZER;
static UCHAR check_inputs(int index, int test);
int comm_open = -1;

illist_t ill;
ollist_t oll;

extern char oFileName[20];
extern char iFileName[20];

extern int ilLoadConfig(char *filename, illist_t *ill ,size_t size, char *errmsg);
extern int olLoadConfig(char *filename, ollist_t *oll, size_t size, char *errmsg);

#define SERIAL_BUFF_SIZE  93

static UCHAR serial_buff[SERIAL_BUFF_SIZE];
static int no_serial_buff;

static char dat_names[NUM_DAT_NAMES][DAT_NAME_STR_LEN];

static int put_sock(UCHAR *buf,int buflen, int block, char *errmsg);
static int get_sock(UCHAR *buf, int buflen, int block, char *errmsg);
static int serial_rec;
static int change_output(int index, int onoff);
static void print_inputstatus(int index);

static UCHAR inportstatus[OUTPORTF_OFFSET-OUTPORTA_OFFSET+1];
static int mask2int(UCHAR mask);

#define ON 1
#define OFF 0

typedef struct
{
	int bank;
	int index;
}REAL_BANKS;

static REAL_BANKS real_banks[40];


/*********************************************************************/
static int mask2int(UCHAR mask)
{
	int i = 0;
	do
	{
		mask >>= 1;
		i++;
	}while(mask);
	return i - 1;
}
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
#endif
/*********************************************************************/
// task to get commands from the host
UCHAR task1(int test)
{
	I_DATA tempi1;
	O_DATA tempo1;
	I_DATA *itp;
	O_DATA *otp;
	int rc = 0;
	UCHAR cmd = 0x21;
	UCHAR onoff;
	char errmsg[50];
	char filename[15];
	size_t size;
	int i;
	int j = 0;
	size_t isize;
	size_t osize;
	UCHAR bank;
	UCHAR test2;
	UCHAR rec_no;
	struct dirent **namelist;
	DIR *d;
	struct dirent *dir;
	int num;
	char tempx[40];
	char *chp;
	int fname_index;
	UCHAR mask;
	char buf[300];
	static buf_ptr;

	// the check_inputs & change_outputs functions
	// use the array to adjust from index to bank
	// since there are only 4 bits in banks 3 & 5
	for(i = 0;i < 20;i++)
	{
		real_banks[i].bank = i/8;
		real_banks[i].index = i - real_banks[i].bank*8;
	}

	for(i = 20;i < 40;i++)
	{
		real_banks[i].bank = (i+4)/8;
		real_banks[i].index = i - (real_banks[i].bank*8)+4;
	}

	memset(dat_names,0,sizeof(dat_names));

	i = NUM_PORT_BITS;
	isize = sizeof(I_DATA);
	isize *= i;

	i = NUM_PORT_BITS;
	osize = sizeof(O_DATA);
	osize *= i;

	lcd_init();
	buf_ptr = 0;

	illist_init(&ill);
	if(access(iFileName,F_OK) != -1)
	{
		rc = ilLoadConfig(iFileName,&ill,isize,errmsg);
		if(rc > 0)
		{
//			printf("%s\n",errmsg);
			myprintf1(errmsg);
//			return 1;
		}
	}else		// oh-boy! create a new file!
	{

	}
//	illist_show(&ill);

	ollist_init(&oll);
	if(access(oFileName,F_OK) != -1)
	{
		rc = olLoadConfig(oFileName,&oll,osize,errmsg);
		if(rc > 0)
		{
			myprintf1(errmsg);
		}
	}else		// oh-boy! create a new file!
	{

	}
//	ollist_show(&oll);

	same_msg = 0;

	myprintf1("task 1: v1.01 \0");

//	printf("sizeof O_DATA: %d\n",sizeof(O_DATA));
//	printf("sizeof I_DATA: %d\n",sizeof(I_DATA));

	test2 = 0x21;
	for(i = 0;i < 300;i++)
	{
		buf[i] = test2;
		if(++test2 > 0x7e)
			test2 = 0x21;
	}
	for(i = 0;i < 300;i++)
	{
		if((i % 9) == 0)
			buf[i] = 0;
	}

	while(TRUE)
	{
		cmd = 0;
		if(test_sock() == 1)
//		if(1)
		{
			rc = recv_tcp(&cmd,1,1);	// blocking
//			printf("\ncmd: %x\n",cmd);
			if(rc > 0)
			{
				rc = 0;
				switch(cmd)
				{
					// update a single IDATA record
					case SHOW_DATA:
						myprintf1("I_DATA: \0");
//						illist_show(&ill);
						strcpy(buf,"O_DATA:\0");
						myprintf1(buf);
//						ollist_show(&oll);
//						close_tcp();
						break;

					case SEND_IDATA:
						rc += recv_tcp((UCHAR *)&rec_no,1,1);
						rc += recv_tcp((UCHAR *)&tempi1,sizeof(I_DATA),1);	// blocking
						myprintf3("send idata: " ,rec_no,rc);
						illist_insert_data(rec_no, &ill, &tempi1);
//						illist_show(&ill);
						break;

					case SEND_ODATA:
					// update a single ODATA record
						rc += recv_tcp((UCHAR *)&rec_no,1,1);
						rc += recv_tcp((UCHAR *)&tempo1,sizeof(O_DATA),1);	// blocking
						printf("send odata: rec: %d rc: %d\n",rec_no,rc);
//						printf("port: %d\tonoff: %d\tlabel: %s\n",tempo1.port,tempo1.onoff,tempo1.label);
						ollist_insert_data(rec_no, &oll, &tempo1);
//						memcpy(pod,&tempo1,sizeof(O_DATA));
//						ollist_show(&oll);
						break;

					case SEND_ALL_IDATA:
						printf("send all IDATA:\n");
						rc = 0;
						itp = &tempi1;
						for(i = 0;i < NUM_PORT_BITS;i++)
						{
							rc += recv_tcp((UCHAR *)itp,sizeof(I_DATA),1);
							printf("%2d\t%2d\t%s\n",itp->port,itp->affected_output,itp->label);
							illist_insert_data(i,&ill,itp);
						}
						printf("%d\n",rc);
						printf("done\n");
//						close_tcp();
						break;

					case SEND_ALL_ODATA:
						printf("send all ODATA: ");
						rc = 0;
						otp = &tempo1;
						for(i = 0;i < NUM_PORT_BITS;i++)
						{
							rc += recv_tcp((UCHAR *)otp,sizeof(O_DATA),1);
							printf("%2d\t%2d\t%s\n",otp->port,otp->onoff,otp->label);
							ollist_insert_data(i,&oll,otp);
						}
						printf("%d\n",rc);
						printf("done\n");
//						close_tcp();
						break;

					case RECV_ALL_IDATA:
						printf("recv all IDATA: ");
						rc = 0;
//						itp = &tempi1;
						recv_tcp((UCHAR*)&fname_index,1,1);
						printf("index: %d\n",fname_index);
						if(fname_index > 0)
						{
							fname_index--;
							ilLoadConfig(dat_names[fname_index],&ill,isize,errmsg);
						}
						for(i = 0;i < NUM_PORT_BITS;i++)
						{
							illist_find_data(i,&itp,&ill);
							rc += send_tcp((UCHAR *)itp,sizeof(I_DATA));
//							printf("%2d\t%2d\t%s\trc: %2d\n",tempi1.port,tempi1.affected_output,tempi1.label,rc);
//							printf("%2d\t%2d\t%s\n",itp->port,itp->affected_output,itp->label);
						}
						printf("%d\n",rc);
						printf("done\n");
//						close_tcp();
						break;

					case RECV_ALL_ODATA:
						printf("recv all ODATA: ");
						rc = 0;
//						otp = &tempo1;
						recv_tcp((UCHAR*)&fname_index,1,1);
						printf("index: %d\n",fname_index);
						if(fname_index > 0)
						{
							fname_index--;
							olLoadConfig(dat_names[fname_index],&oll,isize,errmsg);
						}
						for(i = 0;i < NUM_PORT_BITS;i++)
						{
							ollist_find_data(i,&otp,&oll);
							rc += send_tcp((UCHAR *)otp,sizeof(O_DATA));
/*
							printf("%2d\t%2d\t%2d\t%2d\t%2d\t%s\n",
								otp->port,otp->onoff,otp->type,otp->time_delay,
									otp->pulse_time,otp->label);
*/
						}
						printf("%d\n",rc);
						printf("done\n");
//						close_tcp();
						break;

					case SEND_SERIAL:
						test2 = 0x21;
//#if 0
						for(i = 0;i < 1000;i++)
						{
							pthread_mutex_lock( &serial_write_lock);
							write_serial(test2);
							pthread_mutex_unlock(&serial_write_lock);
							usleep(TIME_DELAY/200);
							if(++test2 > 0x7e)
								test2 = 0x21;
						}
//#endif
//						printf("sent serial\n");
						break;

					case GET_DIR:
//						printf("GET_DIR\n");
						d = opendir( "." );
						if( d == NULL )
						{
							printf("bad OPEN_DIR\n");
//							return -1;
						}
						num = 0;

						memset(dat_names,0,NUM_DAT_NAMES*DAT_NAME_STR_LEN);
						memset(tempx,0,sizeof(tempx));

						while( ( dir = readdir( d ) ) && num < NUM_DAT_NAMES-1)
						{
							if(strcmp( dir->d_name, "." ) == 0 ||  \
								strcmp( dir->d_name, ".." ) == 0 || dir->d_type == DT_DIR)
							  continue;

							memset(tempx,0,sizeof(tempx));
							strcpy(tempx,dir->d_name);

							chp = tempx;
							j = 0;

							while(*chp++ != '.' && j < DAT_NAME_STR_LEN)
								j++;

							strncpy(tempx,chp,j+1);

							if(dir->d_type == DT_REG && strcmp(tempx,"dat") == 0 )
//							if(dir->d_type == DT_REG)
							{
								strcpy(dat_names[num++],dir->d_name);
//								printf("%s\n",dir->d_name);
							}
						}
						closedir( d );
//#if 0
						printf("number of dat files: %d\n",num);
						for(i = 0;i < num;i++)
						{
							printf("%s\n",dat_names[i]);
						}
//#endif
//						memset(tempx,0x20,sizeof(tempx));
						send_tcp((UCHAR*)&num,1);
						for(i = 0;i < num;i++)
						{
							cmd = (UCHAR)strlen(dat_names[i]);
//							printf("%d %s\n",cmd,dat_names[i]);
							send_tcp(&cmd,1);
							send_tcp((UCHAR *)&dat_names[i],cmd);
						}
						for(i = 0;i < num;i++)
						{
							getFileCreationTime(dat_names[i],tempx);
							j = GetFileFormat(dat_names[i]);
							if(j < 0)
								test2 = 0xff;
							else
								test2 = (UCHAR)j;
//							printf("%s  ",dat_names[i]);
//							printf("%s  %x   ",tempx,test2);
//							printf("\n");
							send_tcp((UCHAR*)&tempx,TDATE_STAMP_STR_LEN);
							send_tcp((UCHAR*)&test2,1);
						}
//						close_tcp();
//										-- do this when the host is crashing
//										-- otherwise the target locks up
						break;

					case SAVE_TO_DISK:
						if(ilWriteConfig(iFileName,&ill,isize,errmsg) < 0)
							printf("%s\n",errmsg);
						if(olWriteConfig(oFileName,&oll,osize,errmsg) < 0)
							printf("%s\n",errmsg);
						printf("save to disk\n");
						break;

					case TEST_INPUTS:
						shift_left();
						break;

					case TEST_INPUTS2:
						shift_right();
						break;
					
					case TEST_LCD:
						do
						{
							buf_ptr++;
							if(buf_ptr > 100)
								buf_ptr = 0;
						}while(buf[buf_ptr] != 0);
						buf_ptr++;
						myprintf1(buf+buf_ptr);
						printf("%s\n",buf+buf_ptr);

						for(i = 0;i < 1000;i++)
						{
							if(setdioline(0,i&1) == -1)
								printf("a");
							if(setdioline(7,i&1) == -1)
								printf("a");
							usleep(1000);
						}
						printf("done\n");
						
						break;						

					case TEST_LCD2:
						do
						{
							buf_ptr++;
							if(buf_ptr > 100)
								buf_ptr = 0;
						}while(buf[buf_ptr] != 0);
						buf_ptr++;
						myprintf2("hello\0",buf_ptr);
						printf("%s\n",buf+buf_ptr);
						break;						

					case TEST_LCD3:
						do
						{
							buf_ptr++;
							if(buf_ptr > 100)
								buf_ptr = 0;
						}while(buf[buf_ptr] != 0);
						buf_ptr++;
						myprintf3(buf+buf_ptr,buf_ptr,buf_ptr+1);
						printf("%s\n",buf+buf_ptr);
						break;						

					case TOGGLE_OUTPUTS:
						recv_tcp((UCHAR *)&rec_no,1,1);
						recv_tcp((UCHAR *)&onoff,1,1);

						printf("toggle output: %2d\t%s\n",tempo1.onoff,tempo1.label);
						ollist_toggle_output(rec_no,&oll);
						change_output(rec_no, onoff);
						break;

					case ALL_OFF:
//						pod = curr_o_array;
						printf("all off\n");
						otp = &tempo1;
						for(i = 0;i < NUM_PORT_BITS;i++)
						{
							ollist_change_output(i, &oll, 0);
							change_output(i, 0);
							usleep(500);
						}
						break;

					case ALL_ON:
//						pod = curr_o_array;
						printf("all on\n");
						otp = &tempo1;
						for(i = 0;i < NUM_PORT_BITS;i++)
						{
							ollist_change_output(i, &oll, 1);
							change_output(i, 0);
							usleep(500);
						}
						break;

					case CLEAR_SCREEN:
						for(i = 0;i < 150;i++)
							printf("\n");
						lcd_cls();	
						break;

					case CLOSE_SOCKET:
						printf("closing socket\n");
						close_tcp();
						break;

					case EXIT_PROGRAM:
						close_tcp();
						printf("exiting program\n");
						exit(0);
						break;

					default:
						break;
				}	// end of switch
			}	// if rc > 0
		}else	// if test_sock
		{
//			printf("no connection %d\n",j);
			usleep(TIME_DELAY/1000);
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
UCHAR task2(int test)
{
	I_DATA *itp;
	I_DATA **itpp = &itp;

	O_DATA *otp;
	O_DATA **otpp = &otp;

	int status = -1;
	int bank, index;
	UCHAR result,result2, mask;
	int onoff,i;

#if 0
	while(1)
	{
		usleep(TIME_DELAY);
	}
#endif

	pthread_mutex_lock( &io_mem_lock);
	inportstatus[0] =  ~InPortByteA();
	inportstatus[1] =  ~InPortByteB();
	inportstatus[2] =  ~InPortByteC();
	inportstatus[3] =  ~InPortByteD();
	inportstatus[4] =  ~InPortByteE();
	inportstatus[5] =  ~InPortByteF();
	pthread_mutex_unlock( &io_mem_lock);

	while(TRUE)
	{
		for(bank = 0;bank < NUM_PORTS;bank++)
		{
			pthread_mutex_lock( &io_mem_lock);
			result = InPortByte(bank);
			pthread_mutex_unlock( &io_mem_lock);
			result = ~result;
			
			if(result != inportstatus[bank])
			{
//				printf("%2x %2x ",result,inportstatus[bank]);
				mask = result ^ inportstatus[bank];
//				printf("%2x %2x %2x ",mask,result,inportstatus[bank]);

				index = mask2int(mask);
//				printf("%2x ",index);
				
				if((mask & result) == mask)
				{
					onoff = ON;
					printf("ON  ");
				}
				else
				{
					onoff = OFF;
					printf("OFF ");
				}
//				printf("bit: %2d of bank: %2d\n",real_banks[index+bank*8].index,bank);
				
				index = index+bank*8;
				illist_find_data(index,itpp,&ill);
				printf("affected_output: %d\tport: %d\t%s\n",
						itp->affected_output,itp->port,itp->label);

				ollist_find_data(itp->affected_output,otpp,&oll);
				printf("port: %d\tonoff: %d\t%s\n",otp->port,otp->onoff,otp->label);

				otp->onoff = onoff;
				ollist_insert_data(otp->port,&oll,otp);
				change_output(otp->port,otp->onoff);

			}
			inportstatus[bank] = result;

		}
		usleep(TIME_DELAY);

	}
	return test + 2;
}
/*********************************************************************/
static void print_inputstatus(int index)
{
	int i;
	UCHAR temp;

	printf("\ninputstatus: ");
//	printf("%d ",index);
	for(i = 0;i < 6;i++)
	{
		temp = inportstatus[i];
		if(i == 2 || i == 5)
			printf("%2x ",(temp & 0x0f));
		else
			printf("%2x ",temp);
	}
	printf("\n");
}
/*********************************************************************/
// pass in the index into the total list of outputs
// since each card only has 20 outputs, the last 4 bits of PORT C & F are ignored
// index 0->19 = PORTA(0:7)->PORTC(0:4)
// index 24->39 = PORTD(0:7)->PORTF(0:4)
static int change_output(int index, int onoff)
{
	int bank;

	bank = real_banks[index].bank;
	index = real_banks[index].index;

	pthread_mutex_lock( &io_mem_lock);

	switch(bank)
	{
		case 0:
		OutPortA(onoff, index);		// 0-7
		break;
		case 1:
		OutPortB(onoff, index);		// 0-7
		break;
		case 2:
		OutPortC(onoff, index);		// 0-3
		break;
		case 3:
		OutPortD(onoff, index);		// 0-7
		break;
		case 4:
		OutPortE(onoff, index);		// 0-7
		break;
		case 5:
		OutPortF(onoff, index);		// 0-3
		break;
		default:
		break;
	}
	pthread_mutex_unlock(&io_mem_lock);
//#endif

//	printf("bank: %d\tindex\t%2x\n",bank,index);
	return index;
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
		usleep(TIME_DELAY);

	while(TRUE)
	{
		rc = send_tcp(&test2,1);
		usleep(TIME_DELAY);
		if(++test2 > 0x7e)
			test2 = 0x21;
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
/*
		pthread_mutex_lock( &serial_read_lock);
	//	printf("rec'd %i bytes: %s %d\n",n,(char *)buf,n);
	//	printf("%s\n",(char *)buf);
		if(no_serial_buff > 20)
		{
			printf("%2x ",serial_buff[no_serial_buff]);
			pthread_mutex_unlock(&serial_read_lock);
			no_serial_buff--;
		}
*/
	}
	return test + 3;
}
/*********************************************************************/
// serial receive task
UCHAR task5(int test)
{
	serial_rec = 0;
	int i;
	int j = 0;
	UCHAR ch;
	memset(serial_buff,0,SERIAL_BUFF_SIZE);
	no_serial_buff = 0;
//	printf("%4x\n",&serial_buff[no_serial_buff]);

//	while(TRUE)
//		usleep(TIME_DELAY*10);

	while(TRUE)
	{
//		usleep(TIME_DELAY/1000);
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
			serial_buff[no_serial_buff] = read_serial();
/*
			printf("%c",serial_buff[no_serial_buff]);
			if(++j > 93)
			{
				printf("\n");
				j = 0;
			}
*/
			pthread_mutex_unlock(&serial_read_lock);
			no_serial_buff++;
		}
		else{
//#if 0
			printf("\n");
			for(i = 0;i < SERIAL_BUFF_SIZE;i++)
			{
				printf("%c",serial_buff[i]);
//				if((i % 93) == 0)
//					printf("\n");
			}
//#endif
			no_serial_buff = 0;
//			printf("\n\n");
		}
	}
}
// client calls 'connect' to get accept call below to stop
// blocking and return sd2 socket descriptor

#define PROTOPORT         5193						/* default protocol port number */
#define QLEN              6							/* size of request queue        */

pthread_mutex_t  mut;
static int visits =  0;								  /* counts client connections     */

static struct  sockaddr_in sad;  /* structure to hold server's address  */
static struct timeval tv;
static int sock_open;
static int global_socket;
/*********************************************************************/
// task to monitor for a client requesting a connection
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
#ifndef MAKE_TARGET
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
			usleep(TIME_DELAY*10);
		else
		{
			printf("SERVER: Waiting for contact ...\n");

			if (  (global_socket=accept(listen_sd, (struct sockaddr *)&cad, &alen)) < 0)
			{
				fprintf(stderr, "accept failed\n");
				exit (1);
			}
			sock_open = 1;
			printf("connected to socket: %d\n",global_socket);
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
			printf("address: %d\n",global_socket);
			close_tcp();
		}else strcpy(errmsg,"Success\0");
	}
	else
	{
// this keeps printing out until the client logs on
		strcpy(errmsg,"sock closed");
		rc = -1;
	}
//	printf("%c",buf[0]);
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
	if(comm_open == -1)
		return;
	pthread_mutex_lock( &serial_write_lock);
	write_serial(byte);
	pthread_mutex_unlock(&serial_write_lock);
}
/*********************************************************************/
UCHAR ReceiveByte(void)
{
	int n,i;
	UCHAR rec;
	if(comm_open == -1)
		return -1;

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
//	printf("\nwork_routine()\tthread %d\tI'm Alive\n", *my_id);

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
//	printf("\nwork_routine()\tthread %d\tI'm Proceeding\n", *my_id);

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

