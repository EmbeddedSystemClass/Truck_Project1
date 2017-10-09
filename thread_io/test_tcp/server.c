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
#include "mytypes.h"
#include "tcp.h"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define LEN 200
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

/*	to avoid "server already in use" error:
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
//	setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
	if (sd < 0)
	{
		fprintf(stderr, "socket creation failed\n");
		return -1;
	}

	/* Bind a local address to the socket */
	if (bind(sd, (struct sockaddr *)&sad, sizeof (sad)) < 0)
	{
		fprintf(stderr,"bind failed\n");
		return -1;
	}

	/* Specify a size of request queue */
	if (listen(sd, QLEN) < 0)
	{
		fprintf(stderr,"listen failed\n");
		return -1;
	}

	/* Main server loop - accept and handle requests */
//	fprintf( stderr, "Server up and running.\n");
	sd2 = -1;

	do
	{
		alen = sizeof(cad);

		if (  (sd2=accept(sd, (struct sockaddr *)&cad, &alen)) < 0)
		{
			fprintf(stderr, "accept failed\n");
			closesocket(sd2);
			return -1;
		}
	}while(sd2 < 1);

	if (setsockopt (sd2, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval)) < 0)
		return -2;
	if (setsockopt (sd2, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(struct timeval)) < 0)
		return -3;

	return sd2;
}

//******************************************************************************************//
//***************************************** send_tcp ***************************************//
//******************************************************************************************//
int send_tcp(UCHAR *send_buf,int len, char *errmsg)
{
	int rc;
	char extra_msg[10];

//	rc = send(sd2, send_buf, len,MSG_DONTWAIT); // non-blocking
	rc = send(sd2, send_buf, len,MSG_WAITALL); // blocking
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

//	rc = recv(sd2, recv_buf, len, MSG_DONTWAIT);
	rc = recv(sd2, recv_buf, (size_t)len, MSG_WAITALL);
	if(rc < 0)
	{
		strcpy(errmsg,strerror(errno));
		sprintf(extra_msg," %d",errno);
		strcat(errmsg,extra_msg);
		strcat(errmsg," recv");
	}else strcpy(errmsg,"Success\0");
	return rc;
}
//******************************************************************************************//
//************************************** close_sock ****************************************//
//******************************************************************************************//
int close_sock(void)
{
	closesocket(sd2);
	return 1;
}

#ifdef TEST_SEQUENCE
int main(void)
{
	char test1;
	int rc;
	int i,j;
	char errmsg[20];
	memset(errmsg,0,20);

	init_server();

	j = 0;
//	for(i = 0;i < 1000;i++)
	i = 0;
	do
	{
		rc = recv_tcp((UCHAR *)&test1,1,errmsg);
		if(rc < 0)
			printf("%s\n",errmsg);

		printf("%c",test1);

		rc = send_tcp(&test1,1,errmsg);
		if(rc < 0)
			printf("%s\n",errmsg);
		if(++j > 92)
		{
			printf("\n");
			j = 0;
		}
		i++;
	}while(test1 != 0xff && i < 10000);

	close(sd2);
	return 0;
}
#endif

#ifdef COPY_FILE

//UCHAR buf[30000];
UCHAR *buf;

int main(void)
{
	char test1;
	int rc;
	int i,j;
	char errmsg[20];
	memset(errmsg,0,20);
	int fp;
	off_t fsize;
	char filename[20];
	UCHAR temp[20];

	init_server();

#if 0
	rc = recv_tcp((UCHAR *)&temp[0],20,errmsg);

	for(i = 0;i < 20;i++)
		printf("%2x ",temp[i]);

	printf("\n");

	printf("%s\n",temp);
#endif

	rc = recv_tcp((UCHAR*)&fsize,sizeof(off_t),errmsg);
	printf("sizeof(off_t): %d\n",sizeof(off_t));
//	rc = recv_tcp((UCHAR*)&fsize,4,errmsg);

	if(rc < 0)
	{
		printf("%s\n",errmsg);
		exit(1);
	}

	printf("fsize: %ld \n",fsize);

	buf = (UCHAR *)malloc(fsize);
	if(buf < 0)
	{
		printf("could not malloc %ld\n",fsize);
		exit(1);
	}
	memset(buf,0x20,fsize);

	strcpy(filename,"test.bin");
	fp = open((const char *)filename, O_RDWR | O_CREAT | O_TRUNC, 600);
	if(fp < 0)
	{
		printf("could not create file: %s\n",filename);
		exit(1);
	}

//	rc = recv_tcp((UCHAR *)&buf[0],4,errmsg);

	rc = recv_tcp((UCHAR *)&buf[0],fsize,errmsg);

	printf("bytes recv'd: %d\n",rc);
	if(rc < 0)
		printf("%s\n",errmsg);

	close(sd2);
	rc = write(fp, buf, fsize);
	printf("write to file: %d\n",rc);
	close(fp);
	free(buf);
	
	printf("\n\n");
	return 0;
}
#endif
