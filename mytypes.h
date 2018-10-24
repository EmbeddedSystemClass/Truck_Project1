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

typedef struct
{
	int cmd;
	char cmd_str[30];
} CMD_STRUCT;

#define OLABELSIZE 30

typedef struct _ip
{
	int port;
	int input;
	char label[OLABELSIZE];
}IP;

enum cmd_types
{
	ENABLE_START = 1,
	STARTER_OFF,
	ON_ACC,
	OFF_ACC,
	ON_FUEL_PUMP,
	OFF_FUEL_PUMP,
	ON_FAN,
	OFF_FAN,
	ON_LIGHTS,
	OFF_LIGHTS,
	ON_BRIGHTS,
	OFF_BRIGHTS,
	ON_BRAKES,
	OFF_BRAKES,
	ON_RUNNING_LIGHTS,
	OFF_RUNNING_LIGHTS,
	START_SEQ,
	SHUTDOWN,
	SHUTDOWN_IOBOX,
	TEMP,
	REBOOT_IOBOX,
	SEND_ODATA,
	EDIT_ODATA,
	EDIT_ODATA2,
	SEND_ALL_ODATA,
	RECV_ALL_ODATA,
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
	TOTAL_UP_TIME,
	UPLOAD_NEW,
	GET_DEBUG_INFO,
	GET_DEBUG_INFO2,
	NEW_PASSWORD1,
	SET_SERIAL_RECV_ON,
	SET_SERIAL_RECV_OFF,
	TEST_ALL_IO,
	TEST_LEFT_BLINKER,
	TEST_RIGHT_BLINKER,
	RE_ENTER_PASSWORD,
	CLOSE_DB,
	OPEN_DB,
	BAD_MSG,
	SEND_NL,
	EXIT_PROGRAM
}CMD_TYPES;


// msg's sent from PIC24 to TS-7200
enum upstream_msg
{
	RT_DATA = 0xD0,
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
	START_DS_MSG = 0xD0,
	RE_ENTER_PASSWORD1,
	GET_DEBUG_INFOA,
	GET_DEBUG_INFO2A,
	OUTPUT_MSG,
	TIME_DATA1,
	TIME_DATA2,
	TIME_DATA3,
	NEW_PASSWORD2,
	NEW_PASSWORD3,
	NEW_PASSWORD4,
	CLEAR_SCREEN1,
	END_DS_MSG
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
enum input_types
{
	HEADLAMP_INPUT,				// 0
	RUNNING_LIGHTS_INPUT,		// 1
	LEFTBLINKER_INPUT,			// 2
	RIGHTBLINKER_INPUT,			// 3
	BRIGHTS_INPUT,				// 4
	STARTER_INPUT,				// 5
	COOLINGFAN_INPUT,			// 6
	ESTOP_INPUT					// 7
}INPUT_TYPES;
	
enum output_types
{
	STARTER,
	ACCON,
	FUELPUMP,
	COOLINGFAN,
	LHEADLAMP,
	LBRIGHTS,
	RHEADLAMP,
	RBRIGHTS,
	LEFTBLINKER,
	RIGHTBLINKER,
	RUNNINGLIGHTS,
	RBRAKELIGHT,
	LBRAKELIGHT,
	XLBLINKER,
	XRBLINKER,
	BATTERYCOMPHEATER,
	CRANKCASEHEATER,
	ESTOPSWITCH,
	BATTERYHEATER,
	TRLEFTBLINKER,
	TRRIGHTBLINKER,
	INTRUDERALARM,
	LIGHTBAR,
	BLINKINDICATE,
	ALARMSPEAKER,
	BACKUPLIGHTS,
	TESTOUTPUT24,
	TESTOUTPUT25,
	TESTOUTPUT26,
	TESTOUTPUT27,
	TESTOUTPUT28,
	TESTOUTPUT29,
	TESTOUTPUT30,
	TESTOUTPUT31,
	TESTOUTPUT32,
	TESTOUTPUT33,
	TESTOUTPUT34,
	TESTOUTPUT35,
	TESTOUTPUT36,
	NULL2
}OUTPUT_TYPES;


#define LEN 200

#define COLUMN              40      //Set column number to be e.g. 32 for 8x8 fonts, 2 pages
#define ROWS                16
#define SIZE_NUM			20		// size of buffer used by num entry mode
#define PASSWORD_SIZE 13

// messages sent from PIC24 to AVR using the AVR_cmd task
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
#define SHOW_EEPROM				14
#define PASSWORD_MODE			15
#define DISPLAY_MENU_LABELS		16
#define DISPLAY_RT_LABELS		17
#define SET_NUM_ENTRY_MODE		18

// offsets into eeprom
#define RT_VALUES_OFFSET 1
#define MENU_VALUES_OFFSET 23
#define VARIOUS_MSG_OFFSET 12
#define NO_MENUS 2

// start positions on screen
#define START_RT_VALUE_ROW 10
#define START_MENU_VALUE_ROW 1
#define START_RT_VALUE_COL 2
#define START_MENU_VALUE_COL 2
#define NUM_MENU_LABELS 12
#define NUM_RT_LABELS 11
/*
8) set time/date		(num entry)
*) time till off eng	(num entry)
#) fpga send rate		(num entry)

menu 2: - screen dim	(num entry)
1) rpm/mph update rate	(num entry)
2) high rev limit		(num entry)
3) low rev limit		(num entry)
4) set lights off		(num entry)
6) change password		(num entry)
*/

enum num_entry_types
{
	TIME_DATE,
	TIME_ENG_OFF,
	FPGA_SEND_RATE,
	RPM_MPH_UPDATE_RATE,
	HIGH_REV_LIMIT,
	LOW_REV_LIMIT,
	SET_LIGHTS_OFF,
	CHANGE_PASSWORD
}NUM_ENTRY_TYPES;
	
#endif
