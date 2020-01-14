#ifndef _SERIAL_IO_H
#define _SERIAL_IO_H
int global_handle;
int global_handle2;
int global_handle3;

int init_serial(void);
int write_serial(UCHAR byte);
void printString(const char myString[]);
UCHAR read_serial(char *errmsg);
void close_serial(void);

int init_serial2(void);
int write_serial2(UCHAR byte);
void printString2(char *myString);
UCHAR read_serial2(char *errmsg);
void close_serial2(void);

int init_serial3(void);
int write_serial3(UCHAR byte);
void printString3(char *myString);
UCHAR read_serial3(char *errmsg);
void close_serial3(void);

void printHexByte(UCHAR byte);
char nibbleToHexCharacter(UCHAR nibble);
#endif
