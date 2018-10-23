// example of the server receiving the entire O_DATA record from the client
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>

#define PROTOPORT 8000
#define BUF_SIZE 5000

unsigned char buf[BUF_SIZE];
//static int buf2[BUF_SIZE];

typedef unsigned char UCHAR;

int error(char *msg)
{
    perror(msg);
    exit(1);
}
int WriteFile(char *filename, UCHAR *buf, int size);

int main(int argc, char *argv[])
{

	int ret = -1;
	struct timeval tv;

	int socket_fd;
	unsigned int msg_size;
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
	char tempx[500];
	char tempx2[10];
	int rc;
	int option = 1;
	time_t curtime2;
	struct timeval mtv;
	char *fptr;
	int fp = -1;
	char filename[] = "test.bin\0";
	fptr = (char *)filename;
	
	int temp;
/*
		curtime2 = 0L;
		gettimeofday(&mtv,NULL);
		curtime2 = mtv.tv_sec;
		strftime(tempx,30,"%m-%d-%Y %T\0",localtime(&curtime2));
		printf("%s\n",tempx);
	exit(0);
*/
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
	setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
//	listen_sd = socket (AF_LOCAL, SOCK_STREAM, ptrp->p_proto);

	if (listen_sd < 0)
	{
		printf("socket creation failed\n");
//			exit(1);
	}

/* Bind a local address to the socket */
	if ((rc = bind(listen_sd, (struct sockaddr *)&sad, sizeof (sad))) < 0)
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



// client sends a preamble of "0xF8 0xF0 0xF0 0xF0 0xF0 0xF0 0xF0 0"
// and then sends the msg size as an int (2 bytes)
// and then sends (6) zeros
// and then send the msg in int's
//#if 0
//	for(j = 0;j < 3;j++)
	ret = 0;
//	fp = open((const char *)fptr, O_RDWR | O_CREAT | O_TRUNC, 
//						S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

//	fp = open((const char *)fptr, O_RDWR | O_CREAT);
/*
	if(fp < 0)
	{
		close(fp);
		printf("file error\n");
		return -2;
	}
	j = 0;
	i = lseek(fp,0,SEEK_SET);
*/
	k = 0;
	for(i = 0;i < 4;i++)
	{
		memset(buf,0,BUF_SIZE);
		ret = recv(socket_fd,(void*)&buf[0],8,MSG_WAITALL);
		for(l = 0;l < 8;l++)
			printf("%02x ",buf[l]);
		printf("\n");	
		ret = recv(socket_fd,(void*)&msg_size,2,MSG_WAITALL);
		printf("\nmsg size: %d\n",msg_size);
		recv(socket_fd,&buf[0],6,MSG_WAITALL);	// read (6) 0's
		for(l = 0;l < 6;l++)
			printf("%02x ",buf[l]);

		printf("\n");	

		if(msg_size > 20000)
			msg_size = 20000;
		recv(socket_fd,(void*)&buf[0],(size_t)msg_size,MSG_WAITALL);
//		usleep(1000);
		
		k = 0;
		for(j = 0;j < msg_size;j+=2)
		{
			tempx[k++] = buf[j];			
			printf("%02x ",buf[j]);
		}
		tempx[k] = 0;
		printf("\n");
	
//		j = write(fp,(const void*)&buf[0],msg_size);
//		printf("\n%s\n",tempx);
	}
	close(socket_fd);
	exit(0);
	memset(buf,0,BUF_SIZE);
	ret = recv(socket_fd,(void*)&buf[0],8,MSG_WAITALL);
	ret = recv(socket_fd,(void*)&msg_size,2,MSG_WAITALL);
	printf("\nmsg size: %d\n",msg_size);
	recv(socket_fd,(void*)&buf,6,MSG_WAITALL);	// read (6) 0's
	recv(socket_fd,(void*)&buf[0],msg_size,MSG_WAITALL);
//	usleep(1000);
//	j = write(fp,(const void*)buf,msg_size);
	close(socket_fd);
//	close(fp);
	exit(0);
}
