// server_on_linux2.c - testing the server sending the O_DATA records to the 
// client running on windows 10 laptop - must send a preamble with each
// message which consists of:
// (1) 0xF8
// (6) 0xF0
// (1) 0x00
// message length as a 2 byte in (low byte 1st)
// (6) 0x00
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include "../queue/ollist_threads_rw.h"
#include "../ioports.h"
#include "setiodata.h"

extern int oLoadConfig(char *filename, O_DATA *curr_o_array,size_t size,char *errmsg);
extern int GetFileFormat(char *filename);

typedef unsigned char UCHAR;

#define PROTOPORT 8000
#define BUF_SIZE 2000

static int error(char *msg)
{
    perror(msg);
    exit(1);
}

enum send_msgs
{
	TEST_SEND = 0xFD,
	TEST_DATA,
	TEST_DATA2
}SEND_MSGS;

static int socket_fd;
static unsigned char buf[BUF_SIZE];
static int buf2[BUF_SIZE];
static 	UCHAR send_char;
static void show_menu(void);
static int test_send(int msg_size);
static void send_preamble(int msg_size);
static int test_data(void);

int main(int argc, char *argv[])
{

	int ret = -1;
	struct timeval tv;

	struct  hostent   *ptrh;				  /* pointer to a host table entry */
	struct  protoent  *ptrp;				  /* pointer to a protocol table entry */
	struct  sockaddr_in sad;				  /* structure to hold server's address */
	struct  sockaddr_in cad;				  /* structure to hold client's address */
	int     listen_sd;						  /* socket descriptors */
	int     port;							  /* protocol port number */
	int     alen;							  /* length of address */
	port = PROTOPORT;
	tv.tv_sec = 2;
	tv.tv_usec = 50000;
	int s;
	int i,j,k,l;
	char tempx[40];
	char tempx2[10];
	char ch;
	char ret_char;
	int msg_size;
	int option = 1;

	memset((char  *)&sad,0,sizeof(sad));	  /* clear sockaddr structure   */
	sad.sin_family = AF_INET;				  /* set family to Internet     */
	sad.sin_addr.s_addr = INADDR_ANY;		  /* set the local IP address */

	sad.sin_port = htons((u_short)port);

	socket_fd = -1;

	if ( ((int)(ptrp = getprotobyname("tcp"))) == 0)
	{
		printf("cannot map tcp to protocol number\n");
//			exit (1);
	}
	listen_sd = socket (AF_INET, SOCK_STREAM, ptrp->p_proto);

	if (listen_sd < 0)
	{
		printf("socket creation failed\n");
//			exit(1);
	}

/* Bind a local address to the socket */
	setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
	if (bind(listen_sd, (struct sockaddr *)&sad, sizeof (sad)) < 0)
	{
		printf("bind failed\n");
		error(tempx);
		exit(1);
	}

	/* Specify a size of request queue */
	if (listen(listen_sd, 1000) < 0)
	{
		printf("listen failed\n");
//			exit(1);
	}

	alen = sizeof(cad);

/* Main server loop - accept and handle requests */
	printf("Server Waiting...\r\n");

	if (  (socket_fd=accept(listen_sd, (struct sockaddr *)&cad, (socklen_t *)&alen)) < 0)
	{
		printf("accept failed\r\n");
		return 0;
	}
	if(socket_fd > 0)
		printf("connected to socket: \r\n");
/*
	if (setsockopt (socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval)) < 0)
		return -2;
	if (setsockopt (socket_fd, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(struct timeval)) < 0)
		return -3;
*/
	ret = 0;

/*
	for(i = 0;i < 200;i++)
	{
		ret += recv(socket_fd,&buf[i],1,MSG_WAITALL);
		printf("%d: %02x\n",ret,buf[i]);
	}

	close(socket_fd);
	return 0;
*/
// send preamble of "0xF8 0xF0 0xF0 0xF0 0xF0 0xF0 0xF0"
// and then sends the msg size as an int
// and then send the msg in int's

	memset(buf,0,BUF_SIZE);
	for(i = 0;i < BUF_SIZE;i += 2)
	{
		buf[i] = ch;
		if(++ch > 0x7e)
			ch = 0x21;
	}
	test_data();
	return 0;
/*
	show_menu();
	while((ret_char = getc(stdin)) != 'q')
	{
		switch(ret_char)
		{
			case 'a':
				test_send(10);
			break;
			case 'b':
				test_data();
			break;
			case 'c':
				printf(" \n");
			break;
			case 'd':
				printf(" \n");
			break;
			case 'e':
				printf(" \n");
			break;
			case 'f':
				printf(" \n");
			break;
			case 'g':
				printf(" \n");
			break;
			case 'h':
				printf(" \n");
			break;
			case 'i':
				printf(" \n");
			break;
			case 'j':
				printf(" \n");
			break;
			case 'k':
				printf(" \n");
			break;
			case 'l':
				printf(" \n");
			break;
			case 'm':
				show_menu();
				printf(" \n");
			break;
			case 'n':
				printf(" \n");
			break;
			case 'o':
				printf(" \n");
			break;
			case 'p':
				printf(" \n");
			break;
			case 'q':
				printf(" quiting...\n");
				close(socket_fd);
				exit(0);
			break;
			case 'r':
				printf(" \n");
			break;
			case 's':
				printf(" \n");
			break;
			case 't':
				printf(" \n");
			break;
		}
	}
*/
}		

static int test_send(int msg_size)
{
	int i,j,k;
	char ch;
	ch = 0x20;
	buf[0] = TEST_SEND;
	send_preamble(msg_size);
	send(socket_fd,&buf[0],msg_size*2,MSG_WAITALL);
 	return 0;
 }

static int test_data(void)
{
	int i,j,k;
	char ch;
	ch = 0x20;
	buf[0] = TEST_DATA;
	char temp[100];
	int tempx[100];

	O_DATA *curr_o_array;
	O_DATA *pod;
	size_t isize;
	size_t osize;
	char errmsg[60];
	int num_valids;
	int temp_int;
	int temp_int2;

	i = NUM_PORT_BITS;

	osize = sizeof(O_DATA);
	osize *= i;
//	printf("\nsizeof O_DATA: %lu\n",sizeof(O_DATA));

	curr_o_array = (O_DATA *)malloc(osize);
	memset((void *)curr_o_array,0,osize);

	if(oLoadConfig("odata.dat",curr_o_array,osize,errmsg) < 0)
	{
		printf("%s\n",errmsg);
		close(socket_fd);
		return -1;
	}

	pod = curr_o_array;
/*
	printf("\n");
	printf("port\tonoff\tpolarity\ttype\ttime_delay\ttime_left\tpulse_time\treset\tlabel\n\n");
	for(i = 0;i < osize/sizeof(O_DATA);i++)
	{
		if(pod->label[0] != 0)
		{
			printf("%d\t%d\t%d\t\t%d\t%d\t\t%d\t\t%d\t\t%d\t%s\n",
				pod->port, pod->onoff, pod->polarity, pod->type, pod->time_delay, 
					pod->time_left, pod->pulse_time, pod->reset, pod->label);
		}
		pod++;
	}
*/
	pod = curr_o_array;
	num_valids = 0;
	for(i = 0;i < NUM_PORT_BITS;i++)
	{
		if(pod->label[0] != 0)
			num_valids++;
		pod++;
	}
/*
	printf("num valid records: %d\n",num_valids);
	printf("\n\n");

	printf("sizeof: %ld %ld\n",sizeof(I_DATA),sizeof(O_DATA));
*/
	pod = curr_o_array;
//	for(i = 0;i < osize/sizeof(O_DATA);i++)

	temp_int2 = 1;

	pod->onoff = 2;
	pod->type = 4;
	memset(temp,0,sizeof(temp));
	sprintf(temp,"%s %d %d %d %d ",pod->label,pod->port,pod->onoff,pod->type,pod->time_delay);
	printf("%s ",temp);
	printf("size: %d\n",strlen(temp));

	send_preamble(strlen(temp)*2+4);
	temp_int = (int)pod->port;
	send(socket_fd,&temp_int,2,MSG_WAITALL);
	send(socket_fd,&temp_int2,2,MSG_WAITALL);
	for(j = 0;j < strlen(temp);j++)
	{
		ch = (char)temp[j];
		send(socket_fd,&ch,1,MSG_WAITALL);
		temp_int = (int)temp[j];
		ch = 0;
		send(socket_fd,&ch,1,MSG_WAITALL);
	}
	pod++;
	pod->onoff = 3;
	pod->type = 5;
	memset(temp,0,sizeof(temp));
	sprintf(temp,"%s %d %d %d %d ",pod->label,pod->port,pod->onoff,pod->type,pod->time_delay);
	printf("%s ",temp);
	printf("size: %d\n",strlen(temp));

	send_preamble(strlen(temp)*2+4);
	temp_int = (int)pod->port;
	send(socket_fd,&temp_int,2,MSG_WAITALL);
	send(socket_fd,&temp_int2,2,MSG_WAITALL);
	for(j = 0;j < strlen(temp);j++)
	{
		ch = (char)temp[j];
		send(socket_fd,&ch,1,MSG_WAITALL);
		temp_int = (int)temp[j];
		ch = 0;
		send(socket_fd,&ch,1,MSG_WAITALL);
	}

/*
	for(i = 0;i < 4;i++)
	{
		memset(temp,0,sizeof(temp));
		strcpy(temp,pod->label);
		printf("%s ",temp);
		printf("size: %d\n",strlen(temp));

		send_preamble(strlen(temp)*2+4);
		temp_int = (int)pod->port;
		send(socket_fd,&temp_int,2,MSG_WAITALL);
		send(socket_fd,&temp_int2,2,MSG_WAITALL);
		for(j = 0;j < strlen(temp);j++)
		{
			ch = (char)temp[j];
			send(socket_fd,&ch,1,MSG_WAITALL);
			temp_int = (int)temp[j];
			ch = 0;
			send(socket_fd,&ch,1,MSG_WAITALL);
		}
		temp_int2++;
		usleep(10000);

		sprintf(temp,"%02d %02d %02d %02d %02d %02d %02d %02d xx",
			pod->port,pod->onoff,pod->polarity,pod->type,pod->time_delay,
				pod->time_left,pod->pulse_time,pod->reset);
//		printf("%sx\n",temp);
		send_preamble(strlen(temp)*2+4);
		printf("%d ",strlen(temp));
		temp_int = (int)pod->port;
		printf("%d ",temp_int);
		printf("%s\n",temp);
		send(socket_fd,&temp_int,2,MSG_WAITALL);
		send(socket_fd,&temp_int2,2,MSG_WAITALL);
		for(j = 0;j < strlen(temp);j++)
		{
			ch = (char)temp[j];
			send(socket_fd,&ch,1,MSG_WAITALL);
//			ch = (char)(temp[j] >> 1);
			ch = 0;
			send(socket_fd,&ch,1,MSG_WAITALL);
		}

		pod++;
	}
*/

//	free(curr_i_array);
	free(curr_o_array);

 	return 0;
 }

/*
	UCHAR port;
	UCHAR onoff;			// current state: 1 if on; 0 if off
	UCHAR polarity;			// 0 - on input turns output on; off input turns output off
							// 1 - on input turns output off; off input turns output on
	UCHAR type;				// see below
	UINT time_delay;		// when type 2-4 this is used as the time delay
	UINT time_left;			// gets set to time_delay and then counts down
	UCHAR pulse_time;		// not used
	UCHAR reset;			// used to make 2nd pass
*/

#if 0
	curr_i_array = (I_DATA *)malloc(isize);
	memset((void *)curr_i_array,0,isize);

	pid = curr_i_array;

	memset((void *)curr_i_array,0,isize);

	if(iLoadConfig("idata.dat",curr_i_array,isize,errmsg) < 0)
	{
		printf("%s\n",errmsg);
		return -1;
	}

	pid = curr_i_array;

	printf("\n");
	printf("port\taffected_output\t\tlabel\n\n");

	for(i = 0;i < isize/sizeof(I_DATA);i++)
	{
		if(pid->label[0] != 0)

			if(comma_delim == 1)
			{
//					printf("%d,%d,%s\n",pid->port,pid->affected_output[0],pid->label);
				printf("%d,",pid->port);
				for(j = 0;j < 10;j++)
					printf("%d,",pid->affected_output[j]);
				printf("%s\n",pid->label);
			}else
			{
				printf("%d\t%d\t\t\t%s\n",pid->port,pid->affected_output[0],pid->label);
			}
			pid++;
	}

	pid = curr_i_array;
	for(i = 0;i < NUM_PORT_BITS;i++)
	{
		if(pid->label[0] != 0)
			num_valids++;
		pid++;
	}
	printf("num valid records: %d\n",num_valids);

	printf("\n");
#endif

static void send_preamble(int msg_size)
{
	int i;
	send_char = 0xF8;	// first char of preamble is 0xF8
	send(socket_fd,&send_char,1,MSG_WAITALL);

	send_char = 0xF0;
	for(i = 0;i < 6;i++)	// next 6 are 0xF0
		send(socket_fd,&send_char,1,MSG_WAITALL);

	send_char = 0;			// next one is 0
	send(socket_fd,&send_char,1,MSG_WAITALL);
	
	// then send msg size
	send(socket_fd,&msg_size,2,MSG_WAITALL);

	for(i = 0;i < 6;i++)	// then send 6 0's
		send(socket_fd,&send_char,1,MSG_WAITALL);
}

#if 0
	usleep(1000000);
	ch = 0x20;
	for(i = 0;i < 100;i++)
	{
		buf2[i] = (int)ch;
		if(++ch > 0x7e)
			ch = 0x21;
	}
	for(i = 0;i < 100;i++)
		send(socket_fd,&buf2[i],2,MSG_WAITALL);
	printf("\ndone\n");
	close(socket_fd);
	return 0;

	for(k = 0;k < 40;k++)
	{
		ret = recv(socket_fd,buf,8,MSG_WAITALL);
		// this reads 0xF8 0xF0(6x) 00
	//	for(i = 0;i < 8;i++)
	//		printf("%02x ",buf[i]);
	
//		printf("sizeof(int) = %d\n",sizeof(int));	// returns '4'
		ret = recv(socket_fd,&msg_size,2,MSG_WAITALL);
//		printf("msg size: %d ",msg_size);
		recv(socket_fd,&buf,6,MSG_WAITALL);	// read (6) 0's
		ret = 0;
		// reads msgs_size of int's
		// can't use sizeof(int) because it's 4 on this machine
		for(i = 0;i < msg_size/2;i++)
			ret += recv(socket_fd,&buf2[i],2,MSG_WAITALL);

//		printf("ret: %d\n",ret);

		for(i = 0;i < msg_size/2;i++)
		{
//			printf("%c",(char)buf2[i]);
			tempx[i] = (char)buf2[i];
		}
		tempx[i] = 0;

//		printf("\n");

		for(j = 0;j < 8;j++)
		{
			ret = recv(socket_fd,buf,8,MSG_WAITALL);
			ret = recv(socket_fd,&msg_size,2,MSG_WAITALL);
//			printf("s %d:",msg_size);
			recv(socket_fd,&buf,6,MSG_WAITALL);	// read (6) 0's
			ret = 0;
			// reads msgs_size of int's
			// can't use sizeof(int) because it's 4 on this machine
			ret += recv(socket_fd,&buf[0],msg_size,MSG_WAITALL);

//			printf("ret: %d ",ret);

			l = 0;
			for(i = 0;i < msg_size;i++)
			{
//				printf("%02x ",buf[i]);
				if(i == 0 || i == 2)
					tempx2[l++] = buf[i];
			}
			tempx2[l] = 0;
			printf(" %d ",atoi(tempx2));
		}
		
		printf("%s\n",tempx);
	}
	printf("\n\n");
	close(socket_fd);
#endif

static void show_menu(void)
{
	printf("a - test\n");
	printf("b - test odata\n");
	/*
	printf("c -\n");
	printf("d -\n");
	printf("e -\n");
	printf("f -\n");
	printf("g -\n");
	printf("h -\n");
	printf("i -\n");
	printf("j -\n");
	printf("k -\n");
	printf("l -\n");
	printf("m - menu\n");
	printf("n -\n");
	printf("o -\n");
	printf("p -\n");
	printf("q - quit\n");
	printf("r -\n");
	printf("s -\n");
	*/
	
}
