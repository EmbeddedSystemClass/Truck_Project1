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
#define TDELAY2 1000
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

ESOS_USER_TASK(keyscan_col);
ESOS_USER_TASK(keyscan_row);
ESOS_SEMAPHORE(key_sem);
ESOS_USER_TASK(getkey_task);
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
//******************************************************************************************//
//*************************************** CONFIG_KEYPAD ************************************//
//******************************************************************************************//
inline void CONFIG_KEYPAD()
{
    CONFIG_RE0_AS_DIG_INPUT();  // row 0 - p93
    ENABLE_RE0_PULLUP();     // enable the pull-ups on the inputs
    CONFIG_RE1_AS_DIG_INPUT();  // row 1 - p94
    ENABLE_RE1_PULLUP();
    CONFIG_RE2_AS_DIG_INPUT();  // row 2 - p98
    ENABLE_RE2_PULLUP();
    CONFIG_RE3_AS_DIG_INPUT();  // row 3 - p99
    ENABLE_RE3_PULLUP();
    CONFIG_RA0_AS_DIG_OUTPUT(); // col 0 - p17
    CONFIG_RA1_AS_DIG_OUTPUT(); // col 1 - p38
    CONFIG_RA2_AS_DIG_OUTPUT(); // col 2 - p58
    CONFIG_RA3_AS_DIG_OUTPUT(); // col 3 - p59
}
// send a scan code to the columns outputs on 4 lsb's of port a as:
// 0111
// 1011
// 1101
// 1110
// to test for keypad input on 4 lsb's of port a
//******************************************************************************************//
//*************************************** keyscan_col **************************************//
//******************************************************************************************//
ESOS_USER_TASK(keyscan_col)
{
    static uint8_t scan;
    ESOS_TASK_BEGIN();
//    scan = 0xFFFE;
    scan = 0x0008;
    col = scan;

    while(TRUE)
    {
/*
        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((uint8_t)(scan>>4));
        ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING((uint8_t)scan);
        ESOS_TASK_WAIT_ON_SEND_UINT8(0x30);
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

        LATA = LATA & scan;

        scan <<= 1;
        scan |= 0x0001;
        if(scan == 0xFFF7)
            scan = 0xFFFE;
*/
        if((scan >>= 1) == 0)
            scan = 0x0008;

        if((scan & 0x01) == 1)
            _LATA0 = 0;
        else _LATA0 = 1;

        if((scan & 0x02) == 0x02)
            _LATA1 = 0;
        else _LATA1 = 1;

        if((scan & 0x04) == 0x04)
            _LATA2 = 0;
        else _LATA2 = 1;

        if((scan & 0x08) == 0x08)
            _LATA3 = 0;
        else _LATA3 = 1;

//        ESOS_TASK_WAIT_TICKS(KEYPAD_DEBOUNCE_DELAY);
        col = scan;
        ESOS_TASK_WAIT_TICKS(KEYPAD_DELAY);
        ESOS_SIGNAL_SEMAPHORE(key_sem,1);
    }
    ESOS_TASK_END();
}
// wait until user flag is set then check for a keypad press
// if any of the lower 4 bits of PORTE are 0 then get the keypress
//******************************************************************************************//
//************************************* keyscan_row ****************************************//
//******************************************************************************************//
ESOS_USER_TASK(keyscan_row)
{
//    static ESOS_TASK_HANDLE hTask_LCD;
    static ESOS_TASK_HANDLE h_Task1;
    static uint8_t code;
    static uint8_t ret_code;
    static uint16_t tPORTE;
    static uint8_t temp = 0;
    h_Task1 = esos_GetTaskHandle(getkey_task);

    ESOS_TASK_BEGIN();
//    hTask_LCD = esos_GetTaskHandle(to_LCD_task);
    while(TRUE)
    {
        // PORTE = rows 0x0E - row 0; 0x0D - row 2; 0x0B - row 1;0x07 - row 3
        // PORTA = cols
        ESOS_TASK_WAIT_SEMAPHORE(key_sem,1);
        if((PORTE & 0x000F) != 0x0F)
        {
            tPORTE = PORTE;
            tPORTE &= 0x0f;
            code = (0xF0 & ~(col << 4)) | (uint8_t)(PORTE & 0x000F);

            if(code == last_code && temp < 4)
            {
                ESOS_TASK_WAIT_TICKS(100);
                last_code = 0;
                temp++;
                code = 0;
            }
            else
            {
                last_code = code;
                temp = 0;
            }
			// 0E - bit 0 cleared rest are set
			// 0D - bit 1 cleared  "   "   "
			// 0B - bit 2 cleared  "   "   "
			// 07 - bit 3 cleared  "   "   "
            switch(code)
            {                                       //  col		row
                case 0xEE:  ret_code = KP_1;break;   //  0       0
                case 0xED:  ret_code = KP_4;break;   //  0       1
                case 0xEB:  ret_code = KP_7;break;   //  0       2
                case 0xE7:  ret_code = KP_AST;break;   //  0       3

                case 0xDE:  ret_code = KP_2;break;   //  1       0
                case 0xDD:  ret_code = KP_5;break;   //  1       1
                case 0xDB:  ret_code = KP_8;break;   //  1       2
                case 0xD7:  ret_code = KP_0;break;   //  1       3

                case 0xBE:  ret_code = KP_3;break;   //  2       0
                case 0xBD:  ret_code = KP_6;break;   //  2       1
                case 0xBB:  ret_code = KP_9;break;   //  2       2
                case 0xB7:  ret_code = KP_POUND;break;   //  2       3

                case 0x7E:  ret_code = KP_A;break;   //  3       0
                case 0x7D:  ret_code = KP_B;break;   //  3       1
                case 0x7B:  ret_code = KP_C;break;   //  3       2
                case 0x77:  ret_code = KP_D;break;   //  3       3
                default:
//                    ESOS_TASK_WAIT_ON_SEND_STRING("bad char\n\r");
                    ret_code = 0;break;
            }

            if(ret_code > 1)
				__esos_CB_WriteUINT8(h_Task1->pst_Mailbox->pst_CBuffer,ret_code);
        }
    }
    ESOS_TASK_END();
}
//******************************************************************************************//
//*************************************** getkey_task **************************************//
//******************************************************************************************//
// comm1_task - uses comm1 - waits for incomming mail from keypad
// tasks and sends to AVR over comm3
ESOS_USER_TASK(getkey_task)
{
    static  uint8_t data;
//    static ESOS_TASK_HANDLE h_Task3;

    ESOS_TASK_BEGIN();
//    h_Task3 = esos_GetTaskHandle(task3);
    while (1)
    {
        ESOS_TASK_WAIT_FOR_MAIL();
        while(ESOS_TASK_IVE_GOT_MAIL())
        {
            data = __esos_CB_ReadUINT8(__pstSelf->pst_Mailbox->pst_CBuffer);
            ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM3();
            ESOS_TASK_WAIT_ON_SEND_UINT83(data);
            ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM3();
        }
    } // endof while()
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
        if(data1 == 0)
//        if(data2 >= 0x1f)
        {
			ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
			ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
        }
//        ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(data2);
		ESOS_TASK_WAIT_ON_SEND_UINT8(data2);
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
        
		ESOS_TASK_WAIT_TICKS(TDELAY2);

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

