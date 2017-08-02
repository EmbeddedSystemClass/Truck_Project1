int init_serial(void);
int write_serial(UCHAR byte);
void printString(const char myString[]);
int read_serial(UCHAR *byte);
void close_serial(void);
int global_handle;
