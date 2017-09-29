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
#include "../mytypes.h"
#define closesocket      close
#define PROTOPORT        5193        /* default protocol port number */

int init_client(char *host, int port);
int put_sock(UCHAR *buf,int buflen, int block, char *errmsg);
int get_sock(UCHAR *buf, int buflen, int block, char *errmsg);
void close_sock(void);

static int global_socket;
static int sock_open;

int test_sock(void)
{
	return sock_open;
}

static struct  sockaddr_in sad;  /* structure to hold server's address  */
static struct timeval tv;

/*********************************************************************/
int init_client(char *host, int port)
{
	struct  hostent  *ptrh;   /* pointer to a host table entry       */
	struct  protoent *ptrp;   /* point to a protocol table entry     */
//	int     sd;               /* socket descriptor                   */
	int     n;                /* number of characters read           */
	char    buf[50];        /* buffer for data from the server     */
	char 	send_buf[50];
	int		i;
	ssize_t	rc;
	int		send_int;

	tv.tv_sec = 1;
	tv.tv_usec = 500;  // Not init'ing this can cause strange errors

	memset((char *)&sad,0,sizeof(sad));  /* clear sockaddr structure */
	sad.sin_family = AF_INET;            /* set family to Internet   */

	/* Check command-line argument for protocol port and extract     */
	/* port number if on is specified.  Otherwise, use the default   */
	/* port value biven by constant PROTOPORT                        */

	if (port > 0) sad.sin_port = htons((u_short)port);
	else
	{
		fprintf( stderr,"bad port number %d\n", port);
		return -1;
	}

	printf("host: %s\n",host);

//	host = "192.168.42.115";
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

// so just use '6' (if this is compiled for TS-7200) as the tcp protocol number
//	global_socket = socket (PF_INET, SOCK_STREAM, 6);

	 if (global_socket < 0)
	 {
		fprintf(stderr, "socket creation failed\n");
		return -1;
	 }

#ifndef NOMAIN
	printf("host = %s global_socket = %d\n",host,global_socket);
	printf("trying to connect...\n");
#endif
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

		if (setsockopt (global_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval)) < 0)
			return -2;
		if (setsockopt (global_socket, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(struct timeval)) < 0)
			return -3;
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
#ifndef NOMAIN
int main(int argc, char *argv[])
{
	char test1,test2;
	int rc;
	int i,j;
	char errmsg[20];
	char host[20];
	int port;
	memset(errmsg,0,20);

	if(argc < 2)
	{
		printf("usage: %s <server's IP address> [portno]\n",argv[1]);
		printf("or if just ip address, then portno defaults to 5193\n");
		exit(1);
	}
	if(argc > 2)
		port = atoi(argv[2]);
	else
		port = PROTOPORT;

	strcpy(host,argv[1]);

	rc = init_client(host,port);

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

	test1 = 0x21;
	j = 0;
	for(i = 0;i < 10000;i++)
	{
		rc = put_sock(&test1,1,1,errmsg);
		if(rc < 0)
			printf("%s\n",errmsg);

//		usleep(TIME_DELAY);		// defined in mytypes.h
		usleep(10000);
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

