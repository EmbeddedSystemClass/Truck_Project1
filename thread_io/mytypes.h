#ifndef MYTPYES_H
#define MYTPYES_H
typedef unsigned char UCHAR;
typedef unsigned int UINT;

//#define TIME_DELAY 100000	// 100ms
//#define TIME_DELAY 50000	// 50ms
//#define TIME_DELAY 25000	// 25ms
#define TIME_DELAY 10000	// 10ms

// this is the same as NUM_PORT_BITS (should be...)
#define NUM_DATA_RECS 40

enum cmd_types
{
	SEND_IDATA = 0x70,
	SEND_ODATA,
	SHOW_IDATA,
	SHOW_ODATA,
	SEND_ALL_IDATA,
	SEND_ALL_ODATA,
	RESTORE,
	LOAD_ORG,
	SEND_SERIAL,
	CLOSE_SOCKET,
	CLEAR_SCREEN,
	SAVE_TO_DISK,
	TOGGLE_OUTPUTS,
	ALL_OFF,
	ALL_ON,
	EXIT_PROGRAM
}CMD_TYPES;
#endif
