int test_sock(void);
int init_client(char *host);
int tcp_connect(void);
int put_sock(UCHAR *buf,int buflen, int block, char *errmsg);
int get_sock(UCHAR *buf, int buflen, int block, char *errmsg);
void close_sock(void);

#define HOST1 "192.168.42.145"	// TS-7800 with 2 IO cards used for Truck_Project
#define HOST2 "192.168.42.146"	// TS-7800 with just one IO card (for now)
#define Host3 "192.168.42.148"	// TS-7200 with 2 IO cards
#define Host_Sim "192.168.42.110"	// simulated host on linux box
#ifdef NOMAIN
#warning "NOMAIN defined"
#else
#warning "NOMAIN not defined"
#endif
