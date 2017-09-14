//
// "I am not so much concerned with the return on capital as I am with the return of capital." - Will Rogers
//

#include "../esos/include/esos.h"
#include "../esos/include/pic24/esos_pic24.h"
#include "../esos/include/pic24/esos_pic24_rs232.h"
#include "../esos/include/pic24/esos_pic24_spi.h"
#include "../lib/include/pic24_timer.h"
#include "../lib/include/pic24_util.h"
#include "../lib/include/pic24_adc.h"
#include "../lib/include/pic24_ports_config.h"
#include "../lib/include/pic24_ports_mapping.h"
//#include "../../AVR_t6963/key_defs.h"
#include <stdio.h>
#include <string.h>
// DEFINEs go here
//#define   CONFIG_LED1()   printf("called CONFIG_LED1()\n");
uint8_t     LED1 = TRUE;      // LED1 is initially "on"
#define MY_ESOS1
#define TEMP_DELAY		5000
#define TEMP_DELAY3		2
#define TEMP_DELAY2		1000
#define TDELAY 2
#define TDELAY2 1
#define VREF 3.3  //assume Vref = 3.3 volts
#define FLAG1        ESOS_USER_FLAG_1
#define RT_OFFSET 0x70
//#define AVR_SEND_DATA_SIZE 1024	// this should be the same as AUX_STRING_LEN in AVR_t6963/main.h
#define AVR_SEND_DATA_SIZE 300
#define NO_MENU_LABELS_EEPROM_LOCATION 0x03e0
#define NO_RT_LABELS_EEPROM_LOCATION 0x03e4
#define NO_RTPARAMS_EEPROM_LOCATION 0x03e8
#define NO_MENUS_EEPROM_LOCATION 0x3ea
#define RT_PARAMS_OFFSET_EEPROM_LOCATION 0x0350
#define MAX_LABEL_LEN 10
#define RT_OFFSET 0x70

int pack(UCHAR low_byte, UCHAR high_byte);
void unpack(int myint, UCHAR *low_byte, UCHAR *high_byte);
int update_menu_structs(int i, UCHAR fptr, UCHAR menu0, UCHAR menu1, UCHAR menu2, UCHAR menu3,
			UCHAR menu4, UCHAR menu5, UCHAR index);

//extern UCHAR get_key(UCHAR ch, int size, int start_addr, UCHAR *str, int type);		// from ../../AVR_t6963/PIC_menu.c

enum data_types
{
	RT_LOW,				// uint8_t without high bit set
	RT_HIGH0,			// uint8_t with bit 7 set
	RT_HIGH1,			// UINT with neither 7 or 15 set
	RT_HIGH2,			// bit 7 of UINT set
	RT_HIGH3			// bit 15 of UINT set
} DATA_TYPES;

// warning: these are also defined in the AVR directory
// got mult defined errors if enums have the same names
// but got undefined errors if left out ???
typedef struct rt_params
{
	UCHAR row;			// row, col tells where the param will appear on screen
	UCHAR col;
	UCHAR shown;		// SHOWN_SENT = shown & sent; NOSHOWN_SENT = sent but not shown; NOSHOWN_NOSENT
	UCHAR dtype;		// 0 = UCHAR; 1 = UINT; 2 = dword?
	UCHAR type;			// rt_types
} RT_PARAM;

enum shown_types
{
	SHOWN_SENT,
	NOSHOWN_SENT,
	NOSHOWN_NOSENT
} SHOWN_TYPES;

enum key_types
{
	KP_POUND = 0xE0, // '#'
	KP_AST, // '*'	- E1
	KP_0, // '0'	- E2
	KP_1, // '1'	- E3
	KP_2, // '2'	- E4
	KP_3, // '3'	- E5
	KP_4, // '4'	- E6
	KP_5, // '5'	- E7
	KP_6, // '6'	- E8
	KP_7, // '7'	- E9
	KP_8, // '8'	- EA
	KP_9, // '9'	- EB
	KP_A, // 'A'	- EC
	KP_B, // 'B'	- ED
	KP_C, // 'C'	- EE
	KP_D, // 'D'	- EF
	INIT, //		- F0
	READ_EEPROM,//	- F1
	SPACE,	//		- F2
	SHOW_EEPROM, //	- F3
	SHOW_MENU_STRUCT, //- F4
	LOAD_MENU_STRUCT, // - F5
	BURN_PART,		// - F6
	BURN_PART1,		// - F7
	BURN_PART2,		// - F8
	BURN_PART3,		// - F9
	BURN_PART4		// - FA
} KEY_TYPES2;

enum non_func_type
{
	NF_1 = 0xB0,
	NF_2,
	NF_3,
	NF_4,
	NF_5,
	NF_6,
	NF_7,
	NF_8,
	NF_9,
	NF_10
} NON_FUNC_TYPES2;

enum rt_types
{
	RT_RPM = RT_OFFSET,
	RT_ENGT,
	RT_TRIP,
	RT_TIME,
	RT_AIRT,
	RT_MPH,
	RT_OILP,
	RT_MAP,
	RT_OILT,
	RT_O2,
} RT_TYPES;

enum menu_types
{
	MAIN,		// 0
	MENU1A,		// 1
	MENU1B,		// 2
	MENU1C,		// 3
	MENU1D,		// 4
	MENU1E,		// 5
	MENU2A,		// 6
	MENU2B,		// 7
	MENU2C,		// 8
	MENU2D,		// 9
	MENU2E,		// 10
	MENU3A,		// 11
	MENU3B,		// 12

	ckenter,	// 13
	ckup,		// 14
	ckdown,		// 15
	cktoggle,	// 16
	ckesc,		// 17

	entr,		// 18
	forward,	// 19
	back,		// 20
	eclear,		// 21
	esc,		// 22

	caps,		// 23
	small,		// 24
	spec,		// 25
	next,		// 26

	blank,		// 27
	rpm,
	engt,
	trip,
	time,
	airt,
	mph,
	oilp,
	map,
	oilt,
	o2,
	test
} MENU_TYPES;

enum fptr_types
{
	_menu_change,
	_exec_choice,
	_do_chkbox,
	_non_func,
	_do_numentry
} FPTR_TYPES;

#define NUM_MENUS 13

typedef struct menu_func
{
//	int _index;
	UCHAR fptr;								// which function to call (menu_types)
	int menus[6];							// which menu to goto if _menu_change is the fptr
	UCHAR index;
} MENU_FUNC_STRUCT;

MENU_FUNC_STRUCT menu_structs[NUM_MENUS];

volatile uint8_t row;
volatile uint8_t cmd;
volatile uint8_t data;
volatile UINT32 U32_lastCapture; // UINT32 declared in all_generic.h
#define KEYPAD_DELAY    100
#define KEYPAD_DEBOUNCE_DELAY 2

#define MAX_LABEL_LEN 10
#define NUM_LABELS 40
#define NUM_RT_PARAMS 10
volatile uint8_t col;
volatile uint8_t last_code;
volatile uint8_t user_flag;
/*
volatile uint16_t total_offset;
volatile uint16_t no_menu_labels;
volatile uint16_t no_rtparams;
volatile uint16_t no_rt_labels;
*/

volatile UCHAR avr_send_data[AVR_SEND_DATA_SIZE];

volatile char labels[NUM_LABELS][MAX_LABEL_LEN] =
//#if 0
{"home\0","MENU1a\0","MENU1b\0","MENU1c\0","MENU1d\0","MENU1e\0","MENU2a\0","MENU2B\0","MENU2c\0","MENU2d\0","MENU2e\0","MENU3a\0","MENU3b\0",\
\
"enter\0","up\0","down\0","toggle\0","esc\0","enter\0","forward\0","back\0","clear\0","escape\0",\
\
"caps\0","small\0","spec\0","next\0",\
\
"\0","RPM\0","ENG TEMP\0","TRIP\0","TIME\0","AIR TEMP\0","MPH\0","OIL PRES\0","MAP\0","OIL TEMP\0","O2\0","test\0",\
"oh f*ck\0"};
//#endif

RT_PARAM rt_params[NUM_RT_PARAMS] = {
 {1, 0, SHOWN_SENT, 1, RT_RPM},
 {2, 0, SHOWN_SENT, 0, RT_ENGT},
 {3, 0, SHOWN_SENT, 0, RT_TRIP},
 {4, 0, SHOWN_SENT, 0, RT_TIME},
 {5, 0, SHOWN_SENT, 0, RT_AIRT},
 {1, 15, SHOWN_SENT, 0, RT_MPH},
 {2, 15, SHOWN_SENT, 0, RT_OILP},
 {3, 15, SHOWN_SENT, 0, RT_MAP},
 {4, 15, SHOWN_SENT, 0, RT_OILT},
 {5, 15, SHOWN_SENT, 0, RT_O2} };


UCHAR get_key(UCHAR ch, int size, int start_addr, UCHAR *str, int type);

ESOS_USER_TASK(keypad);
ESOS_USER_TASK(poll_keypad);
ESOS_SEMAPHORE(key_sem);
ESOS_USER_TASK(comm1_task);
ESOS_USER_TASK(comm2_task);
ESOS_USER_TASK(comm3_task);
ESOS_USER_TASK(data_to_AVR);
ESOS_USER_TASK(send_cmd_param);
//ESOS_SEMAPHORE(send_sem);
ESOS_USER_TASK(convADC);
ESOS_USER_TASK(echo_spi_task);

//ESOS_USER_TASK(fast_echo_spi_task);

/*
RE0 - p93 - 5th from right inside top
RE1 - p94 - 4th from right outside top
RE2 - p98 - 2nd from right outside top
RE3 - p99 - 2nd from right inside top

RA0 - p17 - 5th up - outside right
RA1 - p38 - 7th from right - outside bottom
RA2 - p58 - 4th up from bottom - inside left
RA3 - p59 - 5th up from bottom - outside left
*/

#define C0 _RA0
#define C1 _RA1
#define C2 _RA2
#define C3 _RA3

// test

static inline void CONFIG_COLUMN()
{
	CONFIG_RA0_AS_DIG_INPUT();
	ENABLE_RA0_PULLUP();
	CONFIG_RA1_AS_DIG_INPUT();
	ENABLE_RA1_PULLUP();
	CONFIG_RA2_AS_DIG_INPUT();
	ENABLE_RA2_PULLUP();
	CONFIG_RA3_AS_DIG_INPUT();
	ENABLE_RA3_PULLUP();
}

#define R0 _LATE0
#define R1 _LATE1
#define R2 _LATE2
#define R3 _LATE3

#define CONFIG_R0_DIG_OUTPUT() CONFIG_RE0_AS_DIG_OUTPUT()
#define CONFIG_R1_DIG_OUTPUT() CONFIG_RE1_AS_DIG_OUTPUT()
#define CONFIG_R2_DIG_OUTPUT() CONFIG_RE2_AS_DIG_OUTPUT()
#define CONFIG_R3_DIG_OUTPUT() CONFIG_RE3_AS_DIG_OUTPUT()

void CONFIG_ROW()
{
	CONFIG_R0_DIG_OUTPUT();
	CONFIG_R1_DIG_OUTPUT();
	CONFIG_R2_DIG_OUTPUT();
	CONFIG_R3_DIG_OUTPUT();
}

static inline void DRIVE_ROW_LOW()
{
	R0 = 0;
	R1 = 0;
	R2 = 0;
	R3 = 0;
}

static inline void DRIVE_ROW_HIGH()
{
	R0 = 1;
	R1 = 1;
	R2 = 1;
	R3 = 1;
}

typedef enum
{
	STATE_WAIT_FOR_PRESS = 0x30,
	STATE_WAIT_FOR_PRESS2,
	STATE_WAIT_FOR_RELEASE,
} ISRSTATE;

volatile ISRSTATE e_isrState;
volatile uint8_t u8_newKey = 0;

void configKeypad(void)
{
	CONFIG_ROW();
	DRIVE_ROW_LOW();
	CONFIG_COLUMN();
	DELAY_US(1);								  //wait for pullups to stabilize inputs
	e_isrState = STATE_WAIT_FOR_PRESS;
}

//drive one row low
void setOneRowLow(uint8_t u8_x)
{
	switch (u8_x)
	{
		case 0:
			R0 = 0;
			R1 = 1;
			R2 = 1;
			R3 = 1;
			break;
		case 1:
			R0 = 1;
			R1 = 0;
			R2 = 1;
			R3 = 1;
			break;
		case 2:
			R0 = 1;
			R1 = 1;
			R2 = 0;
			R3 = 1;
			break;
		default:
			R0 = 1;
			R1 = 1;
			R2 = 1;
			R3 = 0;
			break;
	}
}
#define NUM_ROWS 4
#define NUM_COLS 4
const uint8_t au8_keyTable[NUM_ROWS][NUM_COLS] =
{
	{KP_1, KP_2, KP_3, KP_A },
	{KP_4, KP_5, KP_6, KP_B },
	{KP_7, KP_8, KP_9, KP_C },
	{KP_AST, KP_0, KP_POUND, KP_D}
};
//	{'1', '2', '3', 'A'},
//	{'4', '5', '6', 'B'},
//	{'7', '8', '9', 'C'},
//	{'*', '0', '#', 'D'}

#define KEY_PRESSED() (!C0 || !C1 || !C2 || !C3)  //any low
#define KEY_RELEASED() (C0 && C1 && C2 && C3)	  //all high

uint8_t doKeyScan(void)
{
	uint8_t u8_row, u8_col;
//determine column
	if (!C0) u8_col = 0;
	else if (!C1) u8_col = 1;
	else if (!C2) u8_col = 2;
	else if (!C3) u8_col = 3;
	else return('E');							  //error
//determine row
	for (u8_row = 0; u8_row < NUM_ROWS; u8_row++)
	{
		setOneRowLow(u8_row);					  //enable one row low
		if (KEY_PRESSED())
		{
			DRIVE_ROW_LOW();					  //return rows to driving low
			return(au8_keyTable[u8_row][u8_col]);
		}
	}
	DRIVE_ROW_LOW();							  //return rows to driving low
	return('E');								  //error
}

//******************************************************************************************//
//*************************************** keypad *******************************************//
//******************************************************************************************//
ESOS_USER_TASK(keypad)
{

    ESOS_TASK_BEGIN();

	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
	ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
	ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
    ESOS_TASK_WAIT_ON_SEND_STRING("keypad task");
	ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
	ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
	ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

	while(TRUE)
	{
		ESOS_TASK_WAIT_TICKS(1);
#if 0
		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(e_isrState);
		ESOS_TASK_WAIT_ON_SEND_UINT8(e_isrState);
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
#endif
		switch (e_isrState)
		{
			case STATE_WAIT_FOR_PRESS:
				if (KEY_PRESSED() && (u8_newKey == 0))
				{
	//ensure that key is sampled low for two consecutive interrupt periods
#if 0
					ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
					ESOS_TASK_WAIT_ON_SEND_STRING("wait for press");
					ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
					ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
					ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
#endif
					e_isrState = STATE_WAIT_FOR_PRESS2;
				}
				break;
			case STATE_WAIT_FOR_PRESS2:
				if (KEY_PRESSED())
				{
	// a key is ready
#if 0
					ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
					ESOS_TASK_WAIT_ON_SEND_STRING("key pressed");
					ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
					ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
					ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
#endif
					u8_newKey = doKeyScan();
					e_isrState = STATE_WAIT_FOR_RELEASE;
				} else e_isrState = STATE_WAIT_FOR_PRESS;
				break;

			case STATE_WAIT_FOR_RELEASE:
	//keypad released
				if (KEY_RELEASED())
				{
					ESOS_SIGNAL_SEMAPHORE(key_sem,1);
#if 0
					ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
					ESOS_TASK_WAIT_ON_SEND_STRING("key released");
					ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
					ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
					ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
#endif
					e_isrState = STATE_WAIT_FOR_PRESS;
				}
				break;
			default:
				e_isrState = STATE_WAIT_FOR_PRESS;
				break;
		}
	}
	ESOS_TASK_END();
}

//******************************************************************************************//
//************************************** poll_keypad ***************************************//
//******************************************************************************************//
ESOS_USER_TASK(poll_keypad)
{
    static ESOS_TASK_HANDLE cmd_param_task;
    static uint8_t send_key;

    ESOS_TASK_BEGIN();
    cmd_param_task = esos_GetTaskHandle(comm1_task);

	configKeypad();

	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
	ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
	ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
    ESOS_TASK_WAIT_ON_SEND_STRING("poll keypad task");
	ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
	ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
	ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();


	while (TRUE)
	{
		ESOS_TASK_WAIT_SEMAPHORE(key_sem,1);
		if (u8_newKey)
		{
			__esos_CB_WriteUINT8(cmd_param_task->pst_Mailbox->pst_CBuffer,u8_newKey);
#if 0
			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		    ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(u8_newKey-0xE2);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
#endif
			u8_newKey = 0;
		}
	}
    ESOS_TASK_END();
}

//******************************************************************************************//
//********************************** CONFIG_SPI_SLAVE **************************************//
//******************************************************************************************//
static void CONFIG_SPI_SLAVE(void)
{
  SPI1CON1 = SEC_PRESCAL_8_1		|		//8:1 secondary prescale
             PRI_PRESCAL_64_1		|		//64:1 primary prescale
             CLK_POL_ACTIVE_HIGH	|		//clock active high (CKP = 0)
             SPI_CKE_ON				|		//out changes active to idle (CKE=1)
             SPI_MODE8_ON			|		//8-bit mode
//             SPI_SMP_ON				|		// input data sampled at end of data output time
             SLAVE_ENABLE_ON;				//master mode

	CONFIG_SDI1_TO_RP(RD14_RP);
	CONFIG_RD14_AS_DIG_INPUT();
	CONFIG_SDO1_TO_RP(RD15_RP);
	CONFIG_RD15_AS_DIG_OUTPUT();
	CONFIG_SCK1IN_TO_RP(RF4_RP);
	CONFIG_RF4_AS_DIG_INPUT();
	CONFIG_SS1IN_TO_RP(RF5_RP);
	CONFIG_RF5_AS_DIG_INPUT();
	SPI1STATbits.SPIEN = 1;  //enable SPI mode
}
volatile uint8_t  cmd_array[24] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
//******************************************************************************************//
//**************************************** echo_spi ****************************************//
//******************************************************************************************//
ESOS_USER_TASK(echo_spi_task)
{
	static uint8_t data1;
//	static uint8_t data2;
	static uint8_t cmd;
	static uint8_t i;

//	static char rpm_disp[5];
//	static char mph_disp[5];
//	static uint16_t res;
	static uint8_t res2;
//	static uint16_t res3, res4;
//	static char res_str[10];
//	static char res2_str[10];

	static uint8_t  res_array[24];

    ESOS_TASK_BEGIN();

//#if 0
	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
    ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
    ESOS_TASK_WAIT_ON_SEND_STRING("echo_spi_task on comm1");
    ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
    ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
	ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
	ESOS_TASK_SIGNAL_AVAILABLE_SPI();
//#endif


	data1 = 0x21;
    while(TRUE)
	{

//		ESOS_TASK_WAIT_TICKS(TDELAY2);
//#if 0
		for(i = 0;i < 24;i++)
		{
			cmd = cmd_array[i];
			ESOS_TASK_WAIT_ON_AVAILABLE_SPI();
			ESOS_TASK_WAIT_ON_READ1SPI1(data1);
			ESOS_TASK_WAIT_ON_WRITE1SPI1(cmd);
			ESOS_TASK_SIGNAL_AVAILABLE_SPI();

			if(res_array[i] != data1)
			{
				ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
				ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(i);
				ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
			}
			res_array[i] = data1;

			if(i == 0)
				ESOS_TASK_WAIT_TICKS(TDELAY2);
			res2++;
			if((res2 % 300) == 0)
			{
				ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
				ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((uint8_t)(res2>>8));
//				ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
//				ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
				ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
			}

#if 0
			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			if(i == 2)
			{
				ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
				ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
			}
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(data1);
	//        ESOS_TASK_WAIT_ON_SEND_UINT8(data2);
		    ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

			switch(i)
			{
				case 2:
					rpm_disp[0] = data1 + 0x30;
					break;
				case 3:
					rpm_disp[1] = data1 + 0x30;
					break;
				case 4:
					rpm_disp[2] = data1 + 0x30;
					break;
				case 5:
					rpm_disp[3] = data1 + 0x30;
					break;
				case 6:
					mph_disp[0] = data1 + 0x30;
					break;
				case 7:
					mph_disp[1] = data1 + 0x30;
					break;
				case 8:
					mph_disp[2] = data1 + 0x30;
					break;
				case 9:
					mph_disp[3] = data1 + 0x30;
					res = res2 = 0;
					res3 = res4 = 0;
					break;

				case 10:
					res = (uint16_t)data1;
	//					printf("%2x\t",res);
					break;
				case 11:
					res2 = (uint16_t)data1;
	//					printf("%2x\t",res2);
					res2 <<= 8;
					res2 &= 0xff00;
					res |= res2;
					break;
				case 12:
					res3 = data1;
					break;
				case 13:
					res4 = data1;
					res4 <<= 8;
					res4 &= 0xff00;
					res3 |= res4;
					break;
				case 15:
					ESOS_TASK_WAIT_ON_SEND_STRING(rpm_disp);
					ESOS_TASK_WAIT_ON_SEND_UINT8('\t');
					ESOS_TASK_WAIT_ON_SEND_STRING(mph_disp);
					ESOS_TASK_WAIT_ON_SEND_UINT8('\t');
					sprintf(res_str,"%4d",res);
					sprintf(res2_str,"%4d",res2);
					ESOS_TASK_WAIT_ON_SEND_STRING(res);
					ESOS_TASK_WAIT_ON_SEND_UINT8('\t');
					ESOS_TASK_WAIT_ON_SEND_STRING(res2);
					ESOS_TASK_WAIT_ON_SEND_UINT8('\t');
					break;
			}
#endif
		}
    }
    ESOS_TASK_END();
}

#define NUM_CHANNELS 2
volatile uint16_t au16_buffer[NUM_CHANNELS];
volatile uint8_t  u8_waiting;

//******************************************************************************************//
//**************************************** convADC  ****************************************//
//******************************************************************************************//
ESOS_USER_TASK(convADC)
{
    static  uint8_t data1, u8_wdtState;
	static uint16_t u16_adcVal1, u16_adcVal2;
	static float f_adcVal;
	static uint8_t   u8_i;
	static uint16_t  u16_pot;

    ESOS_TASK_BEGIN();

	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
	ESOS_TASK_WAIT_ON_SEND_STRING("starting ADC task\r\n");
	ESOS_TASK_WAIT_ON_SEND_STRING("                                 \r");
	ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

	CONFIG_RB0_AS_ANALOG();
	CONFIG_RB1_AS_ANALOG();

	configADC1_AutoScanIrqCH0( ADC_SCAN_AN0 | ADC_SCAN_AN1, 31, 0);
	while ( !AD1CON1bits.DONE)
		ESOS_TASK_WAIT_TICKS(1);

	while (TRUE)
	{
		while (u8_waiting)	// wait for valid data in ISR
			ESOS_TASK_WAIT_TICKS(1);
		u8_waiting = 0;
		for ( u8_i=0; u8_i<NUM_CHANNELS; u8_i++)
		{
			u16_pot = au16_buffer[u8_i];

			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			if(u8_i == 0)
			{
//				ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
				ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
			}
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((uint8_t)(u16_pot>>8));
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((uint8_t)u16_pot);
			ESOS_TASK_WAIT_TICKS(50);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
		}
	}
	ESOS_TASK_END();
}
//******************************************************************************************//
//************************************ ACD1Interrupt  **************************************//
//******************************************************************************************//
void _ISR _ADC1Interrupt (void)
{
	uint8_t			u8_i;
	uint16_t*		au16_adcHWBuff = (uint16_t*) &ADC1BUF0;

	for ( u8_i=0; u8_i<NUM_CHANNELS; u8_i++)
	{
		au16_buffer[u8_i] = au16_adcHWBuff[u8_i];
	} //end for()
	u8_waiting = 0;  // signal main() that data is ready
	_AD1IF = 0;   //clear the interrupt flag
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
int pack(UCHAR low_byte, UCHAR high_byte)
{
	int temp;
	int myint;
	low_byte = ~low_byte;
	myint = (int)low_byte;
	temp = (int)high_byte;
	temp <<= 8;
	myint |= temp;
	return myint;
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
void unpack(int myint, UCHAR *low_byte, UCHAR *high_byte)
{
	*low_byte = (UCHAR)myint;
	myint >>= 8;
	*high_byte = (UCHAR)myint;
	*low_byte = ~(*low_byte);
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
int update_menu_structs(int i, UCHAR fptr, UCHAR menu0, UCHAR menu1, UCHAR menu2, UCHAR menu3,
			UCHAR menu4, UCHAR menu5, UCHAR index)
{
	int len;
	len = sizeof(MENU_FUNC_STRUCT);
	menu_structs[i].menus[0] = menu0;
	menu_structs[i].menus[1] = menu1;
	menu_structs[i].menus[2] = menu2;
	menu_structs[i].menus[3] = menu3;
	menu_structs[i].menus[4] = menu4;
	menu_structs[i].menus[5] = menu5;
	menu_structs[i].fptr = fptr;
	menu_structs[i].index = index;

//	total_offset += len;
	i++;
	return i;
}

