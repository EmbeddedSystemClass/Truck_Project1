#define closesocket close
#define PROTOPORT         5193          /* default protocol port number */
//#define PROTOPORT         1003          /* default protocol port number */
#define QLEN              6             /* size of request queue        */
static int global_socket;
static int sock_open;
int recv_tcp(UCHAR *recv_buf, int len, char *errmsg);
int send_tcp(UCHAR *send_buf,int len, char *errmsg);
int init_server(void);
int recv_tcp(UCHAR *recv_buf, int len, char *errmsg);
int test_sock(void);


