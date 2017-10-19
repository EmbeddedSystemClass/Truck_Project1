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

#define NUM_DAT_NAMES 45
#define DAT_NAME_STR_LEN 25
//#define TDATE_STAMP_STR_LEN 25	// "Tue Oct 17 16:30:37 2017\0"
#define TDATE_STAMP_STR_LEN 16		// "Oct 17 16:30:37\0"

enum cmd_types
{
	SEND_IDATA,
	SEND_ODATA,
	EDIT_IDATA,
	EDIT_ODATA,
	SEND_ALL_IDATA,
	SEND_ALL_ODATA,
	SEND_SERIAL,
	CLOSE_SOCKET,
	CLEAR_SCREEN,
	SAVE_TO_DISK,
	TOGGLE_OUTPUTS,
	ALL_OFF,
	ALL_ON,
	GET_DIR,
	EXIT_PROGRAM
}CMD_TYPES;

#endif
