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
// *********************************************
// Truck_App2.c
// *********************************************

/*
can't call ESOS_TASK_WAIT_TICKS from within mail recipient?

note: don't do any assignment statements before ESOS_TASK_BEGIN()
it causes strange side-effects
*/

#if 1
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

ESOS_SEMAPHORE(key_sem);
ESOS_SEMAPHORE(lcd_sem);
ESOS_USER_TASK(keypad);
ESOS_USER_TASK(poll_keypad);
//ESOS_USER_TASK(spi_task);
ESOS_USER_TASK(AVR_cmd);
ESOS_USER_TASK(main_proc);
ESOS_USER_TASK(send_fpga);
ESOS_USER_TASK(recv_lcd);
ESOS_USER_TASK(send_comm1);
ESOS_USER_TASK(recv_comm1);
ESOS_USER_TASK(menu_task);
ESOS_USER_TASK(password_task);
ESOS_USER_TASK(numentry_task);
ESOS_USER_TASK(display_menu);
ESOS_USER_TASK(display_rtlabels);
ESOS_USER_TASK(key_timer_task);
ESOS_USER_TASK(display_rtvalues);

static UCHAR avr_buffer[15];
static ESOS_TASK_HANDLE avr_handle;
static ESOS_TASK_HANDLE fpga_handle;

#define AVR_CALL() __esos_CB_WriteUINT8Buffer(avr_handle->pst_Mailbox->pst_CBuffer, &avr_buffer[0], 6);

#define MY_ESOS1

#define TDELAY 2
#define TDELAY2 1
#define VREF 3.3  //assume Vref = 3.3 volts
#define FLAG1	ESOS_USER_FLAG_1
#define PASSWORD_RETRIES_NUM 5

// handshaking lines to FPGA
#define DataReady _LATG14		// output
#define CmdParam _LATG12		// output
#define DataSent _RG13			// input

enum rt_values_offsets
{
	RPM,
	MPH,
	ENG_TEMP,
	OIL_PRES,
	AIR_TEMP,
	MAP,
	OIL_TEMP,
	O2,
	ODOM,
	TRIP
} RT_VALUES_OFFSETS_TYPES;

/*
enum menu_values_offsets
{
	ENABLE_STARTER,
	IGNITION,
	FUEL_PUMP,
	COOLING_FAN,
	RUNNING_LIGHTS,
	HEADLIGHTS
} MENU_VALUES_OFFSETS_TYPES;
*/
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
	UCHAR data_col;
	UCHAR str;
	UCHAR onoff;
} FORMAT_STR;

static FORMAT_STR menu_str[NUM_MENU_LABELS*NO_MENUS];

static FORMAT_STR rtlabel_str[NUM_RT_LABELS];

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


#define PWM_OFF_PARAM					0x01 // off
#define PWM_ON_PARAM					0x1F // on
#define PWM_80DC_PARAM					0x1A // duty_cycle = 80%
#define PWM_75DC_PARAM					0x16 // duty_cycle = 75%
#define PWM_60DC_PARAM					0x12 // duty_cycle = 60%
#define PWM_50DC_PARAM					0x0E // duty_cycle = square wave
#define PWM_30DC_PARAM					0x0A // duty_cycle = 30%
#define PWM_25DC_PARAM					0x06 // duty_cycle = 25%
#define PWM_12DC_PARAM					0x02 // duty_cycle = 12%

static uint16_t au16_buffer[NUM_ADC_CHANNELS];
static UCHAR u8_pot[NUM_ADC_CHANNELS];
uint8_t  u8_waiting;
int lat5;
static int key_enter_time;
static char password[PASSWORD_SIZE];
static char correct_password[PASSWORD_SIZE];
static UCHAR password_valid;
static int password_ptr;
static int password_retries;
static int dim_screen;
static int silent_key;
static UINT gl_rpm, gl_mph;
static int engine_run_time;
static int engine_on;
static int engine_shutdown;
static int engine_shuttingdown;
static char num_entry_num[SIZE_NUM];
static int num_entry_ptr;
static int num_entry_limit;
static int num_entry_type;
static UINT curr_num_entry_row;
static UINT curr_num_entry_col;
static void init_menu_labels(void);
static void init_rt_labels(void);

typedef enum
{
	NORMAL,
	PASSWORD,
	NUMENTRY,
}KEY_MODE;

KEY_MODE key_mode;

#endif
//******************************************************************************************//
//*************************************** menu_task ****************************************//
//******************************************************************************************//
ESOS_USER_TASK(menu_task)
{
	static UCHAR data1;
	static UINT data4;
    static ESOS_TASK_HANDLE menu_handle;
    static ESOS_TASK_HANDLE rt_handle;
	static ESOS_TASK_HANDLE comm1_handle;
	static int screen_dim;
	static UCHAR screen_dim_array[9];
	static int screen_dim_ptr;
	static int lights;
	
    ESOS_TASK_BEGIN();

	menu_handle = esos_GetTaskHandle(display_menu);
	rt_handle = esos_GetTaskHandle(display_rtlabels);

	comm1_handle = esos_GetTaskHandle(send_comm1);
	screen_dim = 0;

	screen_dim_array[0] =  PWM_OFF_PARAM;
	screen_dim_array[1] =  PWM_ON_PARAM;
	screen_dim_array[2] =  PWM_80DC_PARAM;
	screen_dim_array[3] =  PWM_75DC_PARAM;
	screen_dim_array[4] =  PWM_60DC_PARAM;
	screen_dim_array[5] =  PWM_50DC_PARAM;
	screen_dim_array[6] =  PWM_30DC_PARAM;
	screen_dim_array[7] =  PWM_25DC_PARAM;
	screen_dim_array[8] =  PWM_12DC_PARAM;
	screen_dim_ptr = 0;
	lights = 0;

	while (TRUE)
	{
		ESOS_TASK_WAIT_FOR_MAIL();

		while(ESOS_TASK_IVE_GOT_MAIL())
		{
			data1 = __esos_CB_ReadUINT8(__pstSelf->pst_Mailbox->pst_CBuffer);

			switch(data1)
			{
				case 	KP_1:
					switch(menu_ptr)
					{
						case 0:
							if(engine_on == 0)
								__esos_CB_WriteUINT8(comm1_handle->pst_Mailbox->pst_CBuffer,ENABLE_START);
						break;
						case 1:
						break;
						default:
						break;
					}
				break;

				case 	KP_2:
					switch(menu_ptr)
					{
						case 0:
							__esos_CB_WriteUINT8(comm1_handle->pst_Mailbox->pst_CBuffer,START_SEQ);
							engine_on = 1;
						break;
						case 1:
						break;
						default:
						break;
					}
				break;

				case	KP_3:
					switch(menu_ptr)
					{
						case 0:
							__esos_CB_WriteUINT8(comm1_handle->pst_Mailbox->pst_CBuffer,SHUTDOWN);
							engine_on = 0;
						break;
						case 1:
						break;
						default:
						break;
					}
				break;

				case	KP_4:
					switch(menu_ptr)
					{
						case 0:
							silent_key = (silent_key == 1?0:1);
						break;
						case 1:
						break;
						default:
						break;
					}
				break;

				case	KP_5:
					switch(menu_ptr)
					{
						case 0:
							data4 = LCD_PWM;
							data4 <<= 8;
							data4 &= 0xFF00;
							data4 |= screen_dim_array[screen_dim_ptr];
							__esos_CB_WriteUINT16(fpga_handle->pst_Mailbox->pst_CBuffer,data4);
							if(++screen_dim_ptr > 9)
								screen_dim_ptr = 0;
						break;
						case 1:
						break;
						default:
						break;
					}
				break;

				case	KP_6:
					switch(menu_ptr)
					{
						case 0:
							data4 = LCD_PWM;
							data4 <<= 8;
							data4 &= 0xFF00;
							data4 |= screen_dim_array[screen_dim_ptr];
							__esos_CB_WriteUINT16(fpga_handle->pst_Mailbox->pst_CBuffer,data4);
							if(--screen_dim_ptr < 0)
								screen_dim_ptr = 9;
						break;
						case 1:
						break;
						default:
						break;
					}
				break;

				case	KP_7:
					switch(menu_ptr)
					{
						case 0:	// toggle lights
							if(lights == 0)
								__esos_CB_WriteUINT8(comm1_handle->pst_Mailbox->pst_CBuffer,ON_LIGHTS);
							else __esos_CB_WriteUINT8(comm1_handle->pst_Mailbox->pst_CBuffer,OFF_LIGHTS);
							lights = (lights == 1?0:1);
						break;
						case 1:
						break;
						default:
						break;
					}
				break;

				case	KP_8:
/*
					switch(menu_ptr)
					{

						case 0:	// set time & date - use numentry and enter as: MMDDYYHHMM
							curr_num_entry_row = 7;
							curr_num_entry_col = 0;
							avr_buffer[0] = GOTO_CMD;
							avr_buffer[1] = (UCHAR)curr_num_entry_row;
							avr_buffer[2] = (UCHAR)curr_num_entry_col;
							AVR_CALL();
							key_mode = NUMENTRY;
							num_entry_type = TIME_DATE;
							num_entry_limit = 1;
						break;
						case 1:
						break;
						default:
						break;
					}
*/
				break;

				case	KP_9:
					switch(menu_ptr)
					{
						case 0:
						break;
						case 1:
						break;
						default:
						break;
					}
				break;

				case	KP_POUND:

					switch(menu_ptr)
					{
						case 0:
						break;
						case 1:
						break;
						default:
						break;
					}
				break;

				case	KP_AST:
					switch(menu_ptr)
					{
						case 0:
						break;
						case 1:
						break;
						default:
						break;
					}
				break;

				case	KP_0:
					switch(menu_ptr)
					{
						case 0:
						break;
						case 1:
						break;
						default:
						break;
					}
				break;

				case	KP_A:		// change menu

					if(menu_ptr > NO_MENUS)
						menu_ptr = 0;
					menu_ptr++;	
					__esos_CB_WriteUINT8(menu_handle->pst_Mailbox->pst_CBuffer,menu_ptr*NUM_MENU_LABELS);
				break;

				case	KP_B:		// toggle screen brightness
					data4 = LCD_PWM;
					data4 <<= 8;
					data4 &= 0xFF00;
					if(screen_dim == 1)
						data4 |= PWM_12DC_PARAM;
					else data4 |= PWM_ON_PARAM;
					__esos_CB_WriteUINT16(fpga_handle->pst_Mailbox->pst_CBuffer,data4);
					screen_dim = (screen_dim == 1?0:1);

				break;

				case	KP_C:		// toggle keypress tones
					silent_key = (silent_key == 1?0:1);
				break;

				case	KP_D:
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
ESOS_TASK_HANDLE password_task_handle;

ESOS_USER_TASK(password_task)
{
	static UCHAR data1, temp;
	static ESOS_TASK_HANDLE comm1_handle;
    static ESOS_TASK_HANDLE menu_handle;
    static ESOS_TASK_HANDLE rt_handle;

    ESOS_TASK_BEGIN();

    password_task_handle = ESOS_TASK_GET_TASK_HANDLE();
	comm1_handle = esos_GetTaskHandle(send_comm1);
	menu_handle = esos_GetTaskHandle(display_menu);
	rt_handle = esos_GetTaskHandle(display_rtlabels);
    
	ESOS_TASK_WAIT_TICKS(100);

	temp = 0;
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
					password[password_ptr] = data1;
					avr_buffer[0] = CHAR_CMD;
					avr_buffer[1] = '*';
					AVR_CALL();

					avr_buffer[0] = SET_MODE_CMD;
					avr_buffer[1] = TEXT_ON | CURSOR_BLINK_ON;
					avr_buffer[2] = 7;
					avr_buffer[3] = password_ptr+1;
					avr_buffer[4] = (LINE_1_CURSOR);
					AVR_CALL();

				break;

				case	KP_A:
					break;

				// backspace over current password
				case	KP_B:
					password_ptr--;
					avr_buffer[0] = SET_MODE_CMD;
					avr_buffer[1] = TEXT_ON | CURSOR_BLINK_ON;
					avr_buffer[2] = 7;
					avr_buffer[3] = password_ptr;
					avr_buffer[4] = (LINE_1_CURSOR);
					AVR_CALL();
					break;

				case	KP_C:
				case	KP_D:
				case	KP_POUND:
				case	KP_AST:
				break;
				
				default:
				temp = 2;
				break;
			}

			// if password incorrect

			if(password[password_ptr] != correct_password[password_ptr])
			{

				// if incorrect password, first display "bad password" message
				avr_buffer[0] = LCD_CLRSCR;
				avr_buffer[1] = 0;
				AVR_CALL();

				avr_buffer[0] = EEPROM_STR;
				avr_buffer[1] = 5;
				avr_buffer[2] = 0;
				avr_buffer[3] = VARIOUS_MSG_OFFSET + 1;	// "bad password"
				avr_buffer[4] = 1;
				AVR_CALL();

				avr_buffer[0] = EEPROM_STR;
				avr_buffer[1] = 6;	// row 7
				avr_buffer[2] = 0;	// col 0
				avr_buffer[3] =  VARIOUS_MSG_OFFSET;	// ("enter password")
				avr_buffer[4] = 1;
				AVR_CALL();

				avr_buffer[0] = SET_MODE_CMD;
				avr_buffer[1] = TEXT_ON | CURSOR_BLINK_ON;
				avr_buffer[2] = 7;
				avr_buffer[3] = 0;
				avr_buffer[4] = (LINE_1_CURSOR);
				AVR_CALL();
				memset(password,0,PASSWORD_SIZE);
				password_ptr = -1;
				password_retries++;
			}else if(password_ptr == strlen(correct_password) - 1)
			{
				memset(password,0,PASSWORD_SIZE);
				password_ptr = -1;
				avr_buffer[0] = LCD_CLRSCR;
				avr_buffer[1] = 0;
				AVR_CALL();

//				__esos_CB_WriteUINT8(menu_handle->pst_Mailbox->pst_CBuffer,data1);

				__esos_CB_WriteUINT8(rt_handle->pst_Mailbox->pst_CBuffer,data1);
				key_mode = NORMAL;
				engine_shuttingdown = 0;
			}
			
			password_ptr++;
			if(password_ptr > PASSWORD_SIZE)
				password_ptr = 0;

		}		// end of mail
	}	// end of 'while(TRUE)'
    ESOS_TASK_END();
}

//******************************************************************************************//
//************************************* numentry_task **************************************//
//******************************************************************************************//
ESOS_USER_TASK(numentry_task)
{
	static UCHAR data1;
	static int i;
	
    ESOS_TASK_BEGIN();
	memset(num_entry_num,0,SIZE_NUM);
	num_entry_ptr = 0;
	num_entry_limit = 0;
	
	while (TRUE)
	{
		ESOS_TASK_WAIT_FOR_MAIL();

		while(ESOS_TASK_IVE_GOT_MAIL())
		{
			data1 = __esos_CB_ReadUINT8(__pstSelf->pst_Mailbox->pst_CBuffer);

			switch(data1)
			{
				case 	KP_1:
				case	KP_2:
				case	KP_3:
				case	KP_4:
				case	KP_5:
				case	KP_6:
				case	KP_7:
				case	KP_8:
				case	KP_9:
				case	KP_0:
					if(data1 == KP_0)
						data1 = data1 - KP_0;
					else	
						data1 = data1 - KP_1 + 1;
					num_entry_num[num_entry_ptr++] = data1;
					if(num_entry_ptr > num_entry_limit)
					{
						num_entry_ptr = num_entry_limit;
					}
					avr_buffer[0] = CHAR_CMD;
					avr_buffer[1] = data1 + 0x30;
					AVR_CALL();
					curr_num_entry_col++;

					avr_buffer[0] = SET_MODE_CMD;
					avr_buffer[1] = TEXT_ON | CURSOR_BLINK_ON;
					avr_buffer[2] = curr_num_entry_row;
					avr_buffer[3] = curr_num_entry_col;
					avr_buffer[4] = (LINE_1_CURSOR);
					AVR_CALL();
				break;

				// 'backspace'
				case	KP_B:
					avr_buffer[0] = GOTO_CMD;
					avr_buffer[1] = curr_num_entry_row;
					avr_buffer[2] = --curr_num_entry_col;
					AVR_CALL();
					avr_buffer[0] = CHAR_CMD;
					avr_buffer[1] = 0x20;
					AVR_CALL();
					avr_buffer[0] = SET_MODE_CMD;
					avr_buffer[1] = TEXT_ON | CURSOR_BLINK_ON;
					avr_buffer[2] = curr_num_entry_row;
					avr_buffer[3] = curr_num_entry_col;
					avr_buffer[4] = (LINE_1_CURSOR);
					AVR_CALL();
					
					if(--num_entry_ptr < 0)
						num_entry_ptr = 0;
				break;
				// clear entry and start over
				case	KP_A:
				case	KP_C:
					curr_num_entry_col -= num_entry_ptr;
					avr_buffer[0] = GOTO_CMD;
					avr_buffer[1] = curr_num_entry_row;
					avr_buffer[2] = curr_num_entry_col;
					AVR_CALL();
					for(i = 0;i < num_entry_ptr;i++)
					{
						avr_buffer[0] = CHAR_CMD;
						avr_buffer[1] = 0x20;
						AVR_CALL();
					}
					avr_buffer[0] = GOTO_CMD;
					avr_buffer[1] = curr_num_entry_row;
					avr_buffer[2] = curr_num_entry_col;
					AVR_CALL();
					avr_buffer[0] = SET_MODE_CMD;
					avr_buffer[1] = TEXT_ON | CURSOR_BLINK_ON;
					avr_buffer[2] = curr_num_entry_row;
					avr_buffer[3] = curr_num_entry_col;
					avr_buffer[4] = (LINE_1_CURSOR);
					AVR_CALL();

					if(data1 == KP_A)
					{
						avr_buffer[0] = GOTO_CMD;
						avr_buffer[1] = 0;
						avr_buffer[2] = 0;
						AVR_CALL();

						avr_buffer[0] = CHAR_CMD;
						for(i = 0;i < num_entry_ptr;i++)
						{
							avr_buffer[1] = num_entry_num[i] + 0x30;
							AVR_CALL();
						}
						key_mode = NORMAL;
						// now do something with the number just entered in num_entry_num
						switch(num_entry_type)
						{
							case TIME_DATE:
							break;
							case TIME_ENG_OFF:
							break;
							case FPGA_SEND_RATE:
							break;
							case RPM_MPH_UPDATE_RATE:
							break;
							case HIGH_REV_LIMIT:
							break;
							case LOW_REV_LIMIT:
							break;
							case SET_LIGHTS_OFF:
							break;
							case CHANGE_PASSWORD:
							break;
							default:
							break;
						}
					}
					memset(num_entry_num,0,SIZE_NUM);
					num_entry_ptr = 0;

				break;
				case	KP_D:
					num_entry_ptr = 0;
					memset(num_entry_num,0,SIZE_NUM);
					key_mode = NORMAL;
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

static ISRSTATE e_isrState;
static uint8_t u8_newKey = 0;

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
	static UINT data1;
	static UCHAR data2;

    ESOS_TASK_BEGIN();

    data2 = 0;
    data1 = 0;
   	silent_key = 0;

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
					if(silent_key == 0)
					{
						data1 = DTMF_TONE_ON;
						data1 <<= 8;
						data1 &= 0xFF00;
						data1 |= u8_newKey - 0xE0;
						__esos_CB_WriteUINT16(fpga_handle->pst_Mailbox->pst_CBuffer,data1);
					}


				} else e_isrState = STATE_WAIT_FOR_PRESS;
				break;

			case STATE_WAIT_FOR_RELEASE:
	//keypad released
				if (KEY_RELEASED())
				{
					ESOS_SIGNAL_SEMAPHORE(key_sem,1);
					e_isrState = STATE_WAIT_FOR_PRESS;
// send the message to FPGA to turn on DTMF tone

					if(silent_key == 0)
					{
						data1 = DTMF_TONE_OFF;
						data1 <<= 8;
						data1 &= 0xFF00;
						__esos_CB_WriteUINT16(fpga_handle->pst_Mailbox->pst_CBuffer,data1);
					}

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
//************************************ key_timer_task **************************************//
//******************************************************************************************//
// dim the display after so many seconds of not getting a keypress
ESOS_USER_TASK(key_timer_task)
{
	static UINT data3;
	static ESOS_TASK_HANDLE comm1_handle;

    ESOS_TASK_BEGIN();
	comm1_handle = esos_GetTaskHandle(send_comm1);

    key_enter_time = 0;
	dim_screen = 0;
	engine_run_time = 0;
	engine_shutdown = 30;
	engine_shuttingdown = 0;
	while (TRUE)
	{
		ESOS_TASK_WAIT_TICKS(1000);

		if(engine_on == 1)
			engine_run_time++;
		else
			engine_run_time = 0;	

/*
		avr_buffer[0] = EEPROM_STR;
		avr_buffer[1] = 1;	// row 1
		avr_buffer[2] = 0;	// col 0
		avr_buffer[3] = VARIOUS_MSG_OFFSET + 8;	//  engine run time msg
		avr_buffer[4] = 10;
		AVR_CALL();
*/
		avr_buffer[0] = SEND_INT_RT_VALUES;
		avr_buffer[1] = 15;
		avr_buffer[2] = 35;
		avr_buffer[3] = (UCHAR)(engine_run_time >> 8);
		avr_buffer[4] = (UCHAR)engine_run_time;
		AVR_CALL();

		if(engine_shutdown == 29)
		{
			data3 = SPECIAL_TONE_ON;
			data3 <<= 8;
			data3 &= 0xFF00;
			__esos_CB_WriteUINT16(fpga_handle->pst_Mailbox->pst_CBuffer,data3);
			data3 = LCD_PWM;
			data3 <<= 8;
			data3 &= 0xFF00;
			data3 |= PWM_ON_PARAM;
			__esos_CB_WriteUINT16(fpga_handle->pst_Mailbox->pst_CBuffer,data3);
			key_enter_time = 0;
			key_mode = PASSWORD;
			password_valid = 0;
 			memset(password,0,PASSWORD_SIZE);
			password_ptr = 0;
			avr_buffer[0] = GOTO_CMD;
			avr_buffer[1] = 0;
			avr_buffer[2] = 7;
			AVR_CALL();
			avr_buffer[0] = PASSWORD_MODE;
//			avr_buffer[1] = (UCHAR)strlen(correct_password);
			avr_buffer[1] = 0;
			AVR_CALL();

			avr_buffer[0] = EEPROM_STR;
			avr_buffer[1] = 0;	// row 0
			avr_buffer[2] = 0;	// col 0
			avr_buffer[3] = VARIOUS_MSG_OFFSET + 6;	//  re-enter password
			avr_buffer[4] = 10;
			AVR_CALL();

			avr_buffer[0] = EEPROM_STR;
			avr_buffer[1] = 0;	// row 0
			avr_buffer[2] = 20;	// col 0
			avr_buffer[3] = VARIOUS_MSG_OFFSET + 7;	//  in x seconds
			avr_buffer[4] = 10;
			AVR_CALL();
		}
		if(engine_shutdown == 28)
		{
			data3 = DTMF_TONE_OFF;
			data3 <<= 8;
			data3 &= 0xFF00;
			__esos_CB_WriteUINT16(fpga_handle->pst_Mailbox->pst_CBuffer,data3);
		}
		if(engine_shutdown > 1 && engine_shutdown < 30)
		{
//			avr_buffer[0] = SEND_INT_RT_VALUES;
			avr_buffer[0] = SEND_BYTE_RT_VALUES;
			avr_buffer[1] = 0;
			avr_buffer[2] = 23;
			avr_buffer[3] = (UCHAR)engine_shutdown;
//			avr_buffer[4] = (UCHAR)(engine_shutdown);
			AVR_CALL();
		}
		if(engine_shutdown == 5)
		{
			data3 = SPECIAL_TONE_ON;
			data3 <<= 8;
			data3 &= 0xFF00;
			__esos_CB_WriteUINT16(fpga_handle->pst_Mailbox->pst_CBuffer,data3);
		}
		if(engine_shutdown == 4)
		{
			data3 = DTMF_TONE_OFF;
			data3 <<= 8;
			data3 &= 0xFF00;
			__esos_CB_WriteUINT16(fpga_handle->pst_Mailbox->pst_CBuffer,data3);
		}
		if(engine_shutdown == 3)
		{
			data3 = SPECIAL_TONE_ON;
			data3 <<= 8;
			data3 &= 0xFF00;
			__esos_CB_WriteUINT16(fpga_handle->pst_Mailbox->pst_CBuffer,data3);
		}
		if(engine_shutdown == 2)
		{
			data3 = DTMF_TONE_OFF;
			data3 <<= 8;
			data3 &= 0xFF00;
			__esos_CB_WriteUINT16(fpga_handle->pst_Mailbox->pst_CBuffer,data3);
		}
		if(engine_shutdown == 1)
		{
			data3 = SPECIAL_TONE_ON;
			data3 <<= 8;
			data3 &= 0xFF00;
			__esos_CB_WriteUINT16(fpga_handle->pst_Mailbox->pst_CBuffer,data3);
		}

		if(engine_shutdown == 0)
		{
			data3 = DTMF_TONE_OFF;
			data3 <<= 8;
			data3 &= 0xFF00;
			__esos_CB_WriteUINT16(fpga_handle->pst_Mailbox->pst_CBuffer,data3);
			__esos_CB_WriteUINT8(comm1_handle->pst_Mailbox->pst_CBuffer,SHUTDOWN);
			engine_shuttingdown = 0;
			engine_shutdown = 30;
			engine_on = 0;
			avr_buffer[0] = LCD_CLRSCR;
			avr_buffer[1] = 0;
			AVR_CALL();
			avr_buffer[0] = EEPROM_STR;
			avr_buffer[1] = 0;	// row 0
			avr_buffer[2] = 0;	// col 0
			avr_buffer[3] = VARIOUS_MSG_OFFSET + 9;
			avr_buffer[4] = 10;
			AVR_CALL();
		}
		if(engine_shuttingdown > 0)
			engine_shutdown--;
		else
		{
			engine_shutdown = 30;
/*
			avr_buffer[0] = GOTO_CMD;
			avr_buffer[1] = 0;
			avr_buffer[1] = 23;
			AVR_CALL();
			avr_buffer[0] = CHAR_CMD;
			avr_buffer[1] = 0x20;
			AVR_CALL();
			avr_buffer[0] = CHAR_CMD;
			avr_buffer[1] = 0x20;
			AVR_CALL();
*/
		}
		if(key_enter_time == 200)	// 3 1/3 minutes
		{
			data3 = LCD_PWM;
			data3 <<= 8;
			data3 &= 0xFF00;
			data3 |= PWM_80DC_PARAM;
			__esos_CB_WriteUINT16(fpga_handle->pst_Mailbox->pst_CBuffer,data3);
			dim_screen++;
		}
		if(key_enter_time == 400)	// 6 2/3 minutes
		{
			data3 = LCD_PWM;
			data3 <<= 8;
			data3 &= 0xFF00;
			data3 |= PWM_75DC_PARAM;
			__esos_CB_WriteUINT16(fpga_handle->pst_Mailbox->pst_CBuffer,data3);
			dim_screen++;
		}
		if(key_enter_time == 800)	// 13 1/3 minutes
		{
			data3 = LCD_PWM;
			data3 <<= 8;
			data3 &= 0xFF00;
			data3 |= PWM_50DC_PARAM;
			__esos_CB_WriteUINT16(fpga_handle->pst_Mailbox->pst_CBuffer,data3);
			dim_screen++;
		}
		if(key_enter_time == 1600)	// ~26 minutes
		{
			data3 = LCD_PWM;
			data3 <<= 8;
			data3 &= 0xFF00;
			data3 |= PWM_25DC_PARAM;
			__esos_CB_WriteUINT16(fpga_handle->pst_Mailbox->pst_CBuffer,data3);
			dim_screen++;
		}
		if(key_enter_time == 3200)	// ~52 minutes
		{
			data3 = LCD_PWM;
			data3 <<= 8;
			data3 &= 0xFF00;
			data3 |= PWM_12DC_PARAM;
			__esos_CB_WriteUINT16(fpga_handle->pst_Mailbox->pst_CBuffer,data3);
			dim_screen++;
		}
		if(key_enter_time == 6400)	// ~1 hour & 46 minutes
		{
			data3 = LCD_PWM;
			data3 <<= 8;
			data3 &= 0xFF00;
			data3 |= PWM_OFF_PARAM;
			__esos_CB_WriteUINT16(fpga_handle->pst_Mailbox->pst_CBuffer,data3);
			dim_screen++;
		}
		key_enter_time++;
	}
    ESOS_TASK_END();
}
//******************************************************************************************//
//************************************** poll_keypad ***************************************//
//******************************************************************************************//
ESOS_USER_TASK(poll_keypad)
{

    static ESOS_TASK_HANDLE menu_task_handle;
    static ESOS_TASK_HANDLE password_handle;
    static ESOS_TASK_HANDLE numentry_handle;
	static UINT data3;

    ESOS_TASK_BEGIN();

	menu_task_handle = esos_GetTaskHandle(menu_task);
	password_handle = esos_GetTaskHandle(password_task);
	numentry_handle = esos_GetTaskHandle(numentry_task);

	configKeypad();

	while (TRUE)
	{
		ESOS_TASK_WAIT_SEMAPHORE(key_sem,1);
		if(u8_newKey)
		{
			switch(key_mode)
			{
				case NORMAL:
					__esos_CB_WriteUINT8(menu_task_handle->pst_Mailbox->pst_CBuffer,u8_newKey);
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
			// if key_enter_time has dimmed screen then
			// turn it back on
	
			if(dim_screen > 0)
			{
				data3 = LCD_PWM;
				data3 <<= 8;
				data3 &= 0xFF00;
				data3 |= PWM_ON_PARAM;
				__esos_CB_WriteUINT16(fpga_handle->pst_Mailbox->pst_CBuffer,data3);
				dim_screen = 0;
			}
			key_enter_time = 0;
		}
	}
    ESOS_TASK_END();
}
//******************************************************************************************//
//**************************************** convADC  ****************************************//
//******************************************************************************************//
ESOS_USER_TASK(convADC)
{
//    static  uint8_t data1, u8_wdtState;
//	static uint16_t u16_adcVal1, u16_adcVal2;
	static uint8_t i;
	static float fres;
//	static ESOS_TASK_HANDLE comm1_handle;

    ESOS_TASK_BEGIN();
//	comm1_handle = esos_GetTaskHandle(send_comm1);

	CONFIG_RB0_AS_ANALOG();		// 2nd knob on monster box
	CONFIG_RB1_AS_ANALOG();		// 1st knob on monster box
	CONFIG_RB2_AS_ANALOG();		// engine temp
	CONFIG_RB3_AS_ANALOG();		// oil pressure

	CONFIG_RB4_AS_ANALOG();		// air temp
	CONFIG_RB5_AS_ANALOG();		// map
/*
	CONFIG_RB6_AS_ANALOG();		// can't use these
	CONFIG_RB7_AS_ANALOG();		// unless prog to chip and jumper
*/
	CONFIG_RB8_AS_ANALOG();		// oil temp
	CONFIG_RB9_AS_ANALOG();		// o2

	CONFIG_RB10_AS_ANALOG();	// extra
	CONFIG_RB11_AS_ANALOG();	// extra

	CONFIG_RB12_AS_ANALOG();	// extra
//	CONFIG_RB13_AS_ANALOG();

//	CONFIG_RB14_AS_ANALOG();
//	CONFIG_RB15_AS_ANALOG();

//	configADC1_AutoScanIrqCH0( ADC_SCAN_AN0 | ADC_SCAN_AN1 | ADC_SCAN_AN2 |
//			 ADC_SCAN_AN3 | ADC_SCAN_AN4 | ADC_SCAN_AN5, 31, 0);

//	configADC1_AutoScanIrqCH0( 0xFFFF, 31, 0);
	/// can't do 6 & 7 unless program to chip and use jumpers
	// because 6 & 7 are shared by Pickit programmer!
	configADC1_AutoScanIrqCH0( 0x1F3F, 31, 0);
//	configADC1_AutoHalfScanIrqCH0(0x00FF, 31, 0);
	while (!AD1CON1bits.DONE)
		ESOS_TASK_WAIT_TICKS(1);

//	ESOS_TASK_SLEEP();

	while (TRUE)
	{
		if(key_mode == NORMAL)
		{
			while (u8_waiting)	// wait for valid data in ISR
				ESOS_TASK_WAIT_TICKS(1);

			u8_waiting = 0;
			for (i = 0;i<NUM_ADC_CHANNELS;i++)
			{
				fres = (float)au16_buffer[i];
				fres *= 107.0;
				fres /= 1023.0;
				u8_pot[i] = (UCHAR)fres;
			}
			// this sends the ADC data to the TS-7200
//			__esos_CB_WriteUINT8(comm1_handle->pst_Mailbox->pst_CBuffer,RT_DATA);
		}
		ESOS_TASK_WAIT_TICKS(500);
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
