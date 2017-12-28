#ifdef CONSOLE_DISABLED
#warning "CONSOLE_DISABLED"

int init_serial2(void);
int write_serial2(UCHAR byte);
void printString2(const char myString[]);
UCHAR read_serial2(char *errmsg);
void close_serial2(void);
int global_handle2;

#endif

int init_serial(void);
int write_serial(UCHAR byte);
void printString(const char myString[]);
UCHAR read_serial(char *errmsg);
void close_serial(void);
int global_handle;


