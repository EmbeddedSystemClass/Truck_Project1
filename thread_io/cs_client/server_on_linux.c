// example of the server receiving the entire O_DATA record from the client
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>

#define PROTOPORT 8000
#define BUF_SIZE 1000
int error(char *msg)
{
    perror(msg);
    exit(1);
}

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
	unsigned char buf[BUF_SIZE];
	int buf2[BUF_SIZE];
	int i,j,k,l;
	char tempx[40];
	char tempx2[10];
	int rc;
	int option = 1;
	
	int temp;

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
#if 0
	for(j = 0;j < 8;j++)
	{
		memset(buf,0,BUF_SIZE);
		memset(buf2,0,BUF_SIZE);
	// test receiving single record
		ret = recv(socket_fd,buf,8,MSG_WAITALL);
		// this reads 0xF8 0xF0(6x) 00
	//	for(i = 0;i < 8;i++)
	//		printf("%02x ",buf[i]);

	//		printf("sizeof(int) = %d\n",sizeof(int));	// returns '4'
		ret = recv(socket_fd,&msg_size,2,MSG_WAITALL);
	//	printf("msg size: %d ",msg_size);
		recv(socket_fd,&buf,6,MSG_WAITALL);	// read (6) 0's
		ret = 0;
		// reads msgs_size of int's
		// can't use sizeof(int) because it's 4 on this machine
		for(i = 0;i < msg_size/2;i++)
			ret += recv(socket_fd,&buf2[i],2,MSG_WAITALL);

	//		printf("ret: %d\n",ret);

		for(i = 0;i < msg_size/2;i++)
		{
	//		printf("%c",(char)buf2[i]);
			printf("%02x ",(char)buf2[i]);
			tempx[i] = (char)buf2[i];
		}
		tempx[i] = 0;
	//	printf("sent: %s\n",tempx);
		printf("\n");
	}
	close(socket_fd);
	exit(0);
#endif

// test receiving entire table

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
			recv(socket_fd,&buf[0],6,MSG_WAITALL);	// read (6) 0's
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
}
