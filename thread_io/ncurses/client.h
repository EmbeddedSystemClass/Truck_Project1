int test_sock(void);
int init_client(char *host);
int tcp_connect(void);
int put_sock(UCHAR *buf,int buflen, int block, char *errmsg);
int get_sock(UCHAR *buf, int buflen, int block, char *errmsg);
void close_sock(void);

#define HOST1 "192.168.42.145"
#define HOST2 "192.168.42.146"
#define Host3 "192.168.42.148"
#ifdef NOMAIN
#warning "NOMAIN defined"
#else
#warning "NOMAIN not defined"
#endif
