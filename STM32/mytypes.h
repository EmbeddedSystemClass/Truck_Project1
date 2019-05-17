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

// msg's sent from client to TS-7200
enum cmd_types
{
	NON_CMD,
	ENABLE_START,		// 1
	STARTER_OFF,		// 2
	ON_ACC,				// 3
	OFF_ACC,			// 4
	ON_FUEL_PUMP,		// 5
	OFF_FUEL_PUMP,		// 6
	ON_FAN,				// 7
	OFF_FAN,			// 8
	ON_LIGHTS,			// 9
	OFF_LIGHTS,			// 10
	ON_BRIGHTS,			// 11
	OFF_BRIGHTS,		// 12
	ON_BRAKES,
	OFF_BRAKES,
	ON_RUNNING_LIGHTS,
	OFF_RUNNING_LIGHTS,
	SPECIAL_CMD,
	START_SEQ,
	SHUTDOWN,
	ON_LLIGHTS,
	OFF_LLIGHTS,
	ON_LBRIGHTS,
	OFF_LBRIGHTS,
	ON_RLIGHTS,
	OFF_RLIGHTS,
	ON_RBRIGHTS,
	OFF_RBRIGHTS,
	SHUTDOWN_IOBOX,
	REBOOT_IOBOX,
	TEST_ALL_IO,
	SEND_ODATA,
	SAVE_TO_DISK,
	BLOWER_OFF,
	BLOWER1,
	BLOWER2,
	BLOWER3,
	WIPER1,
	WIPER2,
	WIPER_OFF,
	GET_DIR,
	LCD_SHIFT_RIGHT,
	LCD_SHIFT_LEFT,
	SCROLL_UP,
	SCROLL_DOWN,
	ENABLE_LCD,
	SET_TIME,
	GET_TIME,
	UPLOAD_NEW,
	NEW_PASSWORD1,
	SET_SERIAL_RECV_ON,
	SET_SERIAL_RECV_OFF,
	TEST_LEFT_BLINKER,
	TEST_RIGHT_BLINKER,
	RE_ENTER_PASSWORD,
	DISCONNECT,
	STOP_MBOX_RECV,
	CLOSE_DB,
	OPEN_DB,
	BAD_MSG,
	CURRENT_TIME,
	SET_PARAMS,
	EXIT_PROGRAM
}CMD_TYPES;

// msg's sent to client
enum client_cmds
{
	SEND_MSG = 60,
	CURRENT_TIME2,
	ENGINE_RUNTIME,
	SERVER_UPTIME,
	SEND_CONFIG,
	GET_TIME2
}CLIENT_CMDS;

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
	START_DS_MSG = 0xD0,		// 208
	STOP_SERIAL_RECV,			// 209
	SEND_PARAMS,				// 210
	OTHER_DATA,					// 211
	ESTOP_SIGNAL,				// 212
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
	ESTOP_INPUT,				// 7
	BRAKE_INPUT					// 8
}INPUT_TYPES;
	
enum output_types
{
	STARTER,			// bank 0
	ACCON,
	FUELPUMP,
	COOLINGFAN,
	LHEADLAMP,
	RHEADLAMP,
	RUNNINGLIGHTS,
	LIGHTBAR,

	LEFTRBLINKER,		// bank 1
	RIGHTRBLINKER,
	LEFTFBLINKER,
	RIGHTFBLINKER,
	LBRAKELIGHT,
	RBRAKELIGHT,
	XLBLINKER,
	XRBLINKER,

	LBRIGHTS,			// bank 2
	RBRIGHTS,
	HTRBLOWERLOW,
	HTRBLOWERMED,
	HTRBLOWERHIGH,
	WWIPER1,
	WWIPER2,
	TRLEFTBLINKER,

	TRRIGHTBLINKER,		// bank 3
	INTRUDERALARM,
	BLINKINDICATE,
	BATTERYCOMPHEATER,
	ALARMSPEAKER,
	BACKUPLIGHTS,
	TESTOUTPUT27,

	TESTOUTPUT28,		// bank 4
	TESTOUTPUT29,
	TESTOUTPUT30,
	TESTOUTPUT31,
	TESTOUTPUT32,
	TESTOUTPUT33,
	TESTOUTPUT34,
	TESTOUTPUT35,
	NULL2
}OUTPUT_TYPES;


#define LEN 30
#define AVR_BUF_LEN 15
#define NUM_STR 32	// no. of strings in eeprom (AVR_t6963/eeprom/main_burn.c)

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
#define EEPROM_STR2				10
#define SEND_BYTE_RT_VALUES		11
#define SEND_BYTE_HEX_VALUES	12
#define SEND_INT_RT_VALUES		13
#define LCD_CLRSCR2				14
#define SHOW_EEPROM				15
#define PASSWORD_MODE			16
#define SET_NUM_ENTRY_MODE		17
#define DISPLAY_STR				18
#define DISPLAY_RTLABELS		19
#define DISPLAY_STATUSLABELS	20
#define DISPLAY_ELAPSED_TIME	21
#define DISPLAY_TEMP			22

// offsets into eeprom
#define RT_VALUES_OFFSET 1
#define STATUS_VALUES_OFFSET 14

#define START_ROW_STATUS 0
#define STATUS_ENGINE START_ROW_STATUS
#define STATUS_COOLING_FAN STATUS_ENGINE+1
#define STATUS_HEAD_LIGHTS STATUS_ENGINE+2
#define STATUS_BRIGHTS STATUS_ENGINE+3
#define STATUS_BRAKES STATUS_ENGINE+4
#define STATUS_RUNNING_LIGHTS STATUS_ENGINE+5
#define STATUS_BLOWER STATUS_ENGINE+6

#define VARIOUS_MSG_OFFSET 21
#define STATUS_ON VARIOUS_MSG_OFFSET+4
#define STATUS_OFF VARIOUS_MSG_OFFSET+5
#define NO_MENUS 2

// start positions on screen
#define NUM_RT_LABELS 13
#define START_RT_VALUE_ROW 8
#define START_RT_VALUE_COL 1
#define ENDING_RT_VALUE_ROW 15
#define RT_VALUE_COL_WIDTH 15

#define NUM_STATUS_LABELS 7
#define START_STATUS_VALUE_ROW 1
#define START_STATUS_VALUE_COL 2
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

typedef struct params
{
	int rpm_update_rate;	// update rates for LED displays
	int mph_update_rate;
	int fpga_xmit_rate;
	int high_rev_limit;
	int low_rev_limit;
	int cooling_fan_on;
	int cooling_fan_off;
	int blower_enabled;
	int blower1_on;
	int blower2_on;
	int blower3_on;
	UCHAR test_bank;

}PARAM_STRUCT;

// commands for RPM/MPH LED display
#define RPM_SEND_CHAR_CMD 			0x01
#define MPH_SEND_CHAR_CMD 			0x02

#define RPM_OFF_CMD 				0x04
#define RPM_SET_BRIGHTNESS_CMD 		0x05
#define RPM_SET_CDECIMAL_CMD 		0x06
#define RPM_SET_UPDATE_RATE_CMD 	0x07
#define RPM_SET_FACTOR_CMD 			0x08
#define RPM_SET_FACTORY_RESET		0x09

#define MPH_OFF_CMD 				0x0A
#define MPH_SET_BRIGHTNESS_CMD 		0x0C
#define MPH_SET_CDECIMAL_CMD 		0x0D
#define MPH_SET_UPDATE_RATE_CMD 	0x0E
#define MPH_SET_FACTOR_CMD 			0x0F
#define MPH_SET_FACTORY_RESET		0x10

// other FPGA commands
#define SET_FPGA_SEND_UPDATE_RATE 	0x20
#define DTMF_TONE_ON 				0x21
#define DTMF_TONE_OFF 				0x22
#define SPECIAL_TONE_ON 			0x23
#define LCD_PWM 					0x24
#define FP_SHUTOFF_OVERRIDE 		0x25
#define SET_MAX_REV_LIMITER 		0x26
#define SET_MIN_REV_LIMITER 		0x27
#define TEST_RPM_LIMIT 				0x28
#define TEST_COMM					0x29
#define TUNE_ON						0x2A
#define TUNE_OFF					0x2B

#define PWM_OFF_PARAM					0x01 // off
#define PWM_ON_PARAM					0x1F // on
#define PWM_80DC_PARAM					0x1A // duty_cycle = 80%
#define PWM_75DC_PARAM					0x16 // duty_cycle = 75%
#define PWM_60DC_PARAM					0x12 // duty_cycle = 60%
#define PWM_50DC_PARAM					0x0E // duty_cycle = square wave
#define PWM_30DC_PARAM					0x0A // duty_cycle = 30%
#define PWM_25DC_PARAM					0x06 // duty_cycle = 25%
#define PWM_12DC_PARAM					0x02 // duty_cycle = 12%

#endif
