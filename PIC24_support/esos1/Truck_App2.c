// .. "Copyright (c) 2016 Dan Hampleman ("AUTHOR")"
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

// INCLUDEs go here  (First include the main esos.h file)
//      After that, the user can include what they need

/************************************************************************
 * User supplied functions
 ************************************************************************
 */
#define _TRUCK_APP
#include "Truck_App.h"

/*
PIC24 UARTS:

UART1 - RS232 to TS-7200
UART2 - AVR
UART3 - monitor
*/

//******************************************************************************************//
//************************************** data_to_AVR ***************************************//
//******************************************************************************************//
ESOS_USER_TASK(data_to_AVR)
{
    static  uint8_t data = 2;
    static  uint8_t data1 = 0;
	static  uint16_t data2 = 0;
	static char tempx[20];
//    static  uint8_t code = RT_RPM;

    ESOS_TASK_BEGIN();
//    h_Task3 = esos_GetTaskHandle(task3);
	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM3();
    ESOS_TASK_WAIT_ON_SEND_UINT83('\n');
    ESOS_TASK_WAIT_ON_SEND_UINT83('\r');
	ESOS_TASK_WAIT_ON_SEND_STRING("data_to_AVR");
    ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING3(0x41);
	ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM3();

	while(TRUE)
    {

    } // endof while()
    ESOS_TASK_END();
}
//******************************************************************************************//
//*************************************** comm2_task ***************************************//
//******************************************************************************************//
ESOS_USER_TASK(comm2_task)
{
    static  uint8_t key1, ret_key;
    
    int i, size, start_addr;
    static ESOS_TASK_HANDLE h_Sender;
    static int once = 1;
    
    ESOS_TASK_BEGIN();
    h_Sender = esos_GetTaskHandle(poll_keypad);

	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
    ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
    ESOS_TASK_WAIT_ON_SEND_STRING("comm2_task");
    ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
    ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
	ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
	size = AVR_SEND_DATA_SIZE;


	while(TRUE)
    {
//		if(++data1 > 0x7e)
//			data1 = 0x21;
#if 0
		if(once == 1)
		{
			size = AVR_SEND_DATA_SIZE;
			ret_key = get_key((UCHAR)key1,size,(UCHAR*)avr_send_data);
			ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
			for(i = 0;i < AVR_SEND_DATA_SIZE;i++)
				ESOS_TASK_WAIT_ON_SEND_UINT82(avr_send_data[i]);
			ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();
			once =0;
		}
#endif
        ESOS_TASK_WAIT_FOR_MAIL();
        while(ESOS_TASK_IVE_GOT_MAIL())
        {
			key1 = __esos_CB_ReadUINT16(h_Sender->pst_Mailbox->pst_CBuffer);

// UCHAR get_key(UCHAR ch, int size, int start_addr, UCHAR *str, int type)

			ret_key = get_key((UCHAR)key1,size,start_addr,(UCHAR*)avr_send_data,0);
		    ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
		    for(i = 0;i < AVR_SEND_DATA_SIZE;i++)
				ESOS_TASK_WAIT_ON_SEND_UINT82(avr_send_data[i]);
		    ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();
		    
			ESOS_TASK_WAIT_TICKS(1);

// if key returned is one of the non_func codes then do something...

			switch(ret_key)
			{
				case NF_1:
				break;
				case NF_2:
				break;
				case NF_3:
				break;
				case NF_4:
				break;
				case NF_5:
				break;
				case NF_6:
				break;
				case NF_7:
				break;
				case NF_8:
				break;
				case NF_9:
				break;
				case NF_10:
				break;
				default:
				break;
			}
			
		}

	}
    ESOS_TASK_END();
}
//******************************************************************************************//
//*************************************** comm1_task ***************************************//
//******************************************************************************************//
// comm 1 is the TS-7200

ESOS_USER_TASK(comm1_task)
{
    static  uint8_t data1, ret_key;
    int i, size, start_addr, type;

    ESOS_TASK_BEGIN();

	ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
    ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
    ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
    ESOS_TASK_WAIT_ON_SEND_STRING("comm1_task on comm1");
    ESOS_TASK_WAIT_ON_SEND_UINT8('\n');
    ESOS_TASK_WAIT_ON_SEND_UINT8('\r');
	ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();

	data1 = 0x21;
	while(TRUE)
    {
/*
		if(++data1 > 0x7e)
			data1 = 0x21;

        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
		ESOS_TASK_WAIT_ON_SEND_UINT8(data1);
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();
*/
		ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
		ESOS_TASK_WAIT_ON_GET_UINT8(data1);
        ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
		switch(data1)
		{
			case INIT:
				size = AVR_SEND_DATA_SIZE;
				start_addr = 0;
				type = 0;
				for(i = 0;i < size;i++)
					avr_send_data[i] = 0;

				ret_key = get_key((UCHAR)data1,size,start_addr,(UCHAR*)avr_send_data,0);

				break;
			case READ_EEPROM:
				break;
			case BURN_EEPROM:
				break;
			default:
				break;	
		}
		ESOS_TASK_WAIT_TICKS(1);

	}
    ESOS_TASK_END();
}
//******************************************************************************************//
//*************************************** comm3_task ***************************************//
//******************************************************************************************//
// comm 3 is the monitor
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
//******************************************************************************************//
//*************************************** user_init  ***************************************//
//******************************************************************************************//
void user_init(void)
{
//    __esos_unsafe_PutString( HELLO_MSG );
//	CONFIG_SPI_MASTER()
//	CONFIG_SPI_SLAVE();
//	esos_RegisterTask(echo_spi_task);
/*
    CONFIG_KEYPAD();
	ESOS_INIT_SEMAPHORE(key_sem,0);
	ESOS_INIT_SEMAPHORE(send_sem,0);

	esos_RegisterTask(recvFPGA);
	esos_RegisterTask(sendFPGA);
	esos_RegisterTask(sendFPGA2);
	esos_RegisterTask(send_cmd_param);
*/
//	esos_RegisterTask(comm3_task);
	esos_RegisterTask(comm1_task);
	esos_RegisterTask(comm2_task);

//	esos_RegisterTask(keypad);
//	esos_RegisterTask(poll_keypad);
//	esos_RegisterTask(convADC);
} // end user_init()


