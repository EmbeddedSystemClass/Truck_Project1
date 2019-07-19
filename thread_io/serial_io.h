int init_serial2(void);
int write_serial2(UCHAR byte);
void printString2(char *myString);
UCHAR read_serial2(char *errmsg);
void close_serial2(void);
int global_handle2;

int init_serial(void);
int write_serial(UCHAR byte);
void printString(const char myString[]);
UCHAR read_serial(char *errmsg);
//int read_serial_buff(UCHAR *buff, char *errmsg);
//int write_serial_buff(UCHAR *buff, char *errmsg);
void close_serial(void);
int global_handle;


