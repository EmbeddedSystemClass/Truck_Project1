#define closesocket close
//#define PROTOPORT         5193          /* default protocol port number */
#define PROTOPORT         1003          /* default protocol port number */
#define QLEN              6             /* size of request queue        */
static int sd2;
int recv_tcp(UCHAR *recv_buf, int len, char *errmsg);
int send_tcp(UCHAR *send_buf,int len, char *errmsg);
int init_server(void);
int close_sock(void);


