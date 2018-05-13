//    All rights reserved.
//
//    Permission to use, copy, modify, and distribute this software and its
//    documentation for any purpose, without fee, and without written agreement is
//    hereby granted, provided that the above copyright notice, the following
//    two paragraphs and the authors appear in all copies of this software.
//
//    IN NO EVENT SHALL THE "AUTHORS" BE LIABLE TO ANY PARTY FOR
//    DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
//    OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE "AUTHORS"
//    HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//    THE "AUTHORS" SPECIFICALLY DISCLAIMS ANY WARRANTIES,
//    INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
//    AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
//    ON AN "AS IS" BASIS, AND THE "AUTHORS" HAS NO OBLIGATION TO
//    PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
//
//    Please maintain this header in its entirety when copying/modifying
//    these files.
//
//
// "I am not so much concerned with the return on capital as I am with the return of capital." - Will Rogers
//
//#define DEBUG
#include "../esos/include/esos.h"
#include "../lib/include/pic24_ports_config.h"
#include "../lib/include/pic24_ports_mapping.h"
#include "../lib/include/pic24_libconfig.h"
#include "../esos/include/pic24/esos_pic24.h"
#include "../esos/include/pic24/esos_pic24_rs232.h"
#include "../esos/include/pic24/esos_pic24_spi.h"
#include "../lib/include/pic24_all.h"
#include "../lib/include/pic24_timer.h"
#include "../lib/include/pic24_util.h"
#include "../lib/include/pic24_adc.h"
//#include "main.h"
//#include "../../AVR_t6963/key_defs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// DEFINEs go here
//#define   CONFIG_LED1()   printf("called CONFIG_LED1()\n");
uint8_t     LED1 = TRUE;      // LED1 is initially "on"
#define MY_ESOS1
#define TIME_DELAY		20
#define TEMP_DELAY3		2
#define TEMP_DELAY2		1000
#define TDELAY 2
#define TDELAY2 1
#define VREF 3.3  //assume Vref = 3.3 volts
#define FLAG1        ESOS_USER_FLAG_1
#define RT_OFFSET 0x70
//#define AUX_STRING_LEN 1024	// this should be the same as AUX_STRING_LEN in AVR_t6963/main.h

#ifdef OC1CON1
#warning "OC1CON1 defined"
#endif
#define CHAR_AT_CMD			0
#define STRING_AT_CMD		1
#define CHAR_CMD			2
#define GOTO_CMD			3
#define SET_MODE_CMD 		4
#define DEBUG_CLRSCR1		5
#define DEBUG_CLRSCR2		6
#define DEBUG_CLRSCR3		7
#define DEBUG_MSG1			8

#define ROWS 16
#define COLUMN 40
#define DISPLAY_OFF         0x90    //0b10010000, display off
#define CURSOR_ON_BLINK_OFF 0x92    //0b1001xx10, cursor on without blinking
#define CURSOR_BLINK_ON     0x93    //0b1001xx11, cursor on with blinking
#define TEXT_ON             0x94    //0b100101xx, text on, graphics off
#define GRAPHIC_ON          0x98    //0b100110xx, text off, graphics on
#define TEXT_GRH_ON         0x9C    //0b100111xx, text on, graphics on
#define ATTR_NORMAL         0x00    //Normal Display
#define ATTR_REVERSE        0x05    //Reverse Di../t6963.h:189:33: error: expected ';', ',' or ')' before 'mode'splay
#define ATTR_INHIBIT        0x03    //Inhibit Display
#define ATTR_BLINK          0x08    //Blinking of Normal Display
#define ATTR_BLINK_REVERSE  0x0D    //Blinking of Reverse Display
#define ATTR_BLINK_INHIBIT  0x0B    //Blinking of Inhibit Display
#define LINE_8_CURSOR       0xA7    //8-line cursor mode
#define LINE_7_CURSOR       0xA6    //7-line cursor mode
#define LINE_6_CURSOR       0xA5    //6-line cursor mode
#define LINE_5_CURSOR       0xA4    //5-line cursor mode
#define LINE_4_CURSOR       0xA3    //4-line cursor mode
#define LINE_3_CURSOR       0xA2    //3-Line cursor mode
#define LINE_2_CURSOR       0xA1    //2-Line cursor mode
#define LINE_1_CURSOR       0xA0    //1-Line cursor mode


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

typedef struct rt_param
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
	TEST1 = 0xC9,
	TEST2,			//  - CA
	TEST3,			//  - CB
	TEST4,			//  - CC
	TEST5,			//	- CD
	TEST6,			//	- CE
	TEST7,			//	- CF
	TEST8,			//	- D0
	TEST9,			//	- D1
	TEST10,			//	- D2
	TEST11,			//	- D3
	TEST12,			//	- D4
	TEST13,			//	- D5
	TEST14,			//	- D6
	TEST15,			//	- D7
	TEST16,			//	- D8
	TEST17,			//  - D9
	LOAD_RAM,		//  - DA
	INIT, 			//	- DB
	SPACE,			//	- DC
	BURN_PART,		//  - DD
	READ_EEPROM1,	//	- DE
	READ_EEPROM2,	//	- DF

	KP_POUND,	// '#'	- E0
	KP_AST, // '*'		- E1
	KP_0, // '0'		- E2
	KP_1, // '1'		- E3
	KP_2, // '2'		- E4
	KP_3, // '3'		- E5
	KP_4, // '4'		- E6
	KP_5, // '5'		- E7
	KP_6, // '6'		- E8
	KP_7, // '7'		- E9
	KP_8, // '8'		- EA
	KP_9, // '9'		- EB
	KP_A, // 'A'		- EC
	KP_B, // 'B'		- ED
	KP_C, // 'C'		- EE
	KP_D, // 'D'		- EF
} KEY_TYPES;

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

volatile UINT32 U32_lastCapture; // UINT32 declared in all_generic.h


ESOS_USER_TASK(keypad);
ESOS_USER_TASK(poll_keypad);
ESOS_SEMAPHORE(key_sem);
ESOS_SEMAPHORE(comm1_sem);
ESOS_SEMAPHORE(comm2_sem);
ESOS_USER_TASK(delay_comm1);
ESOS_USER_TASK(send_char);
ESOS_USER_TASK(send_charat);
ESOS_USER_TASK(send_string);
ESOS_USER_TASK(set_cursor);
ESOS_USER_TASK(goto1);
ESOS_USER_TASK(test1);
ESOS_USER_TASK(get_comm1);
ESOS_USER_TASK(get_comm2);
ESOS_USER_TASK(send_comm1);
ESOS_USER_TASK(dimmer_task);

UCHAR get_keypress(UCHAR key1)
{
	UCHAR wkey;

	switch(key1)
	{
		case '0':
			wkey = KP_0;
			break;
		case '1':
			wkey = KP_1;
			break;
		case '2':
			wkey = KP_2;
			break;
		case '3':
			wkey = KP_3;
			break;
		case '4':
			wkey = KP_4;
			break;
		case '5':
			wkey = KP_5;
			break;
		case '6':
			wkey = KP_6;
			break;
		case '7':
			wkey = KP_7;
			break;
		case '8':
			wkey = KP_8;
			break;
		case '9':
			wkey = KP_9;
			break;
		case '*':
			wkey = KP_AST;
			break;
		case '#':
			wkey = KP_POUND;
			break;
		case 'A':
		case 'a':
			wkey = KP_A;
			break;
		case 'B':
		case 'b':
			wkey = KP_B;
			break;
		case 'C':
		case 'c':
			wkey = KP_C;
			break;
		case 'D':
		case 'd':
			wkey = KP_D;
			break;
// use 'z' as a shortcut to '*' and 'y' as a shortcut to '#'
		case 'Y':
		case 'y':
			wkey = KP_POUND;
			break;
		case 'Z':
		case 'z':
			wkey = KP_AST;
			break;

		case 'g':
			wkey = TEST1;		// set size = 300, start_addr = 0
			break;
		case 'h':
			wkey = TEST2;		// set size = 300, start_addr = 300
			break;
		case 'j':
			wkey = TEST3;		// set size = 300, start_addr = 600
			break;
		case 'k':
			wkey = TEST4;		// set size = 129, start_addr = 900
			break;
		case 'l':
			wkey = TEST5;		// fill aux_string with random data
			break;
		case 'm':
			wkey = TEST6;		// draw border
			break;
		case 'n':
			wkey = TEST7;		// print cblabels
			break;
		case 'o':
			wkey = TEST8;		// restore eeprom.bin
			break;
		case 'p':
			wkey = TEST9;		// copy eeprom2.bin to eeprom.bin in sim_write
			break;
		case 'x':
			wkey = TEST10;		// read aux_string
			break;
		case 's':
			wkey = TEST11;		// tell AVR to print pattern
			break;
		case 'r':
			wkey = LOAD_RAM;	// load all stuff in ram
			break;
		case 'i':
			wkey = INIT;
			break;
		case ' ':
			wkey = SPACE;
			break;
		case 'v':
			wkey = BURN_PART;
			break;
		case 'e':
			wkey = READ_EEPROM1;
			break;
		case 'f':
			wkey = READ_EEPROM2;
			break;
		case 'E':
			wkey = TEST16;
			break;
		case 'F':
			wkey = TEST17;
			break;
		default:
			wkey = 0xff;
			break;
	}
	return wkey;
}

/*
RE0 - p93 - 5th from right inside top			//	row 0
RE1 - p94 - 4th from right outside top			//	row 1
RE2 - p98 - 2nd from right outside top			//	row 2
RE3 - p99 - 2nd from right inside top			//	row 3

RA0 - p17 - 5th up - outside right				//	col 0
RA1 - p38 - 7th from right - outside bottom		//	col 1
RA2 - p58 - 4th up from bottom - inside left	//	col 2
RA3 - p59 - 5th up from bottom - outside left	//	col 3
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
/*
	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
	ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
	ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
    ESOS_TASK_WAIT_ON_SEND_STRING("keypad task");
	ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
	ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
	ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
*/
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
    static UCHAR data1, data2;
	
    ESOS_TASK_BEGIN();
//    cmd_param_task = esos_GetTaskHandle(menu_task);

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
		if(u8_newKey)
		{
			data2 = u8_newKey;
			u8_newKey = 0;
			if(data2 == KP_POUND)
				data1 = 0x23;
			else if(data2 == KP_AST)
				data1 = 0x2a;
			else if(data2 >= KP_A && data2 <= KP_D)
				data1 = data2 - 0xEC + 0x41;
			else
				data1 = data2 - 0xE2 + 0x30;

			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
			ESOS_TASK_WAIT_ON_SEND_UINT82(data1);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();

			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			ESOS_TASK_WAIT_ON_SEND_UINT8(data1);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
		}
	}
    ESOS_TASK_END();
}
//#if 0
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

#ifdef BUILT_ON_ESOS
#warning "BUILT_ON_ESOS defined"
#else
#warning "BUILT_ON_ESOS not defined"
#endif

#if 0
uint32_t ticksToUs2(uint32_t u32_ticks, uint16_t u16_tmrPre);
uint32_t ticksToUs2(uint32_t u32_ticks, uint16_t u16_tmrPre) {
  // Because of the wide range of the numbers, use a float for precision.
  float f_ticks;
  uint32_t u32_timeUs;

  f_ticks = u32_ticks;   //convert to float
  f_ticks = ((f_ticks*u16_tmrPre)/FCY)*1E6;
  u32_timeUs = roundFloatToUint32(f_ticks);  //back to int32_t
  return u32_timeUs;
}
uint16_t usToU16Ticks2(uint16_t u16_us, uint16_t u16_pre);
uint16_t usToU16Ticks2(uint16_t u16_us, uint16_t u16_pre) {
  // Use a float internally for precision purposes to accomodate wide range of FCY, u16_pre
  float f_ticks = FCY;
  uint16_t u16_ticks;
  f_ticks = (f_ticks*u16_us)/u16_pre/1E6;
  ASSERT(f_ticks < 65535.5);
  u16_ticks = roundFloatToUint16(f_ticks);  //back to integer
  return u16_ticks;
}
#endif

#define NUM_CHANNELS 1
volatile uint16_t au16_buffer[NUM_CHANNELS];
volatile uint8_t  u8_waiting;
volatile UINT dimmer;
volatile int lat5;

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
	CONFIG_RB2_AS_ANALOG();
	CONFIG_RB3_AS_ANALOG();
/*
	CONFIG_RB4_AS_ANALOG();
	CONFIG_RB5_AS_ANALOG();
	CONFIG_RB6_AS_ANALOG();
	CONFIG_RB7_AS_ANALOG();
	CONFIG_RB8_AS_ANALOG();
	CONFIG_RB9_AS_ANALOG();
	CONFIG_RB10_AS_ANALOG();
	CONFIG_RB11_AS_ANALOG();
	CONFIG_RB12_AS_ANALOG();
	CONFIG_RB13_AS_ANALOG();
	CONFIG_RB14_AS_ANALOG();
	CONFIG_RB15_AS_ANALOG();
*/
//	configADC1_AutoScanIrqCH0( ADC_SCAN_AN0 | ADC_SCAN_AN1 | ADC_SCAN_AN2 |
//			 ADC_SCAN_AN3 | ADC_SCAN_AN4 | ADC_SCAN_AN5, 31, 0);

	configADC1_AutoScanIrqCH0( 0x000F, 31, 0);
//	configADC1_AutoHalfScanIrqCH0(0x00FF, 31, 0);
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
			u16_pot <<= 6;
			u16_pot |= 0x003F;

			if(u8_i == 0)
			{
				dimmer = u16_pot;
			}
#if 0
			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			if(u8_i == 0)
			{
//				ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
				ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
			}
//			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((uint8_t)(dimmer>>8));
//			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((uint8_t)dimmer);

			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((uint8_t)(u16_pot>>8));
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((uint8_t)u16_pot);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
#endif
		}
		ESOS_TASK_WAIT_TICKS(1);
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
//************************************** dimmer_task  **************************************//
//******************************************************************************************//
// RE5 is connected to the CMOS gate for the LCD backlight
// dimmer is from RA0 ADC
ESOS_USER_TASK(dimmer_task)
{
	static UCHAR data1;
	static UCHAR data2;
	static UINT dim;
	static int i,j;
	
	ESOS_TASK_BEGIN()
	CONFIG_RE5_AS_DIG_OUTPUT();
//	CONFIG_RE5_AS_OUTPUT();
//	ENABLE_RE5_PULLUP();

	_LATE5 = 1;

	while(TRUE)
	{
	}
#if 0
		dim = dimmer;
		dim >>= 8;
		data1 = (UCHAR)dim;
//		data2 = ~data1;
		data1 >>= 5;
//		data2 >>= 2;
/*
		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
		ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((uint8_t)(dim>>8));
		ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((uint8_t)dim);
		ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((uint8_t)data1);
		ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((uint8_t)data2);
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
*/
		if(i++ % 2 == 0)
		{
			_LATE5 = 0;
//			for(j = 0;j < data1;j++)
				ESOS_TASK_WAIT_TICKS(1);
		}else{
			_LATE5 = 1;
			for(j = 0;j < data1;j++)
				ESOS_TASK_WAIT_TICKS(1);
		}
//		ESOS_TASK_WAIT_TICKS(200);
	}
#endif
	ESOS_TASK_END();
}
//******************************************************************************************//
//******************************************************************************************//
//******************************************************************************************//
#if 0
#ifndef PWM_PERIOD
#define PWM_PERIOD 2000  // desired period, in us
#define OC_PWM_CENTER_ALIGN 0x07
#define OC_SYNCSEL_TIMER2 0x0080
#endif

#ifdef _NOFLOAT
#warning "NOFLOAT defined"
#else
#warning "NOFLOAT not defined"
#endif

void  configTimer2(void) 
{
	T2CON = T2_OFF | T2_IDLE_CON | T2_GATE_OFF
		| T2_32BIT_MODE_OFF
		| T2_SOURCE_INT
		| T2_PS_1_256;
//		| T2_PS_1_64;
//		| T2_PS_1_8;
//		| T2_PS_1_1;
	PR2 = usToU16Ticks(PWM_PERIOD, getTimerPrescale(T2CONbits)) - 1;
//	PR2 = usToU16Ticks(PWM_PERIOD, T2CONbits.TCKPS) - 1;
	TMR2  = 0;       //clear timer2 value
	_T2IF = 0;
	_T2IP = 1;
	_T2IE = 1;    //enable the Timer2 interrupt
}


void configOutputCompare1(void) 
{
	T2CONbits.TON = 0;          //disable Timer when configuring Output compare
	CONFIG_OC1_TO_RP(RF13_RP);   //map OC1 to RB0
	OC1RS = 0;  //clear both registers
	OC1R = 0;

//turn on the compare toggle mode using Timer2
	OC1CON1 = OC_TIMER2_SRC |		//Timer2 source
		OC_PWM_CENTER_ALIGN;		//PWM
	OC1CON2 = OC_SYNCSEL_TIMER2;	//synchronize to timer2
}


void _ISR _T2Interrupt(void) 
{
	uint32_t u32_temp;
	_T2IF = 0;    //clear the timer interrupt bit
	//update the PWM duty cycle from the ADC value
	u32_temp = ADC1BUF0;  //use 32-bit value for range
//	u32_temp = dimmer;
	//compute new pulse width that is 0 to 99% of PR2
	// pulse width (PR2) * ADC/1024
	u32_temp = (u32_temp * (PR2))>> 10 ;  // >>10 is same as divide/1024
	OC1RS = u32_temp;  //update pulse width value
	dimmer = u32_temp;
	SET_SAMP_BIT_ADC1();      //start sampling and conversion
	lat5++;
	if(lat5 % 2 == 0)
		_LATE5 = 0;
	else _LATE5 = 1;
}
#endif
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
