int test_sock(void);
int init_client(char *host, char *errmsg);
int tcp_connect(void);
int put_sock(UCHAR *buf,int buflen, int block, char *errmsg);
int get_sock(UCHAR *buf, int buflen, int block, char *errmsg);
void close_sock(void);

#define HOST145 "192.168.42.145"
#define HOST146 "192.168.42.146"
#define HOST148 "192.168.42.148"
#define HOST149 "192.168.42.149"
#define HOST124 "192.168.42.124"
#define HOST116 "192.168.42.116"
#define Host_Sim "192.168.42.110"				  // simulated host on linux box
#ifdef NOMAIN
#warning "NOMAIN defined"
#else
#warning "NOMAIN not defined"
#endif
