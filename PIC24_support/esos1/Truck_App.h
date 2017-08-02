//#define _INT3IF
//#define _INT4IF
#include "../esos/include/esos.h"
#include "../esos/include/pic24/esos_pic24.h"
#include "../esos/include/pic24/esos_pic24_rs232.h"
#include "../esos/include/pic24/esos_pic24_spi.h"
#include "../lib/include/pic24_timer.h"
#include "../lib/include/pic24_util.h"
#include "../lib/include/pic24_adc.h"
#include "../lib/include/pic24_ports_config.h"
#include "../lib/include/pic24_ports_mapping.h"
// DEFINEs go here
//#define   CONFIG_LED1()   printf("called CONFIG_LED1()\n");
uint8_t     LED1 = TRUE;      // LED1 is initially "on"
#define TEMP_DELAY		5000
#define TEMP_DELAY3		2
#define TEMP_DELAY2		1000
#define TDELAY 2
#define TDELAY2 1
#define VREF 3.3  //assume Vref = 3.3 volts
#define FLAG1        ESOS_USER_FLAG_1

enum data_types
{
	RT_LOW,				// UCHAR without high bit set
	RT_HIGH0,			// UCHAR with bit 7 set
	RT_HIGH1,			// UINT with neither 7 or 15 set
	RT_HIGH2,			// bit 7 of UINT set
	RT_HIGH3			// bit 15 of UINT set
} DATA_TYPES;

enum rt_types
{
	RT_TRIP = 0xF6,
	RT_TIME,
	RT_AIRT,
	RT_O2,
	RT_MAP,
	RT_OILT,
	RT_OILP,
	RT_ENGT,
	RT_MPH,
	RT_RPM	// this should be 0xFE
} RT_TYPES;

enum key_types
{
	KP_POUND = 0xE0, // '#'
	KP_AST, // '*'
	KP_0, // '0'
	KP_1, // '1'
	KP_2, // '2'
	KP_3, // '3'
	KP_4, // '4'
	KP_5, // '5'
	KP_6, // '6'
	KP_7, // '7'
	KP_8, // '8'
	KP_9, // '9'
	KP_A, // 'A'
	KP_B, // 'B'
	KP_C, // 'C'
	KP_D // 'D'
} KEY_TYPES;

enum fpga_recv_types
{
	FPGA_RPM = 0x80,
	FPGA_MPH,
	FPGA_REVLIMIT,
	FPGA_O2,
	FPGA_MAP,
	FPGA_AIRT
} FPGA_RECV_TYPES;

enum fpga_send_types
{
	SET_BR_RPM = 0x80,
	SET_BR_MPH,				// 0x81
	SET_DEC_RPM,			// 0x82
	SET_DEC_MPH,
	SET_BAUD_RPM,
	SET_BAUD_MPH,
	SET_DELAY_RPM,
	SET_DELAY_MPH,
	SET_FACT_RPM,
	SET_FACT_MPH			// 0x89
} FPGA_SEND_TYPES;

enum spi_recv_states
{
	CMD = 0xAA,
	DATA0,
	DATA1,
	DATA2
} SPI_RECV_STATES;

// GLOBALs go here
//  Generally, the user-created semaphores will be defined/allocated here

//ESOS_SEMAPHORE( sem_CapturedData );

volatile uint8_t row;
volatile uint8_t cmd;
volatile uint8_t data;
volatile UINT32 U32_lastCapture; // UINT32 declared in all_generic.h

#define KEYPAD_DELAY    100
#define KEYPAD_DEBOUNCE_DELAY 2
volatile uint8_t col;
volatile uint8_t last_code;
volatile uint8_t user_flag;

ESOS_USER_TASK(keypad);
ESOS_USER_TASK(poll_keypad);
ESOS_SEMAPHORE(key_sem);
ESOS_USER_TASK(comm1_task);
ESOS_USER_TASK(comm2_task);
ESOS_USER_TASK(sendFPGA);
ESOS_USER_TASK(sendFPGA2);
ESOS_USER_TASK(recvFPGA);
ESOS_USER_TASK(send_cmd_param);
ESOS_SEMAPHORE(send_sem);
ESOS_USER_TASK(convADC);
ESOS_USER_TASK(test_timer);
ESOS_USER_TASK(comm1_task);
ESOS_USER_TASK(echo_spi_task);
//ESOS_USER_TASK(fast_echo_spi_task);
CONFIG_SPI_MASTER();
CONFIG_SPI_SLAVE();

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
/*
		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(e_isrState);
		ESOS_TASK_WAIT_ON_SEND_UINT8(e_isrState);
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
*/
		switch (e_isrState)
		{
			case STATE_WAIT_FOR_PRESS:
				if (KEY_PRESSED() && (u8_newKey == 0))
				{
	//ensure that key is sampled low for two consecutive interrupt periods
/*
					ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
					ESOS_TASK_WAIT_ON_SEND_STRING("wait for press");
					ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
*/
					e_isrState = STATE_WAIT_FOR_PRESS2;
				}
				break;
			case STATE_WAIT_FOR_PRESS2:
				if (KEY_PRESSED())
				{
	// a key is ready
/*
					ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
					ESOS_TASK_WAIT_ON_SEND_STRING("key pressed");
					ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
*/
					u8_newKey = doKeyScan();
					e_isrState = STATE_WAIT_FOR_RELEASE;
				} else e_isrState = STATE_WAIT_FOR_PRESS;
				break;

			case STATE_WAIT_FOR_RELEASE:
	//keypad released
				if (KEY_RELEASED())
				{
/*
					ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
					ESOS_TASK_WAIT_ON_SEND_STRING("key released");
					ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
*/
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
    ESOS_TASK_BEGIN();

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
		ESOS_TASK_WAIT_TICKS(1);
		if (u8_newKey)
		{
			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(u8_newKey);
//		    ESOS_TASK_WAIT_ON_SEND_UINT8(u8_newKey);
			ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
			ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
			u8_newKey = 0;
		}
	}
    ESOS_TASK_END();
}

//******************************************************************************************//
//************************************ semd_cmd_param **************************************//
//******************************************************************************************//
ESOS_USER_TASK(send_cmd_param)
{
    static  uint8_t cmd, param;
    static  uint16_t data;

    ESOS_TASK_BEGIN();

    while(TRUE)
    {
        ESOS_TASK_WAIT_FOR_MAIL();
        while(ESOS_TASK_IVE_GOT_MAIL())
        {
			// this can also be 8 bytes
            data = __esos_CB_ReadUINT16(__pstSelf->pst_Mailbox->pst_CBuffer);
            cmd = (uint8_t)(data >> 8);
            param = (uint8_t)data;

#if 0
			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(param);
			ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(cmd);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
#endif
			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
			ESOS_TASK_WAIT_ON_SEND_UINT82(0xAA); 		// send byte to FPGA
			ESOS_TASK_WAIT_TICKS(TEMP_DELAY3);

			ESOS_TASK_WAIT_ON_SEND_UINT82(0x55);
			ESOS_TASK_WAIT_TICKS(TEMP_DELAY3);

			ESOS_TASK_WAIT_ON_SEND_UINT82(param);
			ESOS_TASK_WAIT_TICKS(TEMP_DELAY3);

			ESOS_TASK_WAIT_ON_SEND_UINT82(cmd);
			ESOS_TASK_WAIT_TICKS(TEMP_DELAY3);

			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();
        }
    }
    ESOS_TASK_END();
}
#if 0
#define   CONFIG_SLAVE_ENABLE() CONFIG_RF5_AS_DIG_OUTPUT()
#define   SLAVE_ENABLE()        LATFbits.LATF5 = 0
#define   SLAVE_DISABLE()       LATFbits.LATF5 = 1
#endif

//#define   CONFIG_SLAVE2_ENABLE() CONFIG_RG8_AS_DIG_OUTPUT()
//#define   SLAVE2_ENABLE()        LATGbits.LATG8 = 0
//#define   SLAVE2_DISABLE()       LATGbits.LATG8 = 1
//******************************************************************************************//
//************************************** CONFIG_SPI ****************************************//
//******************************************************************************************//
CONFIG_SPI_MASTER()
{
#if 0
  SPI1CON1 = SEC_PRESCAL_1_1		|		//1:1 secondary prescale	40MHz
             PRI_PRESCAL_4_1		|		//4:1 primary prescale
             CLK_POL_ACTIVE_HIGH	|		//clock active high (CKP = 0)
             SPI_CKE_ON				|		//out changes active to idle (CKE=1)
             SPI_MODE8_ON			|		//8-bit mode
             MASTER_ENABLE_ON;				//master mode
#endif
//#if 0
  SPI1CON1 = SEC_PRESCAL_8_1		|		//8:1 secondary prescale
             PRI_PRESCAL_64_1		|		//64:1 primary prescale
             CLK_POL_ACTIVE_HIGH	|		//clock active high (CKP = 0)
             SPI_CKE_ON				|		//out changes active to idle (CKE=1)
             SPI_MODE8_ON			|		//8-bit mode
//             SPI_SMP_ON				|		// input data sampled at end of data output time
             MASTER_ENABLE_ON;				//master mode
//#endif
//	SPI1CON2 = FRAME_ENABLE_ON | FRAME_SYNC_OUTPUT | FRAME_SYNC_ACTIVE_LOW | SPI_FRM_PULSE_PREV_CLK;
//	SPI1CON2 = FRAME_ENABLE_ON | FRAME_SYNC_OUTPUT | FRAME_SYNC_ACTIVE_LOW | SPI_FRM_PULSE_FIRST_CLK;
//	SPI1CON2 = FRAME_ENABLE_ON | FRAME_SYNC_OUTPUT | FRAME_SYNC_ACTIVE_HIGH;
/*
pin 47	RD14	RPI42	MISO	(input)
pin 48	RD15	RP5		MOSI	(output)
pin 49	RF4		RP10	SCLK	(output)
pin 50	RF5		RP17	SS		(output)
*/
	CONFIG_SDI1_TO_RP(RD14_RP);
	CONFIG_RD14_AS_DIG_INPUT();
	CONFIG_SDO1_TO_RP(RD15_RP);
	CONFIG_RD15_AS_DIG_OUTPUT();
	CONFIG_SCK1OUT_TO_RP(RF4_RP);
	CONFIG_RF4_AS_DIG_OUTPUT();
	CONFIG_SS1OUT_TO_RP(RF5_RP);
	CONFIG_RF5_AS_DIG_OUTPUT();
	SPI1STATbits.SPIEN = 1;  //enable SPI mode
}

//******************************************************************************************//
//********************************** CONFIG_SPI_SLAVE **************************************//
//******************************************************************************************//
CONFIG_SPI_SLAVE()
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
	static uint8_t data2;
	static uint8_t cmd;
	static uint8_t i;
	static char rpm_disp[5];
	static char mph_disp[5];
	static uint16_t res, res2;
	static uint16_t res3, res4;
	static char res_str[10];
	static char res2_str[10];
	static uint8_t  res_array[24];

	memset(rpm_disp,0,sizeof(rpm_disp));
	memset(mph_disp,0,sizeof(mph_disp));

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
//******************************************************************************************//
//*************************************** comm1_task ***************************************//
//******************************************************************************************//
// comm1_task - uses comm1 - wait for char from comm1
ESOS_USER_TASK(comm1_task)
{
    static  uint8_t data1, data2;
    static  uint8_t temp;
    static int first_glitch = 0;
//    static ESOS_TASK_HANDLE h_Task3;

    ESOS_TASK_BEGIN();

	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
    ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
    ESOS_TASK_WAIT_ON_SEND_STRING("comm1_task on comm1");
    ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
    ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
	ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
/*
	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
    ESOS_TASK_WAIT_ON_SEND_UINT82('\n');
    ESOS_TASK_WAIT_ON_SEND_UINT82('\r');
    ESOS_TASK_WAIT_ON_SEND_STRING2("comm1_task on comm2");
    ESOS_TASK_WAIT_ON_SEND_UINT82('\n');
    ESOS_TASK_WAIT_ON_SEND_UINT82('\r');
	ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();
*/
//	data = 0x21;
	data1 = 0x21;
	data2 = 0x30;

	while(TRUE)
    {
		// wait for input from comm1
/*
        ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
        ESOS_TASK_WAIT_ON_GET_UINT8(data2);
        ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
*/
		// send data1 to FPGA
		if(++data1 > 0x7e)
			data1 = 0x21;

//        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
//        ESOS_TASK_WAIT_ON_SEND_UINT82(data1);
//        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();

//		ESOS_TASK_WAIT_TICKS(TDELAY2);

		// read what's sent back into data2
//        ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM2();
//        ESOS_TASK_WAIT_ON_GET_UINT82(data2);
//        ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM2();

//		ESOS_TASK_WAIT_TICKS(TDELAY);


//        if(++data1 > 0x7e)
//        	data1 = 0x21;

        if(++data2 > 0x7e)
        	data2 = 0x21;

		// report what's sent back to comm1 (linux box monitor)
        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
/*
        if((data2 < 0x20 || data2 > 0x7e) && first_glitch)
        {
			ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
			ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
    		ESOS_TASK_WAIT_ON_SEND_STRING("glitch: ");
            ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(data2);
			ESOS_TASK_WAIT_TICKS(2000);
			ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
			ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
		} 
        else
	        ESOS_TASK_WAIT_ON_SEND_UINT8(data2);
*/
        if(++temp == 93)
//        if(data2 >= 0x1f)
        {
			ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
			ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
			temp = 0;
        }
//        ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(data2);
		ESOS_TASK_WAIT_ON_SEND_UINT8(data2);
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        
		ESOS_TASK_WAIT_TICKS(3000);

	}
//    h_Task3 = esos_GetTaskHandle(task3);
//        ESOS_TASK_WAIT_FOR_MAIL();
//        while(ESOS_TASK_IVE_GOT_MAIL())
/*
	while(TRUE)
    {
        ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
        ESOS_TASK_WAIT_ON_GET_UINT8(data);
        ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();

        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM3();
        ESOS_TASK_WAIT_ON_SEND_UINT83(data);
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM3();
    } // endof while()
*/
    ESOS_TASK_END();
}
//******************************************************************************************//
//*************************************** comm3_task ***************************************//
//******************************************************************************************//
// just echo what's sent from AVR to comm1 (for debugging)
ESOS_USER_TASK(comm3_task)
{
    static  uint8_t data;
    static  uint8_t data2;
    static	uint8_t dec;
//    static ESOS_TASK_HANDLE h_Task3;

    ESOS_TASK_BEGIN();
	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
    ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
    ESOS_TASK_WAIT_ON_SEND_STRING("comm3_task");
	ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();


	data = 0x21;
	data2 = 0;
    while (1)
    {
		ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
		ESOS_TASK_WAIT_ON_SEND_UINT82(data);
		ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();

		if(++data2 > 0x0f)
			data2 = 0;

		ESOS_TASK_WAIT_TICKS(TDELAY2/2);

        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        ESOS_TASK_WAIT_ON_SEND_UINT8(data);
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        if(++data > 0x7e)
        	data = 0x21;
    } // endof while()
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
//************************************* CONFIG_SPI2 ****************************************//
//******************************************************************************************//
#if 0
CONFIG_SPI2()
{
#if 0
  SPI1CON1 = SEC_PRESCAL_1_1		|		//1:1 secondary prescale	40MHz
             PRI_PRESCAL_4_1		|		//4:1 primary prescale
             CLK_POL_ACTIVE_HIGH	|		//clock active high (CKP = 0)
             SPI_CKE_ON				|		//out changes active to idle (CKE=1)
             SPI_MODE8_ON			|		//8-bit mode
             MASTER_ENABLE_ON;				//master mode
#endif
//#if 0
  SPI2CON1 = SEC_PRESCAL_8_1		|		//8:1 secondary prescale
             PRI_PRESCAL_64_1		|		//64:1 primary prescale
             CLK_POL_ACTIVE_HIGH	|		//clock active high (CKP = 0)
             SPI_CKE_ON				|		//out changes active to idle (CKE=1)
             SPI_MODE8_ON			|		//8-bit mode
             MASTER_ENABLE_ON;				//master mode
//#endif
//	SPI1CON2 = FRAME_ENABLE_ON | FRAME_SYNC_OUTPUT | FRAME_SYNC_ACTIVE_LOW | SPI_FRM_PULSE_PREV_CLK;
//	SPI1CON2 = FRAME_ENABLE_ON | FRAME_SYNC_OUTPUT | FRAME_SYNC_ACTIVE_LOW | SPI_FRM_PULSE_FIRST_CLK;
//	SPI1CON2 = FRAME_ENABLE_ON | FRAME_SYNC_OUTPUT | FRAME_SYNC_ACTIVE_HIGH;
/*
pin 47	RD14	RPI42	MISO	(input)
pin 48	RD15	RP5		MOSI	(output)
pin 49	RF4		RP10	SCLK	(output)
pin 50	RF5		RP17	SS		(output)
*/
	CONFIG_SDI2_TO_RP(RC1_RP);
	CONFIG_RC1_AS_DIG_INPUT();
	CONFIG_SDO2_TO_RP(RG6_RP);
	CONFIG_RG6_AS_DIG_OUTPUT();
	CONFIG_SCK2OUT_TO_RP(RG7_RP);
	CONFIG_RG7_AS_DIG_OUTPUT();
//	CONFIG_SS2OUT_TO_RP(RG8_RP);
	CONFIG_RG8_AS_DIG_OUTPUT();
	SLAVE2_DISABLE();
	SPI2STATbits.SPIEN = 1;  //enable SPI mode
	
}
//******************************************************************************************//
//**************************************** echo_spi ****************************************//
//******************************************************************************************//
ESOS_USER_TASK(fast_echo_spi_task)
{
	static uint8_t data3;
    ESOS_TASK_BEGIN();
	ESOS_TASK_SIGNAL_AVAILABLE_SPI();

	data3 = 0xaa;
	
    while(TRUE)
	{
	    ESOS_TASK_WAIT_ON_AVAILABLE_SPI();
	    SLAVE_ENABLE();
	    ESOS_TASK_WAIT_ON_WRITE1SPI1(data3);
		SLAVE_DISABLE();
		ESOS_TASK_SIGNAL_AVAILABLE_SPI();
		ESOS_TASK_WAIT_TICKS(1);

/*
	    ESOS_TASK_WAIT_ON_AVAILABLE_SPI();
	    SLAVE2_ENABLE();
	    ESOS_TASK_WAIT_ON_WRITE1SPI2(data3);
		SLAVE2_DISABLE();
		ESOS_TASK_SIGNAL_AVAILABLE_SPI();
		ESOS_TASK_WAIT_TICKS(1);
*/
    }
    ESOS_TASK_END();
}
#endif

