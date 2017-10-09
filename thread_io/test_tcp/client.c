/* To compile me in Solaris type:  gcc -o client client.c -lsocket -lnsl */
/* To compile me in Linux type:  gcc -o client client.c */

/* client.c - code for example client that uses TCP         */
/* From Computer Networks and Internets by Douglas F. Comer */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/time.h>
#include "mytypes.h"
#include "client.h"
#define closesocket      close
//#define PROTOPORT        5193        /* default protocol port number */
#define PROTOPORT        1003        /* default protocol port number */

static int global_socket;
static int sock_open;

#ifdef COPY_FILE
#define BUF_SIZE 10000
UCHAR buf[BUF_SIZE];
#endif

int test_sock(void)
{
	return sock_open;
}

//extern int               errno;
char   localhost[] = "localhost";    /* default host name            */
/*---------------------------------------------------------------------
 * Program:   client
 *
 * Purpose:   allocate a socket, connect to a server, and print all output
 *
 * Syntax:    client [ host [port] ]
 *
 *              host - name of a computer on which server is executing
 *              port - protocol port number server is using
 *
 * Note:      Both arguments are optional.  If no host name is specified,
 *            the client uses "localhost";  if no protocol port is
 *            specified, the client uses the default given by PROTOPORT.
 *
 *---------------------------------------------------------------------
 */
static struct  sockaddr_in sad;  /* structure to hold server's address  */
static struct timeval tv;

/*********************************************************************/
int init_client(char *host)
{
	struct  hostent  *ptrh;   /* pointer to a host table entry       */
	struct  protoent *ptrp;   /* point to a protocol table entry     */
//	int     sd;               /* socket descriptor                   */
	int     port;             /* protocol port number                */
//	char    *host;            /* pointer to host name                */
	int     n;                /* number of characters read           */
	char    buf[50];        /* buffer for data from the server     */
	char 	send_buf[50];
	int		i;
	ssize_t	rc;
	int		send_int;

	tv.tv_sec = 1;
	tv.tv_usec = 500;  // Not init'ing this can cause strange errors

	printf("starting client\n");

	memset((char *)&sad,0,sizeof(sad));  /* clear sockaddr structure */
	sad.sin_family = AF_INET;            /* set family to Internet   */
	sad.sin_addr.s_addr = INADDR_ANY;

	/* Check command-line argument for protocol port and extract     */
	/* port number if on is specified.  Otherwise, use the default   */
	/* port value biven by constant PROTOPORT                        */

//	if (argc > 2) port = atoi(argv[2]);
//	else port = PROTOPORT;
	 port = PROTOPORT;

	if (port > 0) sad.sin_port = htons((u_short)port);
	else
	{
		fprintf( stderr,"bad port number %d\n", port);
		return -1;
	}

//	if (argc > 1 ) host = argv[1];
//	else host = localhost;

//	host = "192.168.42.115";
//	host = "192.168.42.145";	// if compiling for target
#ifndef MAKE_TARGET

	ptrh = gethostbyname(host);
	if( ((char *)ptrh) == NULL)
	{
		fprintf( stderr, "invalid host:  %s\n", host);
		return -1;
	}

	memcpy(&sad.sin_addr, ptrh->h_addr, ptrh->h_length);

	 /* Map TCP transport protocol name to protocol number */
	//getprotobyname doesn't work on TS-7200 because there's no /etc/protocols file
	 if ( ((int)(ptrp = getprotobyname("tcp"))) == 0)
	{
		fprintf(stderr, "cannot map \"tcp\" to protocol number");
		return -1;
	}
	 /* Create a socket */
	global_socket = socket (PF_INET, SOCK_STREAM, ptrp->p_proto);

#else
// so just use '6' (if this is compiled for TS-7200) as the tcp protocol number
	global_socket = socket (PF_INET, SOCK_STREAM, 6);
#endif

	 if (global_socket < 0)
	 {
		fprintf(stderr, "socket creation failed\n");
		return -1;
	 }

	printf("host = %s global_socket = %d\n",host,global_socket);
	printf("trying to connect...\n");
	return 1;
}

/*********************************************************************/
int tcp_connect(void)
{
	if (connect(global_socket, (struct sockaddr *)&sad, sizeof(sad)) < 0)
	{
		fprintf( stderr, "connect failed\n");
		sock_open = 0;
		return -1;
	}
	else
	{
		printf("connected\n");

//#ifndef MAKE_TARGET
		if (setsockopt (global_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval)) < 0)
			return -2;
		if (setsockopt (global_socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(struct timeval)) < 0)
			return -3;
//#endif
		sock_open = 1;
		return global_socket;
	}
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
void close_sock(void)
{
	close(global_socket);
	sock_open = 0;
}

/*********************************************************************/
#ifdef TEST_SEQUENCE
int main(int argc, char *argv[])
{
	char test1,test2;
	int rc;
	int i,j;
	char errmsg[20];
	memset(errmsg,0,20);

	if(argc < 2)
	{
		printf("usage: %s [ip address of server]\n",argv[0]);
		exit(1);
	}

//	rc = init_client("192.168.42.146");
	printf("connecting to %s\n",argv[1]);
	rc = init_client(argv[1]);
//	rc = init_client("192.168.42.115");

	if(test_sock() == 0)
	{
//		printf("trying to open socket...\n");
		if(tcp_connect() > 0)
		{
			printf("connected\n");
		}
		else
			return -1;
	}

	test1 = 0x21;
	j = 0;
	for(i = 0;i < 30000;i++)
	{
		rc = put_sock(&test1,1,1,errmsg);
		if(rc < 0)
			printf("%s\n",errmsg);

//		usleep(TIME_DELAY);		// defined in mytypes.h
		usleep(100);
		rc = get_sock(&test2,1,1,errmsg);
		if(rc < 0)
			printf("%s\n",errmsg);

		printf("%c",test2);

		if(++test1 > 0x7e)
			test1 = 0x21;

		if(++j > 92)
		{
			printf("\n");
			j = 0;
		}
	}
	test1 = 0xff;
	put_sock(&test1,1,1,errmsg);
	close_sock();
	return 0;
}
#endif

#ifdef COPY_FILE
int main(int argc, char *argv[])
{
	char test1,test2;
	int rc;
	int i,j;
	char errmsg[20];
	memset(errmsg,0,20);
	char filename[20];
	int fp;
	off_t fsize;
	int buf_bytes;
	int buf_bytes2;

	if(argc < 3)
	{
		printf("usage: %s [ip address of server] [file to send]\n",argv[0]);
		exit(1);
	}else
	printf("sending file: %s to: %s\n",argv[2],argv[1]);

	memset(buf,0,BUF_SIZE);

	rc = init_client(argv[1]);
//	rc = init_client("192.168.42.146");
//	rc = init_client("192.168.42.115");

	if(test_sock() == 0)
	{
		printf("trying to open socket...\n");
		if(tcp_connect() > 0)
		{
			printf("connected\n");
		}
		else
			return -1;
	}

	strcpy(filename,argv[2]);

//	put_sock((UCHAR*)&filename[0],20,1,errmsg);
//	printf("%s\n",errmsg);

	if(access(filename,F_OK) != -1)
	{
		fp = open((const char *)filename, O_RDWR);
		if(fp < 0)
		{
			printf("can't open file for writing");
		}else
		{
			rc = 0;
			fsize = lseek(fp,0,SEEK_END);
			printf("filesize: %ld\n",fsize);

			rc = put_sock((UCHAR *)&fsize,sizeof(off_t),1,errmsg);
// if server is 32-bit machine the sizeof(off_t) is 4
// but if client is 64-bit then sizeof(off_t) is 8
//			rc = put_sock((UCHAR *)&fsize,4,1,errmsg);
			if(rc < 0)
			{
				printf("%s\n",errmsg);
				exit(1);
			}
			printf("rc: %d\n",rc);
			lseek(fp,0,SEEK_SET);

			buf_bytes = (int)fsize;
			printf("starting buf_bytes: %d\n",buf_bytes);
			do
			{
				buf_bytes2 = (buf_bytes > BUF_SIZE?BUF_SIZE:buf_bytes);
				buf_bytes -= BUF_SIZE;

				printf("buf_bytes: %d %d ",buf_bytes2,buf_bytes);
				rc = read(fp,&buf[0],buf_bytes2);

				rc = put_sock((UCHAR *)&buf[0],buf_bytes2,1,errmsg);
				if(rc < 0)
				{
					printf("%s\n",errmsg);
					exit(1);
				}
				else printf("rc: %d\n",rc);
//				printf("%s\n",errmsg);

			}while(buf_bytes > 0);

			close_sock();
			close(fp);
		}
	}
	return 0;
}
#endif

