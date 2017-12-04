#ifdef CONSOLE_DISABLED
#warning "CONSOLE_DISABLED"
#endif
int init_serial(void);
int write_serial(UCHAR byte);
void printString(const char myString[]);
UCHAR read_serial(void);
void close_serial(void);
int global_handle;
void serprintf1(char *str);
void serprintf2(char *str, int x);
void serprintf3(char *str, int x, int y);
void serprintf4(char *str, int x, int y, int z);

