#ifndef MYTPYES_H
#define MYTPYES_H
typedef unsigned char UCHAR;
typedef unsigned int UINT;

//#define TIME_DELAY 000000000
#define TIME_DELAY   990000000L

// this is the same as NUM_PORT_BITS (should be...)
#define NUM_DATA_RECS 40

#define NUM_DAT_NAMES 45
#define DAT_NAME_STR_LEN 25
//#define TDATE_STAMP_STR_LEN 25
#define TDATE_STAMP_STR_LEN 16
#define UPLOAD_BUFF_SIZE 10000

// these have to exactly match what's in cmd_array[] in tasks.c
enum serial_cmd_types
{
	GET_CURRENT_OUTPUT = 0x7F,
	TEST1,
	TEST2,
	TEST3,
	TEST4
}SERIAL_CMD_TYPES;

enum cmd_types
{
	ENABLE_START,
	STARTER_OFF,
	ON_FUEL_PUMP,
	OFF_FUEL_PUMP,
	ON_FAN,
	OFF_FAN,
	ON_ACC,
	OFF_ACC,
	START_SEQ,
	SEND_IDATA,
	SEND_ODATA,
	EDIT_IDATA,
	EDIT_ODATA,
	EDIT_IDATA2,
	EDIT_ODATA2,
	SEND_ALL_IDATA,
	SEND_ALL_ODATA,
	RECV_ALL_IDATA,
	RECV_ALL_ODATA,
	SHOW_IDATA,
	SHOW_ODATA,
	SEND_SERIAL,
	CLOSE_SOCKET,
	CLEAR_SCREEN,
	SAVE_TO_DISK,
	TOGGLE_OUTPUTS,
	SHUTDOWN,
	GET_DIR,
	LCD_SHIFT_RIGHT,
	LCD_SHIFT_LEFT,
	SCROLL_UP,
	SCROLL_DOWN,
	ENABLE_LCD,
	SET_TIME,
	GET_TIME,
	TCP_WINDOW_ON,
	TCP_WINDOW_OFF,
	LIVE_WINDOW_ON,
	LIVE_WINDOW_OFF,
	UPLOAD_NEW,
	EXIT_PROGRAM,
	BLANK
}CMD_TYPES;

typedef struct
{
	int cmd;
	char cmd_str[20];
} CMD_STRUCT;

enum output_types
{
	STARTER,
	ACCON,
	COOLINGFAN,
	FUELPUMP,
	PRELUBE,
	HEADLAMP,
	BRIGHTS,
	LEFTBLINKER,
	RIGHTBLINKER,
	RUNNINGLIGHTS,
	ALARMSPEAKER,
	ALARMLIGHT,
	BATTERYCHARGERELAY,
	DASHHEATER,
	BATTERYCOMPHEATER,
	CRANKCASEHEATER,
	TRAILERBRAKES,
	TRAILERLEFTBLINKER,
	TRAILERRIGHTBLINKER,
	INTRUDERALARM,
	DOMELIGHT,
	LIGHTBAR,
	BLINKINDICATE,
	BRAKELIGHTS,
	BACKUPLIGHTS
}OUTPUT_TYPES;

#endif
