int init_serial(void);
int write_serial(UCHAR byte);
void printString(const char myString[]);
UCHAR read_serial(void);
void close_serial(void);
int global_handle;
