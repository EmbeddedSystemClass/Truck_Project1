#if 1
#ifdef BUILT_ON_ESOS
#warning "BUILT_ON_ESOS defined"
#else
#warning "BUILT_ON_ESOS not defined"
#endif

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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../mytypes.h"

volatile ESOS_SEMAPHORE(key_sem);
volatile ESOS_SEMAPHORE(lcd_sem);
volatile ESOS_USER_TASK(keypad);
volatile ESOS_USER_TASK(poll_keypad);
//ESOS_USER_TASK(spi_task);
volatile ESOS_USER_TASK(AVR_cmd);
volatile ESOS_USER_TASK(main_proc);
volatile ESOS_USER_TASK(send_fpga);
volatile ESOS_USER_TASK(recv_lcd);
volatile ESOS_USER_TASK(send_comm1);
volatile ESOS_USER_TASK(recv_comm1);
volatile ESOS_USER_TASK(menu_task);
volatile ESOS_USER_TASK(password_task);
volatile ESOS_USER_TASK(numentry_task);
volatile ESOS_USER_TASK(display_menu);
volatile ESOS_USER_TASK(display_rtlabels);

volatile UCHAR avr_buffer[6];
static ESOS_TASK_HANDLE avr_handle;

#define AVR_CALL() __esos_CB_WriteUINT8Buffer(avr_handle->pst_Mailbox->pst_CBuffer, &avr_buffer, 5);

#define MY_ESOS1
#define TIME_DELAY		20
#define TEMP_DELAY3		2
#define TEMP_DELAY2		1000
#define TDELAY 2
#define TDELAY2 1
#define VREF 3.3  //assume Vref = 3.3 volts
#define FLAG1        ESOS_USER_FLAG_1
#define RT_OFFSET 0x70
#define PASSWORD_RETRIES 5

enum key_types
{
	KP_1 = 0xE0, // '1'		- E0
	KP_2, // '2'		- E1
	KP_3, // '3'		- E2
	KP_4, // '4'		- E3
	KP_5, // '5'		- E4
	KP_6, // '6'		- E5
	KP_7, // '7'		- E6
	KP_8, // '8'		- E7
	KP_9, // '9'		- E8
	KP_A, // 'A'		- E9
	KP_B, // 'B'		- EA
	KP_C, // 'C'		- EB
	KP_D, // 'D'		- EC
	KP_POUND,	// '#'	- ED
	KP_AST, // '*'		- EE
	KP_0 	// '0'		- EF
} KEY_TYPES;

typedef struct
{
	UCHAR row;
	UCHAR col;
	UCHAR str;
	UCHAR onoff;
} FORMAT_STR;

#define NUM_MENU_LABELS 5
volatile FORMAT_STR menu_str[NUM_MENU_LABELS];

#define NUM_RT_LABELS 13
volatile FORMAT_STR rtlabel_str[NUM_RT_LABELS];

volatile UCHAR menu_ptr;

#define ROWS 16
#define COLUMN 40
#define DISPLAY_OFF         0x90    //0b10010000, display off
#define CURSOR_ON_BLINK_OFF 0x92    //0b1001xx10, cursor on without blinking
#define CURSOR_BLINK_ON     0x93    //0b1001xx11, cursor on with blinking
#define TEXT_ON             0x94    //0b100101xx, text on, graphics off
#define GRAPHIC_ON          0x98    //0b100110xx, text off, graphics on
#define TEXT_GRH_ON         0x9C    //0b100111xx, text on, graphics on
#define ATTR_NORMAL         0x00    //Normal Display
#define ATTR_REVERSE        0x05    
//Reverse Di../t6963.h:189:33: error: expected ';', ',' or ')' before 'mode'splay
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

#define FPGA_OFF_CMD					0x00
#define FPGA_SEND_CHAR_CMD				0x81
#define FPGA_SET_BRIGHTNESS_CMD			0x82 // set brightness for LED display
#define FPGA_SET_CDECIMAL_CMD			0x83 // set decimal point   "    "
#define FPGA_SET_UPDATE_RATE_CMD		0x84 // serial port transmit rate
#define FPGA_SET_FACTOR_CMD				0x85 // set the div. factor for rpm/mph calc
#define FPGA_SET_DISPLAY_UPDATE_RATE 	0x86 // display update rate for LED display
#define FPGA_DTMF_TONE_ON				0x87 // DTMF tones on dual speakers (on)
#define FPGA_DTMF_TONE_OFF				0x88 // DTMF tones on dual speakers (off)
#define FPGA_SPECIAL_TONE_ON			0x89 // siren for dual speakers
#define FPGA_LCD_PWM					0x8A // command for LCD display (brightness)
#define PWM_OFF_PARAM					0x00 // off
#define PWM_ON_PARAM					0x1F // on
#define PWM_80DC_PARAM					0x1A // duty_cycle = 80%
#define PWM_75DC_PARAM					0x16 // duty_cycle = 75%
#define PWM_60DC_PARAM					0x12 // duty_cycle = 60%
#define PWM_50DC_PARAM					0x0E // duty_cycle = square wave
#define PWM_30DC_PARAM					0x0A // duty_cycle = 30%
#define PWM_25DC_PARAM					0x06 // duty_cycle = 25%
#define PWM_12DC_PARAM					0x02 // duty_cycle = 12%
	
#define FP_SHUTOFF_OVERRIDE  			0x8B // override the fuel pump shutoff to get it started
#define REV_LIMITER 					0x8C // set the rev limit min & max

#define BUFFER_SIZE 30
#define PASSWORD_SIZE 12

volatile char password[PASSWORD_SIZE];
volatile char correct_password[PASSWORD_SIZE];
volatile UCHAR password_valid;
volatile int password_ptr;
volatile int password_retries;
volatile int print_string_buffer_size;
volatile char print_string_buffer[BUFFER_SIZE];

typedef enum
{
	NORMAL,
	PASSWORD,
	NUMENTRY
}KEY_MODE;

volatile KEY_MODE key_mode;

//******************************************************************************************//
//*************************************** menu_task ****************************************//
//******************************************************************************************//
ESOS_USER_TASK(menu_task)
{
	static UCHAR data1;
	static UINT row,col;
	static ESOS_TASK_HANDLE comm1_handle;
	static int k;
	static int acc, fuel_pump, fan;
	
	comm1_handle = esos_GetTaskHandle(send_comm1);

    ESOS_TASK_BEGIN();

	acc = fuel_pump = fan = 0;

	while (TRUE)
	{
		ESOS_TASK_WAIT_FOR_MAIL();

		while(ESOS_TASK_IVE_GOT_MAIL())
		{
			data1 = __esos_CB_ReadUINT8(__pstSelf->pst_Mailbox->pst_CBuffer);
			switch(data1)
			{
				case 	KP_1:
					__esos_CB_WriteUINT8(comm1_handle->pst_Mailbox->pst_CBuffer,ENABLE_START);
				break;
				case	KP_2:
					if(acc == 0)
					{
						__esos_CB_WriteUINT8(comm1_handle->pst_Mailbox->pst_CBuffer,ON_ACC);
						acc = 1;
					}
					else{
						__esos_CB_WriteUINT8(comm1_handle->pst_Mailbox->pst_CBuffer,OFF_ACC);
						acc = 0;
					}
				break;
				case	KP_3:
					if(fuel_pump == 0)
					{
						__esos_CB_WriteUINT8(comm1_handle->pst_Mailbox->pst_CBuffer,ON_FUEL_PUMP);
						fuel_pump = 1;
					}
					else{
						__esos_CB_WriteUINT8(comm1_handle->pst_Mailbox->pst_CBuffer,OFF_FUEL_PUMP);
						fuel_pump = 0;
					}
				break;
				case	KP_4:
					if(fan == 0)
					{
						__esos_CB_WriteUINT8(comm1_handle->pst_Mailbox->pst_CBuffer,ON_FAN);
						fan = 1;
					}
					else{
						__esos_CB_WriteUINT8(comm1_handle->pst_Mailbox->pst_CBuffer,OFF_FAN);
						fan = 0;
					}
				break;
				case	KP_5:
					__esos_CB_WriteUINT8(comm1_handle->pst_Mailbox->pst_CBuffer,START_SEQ);
				break;
				case	KP_6:
				break;
				case	KP_7:
				break;
				case	KP_8:
				break;
				case	KP_9:
				break;
				case	KP_A:		// move up in menu
					if(menu_ptr < 1)
						menu_ptr = 6;
					menu_ptr--;	

					avr_buffer[0] = SET_MODE_CMD;
					avr_buffer[1] = TEXT_ON | CURSOR_BLINK_ON;
					avr_buffer[2] = menu_str[menu_ptr].row;
					avr_buffer[3] = menu_str[menu_ptr].col-2;
					avr_buffer[4] = (LINE_8_CURSOR);
					AVR_CALL();

				break;
				case	KP_B:		// move down in menu
					if(++menu_ptr > 5)
						menu_ptr = 0;

					avr_buffer[0] = SET_MODE_CMD;
					avr_buffer[1] = TEXT_ON | CURSOR_BLINK_ON;
					avr_buffer[2] = menu_str[menu_ptr].row;
					avr_buffer[3] = menu_str[menu_ptr].col-2;
					avr_buffer[4] = (LINE_8_CURSOR);
					AVR_CALL();

				break;
				case	KP_C:		// exec menu choice
					avr_buffer[0] = GOTO_CMD;
					avr_buffer[1] = menu_str[menu_ptr].row;
					avr_buffer[2] = menu_str[menu_ptr].col-2;
					AVR_CALL();

					if(menu_str[menu_ptr].onoff == 1)
						menu_str[menu_ptr].onoff = 0;
					else menu_str[menu_ptr].onoff = 1;
					avr_buffer[0] = CHAR_CMD;
					avr_buffer[1] = (menu_str[menu_ptr].onoff?'X':' ');
					AVR_CALL();

//					__esos_CB_WriteUINT8(comm1_handle->pst_Mailbox->pst_CBuffer,
//						(menu_ptr+1)*2+menu_str[menu_ptr].onoff);
				break;
				case	KP_D:
				break;
				case	KP_POUND:
				break;
				case	KP_AST:
				break;
				case	KP_0:
				break;
				default:
				break;
			}
		}
	}
    ESOS_TASK_END();
}

//******************************************************************************************//
//************************************* password_task **************************************//
//******************************************************************************************//
volatile ESOS_TASK_HANDLE password_task_handle;

ESOS_USER_TASK(password_task)
{
	static UCHAR data1;
	static UCHAR row, col;
	static ESOS_TASK_HANDLE comm1_handle;
    static ESOS_TASK_HANDLE menu_handle;
    static ESOS_TASK_HANDLE rt_handle;
	static int i;

    ESOS_TASK_BEGIN();
    password_task_handle = ESOS_TASK_GET_TASK_HANDLE();
	comm1_handle = esos_GetTaskHandle(send_comm1);
	menu_handle = esos_GetTaskHandle(display_menu);
	rt_handle = esos_GetTaskHandle(display_rtlabels);
    
	ESOS_TASK_WAIT_TICKS(1000);

	avr_buffer[0] = PUT_STR;
	avr_buffer[1] = 7;	// row 7
	avr_buffer[2] = 0;	// col 0
	avr_buffer[3] = 1;	// string 1 ("enter password")
	AVR_CALL();

	avr_buffer[0] = GOTO_CMD;
	avr_buffer[1] = 7;
	avr_buffer[2] = 16;
	AVR_CALL();

/*
	avr_buffer[0] = (SET_MODE_CMD);
	avr_buffer[1] = (TEXT_ON | CURSOR_ON_BLINK_OFF);
	avr_buffer[2] = 7;
	avr_buffer[3] = 16;
	avr_buffer[4] = (LINE_8_CURSOR);
	AVR_CALL();
*/
	while (TRUE)
	{
		ESOS_TASK_WAIT_FOR_MAIL();

		while(ESOS_TASK_IVE_GOT_MAIL())
		{
			data1 = __esos_CB_ReadUINT8(__pstSelf->pst_Mailbox->pst_CBuffer);
			switch(data1)
			{
				case	KP_0:
				case 	KP_1:
				case	KP_2:
				case	KP_3:
				case	KP_4:
				case	KP_5:
				case	KP_6:
				case	KP_7:
				case	KP_8:
				case	KP_9:
					data1 = data1 - KP_1 + 0x31;
					password[password_ptr++] = data1;
					avr_buffer[0] = CHAR_CMD;
					avr_buffer[1] = '*';
					AVR_CALL();

					if(password_ptr > strlen(correct_password))
					{
						// if incorrect password, first display "bad password" message
						avr_buffer[0] = LCD_CLRSCR;
						avr_buffer[1] = 0;
						AVR_CALL();

						avr_buffer[0] = PUT_STR;
						avr_buffer[1] = 0;
						avr_buffer[2] = 0;
						avr_buffer[3] = 2;
						AVR_CALL();

						avr_buffer[0] = PUT_STR;
						avr_buffer[1] = 7;	// row 7
						avr_buffer[2] = 0;	// col 0
						avr_buffer[3] = 1;	// string 1 ("enter password")
						AVR_CALL();

						avr_buffer[0] = GOTO_CMD;
						avr_buffer[1] = 7;
						avr_buffer[2] = 16;
						AVR_CALL();

						password_ptr = 0;
						password_retries++;
						memset(password,0,PASSWORD_SIZE);

						if(password_retries > PASSWORD_RETRIES)
						{
							ESOS_TASK_SLEEP();
						}
					}
					if(strcmp(password,correct_password) == 0)
					{
						password_valid = 1;
						avr_buffer[0] = LCD_CLRSCR;
						avr_buffer[1] = 0;
						AVR_CALL();

						avr_buffer[0] = (SET_MODE_CMD);
						avr_buffer[1] = (TEXT_ON | DISPLAY_OFF);
						avr_buffer[2] = 0;
						avr_buffer[3] = 0;
						avr_buffer[4] = (LINE_8_CURSOR);
						AVR_CALL();

						__esos_CB_WriteUINT8(menu_handle->pst_Mailbox->pst_CBuffer,data1);

						__esos_CB_WriteUINT8(rt_handle->pst_Mailbox->pst_CBuffer,data1);
/*
						avr_buffer[0] = PUT_STR;
						avr_buffer[1] = 0;
						avr_buffer[2] = 0;
						avr_buffer[3] = 3;
						AVR_CALL();

						avr_buffer[0] = GOTO_CMD;
						avr_buffer[1] = 0;
						avr_buffer[2] = 0;
						AVR_CALL();
*/
						key_mode = NORMAL;
						password_ptr = 0;
						__esos_CB_WriteUINT8(comm1_handle->pst_Mailbox->pst_CBuffer,ABLE_TO_START);
					}

				break;
				case	KP_A:
				break;
				case	KP_B:
				break;
				case	KP_C:
				break;
				case	KP_D:
				break;
				case	KP_POUND:
				break;
				case	KP_AST:
				break;
				default:
				break;
			}
		}
	}
    ESOS_TASK_END();
}

//******************************************************************************************//
//************************************* numentry_task **************************************//
//******************************************************************************************//
ESOS_USER_TASK(numentry_task)
{
	static UCHAR data1;
	
    ESOS_TASK_BEGIN();
	while (TRUE)
	{
		ESOS_TASK_WAIT_FOR_MAIL();

		while(ESOS_TASK_IVE_GOT_MAIL())
		{
			data1 = __esos_CB_ReadUINT8(__pstSelf->pst_Mailbox->pst_CBuffer);

			switch(data1)
			{
				case 	KP_1:
				break;
				case	KP_2:
				break;
				case	KP_3:
				break;
				case	KP_4:
				break;
				case	KP_5:
				break;
				case	KP_6:
				break;
				case	KP_7:
				break;
				case	KP_8:
				break;
				case	KP_9:
				break;
				case	KP_A:
				break;
				case	KP_B:
				break;
				case	KP_C:
				break;
				case	KP_D:
				break;
				case	KP_POUND:
				break;
				case	KP_AST:
				break;
				case	KP_0:
				break;
				default:
				break;
			}
		}
	}
    ESOS_TASK_END();
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
	STATE_WAIT_FOR_RELEASE
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
#endif
//******************************************************************************************//
//*************************************** keypad *******************************************//
//******************************************************************************************//
// DTMF tones are: 1->9, A->D, #, *, 0 (0->15)
ESOS_USER_TASK(keypad)
{
	static ESOS_TASK_HANDLE fpga_handle;
	static UINT data1;
	static UCHAR data2;

	fpga_handle = esos_GetTaskHandle(send_fpga);

    ESOS_TASK_BEGIN();
    data2 = 0;
    data1 = 0;
	while(TRUE)
	{
		ESOS_TASK_WAIT_TICKS(2);
		switch (e_isrState)
		{
			case STATE_WAIT_FOR_PRESS:
				if (KEY_PRESSED() && (u8_newKey == 0))
				{
	//ensure that key is sampled low for two consecutive interrupt periods
					e_isrState = STATE_WAIT_FOR_PRESS2;
				}
				break;
			case STATE_WAIT_FOR_PRESS2:
				if (KEY_PRESSED())
				{
	// a key is ready
					u8_newKey = doKeyScan();
					e_isrState = STATE_WAIT_FOR_RELEASE;
// send the message to FPGA to turn on DTMF tone

					data1 = FPGA_DTMF_TONE_ON;
					data1 <<= 8;
					data1 &= 0xFF00;

					data1 |= u8_newKey - 0xE0;

					__esos_CB_WriteUINT16(fpga_handle->pst_Mailbox->pst_CBuffer,data1);

				} else e_isrState = STATE_WAIT_FOR_PRESS;
				break;

			case STATE_WAIT_FOR_RELEASE:
	//keypad released
				if (KEY_RELEASED())
				{
					ESOS_SIGNAL_SEMAPHORE(key_sem,1);
					e_isrState = STATE_WAIT_FOR_PRESS;
// send the message to FPGA to turn on DTMF tone

					data1 = FPGA_DTMF_TONE_OFF;
					data1 <<= 8;
					data1 &= 0xFF00;

					__esos_CB_WriteUINT16(fpga_handle->pst_Mailbox->pst_CBuffer,data1);
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

    static ESOS_TASK_HANDLE menu_handle;
    static ESOS_TASK_HANDLE password_handle;
    static ESOS_TASK_HANDLE numentry_handle;

	static UCHAR data1;
	static UINT data3;
	
    ESOS_TASK_BEGIN();

	menu_handle = esos_GetTaskHandle(menu_task);
	password_handle = esos_GetTaskHandle(password_task);
	numentry_handle = esos_GetTaskHandle(numentry_task);

	configKeypad();
	data1 = 0;
	while (TRUE)
	{

		ESOS_TASK_WAIT_SEMAPHORE(key_sem,1);
		if(u8_newKey)
		{
			switch(key_mode)
			{
				case NORMAL:
					__esos_CB_WriteUINT8(menu_handle->pst_Mailbox->pst_CBuffer,u8_newKey);
					break;
				case PASSWORD:
					__esos_CB_WriteUINT8(password_handle->pst_Mailbox->pst_CBuffer,u8_newKey);
					break;
				case NUMENTRY:
					__esos_CB_WriteUINT8(numentry_handle->pst_Mailbox->pst_CBuffer,u8_newKey);
					break;
				default:
					break;
			}
			u8_newKey = 0;	// very important to reset this to 0
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
ESOS_USER_TASK(spi_task)
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
#define NUM_SIZE 200
volatile uint16_t au16_buffer[NUM_ADC_CHANNELS];
//volatile uint16_t  u16_pot[NUM_ADC_CHANNELS];
volatile UCHAR  u8_pot[NUM_ADC_CHANNELS];
volatile UCHAR curr_num[NUM_SIZE];
volatile UCHAR curr_num_ptr;
volatile uint8_t  u8_waiting;
volatile int lat5;

//******************************************************************************************//
//**************************************** convADC  ****************************************//
//******************************************************************************************//
ESOS_USER_TASK(convADC)
{
//    static  uint8_t data1, u8_wdtState;
//	static uint16_t u16_adcVal1, u16_adcVal2;
	static ESOS_TASK_HANDLE comm1_handle;
	static uint8_t u8_i;
	static float fres;
	comm1_handle = esos_GetTaskHandle(send_comm1);

    ESOS_TASK_BEGIN();
/*
	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
	ESOS_TASK_WAIT_ON_SEND_STRING("starting ADC task\r\n");
	ESOS_TASK_WAIT_ON_SEND_STRING("                                 \r");
	ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
*/
	CONFIG_RB0_AS_ANALOG();		// 1st knob on monster box
	CONFIG_RB1_AS_ANALOG();		// 2nd knob on monster box
	CONFIG_RB2_AS_ANALOG();		// engine temp
	CONFIG_RB3_AS_ANALOG();		// oil pressure
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
		for ( u8_i=0; u8_i<NUM_ADC_CHANNELS; u8_i++)
		{
//			u16_pot[u8_i] = au16_buffer[u8_i];
			fres = (float)au16_buffer[u8_i];
			fres *= 100.0;
			fres /= 1023.0;
			u8_pot[u8_i] = (UCHAR)fres;
/*			
			u16_pot[u8_i] <<= 6;
			if(u16_pot[u8_i] > 1)
				u16_pot[u8_i] |= 0x003F;
*/

#if 0
			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			if(u8_i == 0)
			{
//				ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
//				ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
				ESOS_TASK_WAIT_ON_SEND_UINT8(0xFE);
			}
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((uint8_t)(u16_pot[u8_i]>>8));
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((uint8_t)u16_pot[u8_i]);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
#endif
		}
		__esos_CB_WriteUINT8(comm1_handle->pst_Mailbox->pst_CBuffer,RT_DATA);
		ESOS_TASK_WAIT_TICKS(200);
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

	for ( u8_i=0; u8_i<NUM_ADC_CHANNELS; u8_i++)
	{
		au16_buffer[u8_i] = au16_adcHWBuff[u8_i];
	} //end for()
	u8_waiting = 0;  // signal main() that data is ready
	_AD1IF = 0;   //clear the interrupt flag
}

