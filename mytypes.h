#ifndef MYTPYES_H
#define MYTPYES_H

typedef unsigned char UCHAR;
typedef unsigned int UINT;
typedef UCHAR* PUCHAR;
typedef unsigned long ULONG;

#define TIME_DELAY   990000000L

// this is the same as NUM_PORT_BITS (should be...)
#define NUM_DATA_RECS 40

#define NUM_DAT_NAMES 45
#define DAT_NAME_STR_LEN 25
//#define TDATE_STAMP_STR_LEN 25
#define TDATE_STAMP_STR_LEN 16
#define UPLOAD_BUFF_SIZE 10000
#define NUM_ADC_CHANNELS 11

// tcp command sent to TS-7200 from laptop
// these have to exactly match what's in cmd_array[] in tasks.c
enum cmd_types
{
	RE_ENTER_PASSWORD,
	ENABLE_START,
	STARTER_OFF,
	ON_ACC,
	OFF_ACC,
	ON_ESTOP,
	OFF_ESTOP,
	ON_FUEL_PUMP,
	OFF_FUEL_PUMP,
	ON_FAN,
	OFF_FAN,
	ON_LIGHTS,
	OFF_LIGHTS,
	START_SEQ,
	SHUTDOWN,
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
	TEST_WRITE_FILE,
	TOTAL_UP_TIME,
	UPLOAD_NEW,
	GET_DEBUG_INFO,
	GET_DEBUG_INFO2,
	NEW_PASSWORD1,
	SET_SERIAL_RECV_ON,
	SET_SERIAL_RECV_OFF,
	TEST_ALL_IO,
	_RESET_TYPE4,
	EXIT_PROGRAM
}CMD_TYPES;

typedef struct
{
	int cmd;
	char cmd_str[20];
} CMD_STRUCT;

// msg's sent from PIC24 to TS-7200
enum upstream_msg
{
	RT_DATA = 0x21,
	RPM_DATA,
	ABLE_TO_START,
	RPM_OK,
	RPM_TOO_LOW,
	RPM_TOO_HIGH,
	OIL_PRESS_TOO_LOW,
	ENGINE_TEMP_TOO_HIGH,
	RESET_TYPE4
} UPSTREAM_MSG;

// msg's sent from TS-7200 to PIC24
// each cmd must be followed by 4 bytes of data
// using send_PIC_serialother()
enum downstream_msg
{
	COMM_CMD = 0x3F,
	RE_ENTER_PASSWORD1,
	ESTOP_MSG,
	OUTPUT_MSG,
	TIME_DATA1,
	TIME_DATA2,
	TIME_DATA3,
	NEW_PASSWORD2,
	NEW_PASSWORD3,
	NEW_PASSWORD4,
} DOWNSTREAM_MSG;

// 25 total output_types
// until now the input labels match the outputs
// this is not always the case
// the labels for the I_DATA types
// are the same up to TESTOUTPUT25
// then all the labels for the inputs
// are called TESTINPUTXX

// also need to insert signals for fuelpump and ignition interrupt relays
// controlled by the FPGA in series with primary fuelpump and ignition relays (1 & 2)
enum output_types
{
	STARTER,				// 0
	ACCON,					// 1
	FUELPUMP,				// 2
	COOLINGFAN,				// 3
	HEADLAMP,				// 4
	BRIGHTS,				// 5
	LEFTBLINKER,			// 6
	RIGHTBLINKER,			// 7
	RUNNINGLIGHTS,			// 8
	ALARMSPEAKER,			// 9
	ALARMLIGHT,				// 10
	BATTERYCHARGERELAY,		// 11
	DASHHEATER,				// 12
	BATTERYCOMPHEATER,		// 13
	CRANKCASEHEATER,		// 14
	TRAILERBRAKES,			// 15
	TRAILERLEFTBLINKER,		// 16
	TRAILERRIGHTBLINKER,	// 17
	INTRUDERALARM,			// 18
	ESTOPSWITCH,			// 19
	LIGHTBAR,				// 20
	BLINKINDICATE,			// 21
	BRAKELIGHTS,			// 22
	BACKUPLIGHTS,			// 23
	TESTOUTPUT24,			// 24
	TESTOUTPUT25,			// 25
	TESTOUTPUT26,			// 26
	TESTOUTPUT27,			// 27
	TESTOUTPUT28,			// 28
	TESTOUTPUT29,			// 29
	TESTOUTPUT30,			// 30
	TESTOUTPUT31,			// 31
	TESTOUTPUT32,			// 32
	TESTOUTPUT33,			// 33
	TESTOUTPUT34,			// 34
	TESTOUTPUT35,			// 35
	TESTOUTPUT36,			// 36
	TESTOUTPUT37,			// 37
	TESTOUTPUT38,			// 38
	NULL2					// 39
}OUTPUT_TYPES;


#define CHAR_CMD				2
#define GOTO_CMD				3
#define SET_MODE_CMD	 		4
#define LCD_CLRSCR				5
#define LCD_MSG1				6
#define BURN_EEPROM				7	// these are used by the eeprom/burn_main.c program
#define READ_EEPROM				8
#define EEPROM_STR				9
#define SEND_BYTE_RT_VALUES		10
#define SEND_BYTE_HEX_VALUES	11
#define SEND_INT_RT_VALUES		12
#define LCD_CLRSCR2				13

#endif
