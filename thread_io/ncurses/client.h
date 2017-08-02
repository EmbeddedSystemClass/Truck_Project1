int test_sock(void);
int init_client(void);
int tcp_connect(void);
int put_sock(UCHAR *buf,int buflen, int block, char *errmsg);
int get_sock(UCHAR *buf, int buflen, int block, char *errmsg);
void close_sock(void);

#ifdef NOMAIN
#warning "NOMAIN defined"
#else
#warning "NOMAIN not defined"
#endif
