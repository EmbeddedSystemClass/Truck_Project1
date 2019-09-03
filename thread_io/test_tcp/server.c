// 10/10/17 - server doesn't quit work on host
#if 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
//#include <ncurses.h>
#include "../../mytypes.h"
#include "tcp.h"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define TIME_DELAY2 20000
#define KEYPRESS_OFFSET 57
#define MENU_OFFSET 58
#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while(0)
#endif
//******************************************************************************************//
//************************************** init_server ***************************************//
//******************************************************************************************//
int init_server(void)
{
	int i,j;
//	useconds_t tdelay = TIME_DELAY2;
	UCHAR wkey = 0;
	UCHAR key = 0;
	struct timeval tv;

	tv.tv_sec = 0;
	tv.tv_usec = 50000;  // Not init'ing this can cause strange errors

	struct   hostent   *ptrh;     /* pointer to a host table entry */
	struct   protoent  *ptrp;     /* pointer to a protocol table entry */
	struct   sockaddr_in sad;     /* structure to hold server's address */
	struct   sockaddr_in cad;     /* structure to hold client's address */
	int      sd; 		           /* socket descriptors */
	int      port;                /* protocol port number */
	int      alen;                /* length of address */
	char     buf[50];           /* buffer for string the server sends */
	int     n;                /* number of characters read           */
	char 	send_buf[50];
	ssize_t	rc;

	memset((char  *)&sad,0,sizeof(sad)); /* clear sockaddr structure   */
	sad.sin_family = AF_INET;            /* set family to Internet     */
	sad.sin_addr.s_addr = INADDR_ANY;    /* set the local IP address */

	/* Check  command-line argument for protocol port and extract      */
	/* port number if one is specfied.  Otherwise, use the default     */
	/* port value given by constant PROTOPORT                          */

/*	to avoid "server already in use" error: */
/*
	int sockfd;
	int option = 1;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
*/
	port = PROTOPORT;     /* use default port number   */

	sad.sin_port = htons((u_short)port);

//	printf("starting server...\n\n\n");

	/* Map TCP transport protocol name to protocol number */

#ifndef MAKE_TARGET
	if ( ((int)(ptrp = getprotobyname("tcp"))) == 0)
	{
		fprintf(stderr, "cannot map \"tcp\" to protocol number\n");
		return -1;
	}

	/* Create a socket */
	sd = socket (PF_INET, SOCK_STREAM, ptrp->p_proto);
#else
//	getprotobyname doesn't work on TS-7200 cause there's not /etc/protocols file
//	so just say '6' if compiling for TS-7200
	sd = socket (PF_INET, SOCK_STREAM, 6);
#endif
	int option = 1;
//	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
	if (sd < 0)
	{
		fprintf(stderr, "socket creation failed\n");
		printf("\n%s\n",stderr);
		return -1;
	}

	/* Bind a local address to the socket */
	if (bind(sd, (struct sockaddr *)&sad, sizeof (sad)) < 0)
	{
		fprintf(stderr,"bind failed\n");
		printf("\n%s\n",strerror(errno));
		return -1;
	}

	/* Specify a size of request queue */
	if (listen(sd, QLEN) < 0)
	{
		fprintf(stderr,"listen failed\n");
		printf("\n%s\n",strerror(errno));
		return -1;
	}

	/* Main server loop - accept and handle requests */
//	fprintf( stderr, "Server up and running.\n");
	global_socket = -1;

	do
	{
		alen = sizeof(cad);

		if (  (global_socket=accept(sd, (struct sockaddr *)&cad, &alen)) < 0)
		{
			fprintf(stderr, "accept failed\n");
			closesocket(global_socket);
			printf("\n%s\n",strerror(errno));
			return -1;
		}
	}while(global_socket < 1);

	if (setsockopt (global_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval)) < 0)
		return -2;
	if (setsockopt (global_socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(struct timeval)) < 0)
		return -3;

	return global_socket;
}

//******************************************************************************************//
//***************************************** send_tcp ***************************************//
//******************************************************************************************//
int send_tcp(UCHAR *send_buf,int len, char *errmsg)
{
	int rc;
	char extra_msg[10];

//	rc = send(global_socket, send_buf, len,MSG_DONTWAIT); // non-blocking
	rc = send(global_socket, send_buf, len,MSG_WAITALL); // blocking
	if(rc < 0)
	{
		strcpy(errmsg,strerror(errno));
		sprintf(extra_msg," %d",errno);
		strcat(errmsg,extra_msg);
		strcat(errmsg," send\n");
	}else strcpy(errmsg,"Success\0");
	return rc;
}

//******************************************************************************************//
//**************************************** recv_tcp ****************************************//
//******************************************************************************************//
int recv_tcp(UCHAR *recv_buf, int len, char *errmsg)
{
	int rc;
	char extra_msg[10];

//	rc = recv(global_socket, recv_buf, len, MSG_DONTWAIT);
	rc = recv(global_socket, recv_buf, (size_t)len, MSG_WAITALL);
	if(rc < 0)
	{
		strcpy(errmsg,strerror(errno));
		sprintf(extra_msg," %d",errno);
		strcat(errmsg,extra_msg);
		strcat(errmsg," recv");
	}else strcpy(errmsg,"Success\0");
	return rc;
}
/*********************************************************************/
int test_sock(void)
{
	return sock_open;
}

/*********************************************************************/
void close_tcp(void)
{
	if(sock_open)
	{
		sock_open = 0;
		printf("closing socket...\r\n");
		close(global_socket);
		printf("socket closed!\r\n");
		global_socket = -1;
	}else
	{
		printf("socket already closed\r\n");
	}
}

#ifdef TEST_SEQUENCE
#warning "TEST_SEQUENCE defined"

int main(void)
{
	char test1;
	int rc;
	int i,j;
	char errmsg[20];
	memset(errmsg,0,20);

	if(init_server() < 0)
	{
		close(global_socket);
		return 1;
	}

	j = 0;
//	for(i = 0;i < 1000;i++)
	i = 0;
	do
	{
		rc = recv_tcp((UCHAR *)&test1,1,errmsg);
		if(rc < 0)
		{
			printf("%s\n",errmsg);
			close(global_socket);
			return 1;
		}

		printf("%c",test1);

		rc = send_tcp(&test1,1,errmsg);
		if(rc < 0)
		{
			printf("%s\n",errmsg);
			close(global_socket);
			return 1;
		}
		if(++j > 92)
		{
			printf("\n");
			j = 0;
		}
		i++;
	}while(test1 != 0xff && i < 10000);

	close(global_socket);
	return 0;
}
#endif

#ifdef COPY_FILE

#warning "COPY_FILE defined"
//UCHAR buf[30000];
#define UPLOAD_BUFF_SIZE2 10000
UCHAR msg_buf[UPLOAD_BUFF_SIZE2];

UCHAR *buf;

int main(void)
{
	char test1;
	UCHAR test2;
	int rc;
	int i,j;
	char errmsg[20];
	memset(errmsg,0,20);
	int fp;
	long fsize, ofsize;
	char filename[20];
	UCHAR temp[100];
	UCHAR temp2[4];
	UCHAR temp3;
	int chunk = 10000;
	long iter;
	int chunk_num;

	memset(msg_buf,0,UPLOAD_BUFF_SIZE2);
	strcpy(filename,"sched2");
	fp = open((const char *)filename, O_RDWR | O_CREAT | O_TRUNC, 600);
	if(fp < 0)
	{
		printf("could not create file: %s\n",filename);
		exit(1);
	}

	printf("starting program...\n");

	init_server();
/*
	recv_tcp((UCHAR *)&temp[0],50,errmsg);
	for(i = 0;i < 50;i++)
	{
		printf("%d: %02x ",i,temp[i]);
		if(temp[i] > 0x19 && temp[i] < 0x7A)
			printf("%c",temp[i]);
		printf("\n");
	}
	close_tcp();
	return 1;
*/
	recv_tcp((UCHAR *)&temp[0],8,errmsg);
/*
	for(i = 0;i < 8;i++)
	{
		printf("%02x ",temp[i]);
	}
*/
	fsize = (int)temp[3];
	fsize <<= 8;
	fsize |= (int)temp[2];
	fsize <<= 8;
	fsize |= (int)temp[1];
	fsize <<= 8;
	fsize |= (int)temp[0];
	ofsize = fsize;
	printf("\nfsize: %ld\n",fsize);

//	iter = fsize/chunk;
	iter = (int)temp[7];
	iter <<= 8;
	iter |= (int)temp[6];
	iter <<= 8;
	iter |= (int)temp[5];
	iter <<= 8;
	iter |= (int)temp[4];
	printf("iter: %ld\n",iter);

//	printf("\n");

	for(j = 0;j < iter;j++)
	{
//		printf("%d: fsize: %ld\n",j,ofsize - fsize);
/*
		recv_tcp((UCHAR *)&temp,4,errmsg);
		chunk_num = (int)temp[1];
		chunk_num <<= 8;
		chunk_num |= (int)temp[0];
		printf("chunk: %d %04x\n",chunk_num,chunk_num);
*/
		if(j == iter-1)
		{
			rc = recv_tcp((UCHAR *)&msg_buf[0],fsize,errmsg);
			rc = write(fp, &msg_buf[0], fsize);
			printf("final fsize: %ld\n",fsize);
		}
		else
		{
			rc = recv_tcp((UCHAR *)&msg_buf[0],chunk,errmsg);
			rc = write(fp, &msg_buf[0], chunk);
		}
//			printf("rc (file): %d\n",rc);
		fsize -= chunk;
	}

//	printf("%d: fsize: %ld\n",j,ofsize - fsize);
	printf("\n");
//	printf("%s\n",temp);
	fsize = lseek(fp,0,SEEK_END);
	printf("fsize: %ld\n",fsize);
	close(fp);
	close(global_socket);
	printf("done\n");
	return 0;
}

#endif

#ifdef SIMPLE_TEST

#warning "SIMPLE_TEST defined"

UCHAR msg_buf[UPLOAD_BUFF_SIZE];
UCHAR msg_buf2[UPLOAD_BUFF_SIZE/2];
void send_msg(int msg_len, UCHAR *msg, UCHAR msg_type);
int get_msg(void);
static UCHAR pre_preamble[] = {0xF8,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0x00};

CMD_STRUCT cmd_array[68] =
{
	{		NON_CMD,"NON_CMD\0" },
	{		ENABLE_START,"ENABLE_START\0" },
	{		STARTER_OFF,"STARTER_OFF\0" },
	{		ON_ACC,"ON_ACC\0" },
	{		OFF_ACC,"OFF_ACC\0" },
	{		ON_FUEL_PUMP,"ON_FUEL_PUMP\0" },
	{		OFF_FUEL_PUMP,"OFF_FUEL_PUMP\0" },
	{		ON_FAN,"ON_FAN\0" },
	{		OFF_FAN,"OFF_FAN\0" },
	{		ON_LIGHTS,"ON_LIGHTS\0" },
	{		OFF_LIGHTS,"OFF_LIGHTS\0" },
	{		ON_BRIGHTS,"ON_BRIGHTS\0" },
	{		OFF_BRIGHTS,"OFF_BRIGHTS\0" },
	{		BLANK,"BLANK\0" },
	{		ON_BRAKES,"ON_BRAKES\0" },
	{		OFF_BRAKES,"OFF_BRAKES\0" },
	{		ON_RUNNING_LIGHTS,"ON_RUNNING_LIGHTS\0" },
	{		OFF_RUNNING_LIGHTS,"OFF_RUNNING_LIGHTS\0" },
	{		SPECIAL_CMD,"SPECIAL_CMD\0" },
	{		START_SEQ,"START_SEQ\0" },
	{		SHUTDOWN,"SHUTDOWN\0" },
	{		ON_LLIGHTS,"ON_LLIGHTS\0" },
	{		OFF_LLIGHTS,"OFF_LLIGHTS\0" },
	{		ON_LBRIGHTS,"ON_LBRIGHTS\0" },
	{		OFF_LBRIGHTS,"OFF_LBRIGHTS\0" },
	{		ON_RLIGHTS,"ON_RLIGHTS\0" },
	{		OFF_RLIGHTS,"OFF_RLIGHTS\0" },
	{		ON_RBRIGHTS,"ON_RBRIGHTS\0" },
	{		OFF_RBRIGHTS,"OFF_RBRIGHTS\0" },
	{		BLOWER_OFF,"BLOWER_OFF\0" },
	{		BLOWER1,"BLOWER1\0" },
	{		BLOWER2,"BLOWER2\0" },
	{		BLOWER3,"BLOWER3\0" },
	{		WIPER1,"WIPER1\0" },
	{		WIPER2,"WIPER2\0" },
	{		WIPER_OFF,"WIPER_OFF\0" },
	{		STOP_MBOX_XMIT,"STOP_MBOX_XMIT\0" },
	{		START_MBOX_XMIT,"START_MBOX_XMIT\0" },
	{		SEND_TIME_DATA,"SEND_TIME_DATA\0" },
	{		SHUTDOWN_IOBOX,"SHUTDOWN_IOBOX\0" },
	{		REBOOT_IOBOX,"REBOOT_IOBOX\0" },
	{		TEST_ALL_IO,"TEST_ALL_IO\0" },
	{		SEND_ODATA,"SEND_ODATA\0" },
	{		SAVE_TO_DISK,"SAVE_TO_DISK\0" },
	{		GET_DIR,"GET_DIR\0" },
	{		LCD_SHIFT_RIGHT,"LCD_SHIFT_RIGHT\0" },
	{		LCD_SHIFT_LEFT,"LCD_SHIFT_LEFT\0" },
	{		SCROLL_UP,"SCROLL_UP\0" },
	{		SCROLL_DOWN,"SCROLL_DOWN\0" },
	{		ENABLE_LCD,"ENABLE_LCD\0" },
	{		SET_TIME,"SET_TIME\0" },
	{		GET_TIME,"GET_TIME\0" },
	{		UPLOAD_NEW,"UPLOAD_NEW\0" },
	{		NEW_PASSWORD1,"NEW_PASSWORD1\0" },
	{		SET_SERIAL_RECV_ON,"SET_SERIAL_RECV_ON\0" },
	{		SET_SERIAL_RECV_OFF,"SET_SERIAL_RECV_OFF\0" },
	{		TEST_LEFT_BLINKER,"TEST_LEFT_BLINKER\0" },
	{		TEST_RIGHT_BLINKER,"TEST_RIGHT_BLINKER\0" },
	{		RE_ENTER_PASSWORD,"RE_ENTER_PASSWORD\0" },
	{		DISCONNECT,"DISCONNECT\0" },
	{		CLOSE_DB,"CLOSE_DB\0" },
	{		OPEN_DB,"OPEN_DB\0" },
	{		BAD_MSG,"BAD_MSG\0" },
	{		CURRENT_TIME,"CURRENT_TIME\0" },
	{		SET_PARAMS,"SET_PARAMS\0" },
	{		EXIT_PROGRAM,"EXIT_PROGRAM\0" },
	{		ENGINE_TEMP,"ENGINE_TEMP\0" },
	{		SEND_RT_VALUES,"SEND_RT_VALUES\0" }
};


int main(void)
{
	char test1;
	int rc;
	int i,j;
	char errmsg[20];
	memset(errmsg,0,20);
	int msg_len;
	UCHAR cmd;
	char tempx[200];

	printf("simple test...\n");
	init_server();

	j = 0;
//	for(i = 0;i < 1000;i++)
	i = 0;
	do
	{
		memset(msg_buf,0,sizeof(msg_buf));
		// well this doesn't work because I don't know what the client using EpClient is doing 
		/// under the covers...
		msg_len = get_msg();
		printf("msg_len:%d\n",msg_len);
		if(msg_len < 0)
		{
			printf("bad msg\r\n");
			cmd = BAD_MSG;
		}else
		{
			rc = recv_tcp(&msg_buf[0],msg_len,errmsg);
			cmd = msg_buf[0];
			printf("cmd: %d\n",cmd);
			printf("rc: %d\n",rc);
			j = 0;

		//				for(i = 0;i < msg_len;i++)
		//					printHexByte(msg_buf[i]);

		//				printString2("\r\n");	

			for(i = 2;i < msg_len+2;i+=2)
				msg_buf2[j++] = msg_buf[i];
		}

		if(cmd > 0)
		{
			printf("cmd: %d %s\n",cmd,cmd_array[cmd].cmd_str);
		}
		if(cmd > 0)
		{
			rc = 0;
			switch(cmd)
			{
				// ENABLE_START closes a relay for so many seconds so a button
				// on the dash can close the ckt to the starter solinoid
				case ENABLE_START:
				case STARTER_OFF:
				// ingition - power to the coil
				case ON_ACC:
				case OFF_ACC:
				case ON_FUEL_PUMP:
				case OFF_FUEL_PUMP:
				case ON_FAN:
				case OFF_FAN:
				case ON_LIGHTS:
				case OFF_LIGHTS:
				// START_SEQ turns on acc, fp, fan and enables starter button
				case START_SEQ:
				// SHUTDOWN shuts off the starter enable, fp, acc and fan
				case SHUTDOWN:
				case SHUTDOWN_IOBOX:
				case REBOOT_IOBOX:
				case TEST_ALL_IO:
				case TEST_LEFT_BLINKER:
				case TEST_RIGHT_BLINKER:
				case ON_BRIGHTS:
				case OFF_BRIGHTS:
				case ON_BRAKES:
				case OFF_BRAKES:
				case ON_LLIGHTS:
				case OFF_LLIGHTS:
				case ON_LBRIGHTS:
				case OFF_LBRIGHTS:
				case ON_RLIGHTS:
				case OFF_RLIGHTS:
				case ON_RBRIGHTS:
				case OFF_RBRIGHTS:
				case ON_RUNNING_LIGHTS:
				case OFF_RUNNING_LIGHTS:
				case SPECIAL_CMD:
				case BLOWER_OFF:
				case BLOWER1:
				case BLOWER2:
				case BLOWER3:
				case WIPER1:
				case WIPER2:
				case WIPER_OFF:
					break;
			}
			if(cmd == BAD_MSG)
			{
				close_tcp();
				return -1;
			}
		}

/*
		rc = recv_tcp(&test1,1,errmsg);
		if(rc < 0)
			printf("%s\n",errmsg);

		printf("%02x",test1);

		rc = send_tcp(&test1,1,errmsg);
		if(rc < 0)
			printf("%s\n",errmsg);
		if(++j > 92)
		{
			printf("\n");
			j = 0;
		}
		i++;
*/
	}while(cmd != SHUTDOWN_IOBOX && cmd != REBOOT_IOBOX);

	close_tcp();
	return 0;
}

/*********************************************************************/
// get preamble & msg len from client
// preamble is: {0xF8,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0x00,
// msg_len(lowbyte),msg_len(highbyte),0x00,0x00,0x00,0x00,0x00,0x00}
// returns message length
int get_msg(void)
{
	int len;
	UCHAR low, high;
	int ret;
	char errmsg[20];
	int i;

	UCHAR preamble[20];
	ret = recv_tcp(preamble,16,errmsg);
	if(ret < 0)
	{
		printf("get_msg error\n");
	}

	for(i = 0;i < 16;i++)
		printf("%2x ",preamble[i]);

	printf("\n");

//	for(i = 0;i < 16;i++)
//		printf("%c",preamble[i]);

	printf("\n");

	if(memcmp(preamble,pre_preamble,8) != 0)
		return -1;

	low = preamble[8];
	high = preamble[9];
	len = (int)(high);
	len <<= 8;
	len |= (int)low;

	return len;
}

/*********************************************************************/
void send_msg(int msg_len, UCHAR *msg, UCHAR msg_type)
{
	int len;
	int ret;
	int i;
	char errmsg[20];

	if(test_sock())
	{
		ret = send_tcp(&pre_preamble[0],8,errmsg);
		msg_len++;
		send_tcp((UCHAR *)&msg_len,1,errmsg);
		ret = 0;
		send_tcp((UCHAR *)&ret,1,errmsg);

		for(i = 0;i < 6;i++)
			send_tcp((UCHAR *)&ret,1,errmsg);

		send_tcp((UCHAR *)&msg_type,1,errmsg);

		ret = 0;
		send_tcp((UCHAR *)&ret,1,errmsg);

		for(i = 0;i < msg_len;i++)
		{
			send_tcp((UCHAR *)&msg[i],1,errmsg);
			send_tcp((UCHAR *)&ret,1,errmsg);
		}

	}
}

#endif


